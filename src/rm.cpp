//
// Created by whenke on 3/16/18.
//

#include <iostream>
#include <dirent.h>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <cstring>
#include <libgen.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <utime.h>
#include <sys/time.h>
#include <sys/types.h>

/* (shown here for reference)
struct  dirent {
        ino_t    d_ino;               * file number of entry *
        ushort_t d_reclen;            * length of this record *
        ushort_t d_namlen;            * length of string in d_name *
        char    d_name[256];	      * the actual directory names *
};

struct stat {
        dev_t     st_dev;      ID of device containing file
        ino_t     st_ino;      inode number
        mode_t    st_mode;     protection
        nlink_t   st_nlink;    number of hard links
        uid_t     st_uid;      user ID of owner
        gid_t     st_gid;      group ID of owner
        dev_t     st_rdev;     device ID (if special file)
        off_t     st_size;     total size, in bytes
        blksize_t st_blksize;  blocksize for file system I/O
        blkcnt_t  st_blocks;   number of 512B blocks allocated
        time_t    st_atime;    time of last access
        time_t    st_mtime;    time of last modification
        time_t    st_ctime;    time of last status change
};
*/

using namespace std;

//see if vector contains specific file
bool contains(char* tofind, vector<struct dirent*> vector) {
    bool found = false;
    
    for (auto &i : vector) {
        if (strcmp(i->d_name, tofind) == 0) {
            found = true;
            return found;
        }
    }
    return found;
}

int cp(const char *to, const char *from)
{
    int fd_to, fd_from;
    char buf[4096];
    ssize_t nread;
    int saved_errno;
    
    fd_from = open(from, O_RDONLY);
    if (fd_from < 0)
        return -1;
    
    fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
    if (fd_to < 0)
        goto out_error;
    
    while (nread = read(fd_from, buf, sizeof buf), nread > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;
        
        do {
            nwritten = write(fd_to, out_ptr, nread);
            
            if (nwritten >= 0)
            {
                nread -= nwritten;
                out_ptr += nwritten;
            }
            else if (errno != EINTR)
            {
                goto out_error;
            }
        } while (nread > 0);
    }
    
    if (nread == 0)
    {
        if (close(fd_to) < 0)
        {
            fd_to = -1;
            goto out_error;
        }
        close(fd_from);
    
        auto * stat1 = static_cast<struct stat *>(malloc(sizeof(struct stat)));
        stat(from, stat1);
        
        unlink(from);
        chown(to, stat1->st_uid, stat1->st_gid);
        chmod(to, stat1->st_mode);
        struct utimbuf times;
//        times.actime = stat1->st_mtim;
//        utime(to, )
        
        /* Success! */
        return 0;
    }
    
    out_error:
    saved_errno = errno;
    
    close(fd_from);
    if (fd_to >= 0)
        close(fd_to);
    
    errno = saved_errno;
    return -1;
}

//remove each file in vector
void processFiles(vector<string> filesToRemove, string workDir, bool skipDumpster, string dumpsterPath, bool recursive,
                  vector<dirent *> dumpdirentries, dev_t dumpDisk) {
    
    //for each string in files to remove
    for (string &i : filesToRemove) {

        string oldfile;
        
        //allocate space and copy strings because dirname and basename modify incoming args
        auto *arg = static_cast<char *>(malloc(100));
        auto *arg2 = static_cast<char *>(malloc(100));
        i.copy(arg, 100);
        i.copy(arg2, 100);
        
        //get the directory
        string directory = dirname(arg);
        
        //remove extra . if present
        if (strcmp(".", directory.c_str()) == 0) {
            oldfile = i;
        } else {
            oldfile = directory + "/" + basename(arg2);
        }
        
        
        if (oldfile.at(0) != (char) '/') {
            oldfile = workDir + '/' + oldfile;
        }
        
        //get the stats of the file
        auto * stat1 = static_cast<struct stat *>(malloc(sizeof(struct stat)));
        stat(oldfile.c_str(), stat1);
        
        //if directory and not using -r flag, exit with error
        if (S_ISDIR(stat1->st_mode) && !recursive) {
            cout << "given a directory without recursive flag" << endl;
            exit(1);
        }
        
        //base name for new path in dumpster
        string newfile = dumpsterPath + '/' + basename(arg2);
        
        //if we arent skipping the dumpster
        if (!skipDumpster) {
            
            //see if dumpster contains a copy of the file we are trying to remove
            if (contains((char *) i.c_str(), dumpdirentries)) {
                
                //try to find a new name for the file, unsing the .num extension
                bool fixed = false;
                for (int j = 1; j < 10; ++j) {
                    string possibleNewName = string(i) + string(".") + to_string(j);
                    if (!contains(const_cast<char *>(possibleNewName.c_str()), dumpdirentries)) {
                        newfile += "." + to_string(j);
                        fixed = true;
                        break;
                    }
                }
                
                //exit with error if no possibility is found,
                if (!fixed) {
                    cout << "too many versions of file in dumpster" << endl;
                    exit(-8);
                }
            }
            
            //check if on same partition
            if (stat1->st_dev == dumpDisk) {
                //rename, and exit if error
                if (rename(oldfile.c_str(), newfile.c_str()) != 0) {
                    cout << "rename error" << endl;
                    cout << errno << endl;
                    cout << oldfile << endl;
                    exit(-4);
                }
            //else, not on same partition
            } else {
            
            }
            
            
            
            //if we are skipping the dumpster
        } else {
            
            //if we are removing a directory
            if (S_ISDIR(stat1->st_mode)) {
                DIR *dp;
                struct dirent *d;
                vector<string> filesInFolder;
    
                //open the directory
                dp = opendir(oldfile.c_str());
                
                //exit if we cant open the directory
                if (dp == nullptr) {
                    perror("open");
                    exit(1);
                }
    
                //read files from directory
                d = readdir(dp);
                while (d) {
                    //add files to vector
                    if (strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0) {
                        filesInFolder.emplace_back(d->d_name);
                    }
                    d = readdir(dp);
                }
    
                //close directory
                closedir(dp);
                
                //recursive call
                processFiles(filesInFolder, workDir + "/" + i, skipDumpster, dumpsterPath, recursive, dumpdirentries, 0);
            }
            
            //if just removing a file, go for it, exit if error
            if (remove(oldfile.c_str()) != 0) {
                cout << "remove error" << endl;
                cout << errno << endl;
                cout << oldfile << endl;
                exit(-5);
            }
        }

//        if (link(oldfile.c_str(), newfile.c_str()) != 0) {
//            cout << "link error" << endl;
//            cout << errno << endl;
//            exit(-4);
//        }

//        if (unlink(oldfile.c_str()) != 0) {
//            cout << "unlink error" << endl;
//            cout << errno << endl;
//            cout << workDir.append("/").append(i->d_name) << endl;
//            exit(-5);
//        }
    }
}

int main(int argc, char* argv[]) {
    DIR* dumpdir;
    vector<struct dirent*> currentdirentries;
    struct dirent* dumpdirentry;
    vector<struct dirent*> dumpdirentries;
    vector<string> filesToRemove;
    string dumpsterPath;
    string dumpsterEnvVar = "DUMPSTER";
    string workDir = get_current_dir_name();
    bool recursive = false;
    bool skipDumpster = false;
    dev_t dumpDisk;
    
//    cout << argv << endl;
    
    //get Environment Variable
    dumpsterPath = getenv(dumpsterEnvVar.c_str());
    
    //check env var, exit if not found
    if (dumpsterPath.empty()) {
        printf("No match for '%s' in environment\n", dumpsterPath.c_str());
        exit(-1);
    } else {
    }
    
    //check that there is an argument to program, exit if not
    if (argc < 1) {
        perror("no arguments");
        exit(-2);
    }
    
    //open the dumnpster path, exit if error
    dumpdir = opendir(dumpsterPath.c_str());
    if (dumpdir == nullptr) {
        perror("open");
        exit(-6);
    }
    
    //read in all entries in the dumpster
    dumpdirentry = readdir(dumpdir);
    while (dumpdirentry) {
        dumpdirentries.push_back(dumpdirentry);
        dumpdirentry = readdir(dumpdir);
    }
    
    //get stats for the dumpster, to determine partition
    auto * dumpstat = static_cast<struct stat *>(malloc(sizeof(struct stat)));
    stat(dumpsterPath.c_str(), dumpstat);
    dumpDisk = dumpstat->st_dev;
    
    //parse args
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-r") == 0) {
            recursive = true;
        } else if (strcmp(argv[i], "-h") == 0) {
            cout << "use -f to skip the dumpster, -r for recursive, -h for this messahe, and all other arguments are for files and folders to remove" << endl;
            exit(1);
        } else if (strcmp(argv[i], "-f") == 0) {
            skipDumpster = true;
        } else {
            filesToRemove.emplace_back(argv[i]);
        }
        
    }
    
    //process the file list
    processFiles(filesToRemove, workDir, skipDumpster, dumpsterPath, recursive, dumpdirentries, dumpDisk);
    
    
//
//
//        if (link(oldfile.c_str(), newfile.c_str()) != 0) {
//            cout << "link error" << endl;
//            cout << errno << endl;
//            exit(-4);
//        }
//
//        if (unlink(oldfile.c_str()) != 0) {
//            cout << "unlink error" << endl;
//            cout << errno << endl;
//            cout << workDir.append("/").append(i->d_name) << endl;
//            exit(-5);
//        }
//    }
    
    //close dump directory
    closedir(dumpdir);
    
    //exit
    return 0;
}
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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
    
    dumpsterPath = getenv(dumpsterEnvVar.c_str());
    
    if (dumpsterPath.empty()) {
        printf("No match for '%s' in environment\n", dumpsterPath.c_str());
        exit(-1);
    } else {
//        cout << dumpsterPath << endl;
    }
    
    if (argc < 1) {
        perror("no arguments");
        exit(-2);
    }
    
//    curdir = opendir(workDir.c_str());
//    if (curdir == nullptr) {
//        perror("open");
//        exit(-3);
//    }
//
//    currentdirentry = readdir(curdir);
//    while (currentdirentry) {
//        currentdirentries.push_back(currentdirentry);
//        currentdirentry = readdir(curdir);
//    }
    
    dumpdir = opendir(dumpsterPath.c_str());
    if (dumpdir == nullptr) {
        perror("open");
        exit(-6);
    }
    
    dumpdirentry = readdir(dumpdir);
    while (dumpdirentry) {
        dumpdirentries.push_back(dumpdirentry);
        dumpdirentry = readdir(dumpdir);
    }
    
    auto * dumpstat = static_cast<struct stat *>(malloc(sizeof(struct stat)));
    stat(dumpsterPath.c_str(), dumpstat);
    dumpDisk = dumpstat->st_dev;
    
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-r") == 0) {
            recursive = true;
        } else if (strcmp(argv[i], "-h") == 0) {
            cout << "help text" << endl; //TODO
            exit(1);
        } else if (strcmp(argv[i], "-f") == 0) {
            skipDumpster = true;
        } else {
            filesToRemove.emplace_back(argv[i]);
        }
        
    }
    
    for (const auto &i : filesToRemove) {
//        cout << i << endl;
        string oldfile;
        auto *arg = static_cast<char *>(malloc(100));
        auto *arg2 = static_cast<char *>(malloc(100));
        i.copy(arg, 100);
        i.copy(arg2, 100);
    
        string directory = dirname(arg);
        oldfile = directory + "/" + basename(arg2);
        
//        cout << oldfile.at(0) << endl;
        
        if (oldfile.at(0) != (char) '/') {
            oldfile = workDir + '/' + oldfile;
//            cout << oldfile << endl;
        }
    
        auto * stat1 = static_cast<struct stat *>(malloc(sizeof(struct stat)));
        stat(oldfile.c_str(), stat1);
        
        if (S_ISDIR(stat1->st_mode) && !recursive) {
            cout << "given a directory without recursive flag" << endl;
            exit(1);
        }
    
        string newfile = dumpsterPath + '/' + basename(arg2);
        
        if (!skipDumpster) {
            if (contains((char *) i.c_str(), dumpdirentries)) {
                bool fixed = false;
                for (int j = 1; j < 10; ++j) {
                    string possibleNewName = string(i) + string(".") + to_string(j);
                    if (!contains(const_cast<char *>(possibleNewName.c_str()), dumpdirentries)) {
                        newfile += "." + to_string(j);
                        fixed = true;
                        break;
                    }
                }
                if (!fixed) {
                    cout << "too many versions of file in dumpster" << endl;
                    exit(-8);
                }
            }
            if (rename(oldfile.c_str(), newfile.c_str()) != 0) {
                if (errno == 0) {
            
                }
                cout << "rename error" << endl;
                cout << errno << endl;
                cout << oldfile << endl;
                exit(-4);
            }
        } else {
            if (S_ISDIR(stat1->st_mode)) {
                DIR *dp;
                struct dirent *d;
                vector<char*> args;
                
                char* arg1 = static_cast<char *>(malloc(sizeof(char[5])));
                char* arg2 = static_cast<char *>(malloc(sizeof(char[5])));
                char* arg3 = static_cast<char *>(malloc(sizeof(char[5])));
                
                arg1 = reinterpret_cast<char *>('rm');
                arg2 = reinterpret_cast<char *>('-f');
                arg3 = reinterpret_cast<char *>('-r');
                
                args.push_back(arg1);
                args.push_back(arg2);
                args.push_back(arg3);
                
                dp = opendir(".");
                if (dp == nullptr) {
                    perror("open");
                    exit(1);
                }
    
                d = readdir(dp);
                while (d) {
                    args.push_back(d->d_name);
                    
                    d = readdir(dp);
                }
                
                main(static_cast<int>(args.size()), &args[0]);
            }
            if (remove(oldfile.c_str()) != 0) {
            cout << "remove error" << endl;
            cout << errno << endl;
            cout << oldfile << endl;
            exit(-5);
            }
        }
        
//        string oldfile = workDir + "/" + i;
        

    
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
    
    closedir(dumpdir);
    return 0;
}
//
// Created by whenke on 3/16/18.
//

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <dirent.h>
#include <sys/types.h>
#include <vector>
#include <algorithm>
#include <errno.h>
#include <unistd.h>
#include <cstring>
#include <libgen.h>

/* (shown here for reference)
struct  dirent {
        ino_t    d_ino;               * file number of entry *
        ushort_t d_reclen;            * length of this record *
        ushort_t d_namlen;            * length of string in d_name *
        char    d_name[256];	      * the actual directory names *
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
    DIR* curdir;
    DIR* dumpdir;
    struct dirent* currentdirentry;
    vector<struct dirent*> currentdirentries;
    struct dirent* dumpdirentry;
    vector<struct dirent*> dumpdirentries;
    vector<string> filesToRemove;
    string dumpsterPath;
    string dumpsterEnvVar = "DUMPSTER";
    string workDir = get_current_dir_name();
    
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
    
    curdir = opendir(workDir.c_str());
    if (curdir == nullptr) {
        perror("open");
        exit(-3);
    }
    
    currentdirentry = readdir(curdir);
    while (currentdirentry) {
        currentdirentries.push_back(currentdirentry);
        currentdirentry = readdir(curdir);
    }
    
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
    
    for (int i = 1; i < argc; ++i) {
        filesToRemove.emplace_back(argv[i]);
    }
    
//    currentdirentries.erase(remove_if(currentdirentries.begin(), currentdirentries.end(), [&] (struct dirent* e) {
//        for (const auto &i : filesToRemove) {
//            if (i == e->d_name) {
//                return false;
//            }
//        }
//        return true;
//    }), currentdirentries.end());
//
//    if (filesToRemove.size() != currentdirentries.size()) {
//        cout << "not all files found in current directory" << endl;
//        exit(-7);
//    }
    
    for (const auto &i : filesToRemove) {
//        cout << i->d_name << endl;
        
        if (!dirname(const_cast<char *>(i.c_str()))) {
            cout << "arg is a folder" << endl;
        }
    
        string newfile = dumpsterPath + "/" + i;
        
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
        
        string oldfile = workDir + "/" + i;
        
        if (rename(oldfile.c_str(), newfile.c_str()) != 0) {
            cout << "rename error" << endl;
            cout << errno << endl;
            exit(-4);
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
    
    closedir(curdir);
    closedir(dumpdir);
    return 0;
}
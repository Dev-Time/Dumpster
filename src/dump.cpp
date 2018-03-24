//
// Created by Luke Gardner on 3/20/18.
// export DUMPSTER=/home/student/DUMPSTER/
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
#include <sys/stat.h>

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

int recurse_remove(DIR *dp, string path){
    struct dirent *d;
    DIR *dpN;
    struct stat s;
    d = readdir(dp);
    while (d) {
        string oldfile = path + d->d_name;
        if(oldfile.compare(path + ".") != 0 && oldfile.compare(path + "..") != 0){
            if( stat(oldfile.c_str(),&s) == 0 )
            {
                if( s.st_mode & S_IFDIR )
                {
                    //it's a directory
                    dpN = opendir(oldfile.c_str());
                    if (dpN == NULL) {
                        perror("open");
                        exit(1);
                    }
                    recurse_remove(dpN, oldfile + "/");
                    rmdir(oldfile.c_str());
                }else {
                    unlink(oldfile.c_str());
                }
            }
        }
        d = readdir(dp);
    }
    closedir(dp);
}

int main(int argc, char* argv[]) {
    string fileName;
    DIR* dumpdir;
    vector<struct dirent*> currentdirentries;
    struct dirent* dumpdirentry;
    vector<struct dirent*> dumpdirentries;
    vector<string> filesToRestore;
    string dumpsterPath;
    string dumpsterEnvVar = "DUMPSTER";
    string workDir = get_current_dir_name();
    DIR *dp;

    dumpsterPath = getenv(dumpsterEnvVar.c_str());

    //open the dumpster dierectory
    dumpdir = opendir(dumpsterPath.c_str());
    if (dumpdir == nullptr) {
        perror("open");
        exit(-6);
    }

    //read the dumpster directory
    dumpdirentry = readdir(dumpdir);
    while (dumpdirentry) {
        dumpdirentries.push_back(dumpdirentry);
        dumpdirentry = readdir(dumpdir);
    }

    dp = opendir(dumpsterPath.c_str());
    if (dp == NULL) {
        perror("open");
        exit(1);
    }

    recurse_remove(dp, dumpsterPath);


    //closedir(dp);
    return 0;
}

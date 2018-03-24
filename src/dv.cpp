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
    string fileName;
    DIR* curdir;
    DIR* dumpdir;
    struct dirent* currentdirentry;
    vector<struct dirent*> currentdirentries;
    struct dirent* dumpdirentry;
    vector<struct dirent*> dumpdirentries;
    vector<string> filesToRestore;
    string dumpsterPath;
    string dumpsterEnvVar = "DUMPSTER";
    string workDir = get_current_dir_name();

    dumpsterPath = getenv(dumpsterEnvVar.c_str());

    //if empty exit
    if (dumpsterPath.empty()) {
        printf("No match for '%s' in environment\n", dumpsterPath.c_str());
        exit(-1);
    } else {
        //printf("TEST");
    }

    //if no arguments provided exit and tell user.
    if (argc < 2) {
        perror("No File Argument given. use \"dv -h\" for help.");
        exit(-2);
    } else {
        fileName = argv[1];
    }

    //current directory to restore things too
    curdir = opendir(workDir.c_str());
    if (curdir == nullptr) {
        perror("open");
        exit(-3);
    }

    //read the current directory
    currentdirentry = readdir(curdir);
    while (currentdirentry) {
        currentdirentries.push_back(currentdirentry);
        currentdirentry = readdir(curdir);
    }

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

    //figure out what file to restore from the arguments
    for (int i = 1; i < argc; ++i) {
        filesToRestore.emplace_back(argv[i]);
    }

    //restore the files
    for (const auto &i : filesToRestore) {
//        cout << i->d_name << endl;

        if (!dirname(const_cast<char *>(i.c_str()))) {
            cout << "arg is a folder" << endl;
        }

        //create the new file in the working (current) directory
        string newfile = workDir + "/" + i;

        //if it is a folder, recurse, if not copy over
        if (contains((char *) i.c_str(), currentdirentries)) {
            bool fixed = false;
            for (int j = 1; j < 10; ++j) {
                string possibleNewName = string(i) + string(".") + to_string(j);
                if (!contains(const_cast<char *>(possibleNewName.c_str()), currentdirentries)) {
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

        //define the old file
        string oldfile = dumpsterPath + "/" + i;

        //rename the old file
        if (rename(oldfile.c_str(), newfile.c_str()) != 0) {
            cout << "rename error" << endl;
            cout << errno << endl;
            exit(-4);
        }
    }
    closedir(curdir);
    closedir(dumpdir);
    return 0;
}

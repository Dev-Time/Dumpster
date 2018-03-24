//
// Created by Matt Jackman on 3/20/18.
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
    DIR* dumpdir;
    struct dirent* dumpdirentry;
    vector<struct dirent*> dumpdirentries;
    vector<string> filesToRestore;
    string dumpsterPath;
    string dumpsterEnvVar = "DUMPSTER";

    dumpsterPath = getenv(dumpsterEnvVar.c_str());

    if (dumpsterPath.empty()) {
        printf("No match for '%s' in environment\n", dumpsterPath.c_str());
        exit(-1);
    } else {
        printf("TEST");
    }
    //parse args, handle -h flag
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0) {
            cout << "empties the DUMPSTER folder" << endl;
            exit(1);
        }
    }
    dumpdir = opendir(dumpsterPath.c_str());
    if (dumpdir == nullptr) {
        perror("directory not valid");
        exit(-6);
    }
    while ((dumpdirentry = readdir (dir)) != NULL) {
          printf("%s\n", dumpdirentry->d_name);
          unlink(dumpsterPath);
       }
    dumpdirentry = readdir(dumpdir);
    while (dumpdirentry) {
        dumpdirentries.push_back(dumpdirentry);
        dumpdirentry = readdir(dumpdir);
        if(){//check to see if the current file to be removed is a directory or not

        }
    }
//    DIR *dir;
//    struct dirent *ent;
//    if ((dir = opendir ("c:\\src\\")) != NULL) {
//        /* print all the files and directories within directory */
//        while ((ent = readdir (dir)) != NULL) {
//            printf ("%s\n", ent->d_name);
//        }
//        closedir (dir);
//    } else {
//        /* could not open directory */
//        perror ("");
//        return EXIT_FAILURE;
//    }

    closedir(dumpdir);
    return 0;
}

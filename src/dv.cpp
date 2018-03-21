//
// Created by Luke Gardner on 3/20/18.
//

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <dirent.h>
#include <sys/types.h>
#include <vector>
#include <string>
#include <algorithm>
#include <errno.h>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[]) {
    string fileName;
    DIR* dumpdir;
    struct dirent* dumpdirentry;
    struct dirent *d;
    vector<struct dirent*> dumpdirentries;
    string dumpsterPath;
    string dumpsterEnvVar = "DUMPSTER";

    dumpsterPath = getenv(dumpsterEnvVar.c_str());

    if (dumpsterPath.empty()) {
        printf("No match for '%s' in environment\n", dumpsterPath.c_str());
        exit(-1);
    } else {
        //printf("TEST");
    }

    if (argc < 2) {
        perror("No File Argument given. use \"dv -h\" for help.");
        exit(-2);
    } else {
        fileName = argv[1];
    }

    printf("input: %s", argv[1]);

    printf("\noutput: ");

    dumpdir = opendir(dumpsterPath.c_str());
    if (dumpdir == nullptr) {
        perror("open");
        exit(-6);
    }

    d = readdir(dumpdir);
    while (d) {
        printf("\n%s", d->d_name);
        if(fileName.compare( d->d_name)==0){
            printf("\n Found the file.");
        }
        d = readdir(dumpdir);
    }
}

//
// Created by whenke on 3/16/18.
//

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <dirent.h>
#include <sys/types.h>
#include <vector>
#include <zconf.h>
#include <algorithm>

/* (shown here for reference)
struct  dirent {
        ino_t    d_ino;               * file number of entry *
        ushort_t d_reclen;            * length of this record *
        ushort_t d_namlen;            * length of string in d_name *
        char    d_name[256];	      * the actual directory names *
};
*/

using namespace std;

int main(int argc, char* argv[]) {
    DIR* dump;
    struct dirent* entry;
    vector<struct dirent*> entries;
    vector<string> filesToRemove;
    string dumpsterPath;
    string dumpsterEnvVar = "DUMPSTER";
    
    dumpsterPath = getenv(dumpsterEnvVar.c_str());
    
    if (dumpsterPath.empty()) {
        printf("No match for '%s' in environment\n", dumpsterPath.c_str());
        exit(1);
    } else {
//        cout << dumpsterPath << endl;
    }
    
    if (argc < 2) {
        perror("no arguments");
        exit(2);
    }
    
    dump = opendir(get_current_dir_name());
    if (dump == nullptr) {
        perror("open");
        exit(1);
    }
    
    entry = readdir(dump);
    while (entry) {
        entries.push_back(entry);
        entry = readdir(dump);
    }
    
    for (int i = 1; i < argc; ++i) {
        filesToRemove.emplace_back(argv[i]);
    }
    
    entries.erase(remove_if(entries.begin(), entries.end(), [&] (struct dirent* e) {
        for (const auto &i : filesToRemove) {
            if (i == e->d_name) {
                return false;
            }
        }
        return true;
    }), entries.end());
    
    
    for (const auto &i : entries) {
        cout << i->d_name << endl;
    }
    
    closedir(dump);
    return 0;
}
//
// Created by whenke on 3/16/18.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

int main() {
    char *dumpsterPath;
    const char *dumpesterEnvVar = "DUMPSTER";
    
    dumpsterPath = getenv(dumpesterEnvVar);
    
    std::cout << dumpsterPath << std::endl;
    return 0;
}
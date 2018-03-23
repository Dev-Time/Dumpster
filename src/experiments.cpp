//
// Created by student on 3/23/18.
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
#include <chrono>

using namespace std;

int linkE() {
    //Setup
    cout << "Setting up Link & Unlink Experiment.\n";
    int numTrials = 1000;
    const char* testFile = "test.txt";
    const char* testLocation = "testcopy.txt";
// Record start time


// Portion of code to be timed
//    cout << "Hit enter to proceed with experiment.\n";
//    cin.ignore();
    auto start = chrono::high_resolution_clock::now();

    for ( int i=0; i<numTrials; i++ ) {
        link(testFile, testLocation);
        unlink(testLocation);
    }

// Record end time
    auto finish = std::chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = finish - start;
    cout << "Elapsed time for "<< numTrials <<" trials of link & unlink: " << elapsed.count() << " s\n";
    cout << "Average time for each link & unlink: " << elapsed.count()/numTrials << " s\n";

    return 1;
}

int renameE() {
    //Setup
    cout << "Setting up Rename Experiment.\n";
    int numTrials = 1000;
    const char* testFile = "test.txt";
    const char* testLocation = "testnew.txt";
// Record start time


// Portion of code to be timed
//    cout << "Hit enter to proceed with experiment.\n";
//    cin.ignore();
    auto start = chrono::high_resolution_clock::now();

    for ( int i=0; i<numTrials; i++ ) {
        rename(testFile, testLocation);
        rename(testLocation, testFile);
    }

// Record end time
    auto finish = std::chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = finish - start;
    cout << "Elapsed time for "<< numTrials <<" trials of rename & rename: " << elapsed.count() << " s\n";
    cout << "Average time for each pair of rename: " << elapsed.count()/numTrials << " s\n";
    cout << "Average time for each single rename: " << elapsed.count()/(numTrials*2) << " s\n";

    return 1;
}

int main() {
    cout << "These experiments expect test.txt to be present before you run them. Please ensure that file is there.\n"
            "Hit enter to proceed with experiment.\n";
    cin.ignore();
    linkE();
    cout << "\n---------------------------------------\n";
    renameE();
    return 1;
}
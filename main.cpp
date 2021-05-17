#include "workload.h"
#include <cstdlib>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Usage: ./benchmark.out workload_file_name (random_seed)" << endl;
        exit(1);
    }
    Workload *w = new Workload(argv[1]);
    if (argc == 3) {
        w->setRandomSeed(stoi(argv[2]));
    } else {
        w->setRandomSeed(0);
    }
    w->printParams();
    w->run();
    return 0;
}

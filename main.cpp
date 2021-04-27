#include "workload.h"
#include <cstdlib>
#include <iostream>

using namespace std;

int main() {
    Workload *w = new Workload("workloads/test");
    w->printParams();
    w->run();
    return 0;
}

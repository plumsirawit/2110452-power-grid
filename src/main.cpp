#include "util.h"
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./main <input-filename> <output-filename>"
                  << std::endl;
        return 0;
    }
    std::ifstream inf(argv[1]);
    if (inf.fail()) {
        std::cout << "Error opening input file. Check whether file exists."
                  << std::endl;
        return 0;
    }
    std::ofstream ouf(argv[2]);
    if (ouf.fail()) {
        std::cout << "Error opening output file." << std::endl;
        return 0;
    }
    Instance inst;
    inst.load(inf);
    std::cout << "Loading complete. Max number of threads: " << MAX_THREADS
              << std::endl;
    ouf << inst.solve() << std::endl;
    return 0;
}
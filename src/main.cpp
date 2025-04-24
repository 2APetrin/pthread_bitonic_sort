#include "bitonic_sort.hpp"

#include <string>
#include <fstream>
#include <vector>
#include <iterator>
#include <iostream>

int main(int argc, char *argv[]) {
    namespace bs = bitonic_sort;

    if (argc < 3) return 1;
    int threadc = std::stoi(argv[1]);
    if (threadc < 1) return 1;

    std::ifstream file(argv[2]);
    if (!file.is_open()) return 1;

    std::vector<int> array{
        std::istream_iterator<int>(file),
        std::istream_iterator<int>()
    };

    bs::bitonic_sort(array.begin(), array.end(), threadc, std::less<int>());

    for (auto &&i : array)
        std::cout << i << " ";
    std::cout << std::endl;

    return 0;
}

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <fstream>
#include <iterator>
#include <algorithm>
#include <filesystem>
#include <string>
#include <cmath>
#include <ctime>

#include "bitonic_sort.hpp"

namespace bs = bitonic_sort;

class bitonic_sort_sanity_test : public ::testing::TestWithParam<int> {
protected:
    void SetUp() override {
        namespace fs = std::filesystem;

        auto file_folder = fs::absolute(__FILE__).remove_filename();
        fs::path data = fs::canonical(file_folder.string() + "/../data/");

        std::string param = std::to_string(GetParam());
        std::string number = (GetParam() < 10) ? "0" + param : param;
        auto filename = data.string() + "/data" + number + ".in";

        std::ifstream file(filename);
        if (!file.is_open())
            FAIL() << "Failed to open file: " << filename;

        array = {std::istream_iterator<int>(file), 
                 std::istream_iterator<int>()};
        array_copy = array;
    }

    std::vector<int> array;
    std::vector<int> array_copy;
};

TEST_P(bitonic_sort_sanity_test, sanity_check) {
    bs::bitonic_sort(array.begin(), array.end(), 2, std::less<int>());
    std::sort(array_copy.begin(), array_copy.end(), std::less<int>());
    ASSERT_TRUE(array == array_copy);
}

INSTANTIATE_TEST_SUITE_P(
    sanity_check_suite,
    bitonic_sort_sanity_test,
    ::testing::Range(0, 5)
);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

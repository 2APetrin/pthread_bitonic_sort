#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <algorithm>
#include <cmath>
#include <ctime>
#include <string>
#include <chrono>
#include <cstdlib>

#include "bitonic_sort.hpp"

namespace bs = bitonic_sort;

class bitonic_sort_generative_test : public ::testing::TestWithParam<std::tuple<int, int>> {
protected:
    void SetUp() override {
        auto param = GetParam();
        int pow = std::get<0>(param);
        int size = std::pow(2, pow);
        std::srand(std::time(nullptr));

        for (int i = 0; i < size; ++i) array.push_back((std::rand() % size) - size / 2);
        array_copy_stlsort   = array;
        array_copy_quicksort = array;
        cores = std::get<1>(param);
    }

    std::vector<int> array;
    std::vector<int> array_copy_stlsort;
    std::vector<int> array_copy_quicksort;
    int cores;
};

TEST_P(bitonic_sort_generative_test, generative_test) {
    int warmup_iterations = 5;

    // warmup bitonic_sort
    for (int i = 0; i < warmup_iterations; ++i) {
        auto arr_copy = array;
        bs::bitonic_sort(arr_copy.begin(), arr_copy.end(), cores, std::less<int>());
    }
    auto start_bitonic = std::chrono::high_resolution_clock::now();
    bs::bitonic_sort(array.begin(), array.end(), cores, std::less<int>());
    auto end_bitonic = std::chrono::high_resolution_clock::now();
    auto time_bitonic = std::chrono::duration_cast<std::chrono::nanoseconds>(end_bitonic - start_bitonic).count();
    std::cout << "bitonic time = " << (double)time_bitonic/1000000 << " ms" << std::endl;

    // warmup std::sort
    for (int i = 0; i < warmup_iterations; ++i) {
        auto arr_copy = array;
        std::sort(arr_copy.begin(), arr_copy.end(), std::less<int>());
    }
    auto start_stl = std::chrono::high_resolution_clock::now();
    std::sort(array_copy_stlsort.begin(), array_copy_stlsort.end(), std::less<int>());
    auto end_stl = std::chrono::high_resolution_clock::now();
    auto time_stl = std::chrono::duration_cast<std::chrono::nanoseconds>(end_stl - start_stl).count();
    std::cout << "stl time     = " << (double)time_stl/1000000 << " ms" << std::endl;

    // warmup qsort
    for (int i = 0; i < warmup_iterations; ++i) {
        auto arr_copy = array;
        std::qsort(arr_copy.data(),
                  arr_copy.size(),
                  sizeof(decltype(arr_copy)::value_type),
                  [](const void* x, const void* y) {
                      const int arg1 = *static_cast<const int*>(x);
                      const int arg2 = *static_cast<const int*>(y);
                      const auto cmp = arg1 <=> arg2;
                      if (cmp < 0)
                          return -1;
                      if (cmp > 0)
                          return 1;
                      return 0;
                  }
                  );
    }
    auto start_qsort = std::chrono::high_resolution_clock::now();
    std::qsort(array_copy_quicksort.data(),
               array_copy_quicksort.size(),
               sizeof(decltype(array_copy_quicksort)::value_type),
               [](const void* x, const void* y) {
                   const int arg1 = *static_cast<const int*>(x);
                   const int arg2 = *static_cast<const int*>(y);
                   const auto cmp = arg1 <=> arg2;
                   if (cmp < 0)
                       return -1;
                   if (cmp > 0)
                       return 1;
                   return 0;
               });
    auto end_qsort = std::chrono::high_resolution_clock::now();
    auto time_qsort = std::chrono::duration_cast<std::chrono::nanoseconds>(end_qsort - start_qsort).count();
    std::cout << "qsort time   = " << (double)time_qsort/1000000 << " ms" << std::endl;

    ASSERT_TRUE(array == array_copy_quicksort && array == array_copy_stlsort);
}

INSTANTIATE_TEST_SUITE_P(
    generative_suite,
    bitonic_sort_generative_test,
    ::testing::Combine(
        ::testing::Range(10, 26),
        ::testing::Values(1, 2, 4, 8, 16, 32, 64, 128, 256)
    ),
    [](const testing::TestParamInfo<std::tuple<int, int>>& info) {
        int pow   = std::get<0>(info.param);
        int cores = std::get<1>(info.param);
        return "len_2pow" + std::to_string(pow) + "_cores_" + std::to_string(cores);
    }

);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

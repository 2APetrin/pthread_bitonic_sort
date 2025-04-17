#include <iostream>
#include <pthread.h>
#include <string>
#include <vector>
#include <iterator>
#include <fstream>

namespace bitonic_sort {

    namespace detail {

        void* cal(void *args) {
            std::cout << "HELLO WORLD\n";
            return nullptr;
        }


        enum class SEQ_TYPE {
            INCREASE,
            DECREASE
        };


        template<typename it>
        struct sort_arg {
            it first;
            it last;
            SEQ_TYPE type;
        };


        template<typename comp, typename it>
        void* sort_executer(void *args) {
            sort_arg<it>* arg = static_cast<sort_arg<it>*>(args);
            for (auto i = arg->first; (i != arg->last) && (i + 1 != arg->last); ++i)
                std::cout << comp()(*i, *(i+1)) << std::endl;

            return nullptr;
        }

    }

    template<typename comp, typename it>
    void bitonic_sort(it first, it last, int threadc) {

        using arg_t = detail::sort_arg<it>;
        std::vector<pthread_t> threads(threadc);
        std::vector<arg_t> args(threadc);

        detail::SEQ_TYPE sqtype = detail::SEQ_TYPE::INCREASE;
        for (int i = 0; i < threadc; ++i) {
            args[i] = {first + i*2, first + i*2 + 2, sqtype};
            // std::cout << "args[i]->first = " << *args[i].first << std::endl;
            // std::cout << "args[i]->last = " << *args[i].last << std::endl;

            if (detail::SEQ_TYPE::INCREASE == sqtype)
                sqtype = detail::SEQ_TYPE::DECREASE;
            else
                sqtype = detail::SEQ_TYPE::INCREASE;
            pthread_create(&threads[i], NULL, detail::sort_executer<comp, it>, &args[i]);
        }

        for (auto &&thread : threads)
            pthread_join(thread, NULL);
    }
}

int main(int argc, char *argv[]) {
    namespace bc = bitonic_sort;

    if (argc < 2) return 1;
    int threadc = std::stoi(argv[1]);
    if (threadc < 1) return 1;

    std::ifstream file("../data.in");
    if (!file.is_open()) return 1;

    std::vector<int> array{
        std::istream_iterator<int>(file),
        std::istream_iterator<int>()
    };

    for (auto &&i : array)
        std::cout << i << " ";
    std::cout << std::endl;

    bc::bitonic_sort<std::less<int>>(array.begin(), array.end(), threadc);

    std::cout << "HELLO WORLD MAIN\n";

    return 0;
}

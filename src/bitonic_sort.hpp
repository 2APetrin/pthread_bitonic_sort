#ifndef BITONIC_SORT_HPP
#define BITONIC_SORT_HPP

#include <string>
#include <vector>
#include <iterator>

#include <iostream>

#include <pthread.h>

namespace bitonic_sort {

namespace detail {

    enum class direction {
        UP,
        DOWN
    };

    template<typename it>
    struct sort_arg {
        pthread_barrier_t *barrier_;
        it first_;
        it last_;
        int thread_rank_;
        int thread_count_;
        int arr_len_;
        int elem_per_thread_;

    public:
        sort_arg(pthread_barrier_t *barrier, it first, it last,
                int thread_rank, int thread_count, int arr_len) :
            barrier_(barrier),
            first_(first),
            last_(last),
            thread_rank_(thread_rank),
            thread_count_(thread_count),
            arr_len_(arr_len),
            elem_per_thread_(arr_len / thread_count) {}

        void dump(std::string log) {
            std::string str;
            for (it iter = first_; iter != last_; ++iter)
                str += std::to_string(*iter) + " ";
            std::cout << log + " thread" + std::to_string(thread_rank_) + " " + str + "\n";
        }
    };

    inline direction swap_direction(direction dir) {
        return (direction::UP == dir) ? direction::DOWN : direction::UP;
    }

    inline direction get_dir(int begin, int sort_width) {
        int blocks_in_range = begin / sort_width;
        return (blocks_in_range % 2) == 0 ? direction::UP : direction::DOWN;
    }

    template<typename comp, typename it>
    void solo_sort_step(it curr, it half, it block_end, direction dir) {
        while (half != block_end) {
            bool cond_comp = comp()(*curr, *half);
            bool cond_dir  = (dir == direction::DOWN);
            bool cond = (cond_comp == cond_dir);
            if (cond) std::iter_swap(curr, half);
            ++curr; ++half;
        }
    }

    template<typename comp, typename it>
    void solo_sort(it begin, it last, int sort_width, int block_width, direction start_dir) {
        direction dir = start_dir;
        it curr = begin;
        int pos = 0;
        while (curr != last) {
            it half      = curr + block_width / 2;
            it block_end = curr + block_width;
            pos += block_width;
            solo_sort_step<comp, it>(curr, half, block_end, dir);
            if (pos >= sort_width) {
                dir = swap_direction(dir);
                pos = 0;
            }
            curr = block_end;
        }
    }

    template<typename comp, typename it>
    void solo_thread_sort(sort_arg<it> *arg, int sort_width) {
        solo_thread_sort<comp, it>(arg, sort_width, sort_width);
    }

    template<typename comp, typename it>
    void solo_thread_sort(sort_arg<it> *arg, int sort_width, int block_width) {
        for (; block_width > 1; block_width /= 2) {
            int beg_pos = arg->elem_per_thread_ * arg->thread_rank_;
            it begin = arg->first_ + beg_pos;
            direction start_dir = get_dir(beg_pos, sort_width);
            it last = begin + arg->elem_per_thread_;
            solo_sort<comp>(begin, last, sort_width, block_width, start_dir);
        }
    }

    template<typename it>
    std::pair<int, int> get_thread_range(sort_arg<it> *arg, int sort_width) {
        int number_per_block = sort_width / arg->elem_per_thread_;
        int sort_block_pos   = arg->thread_rank_ / number_per_block;
        int pos_in_block     = arg->thread_rank_ % number_per_block;
        int elems_for_thread = sort_width / (2 * number_per_block);
        int beg = sort_block_pos * sort_width + pos_in_block * elems_for_thread;
        int end = beg + elems_for_thread;
        return {beg, end};
    }

    template<typename comp, typename it>
    void multi_thread_sort_step(it beg, it end, int block_width, direction dir) {
        it curr = beg;
        it half = beg + (block_width / 2);
        while (curr != end) {
            bool cond_comp = comp()(*curr, *half);
            bool cond_dir  = (dir == direction::DOWN);
            bool cond = (cond_comp == cond_dir);
            if (cond) std::iter_swap(curr, half);
            ++curr; ++half;
        }
    }

    template<typename comp, typename it>
    void multi_thread_sort(sort_arg<it> *arg, int sort_width) {
        int block_width = sort_width;
        for (; block_width > arg->elem_per_thread_; block_width /= 2) {
            auto thread_range = get_thread_range(arg, block_width);
            direction dir = get_dir(thread_range.first, sort_width);
            it beg = arg->first_ + thread_range.first;
            it end = arg->first_ + thread_range.second;
            multi_thread_sort_step<comp>(beg, end, block_width, dir);
            pthread_barrier_wait(arg->barrier_);
        }

        solo_thread_sort<comp>(arg, sort_width, arg->elem_per_thread_);
        pthread_barrier_wait(arg->barrier_);
    }

    template<typename comp, typename it>
    void* sort_executor(void *args) {
        sort_arg<it>* arg = static_cast<sort_arg<it>*>(args);

        int sort_width = 2;
        for (; sort_width <= arg->elem_per_thread_; sort_width *= 2)
            solo_thread_sort<comp>(arg, sort_width);

        pthread_barrier_wait(arg->barrier_);

        for (; sort_width <= arg->arr_len_; sort_width *= 2)
            multi_thread_sort<comp>(arg, sort_width);

        return NULL;
    }

} // <-- namespace detail

    // Works only with lengths 2^n and 2^m processor amount otherwise undefined behaviour
    template<typename comp, typename it>
    void bitonic_sort(it first, it last, int threadc, comp) {
        using arg_t = detail::sort_arg<it>;
        std::vector<pthread_t> threads(threadc);
        std::vector<arg_t> args;

        pthread_barrier_t barrier;
        pthread_barrier_init(&barrier, NULL, threadc);

        int arr_len = std::distance(first, last);

        for (int i = 0; i < threadc; ++i)
            args.emplace_back(&barrier, first, last, i, threadc, arr_len);

        for (int i = 0; i < threadc; ++i)
            pthread_create(&threads[i], NULL, detail::sort_executor<comp, it>, &args[i]);

        for (auto &&thread : threads)
            pthread_join(thread, NULL);

        pthread_barrier_destroy(&barrier);
    }

} // <-- namespace bitonic_sort

#endif

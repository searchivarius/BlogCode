/*
 * This benchmark is built on top of:
 */
/*
 *
 * B-heap priority queue
 *
 * Copyright Björn Fahller 2015
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/prio_queue
 */

#include "prio_queue.hpp"
#include "falconn_heap_mod.h"
#include "gheap.hpp"
#include "gpriority_queue.hpp"


#include <tachymeter/benchmark.hpp>
#include <tachymeter/seq.hpp>
#include <tachymeter/CSV_reporter.hpp>
#include <queue>
#include <random>
#include <chrono>
#include <sstream>
#include <algorithm>
#include <iostream>

#include <memory>
#include <sstream>
//using namespace std::literals::chrono_literals;
using namespace tachymeter;
using Clock = std::chrono::high_resolution_clock;
using rollbear::prio_queue;

template <typename T>
std::string toString(const T& val) {
  std::stringstream ss;
  ss << val;
  return ss.str();
}

struct null_obj_t
{
  constexpr operator int() const { return 0; }
  template <typename T>
  constexpr operator std::unique_ptr<T>() const { return nullptr; }
};

static const constexpr null_obj_t null_obj{ };
std::vector<int>       nvect;
int*                   n;

auto test_sizes        = powers(seq(1, 2, 5), 1, 100000, 10);
//auto test_sizes        = powers(seq(1, 2, 5), 1, 1000000, 10);
//auto test_sizes        = {1, 10, 100, 1000, 10000, 100000, 10000000, 100000000};
//auto const min_test_duration = std::chrono::milliseconds(1000);
auto const min_test_duration = std::chrono::milliseconds(1);

template <typename T>
struct is_pair
{
  struct no {};
  static no func(...);
  struct yes {};
  template <typename U, typename V>
  static yes func(std::pair<U, V> const*);
  static constexpr bool value = std::is_same<yes, decltype(func(std::declval<T*>()))>::value;
};

template <typename T, typename ... V>
struct has_push
{
  struct no {};
  static no func(...);
  template <typename U>
  static auto func(U* u) -> decltype(u->push(std::declval<V>()...));
  static constexpr bool value = !std::is_same<no, decltype(func(std::declval<T*>()))>::value;
};

template <typename Q>
inline
cpp14::enable_if_t<is_pair<typename Q::value_type>::value>
add(Q& q, int n)
{
  q.push(typename Q::value_type(n, null_obj));
}

template <typename Q>
inline
cpp14::enable_if_t<!is_pair<typename Q::value_type>::value && has_push<Q, int>::value>
add(Q& q, int n)
{
  q.push(n);
}


template <typename Q>
inline
cpp14::enable_if_t<!is_pair<typename Q::value_type>::value && !has_push<Q, int>::value>
add(Q& q, int n)
{
  q.push(n, null_obj);
}

template <typename Q>
class populate
{
public:
  populate(uint64_t) { }
  void operator()(uint64_t size)
  {
    for (uint64_t i = 0; i != size; ++i)
    {
      add(q, n[i]);
    }
  }
private:
  Q q;
};

template <typename Q>
class pop_all
{
public:
  pop_all(std::size_t size)
  {
    for (uint64_t i = 0; i != size; ++i)
    {
      add(q, n[i]);
    }
  }
  void operator()(uint64_t size)
  {
    while (size--)
    {
      q.pop();
    }
  }
private:
  Q q;
};

template <typename Q, uint64_t delta_size, uint64_t num_cycles>
class operate
{
public:
  operate(std::size_t size)
  {
    for (uint64_t i = 0; i != size; ++i)
    {
      add(q, n[i]);
    }
  }
  void operator()(uint64_t size)
  {
    auto p                = n + size;
    auto remaining_cycles = num_cycles;
    while (remaining_cycles--)
    {
      auto elements_to_push = delta_size;
      while (elements_to_push--)
      {
        add(q, *p++);
      }
      auto elements_to_pop = delta_size;
      while (elements_to_pop--)
      {
        q.pop();
      }
    }
  }
private:
  Q q;
};


template <typename Q, uint64_t num_cycles>
class reschedule
{
public:
  reschedule(std::size_t size)
  {
    for (uint64_t i = 0; i != size; ++i)
    {
      add(q, n[i]);
    }
  }
  void operator()(uint64_t size)
  {
    auto p                = n + size;
    auto remaining_cycles = num_cycles;
    while (remaining_cycles--)
    {
      q.reschedule_top(*p++);
    }
  }
private:
  Q q;
};

template <typename Q, uint64_t num_cycles>
class pop_push
{
public:
  pop_push(std::size_t size)
  {
    for (uint64_t i = 0; i != size; ++i)
    {
      add(q, n[i]);
    }
  }
  void operator()(uint64_t size)
  {
    auto p                = n + size;
    auto remaining_cycles = num_cycles;
    while (remaining_cycles--)
    {
      q.pop();
      add(q, *p++);
    }
  }
private:
  Q q;
};

template <typename Q, uint64_t num_cycles>
class replace_top
{
public:
  replace_top(std::size_t size)
  {
    for (uint64_t i = 0; i != size; ++i)
    {
      add(q, n[i]);
    }
  }
  void operator()(uint64_t size)
  {
    auto p                = n + size;
    auto remaining_cycles = num_cycles;
    while (remaining_cycles--)
    {
      q.replace_top(*p++);
    }
  }
private:
  Q q;
};

inline
bool operator<(const std::pair<int, std::unique_ptr<int>> &lh,
               const std::pair<int, std::unique_ptr<int>> &rh)
{
  return lh.first < rh.first;
}

template <std::size_t size>
void measure_prio_queue(int argc, char *argv[])
{
  std::ostringstream os;
  os << "/tmp/q/" << size;
  std::string path = os.str();

  std::cout << path << '\n';

  CSV_reporter     reporter(path.c_str(), &std::cout);
  benchmark<Clock> benchmark(reporter);

  using qint = prio_queue<8, int, void>;
  using qintintp = prio_queue<size, std::pair<int, int>, void>;
  using qintptrp = prio_queue<size, std::pair<int, std::unique_ptr<int>>, void>;
  using qintp = prio_queue<size, int, std::unique_ptr<int>>;
  using qintint = prio_queue<size, int, int>;

  using std::to_string;

  benchmark.measure<populate<qint>>(test_sizes,
                                    "populate prio_queue" + toString(size)+"<int,void>",
                                    min_test_duration);
  benchmark.measure<pop_all<qint>>(test_sizes,
                                   "pop all prio_queue" + toString(size)+"<int,void>",
                                   min_test_duration);
  benchmark.measure<operate<qint, 320, 200>>(test_sizes,
                                           "operate prio_queue" + toString(size)+"<int,void>",
                                           min_test_duration);

  benchmark.measure<populate<qintintp>>(test_sizes,
                                        "populate prio_queue" + toString(size)+"<<int,int>, void>",
                                        min_test_duration);
  benchmark.measure<pop_all<qintintp>>(test_sizes,
                                       "pop all prio_queue" + toString(size)+"<<int,int>, void>",
                                       min_test_duration);
  benchmark.measure<operate<qintintp, 320, 200>>(test_sizes,
                                               "operate prio_queue" + toString(size)+"<<int,int>, void>",
                                               min_test_duration);

  benchmark.measure<populate<qintptrp>>(test_sizes,
                                        "populate prio_queue" + toString(size)+"<<int,ptr>, void>",
                                        min_test_duration);
  benchmark.measure<pop_all<qintptrp>>(test_sizes,
                                       "pop all prio_queue" + toString(size)+"<<int,ptr>, void>",
                                       min_test_duration);
  benchmark.measure<operate<qintptrp, 320, 200>>(test_sizes,
                                               "operate prio_queue" + toString(size)+"<<int,ptr>, void>",
                                               min_test_duration);


  benchmark.measure<populate<qintint>>(test_sizes,
                                       "populate prio_queue" + toString(size)+"<int,int>",
                                       min_test_duration);
  benchmark.measure<pop_all<qintint>>(test_sizes,
                                      "pop all prio_queue" + toString(size)+"<int,int>",
                                      min_test_duration);
  benchmark.measure<operate<qintint, 320, 200>>(test_sizes,
                                              "operate prio_queue" + toString(size)+"<int,int>",
                                              min_test_duration);

  benchmark.measure<reschedule<qintint, 1000>>(test_sizes,
                                               "reschedule prio_queue" + toString(size)+"<int,int>",
                                               min_test_duration);
  benchmark.measure<pop_push<qintint, 1000>>(test_sizes,
                                             "reschedule with pop/push prio_queue" + toString(size)+"<int,int>",
                                             min_test_duration);

  benchmark.measure<populate<qintp>>(test_sizes,
                                     "populate prio_queue" + toString(size)+"<int,ptr>",
                                     min_test_duration);
  benchmark.measure<pop_all<qintp>>(test_sizes,
                                    "pop all prio_queue" + toString(size)+"<int,ptr>",
                                    min_test_duration);
  benchmark.measure<operate<qintp, 320, 200>>(test_sizes,
                                            "operate prio_queue" + toString(size)+"<int,ptr>",
                                            min_test_duration);
  benchmark.run(argc, argv);
}

void measure_falconn(int argc, char *argv[])
{
  using qint = FalconnHeapMod2<int>;
  using qintintp = FalconnHeapMod1<int, int>;
  using qintptrp = FalconnHeapMod1<int, int64_t>; // Can't work with a unique_ptr (also problems if a pointer is used in this benchmark)

  CSV_reporter     reporter("/tmp/q/falconn", &std::cout);
  benchmark<Clock> benchmark(reporter);

  benchmark.measure<pop_push<qint, 1000>>(test_sizes,
                                           "falconn_mod2 pop/push",
                                           min_test_duration);
  benchmark.measure<replace_top<qint, 1000>>(test_sizes,
                                           "falconn_mod2 replace_top",
                                           min_test_duration);

  benchmark.measure<populate<qint>>(test_sizes,
                                    "populate falconn_mod2<int>",
                                    min_test_duration);
  benchmark.measure<pop_all<qint>>(test_sizes,
                                   "pop all falconn_mod2<int>",
                                   min_test_duration);
  benchmark.measure<operate<qint, 320, 200>>(test_sizes,
                                           "operate falconn_mod2<int>",
                                           min_test_duration);

  benchmark.measure<populate<qintintp>>(test_sizes,
                                    "populate falconn_mod1<<int,int>>",
                                    min_test_duration);
  benchmark.measure<pop_all<qintintp>>(test_sizes,
                                   "pop all falconn_mod1<<int,int>>",
                                   min_test_duration);
  benchmark.measure<operate<qintintp, 320, 200>>(test_sizes,
                                           "operate falconn_mod1<<int,int>>",
                                           min_test_duration);

  benchmark.measure<populate<qintptrp>>(test_sizes,
                                        "populate falconn_mod1<<int,int64>>",
                                        min_test_duration);
  benchmark.measure<pop_all<qintptrp>>(test_sizes,
                                       "pop all falconn_mod1<<int,int64>>",
                                       min_test_duration);
  benchmark.measure<operate<qintptrp, 320, 200>>(test_sizes,
                                               "operate falconn_mod1<<int,int64>>",
                                               min_test_duration);


  benchmark.run(argc, argv);
}

void measure_std(int argc, char *argv[])
{ 

  using qint = std::priority_queue<int>;
  using qintintp = std::priority_queue<std::pair<int, int>>;
  using qintptrp = std::priority_queue<std::pair<int, std::unique_ptr<int>>>;


  CSV_reporter     reporter("/tmp/q/std", &std::cout);
  benchmark<Clock> benchmark(reporter);

  benchmark.measure<pop_push<qint, 1000>>(test_sizes,
                                             "std pop/push",
                                             min_test_duration);

  benchmark.measure<populate<qint>>(test_sizes,
                                    "populate priority_queue<int>",
                                    min_test_duration);
  benchmark.measure<pop_all<qint>>(test_sizes,
                                   "pop all priority_queue<int>",
                                   min_test_duration);
  benchmark.measure<operate<qint, 320, 200>>(test_sizes,
                                           "operate priority_queue<int>",
                                           min_test_duration);


  benchmark.measure<populate<qintintp>>(test_sizes,
                                    "populate priority_queue<<int,int>>",
                                    min_test_duration);
  benchmark.measure<pop_all<qintintp>>(test_sizes,
                                   "pop all priority_queue<<int,int>>",
                                   min_test_duration);
  benchmark.measure<operate<qintintp, 320, 200>>(test_sizes,
                                           "operate priority_queue<<int,int>>",
                                           min_test_duration);

  benchmark.measure<populate<qintptrp>>(test_sizes,
                                        "populate priority_queue<<int,ptr>>",
                                        min_test_duration);
  benchmark.measure<pop_all<qintptrp>>(test_sizes,
                                       "pop all priority_queue<<int,ptr>>",
                                       min_test_duration);
  benchmark.measure<operate<qintptrp, 320, 200>>(test_sizes,
                                               "operate priority_queue<<int,ptr>>",
                                               min_test_duration);

  benchmark.run(argc, argv);
}

template <std::size_t fanout, std::size_t heap_size>
void measure_gheap(int argc, char *argv[])
{ 

  using qint =     gpriority_queue<gheap<fanout,heap_size>,int>;
  using qintintp = gpriority_queue<gheap<fanout,heap_size>,std::pair<int, int>>;
  using qintptrp = gpriority_queue<gheap<fanout,heap_size>,std::pair<int, int64_t>>; // Can't work with a unique_ptr (also problems if a pointer is used in this benchmark)


  CSV_reporter     reporter("/tmp/q/gheap", &std::cout);
  benchmark<Clock> benchmark(reporter);

  benchmark.measure<pop_push<qint, 1000>>(test_sizes,
                                             "gpriority_queue" + toString(fanout)+","+toString(heap_size)+" pop/push",
                                             min_test_duration);

  benchmark.measure<populate<qint>>(test_sizes,
                                    "populate gpriority_queue" + toString(fanout)+","+toString(heap_size)+"<int>",
                                    min_test_duration);
  benchmark.measure<pop_all<qint>>(test_sizes,
                                   "pop all gpriority_queue" + toString(fanout)+","+toString(heap_size)+"<int>",
                                   min_test_duration);
  benchmark.measure<operate<qint, 320, 200>>(test_sizes,
                                           "operate gpriority_queue" + toString(fanout)+","+toString(heap_size)+"<int>",
                                           min_test_duration);


  benchmark.measure<populate<qintintp>>(test_sizes,
                                    "populate gpriority_queue" + toString(fanout)+","+toString(heap_size)+"<<int,int>>",
                                    min_test_duration);
  benchmark.measure<pop_all<qintintp>>(test_sizes,
                                   "pop all gpriority_queue" + toString(fanout)+","+toString(heap_size)+"<<int,int>>",
                                   min_test_duration);
  benchmark.measure<operate<qintintp, 320, 200>>(test_sizes,
                                           "operate gpriority_queue" + toString(fanout)+","+toString(heap_size)+"<<int,int>>",
                                           min_test_duration);

  benchmark.measure<populate<qintptrp>>(test_sizes,
                                        "populate gpriority_queue" + toString(fanout)+","+toString(heap_size)+"<<int,int64>>",
                                        min_test_duration);
  benchmark.measure<pop_all<qintptrp>>(test_sizes,
                                       "pop all gpriority_queue" + toString(fanout)+","+toString(heap_size)+"<<int,int64>>",
                                       min_test_duration);
  benchmark.measure<operate<qintptrp, 320, 200>>(test_sizes,
                                               "operate priority_queue" + toString(fanout)+","+toString(heap_size)+"<<int,int64>>",
                                               min_test_duration);

  benchmark.run(argc, argv);
}

int main(int argc, char *argv[])
{
  std::random_device              rd;
  std::mt19937                    gen(rd());
  std::uniform_int_distribution<> dist(1, 10000000);

  size_t maxQty = *std::max_element(test_sizes.begin(), test_sizes.end()); 
  nvect.resize(maxQty + 1);

  for (auto& i : nvect) i = dist(gen);
  n = &nvect[0];

  std::cout << sizeof(int) << ' '
      << sizeof(std::pair<int, std::unique_ptr<int>>) << '\n';

#if 0
  measure_prio_queue<8>(argc, argv);
  measure_prio_queue<16>(argc, argv);
  measure_prio_queue<32>(argc, argv);
#endif
  measure_prio_queue<64>(argc, argv);

  measure_gheap<2,1>(argc, argv);
  measure_gheap<4,1>(argc, argv);
  measure_gheap<8,1>(argc, argv);
  measure_gheap<2,32>(argc, argv);
  measure_gheap<4,32>(argc, argv);
  measure_gheap<8,32>(argc, argv);
  measure_gheap<2,1024>(argc, argv);
  measure_gheap<4,1024>(argc, argv);
  measure_gheap<8,1024>(argc, argv);

  measure_falconn(argc, argv);
  measure_std(argc, argv);
}

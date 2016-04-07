/*
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

#include <queue>
#include <memory>
#include <random>
#include <algorithm>

#include "bunit.h"

using A = rollbear::prio_q_internal::heap_heap_addressing<8>;
using V = rollbear::prio_q_internal::skip_vector<int, 4>;
using rollbear::prio_queue;

TEST(constr_empty, "a default constructed vector is empty", "[vector]")
{
  V v;
  EXPECT_TRUE(v.size() == 0);
  EXPECT_TRUE(v.empty());
}

TEST(size2_after_push, "a has size 2 after one push_key", "[vector]")
{
  V v;
  auto i = v.push_back(1);
  EXPECT_TRUE(!v.empty());
  EXPECT_TRUE(v.size() == 2);
  EXPECT_TRUE(i == 1);
}

TEST(size2_becomes_empty_on_pop, "a vector of size 2 becomes empty on pop", "[vector]")
{
  V v;
  v.push_back(1);
  v.pop_back();
  EXPECT_TRUE(v.empty());
  EXPECT_TRUE(v.size() == 0);
}

TEST(push_key_indexes_skip_mult_of_4, "push_key indexes skip multiples of 4", "[vector]")
{
  V v;
  EXPECT_TRUE(v.push_back(1) == 1);
  EXPECT_TRUE(v.push_back(1) == 2);
  EXPECT_TRUE(v.push_back(1) == 3);
  EXPECT_TRUE(v.push_back(1) == 5);
  EXPECT_TRUE(v.push_back(1) == 6);
  EXPECT_TRUE(v.push_back(1) == 7);
  EXPECT_TRUE(v.push_back(1) == 9);
}

TEST(back_ref_last_elem_through_push_and_pop, "back refers to last element through push_key and pop", "[vector]")
{
  V v;
  v.push_back(21);
  EXPECT_TRUE(v.back() == 21);
  v.push_back(20);
  EXPECT_TRUE(v.back() == 20);
  v.push_back(19);
  EXPECT_TRUE(v.back() == 19);
  v.push_back(18);
  EXPECT_TRUE(v.back() == 18);
  v.push_back(17);
  EXPECT_TRUE(v.back() == 17);
  v.pop_back();
  EXPECT_TRUE(v.back() == 18);
  v.pop_back();
  EXPECT_TRUE(v.back() == 19);
  v.pop_back();
  EXPECT_TRUE(v.back() == 20);
  v.pop_back();
  EXPECT_TRUE(v.back() == 21);
  v.pop_back();
  EXPECT_TRUE(v.empty());

}
TEST(block_root, "block root", "[addressing]")
{
  EXPECT_TRUE(A::is_block_root(1));
  EXPECT_TRUE(A::is_block_root(9));
  EXPECT_TRUE(A::is_block_root(17));
  EXPECT_TRUE(A::is_block_root(73));
  EXPECT_TRUE(!A::is_block_root(2));
  EXPECT_TRUE(!A::is_block_root(3));
  EXPECT_TRUE(!A::is_block_root(4));
  EXPECT_TRUE(!A::is_block_root(7));
  EXPECT_TRUE(!A::is_block_root(31));
}

TEST(block_leaf, "block leaf", "[addressing]")
{
  EXPECT_TRUE(!A::is_block_leaf(1));
  EXPECT_TRUE(!A::is_block_leaf(2));
  EXPECT_TRUE(!A::is_block_leaf(3));
  EXPECT_TRUE(A::is_block_leaf(4));
  EXPECT_TRUE(A::is_block_leaf(5));
  EXPECT_TRUE(A::is_block_leaf(6));
  EXPECT_TRUE(A::is_block_leaf(7));
  EXPECT_TRUE(A::is_block_leaf(28));
  EXPECT_TRUE(A::is_block_leaf(29));
  EXPECT_TRUE(A::is_block_leaf(30));
  EXPECT_TRUE(!A::is_block_leaf(257));
  EXPECT_TRUE(A::is_block_leaf(255));
}
TEST(obtaining_child, "Obtaining child", "[addressing]")
{
  EXPECT_TRUE(A::child_of(1) == 2);
  EXPECT_TRUE(A::child_of(2) == 4);
  EXPECT_TRUE(A::child_of(3) == 6);
  EXPECT_TRUE(A::child_of(4) == 9);
  EXPECT_TRUE(A::child_of(31) == 249);
}

TEST(obtaining_parent, "Obtaining parent", "[addressing]")
{
  EXPECT_TRUE(A::parent_of(2) == 1);
  EXPECT_TRUE(A::parent_of(3) == 1);
  EXPECT_TRUE(A::parent_of(6) == 3);
  EXPECT_TRUE(A::parent_of(7) == 3);
  EXPECT_TRUE(A::parent_of(9) == 4);
  EXPECT_TRUE(A::parent_of(17) == 4);
  EXPECT_TRUE(A::parent_of(33) == 5);
  EXPECT_TRUE(A::parent_of(29) == 26);
  EXPECT_TRUE(A::parent_of(1097) == 140);
}

TEST(default_constructed_queue_is_empty, "a default constructed queue is empty", "[empty]")
{
  prio_queue<16, int, void> q;
  EXPECT_TRUE(q.empty());
  EXPECT_TRUE(q.size() == 0);
}

TEST(queue_isnot_empty_after_one_insert, "an empty queue is not empty when one element is inserted", "[empty]")
{
  prio_queue<16, int, void> q;
  q.push(1);
  EXPECT_TRUE(!q.empty());
  EXPECT_TRUE(q.size() == 1);
}

TEST(queue_with_one_elem_has_top, "a queue with one element has it on top", "[single element]")
{
  prio_queue<16, int, void> q;
  q.push(8);
  EXPECT_TRUE(q.top() == 8);
}

TEST(queue_with_one_elem_empty_after_pop, "a queue with one element becomes empty when popped",
          "[single element],[empty]")
{
  prio_queue<16, int, void> q;
  q.push(9);
  q.pop();
  EXPECT_TRUE(q.empty());
  EXPECT_TRUE(q.size() == 0);
}

TEST(insert_sorted_stays_sorted, "insert sorted stays sorted", "[dead]")
{
  prio_queue<16, int, void> q;
  q.push(1);
  q.push(2);
  q.push(3);
  q.push(4);
  q.push(5);
  q.push(6);
  q.push(7);
  q.push(8);
  EXPECT_TRUE(q.top() == 1);
  q.pop();
  EXPECT_TRUE(q.top() == 2);
  q.pop();
  EXPECT_TRUE(q.top() == 3);
  q.pop();
  EXPECT_TRUE(q.top() == 4);
  q.pop();
  EXPECT_TRUE(q.top() == 5);
  q.pop();
  EXPECT_TRUE(q.top() == 6);
  q.pop();
  EXPECT_TRUE(q.top() == 7);
  q.pop();
  EXPECT_TRUE(q.top() == 8);
  q.pop();
  EXPECT_TRUE(q.empty());
}

TEST(key_val_pairs_synced, "key value pairs go in tandem", "[nontrivial]")
{
  struct P
  {
    int a;
    int b;
    bool operator==(const std::pair<int const&, int&>& rh) const
    {
      return a == rh.first && b == rh.second;
    }
  };
  prio_queue<16, int, int> q;
  q.push(3, -3);
  q.push(4, -4);
  q.push(8, -8);
  q.push(1, -1);
  q.push(22, -22);
  q.push(23, -23);
  q.push(16, -16);
  q.push(9, -9);
  q.push(25, -25);
  q.push(20, -20);
  q.push(10, -10);
  q.push(5, -5);
  q.push(11, -11);
  q.push(12, -12);
  q.push(19, -19);
  q.push(2, -2);

  EXPECT_TRUE((P{1, -1}) == q.top());
  q.pop();
  EXPECT_TRUE((P{2, -2}) == q.top());
  q.pop();
  EXPECT_TRUE((P{3, -3}) == q.top());
  q.pop();
  EXPECT_TRUE((P{4, -4}) == q.top());
  q.pop();
  EXPECT_TRUE((P{5, -5}) == q.top());
  q.pop();
  EXPECT_TRUE((P{8, -8}) == q.top());
  q.pop();
  EXPECT_TRUE((P{9, -9}) == q.top());
  q.pop();
  EXPECT_TRUE((P{10, -10}) == q.top());
  q.pop();
  EXPECT_TRUE((P{11, -11}) == q.top());
  q.pop();
  EXPECT_TRUE((P{12, -12}) == q.top());
  q.pop();
  EXPECT_TRUE((P{16, -16}) == q.top());
  q.pop();
  EXPECT_TRUE((P{19, -19}) == q.top());
  q.pop();
  EXPECT_TRUE((P{20, -20}) == q.top());
  q.pop();
  EXPECT_TRUE((P{22, -22}) == q.top());
  q.pop();
  EXPECT_TRUE((P{23, -23}) == q.top());
  q.pop();
  EXPECT_TRUE((P{25, -25}) == q.top());
  q.pop();
  EXPECT_TRUE(q.empty());

}

TEST(key_val_pairs_can_have_complex_value_type, "key value pairs can have complex value type", "[nontrivial]")
{
  prio_queue<16, int, std::unique_ptr<int>> q;
  q.push(2, nullptr);
  q.push(1, nullptr);
  EXPECT_TRUE(q.top().first == 1);
  q.pop();
  EXPECT_TRUE(q.top().first == 2);
  q.pop();
  EXPECT_TRUE(q.empty());
}
TEST(randomly_inserted_popped_sorted, "randomly inserted elements are popped sorted", "heap")
{
  prio_queue<16, int, void> q;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(1,100000);
  int n[36000];
  for (auto& i : n)
  {
    i = dist(gen);
    q.push(i);
  }

  EXPECT_TRUE(!q.empty());
  EXPECT_TRUE(q.size() == 36000);
  std::sort(std::begin(n), std::end(n));
  for (auto i : n)
  {
    EXPECT_TRUE(q.top() == i);
    q.pop();
  }
  EXPECT_TRUE(q.empty());
}

TEST(reschedule_top_highest_prio_leaves_order_unchanged, "reschedule top with highest prio leaves order unchanged", "heap")
{
  prio_queue<4, int, int*> q;
  //              0  1   2   3  4   5  6   7   8
  int nums[] = { 32, 1, 88, 16, 9, 11, 3, 22, 23 };
  for (auto& i : nums) q.push(i, &i);
  EXPECT_TRUE(q.top().first == 1);
  EXPECT_TRUE(q.top().second == &nums[1]);
  EXPECT_TRUE(*q.top().second == 1);

  q.reschedule_top(2);

  EXPECT_TRUE(q.top().first == 2);
  EXPECT_TRUE(q.top().second == &nums[1]);
  q.pop();
  EXPECT_TRUE(q.top().first == 3);
  EXPECT_TRUE(q.top().second == &nums[6]);
  q.pop();
  EXPECT_TRUE(q.top().first == 9);
  EXPECT_TRUE(q.top().second == &nums[4]);
  q.pop();
  EXPECT_TRUE(q.top().first == 11);
  EXPECT_TRUE(q.top().second == &nums[5]);
  q.pop();
  EXPECT_TRUE(q.top().first == 16);
  EXPECT_TRUE(q.top().second == &nums[3]);
  q.pop();
  EXPECT_TRUE(q.top().first == 22);
  EXPECT_TRUE(q.top().second == &nums[7]);
  q.pop();
  EXPECT_TRUE(q.top().first == 23);
  EXPECT_TRUE(q.top().second == &nums[8]);
  q.pop();
  EXPECT_TRUE(q.top().first == 32);
  EXPECT_TRUE(q.top().second == &nums[0]);
  q.pop();
  EXPECT_TRUE(q.top().first == 88);
  EXPECT_TRUE(q.top().second == &nums[2]);
  q.pop();
  EXPECT_TRUE(q.empty());
}

TEST(reschedule_to_mid_range_moves_element_to_correct_place, "reschedule to mid range moves element to correct place", "heap")
{
  prio_queue<4, int, int*> q;
  //              0  1   2   3  4   5  6   7   8
  int nums[] = { 32, 1, 88, 16, 9, 11, 3, 22, 23 };
  for (auto& i : nums) q.push(i, &i);
  EXPECT_TRUE(q.top().first == 1);
  EXPECT_TRUE(q.top().second == &nums[1]);
  EXPECT_TRUE(*q.top().second == 1);

  q.reschedule_top(12);

  EXPECT_TRUE(q.top().first == 3);
  EXPECT_TRUE(q.top().second == &nums[6]);
  q.pop();
  EXPECT_TRUE(q.top().first == 9);
  EXPECT_TRUE(q.top().second == &nums[4]);
  q.pop();
  EXPECT_TRUE(q.top().first == 11);
  EXPECT_TRUE(q.top().second == &nums[5]);
  q.pop();
  EXPECT_TRUE(q.top().first == 12);
  EXPECT_TRUE(q.top().second == &nums[1]);
  q.pop();
  EXPECT_TRUE(q.top().first == 16);
  EXPECT_TRUE(q.top().second == &nums[3]);
  q.pop();
  EXPECT_TRUE(q.top().first == 22);
  EXPECT_TRUE(q.top().second == &nums[7]);
  q.pop();
  EXPECT_TRUE(q.top().first == 23);
  EXPECT_TRUE(q.top().second == &nums[8]);
  q.pop();
  EXPECT_TRUE(q.top().first == 32);
  EXPECT_TRUE(q.top().second == &nums[0]);
  q.pop();
  EXPECT_TRUE(q.top().first == 88);
  EXPECT_TRUE(q.top().second == &nums[2]);
  q.pop();
  EXPECT_TRUE(q.empty());
}

TEST(reschedule_to_last_moves_element_to_correct_place, "reschedule to last moves element to correct place", "heap")
{
  prio_queue<4, int, int*> q;
  //              0  1   2   3  4   5  6   7   8
  int nums[] = { 32, 1, 88, 16, 9, 11, 3, 22, 23 };
  for (auto& i : nums) q.push(i, &i);
  EXPECT_TRUE(q.top().first == 1);
  EXPECT_TRUE(q.top().second == &nums[1]);
  EXPECT_TRUE(*q.top().second == 1);

  q.reschedule_top(89);

  EXPECT_TRUE(q.top().first == 3);
  EXPECT_TRUE(q.top().second == &nums[6]);
  q.pop();
  EXPECT_TRUE(q.top().first == 9);
  EXPECT_TRUE(q.top().second == &nums[4]);
  q.pop();
  EXPECT_TRUE(q.top().first == 11);
  EXPECT_TRUE(q.top().second == &nums[5]);
  q.pop();
  EXPECT_TRUE(q.top().first == 16);
  EXPECT_TRUE(q.top().second == &nums[3]);
  q.pop();
  EXPECT_TRUE(q.top().first == 22);
  EXPECT_TRUE(q.top().second == &nums[7]);
  q.pop();
  EXPECT_TRUE(q.top().first == 23);
  EXPECT_TRUE(q.top().second == &nums[8]);
  q.pop();
  EXPECT_TRUE(q.top().first == 32);
  EXPECT_TRUE(q.top().second == &nums[0]);
  q.pop();
  EXPECT_TRUE(q.top().first == 88);
  EXPECT_TRUE(q.top().second == &nums[2]);
  q.pop();
  EXPECT_TRUE(q.top().first == 89);
  EXPECT_TRUE(q.top().second == &nums[1]);
  q.pop();
  EXPECT_TRUE(q.empty());
}

TEST(reschedule_top_of_2_elements_to_last, "reschedule top of 2 elements to last", "[heap]")
{
  prio_queue<8, int, void> q;
  q.push(1);
  q.push(2);
  EXPECT_TRUE(q.top() == 1);
  q.reschedule_top(3);
  EXPECT_TRUE(q.top() == 2);
}

TEST(reschedule_top_of_3_elements_left_to_2nd, "reschedule top of 3 elements left to 2nd", "[heap]")
{
  prio_queue<8, int, void> q;
  q.push(1);
  q.push(2);
  q.push(4);
  EXPECT_TRUE(q.top() == 1);
  q.reschedule_top(3);
  EXPECT_TRUE(q.top() == 2);
}

TEST(reschedule_top_of_3_elements_right_to_2nd, "reschedule top of 3 elements right to 2nd", "[heap]")
{
  prio_queue<8, int, void> q;
  q.push(1);
  q.push(4);
  q.push(2);
  EXPECT_TRUE(q.top() == 1);
  q.reschedule_top(3);
  EXPECT_TRUE(q.top() == 2);
}


TEST(reschedule_top_random_gives_same_results_pop_push, "reschedule top random gives same results pop/push", "[heap]")
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<unsigned> dist(1,100000);

  prio_queue<8, unsigned, void> pq;
  std::priority_queue<unsigned, std::vector<unsigned>, std::greater<unsigned>> stdq;

  for (size_t outer = 0; outer < 100U; ++outer)
  {
    unsigned num = gen();
    pq.push(num);
    stdq.push(num);
    for (size_t inner = 0; inner < 100; ++inner)
    {
      unsigned newval = gen();
      pq.reschedule_top(newval);
      stdq.pop();
      stdq.push(newval);
      auto n = pq.top();
      auto sn = stdq.top();
      EXPECT_TRUE(sn == n);
    }
  }
}

struct ptr_cmp
{
  template <typename T>
  bool operator()(T const& lh, T const& rh) const { return *lh < *rh;}
};

TEST(unique_ptrs_are_sorted_with_custom_compare, "unique ptrs are sorted with custom compare", "[nontrivial]")
{
  prio_queue<8, std::unique_ptr<int>, void, ptr_cmp> q;
  for (int i = 255; i >= 0; --i)
  {
    q.push(std::unique_ptr<int>(new int(i)));
  }

  for (int i = 0; i < 256; ++i)
  {
    EXPECT_TRUE(*q.top() == i);
    q.pop();
  }
  EXPECT_TRUE(q.empty());
}

unsigned obj_count;
unsigned copy_count;
unsigned move_throw_count;
unsigned copy_throw_count;
struct traced_throwing_move
{
  traced_throwing_move(int n_) : n{n_} { ++obj_count; }
  traced_throwing_move(const traced_throwing_move& rh) :n{rh.n} { if (--copy_throw_count == 0) throw 2; ++obj_count; ++copy_count; }
  traced_throwing_move(traced_throwing_move&& rh) : n{rh.n} { if (--move_throw_count == 0) throw 3; ++obj_count; rh.n = -rh.n;}
  ~traced_throwing_move() { --obj_count;}
  traced_throwing_move& operator=(const traced_throwing_move& rh) { n = rh.n;return *this; }
  int n;
  bool operator<(const traced_throwing_move& rh) const { return n < rh.n;}
};

TEST(throwable_move_ctor_causes_copies, "throwable move ctor causes copies", "[nontrivial]")
{
  obj_count = 0;
  move_throw_count = 0;
  copy_throw_count = 0;
  copy_count = 0;
  {
    prio_queue<16, traced_throwing_move, void> q;
    for (int i = 0; i < 15*16; ++i)
    {
      q.push(500 - i);
    }
    EXPECT_TRUE(obj_count == 15*16);
    EXPECT_TRUE(copy_count == 0);
    q.push(100);
    EXPECT_TRUE(obj_count == 15*16 + 1);
    EXPECT_TRUE(copy_count == 15*16);
  }

  EXPECT_TRUE(obj_count == 0);
  EXPECT_TRUE(copy_count == 15*16);
}

TEST(throwing_move_allows_safe_destruction, "throwing move allows safe destruction", "[nontrivial")
{
  obj_count = 0;
  move_throw_count = 18;
  copy_throw_count = 0;
  copy_count = 0;
  bool too_soon = true;
  try {
    prio_queue<16, traced_throwing_move, void> q;
    for (int i = 0; i < 17; ++i)
    {
      q.push(i);
    }
    too_soon = false;
    q.push(18);
    FAIL("didn't throw");
  }
  catch (int)
  {
  }
  EXPECT_FALSE(too_soon);
  EXPECT_TRUE(obj_count == 0);
}

TEST(throwing_copy_allows_safe_destruction, "throwing copy allows safe destruction", "[nontrivial]")
{
  obj_count = 0;
  move_throw_count = 0;
  copy_throw_count = 15*15;
  copy_count = 0;
  std::cout << "begin\n";
  bool too_soon = true;
  try {
    prio_queue<16, traced_throwing_move, void> q;
    for (int i = 0; i < 15*16; ++i)
    {
      q.push(i);
    }
    too_soon = false;
    q.push(1000);
    FAIL("didn't throw");
  }
  catch (int)
  {
  }
  EXPECT_FALSE(too_soon);
  EXPECT_TRUE(obj_count == 0);
}

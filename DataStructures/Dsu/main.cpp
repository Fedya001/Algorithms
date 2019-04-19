#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <assert.h>
#include <random>
#include <chrono>

class Dsu {
 public:
  explicit Dsu(size_t set_count) : rank_(set_count, 0), parent_(set_count) {
    std::iota(parent_.begin(), parent_.end(), 0);
  }

  size_t Find(size_t vertex) {
    assert(vertex < parent_.size());

    if (vertex != parent_[vertex]) {
      parent_[vertex] = Find(parent_[vertex]);
    }
    return parent_[vertex];
  }

  void Union(size_t first, size_t second) {
    assert(first < parent_.size());
    assert(second < parent_.size());

    size_t first_id = Find(first),
        second_id = Find(second);
    if (first_id == second_id) {
      return;
    }
    if (rank_[first_id] < rank_[second_id]) {
      parent_[first_id] = second_id;
    } else {
      parent_[second_id] = first_id;
      if (rank_[first_id] == rank_[second_id]) {
        ++rank_[first_id];
      }
    }
  }

 private:
  std::vector<int> rank_;
  std::vector<size_t> parent_;
};

void CheckEquality(Dsu& dsu, const std::vector<size_t>& ids) {
  for (size_t i = 0; i < ids.size(); ++i) {
    for (size_t j = 0; j < ids.size(); ++j) {
      assert((dsu.Find(i) == dsu.Find(j)) == (ids[i] == ids[j]));
    }
  }
}

int main() {
  const long long seed =
      std::chrono::duration_cast<std::chrono::microseconds>
          (std::chrono::system_clock::now().time_since_epoch()).count();
  std::cerr << "seed = " << seed << std::endl;
  const int unit_test_number = 200;
  const int set_count = 500;

  std::vector<size_t> ids(set_count);
  std::iota(ids.begin(), ids.end(), 0);

  Dsu dsu(500);

  std::mt19937 random_gen(seed);
  std::uniform_int_distribution<> vertex_gen(0, set_count - 1);

  size_t first, second;
  size_t first_id, second_id;
  CheckEquality(dsu, ids);
  for (int i = 0; i < unit_test_number; ++i) {
    first = static_cast<size_t>(vertex_gen(random_gen));
    second = static_cast<size_t>(vertex_gen(random_gen));

    dsu.Union(first, second);
    first_id = ids[first];
    second_id = ids[second];
    std::for_each(ids.begin(), ids.end(),
        [&first_id, &second_id](auto& item_id) {
      if (item_id == second_id) {
        item_id = first_id;
      }
    });

    CheckEquality(dsu, ids);
  }
  std::cerr << "All tests successfully passed" << std::endl;

  return 0;
}
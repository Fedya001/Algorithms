#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <stack>
#include <set>
#include <random>
#include <functional>
#include <chrono>

template<class T>
class SmartStack {
 public:
  explicit SmartStack(std::function<T(const T&, const T&)> get_best_func) :
      get_best_func_(std::move(get_best_func)) {}

  void Push(const T& item) {
    data_.push(item);
    if (best_.empty()) {
      best_.push(item);
    } else {
      best_.push(get_best_func_(best_.top(), item));
    }
  }

  T Pop() {
    assert(!Empty());
    int top = data_.top();
    data_.pop();
    best_.pop();
    return top;
  }

  T GetBest() const {
    assert(!Empty());
    return best_.top();
  }

  size_t Size() const {
    return data_.size();
  }

  bool Empty() const {
    return data_.empty();
  }
 private:
  std::stack<T> data_;
  std::stack<T> best_;
  std::function<T(const T&, const T&)> get_best_func_;
};

template<class T>
class SmartQueue {
 public:
  SmartQueue(std::function<T(const T&, const T&)> get_best_func) :
      push_(std::move(get_best_func)), pull_(std::move(get_best_func)),
      get_best_func_(std::move(get_best_func)) {}

  void Push(const T& item) {
    push_.Push(item);
  }

  T Pop() {
    assert(!Empty());
    if (pull_.Empty()) {
      while (!push_.Empty()) {
        pull_.Push(push_.Pop());
      }
    }
    return pull_.Pop();
  }

  T GetBest() const {
    assert(!Empty());
    return get_best_func_(push_.GetBest(), pull_.GetBest());
  }

  size_t Size() const {
    return push_.Size() + pull_.Size();
  }

  bool Empty() const {
    return push_.Empty() || pull_.Empty();
  }

 private:
  SmartStack<T> push_;
  SmartStack<T> pull_;
  std::function<T(const T&, const T&)> get_best_func_;
};

enum class Operation {
  ADD, DELETE
};

int main() {
  const int test_number = 10'000;
  const int initial_numbers = 5'000;
  const long long seed =
      std::chrono::duration_cast<std::chrono::microseconds>
          (std::chrono::system_clock::now().time_since_epoch()).count();
  std::cerr << "seed = " << seed << std::endl;

  std::mt19937 random_generator(seed);
  std::uniform_int_distribution<> option_unif(0, 1);
  std::uniform_int_distribution<> number_unif
      (-2'000'000'000, 2'000'000'000);

  SmartQueue<int> queue(std::min<int>);
  std::set<int> status;

  for (int i = 0; i < initial_numbers; ++i) {
    int number = number_unif(random_generator);
    status.insert(number);
    queue.Push(number);
  }

  for (int i = 0; i < test_number; ++i) {
    Operation operation =
        static_cast<Operation>(option_unif(random_generator));
    switch (operation) {
      case Operation::ADD: {
        int number = number_unif(random_generator);
        status.insert(number);
        queue.Push(number);
        break;
      }
      case Operation::DELETE: {
        if (!queue.Empty()) {
          status.erase(queue.Pop());
        }
        break;
      }
    }
    assert(status.size() == queue.Size());

    if (!queue.Empty()) {
      assert(*status.begin() == queue.GetBest());
    }
  }
  std::cerr << "Tests passed successfully!";

  return 0;
}
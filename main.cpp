#include <bitset>
#include <iostream>
#include <queue>
#include <set>
#include <unordered_set>

using std::cin, std::cout, std::endl;

template <size_t VariablesCount>
class ConjuctiveNormalForm {
 public:
  using DisjunctionT = std::bitset<2 * VariablesCount>;

 protected:
  std::unordered_set<DisjunctionT> disjunctions_;

  struct Variable {
    size_t index;
    bool is_negated;

    Variable operator~() const { return {index, !is_negated}; }

    operator DisjunctionT() const {
      DisjunctionT disjunction;
      disjunction[index * 2 + (is_negated ? 1 : 0)] = true;

      return disjunction;
    }
  };

  struct VariablesList {
    Variable operator[](size_t index) const { return {index, false}; }
  };

 public:
  ConjuctiveNormalForm() = default;

  void AddDisjunction(const DisjunctionT& disjunction) {
    disjunctions_.insert(disjunction);
  }

  const std::unordered_set<DisjunctionT>& GetDisjunctions() const {
    return disjunctions_;
  }

  static VariablesList GetVariables() { return {}; }

  friend DisjunctionT operator|(const Variable& first, const Variable& second) {
    return static_cast<DisjunctionT>(first) | static_cast<DisjunctionT>(second);
  }

  friend DisjunctionT operator|(const DisjunctionT& first,
                                const Variable& second) {
    return first | static_cast<DisjunctionT>(second);
  }

  friend DisjunctionT operator|(const Variable& first,
                                const DisjunctionT& second) {
    return static_cast<DisjunctionT>(first) | second;
  }
};

template <size_t VariablesCount>
void PrettyPrintDisjunction(const std::bitset<2 * VariablesCount>& disjunct) {
  bool first_variable = true;

  for (size_t i = 0; i < 2 * VariablesCount; i += 2) {
    if (!disjunct[i] && !disjunct[i + 1]) {
      continue;
    }

    if (!first_variable) {
      cout << " | ";
    }

    if (disjunct[i + 1]) {
      cout << "~";
    }

    cout << i / 2;
    first_variable = false;
  }

  if (first_variable) {
    cout << "false";
  }
}

template <size_t VariablesCount>
class Resolver {
 protected:
  using DisjunctionT =
      typename ConjuctiveNormalForm<VariablesCount>::DisjunctionT;

  struct DisjunctionPair {
    size_t resolved_size;
    DisjunctionT resolution_result;

    DisjunctionPair(const DisjunctionT& disjunction)
        : resolved_size(disjunction.count()), resolution_result(disjunction) {}

    bool operator<(const DisjunctionPair& other) const {
      if (resolved_size == other.resolved_size) {
        return resolution_result != other.resolution_result;
      }

      return resolved_size < other.resolved_size;
    }

    auto operator<=>(const DisjunctionPair& other) const {
      return resolved_size <=> other.resolved_size;
    }

    auto operator==(const DisjunctionPair& other) const {
      return resolved_size == other.resolved_size &&
             resolution_result == other.resolution_result;
    }
  };

  std::set<DisjunctionPair> queue_;
  std::unordered_set<DisjunctionT> disjunctions_;

  void AddPairToQueue(const DisjunctionT& first, const DisjunctionT& second) {
    std::bitset resolved = first | second;

    size_t opposite_count = 0;
    size_t opposite_bit_index = 0;
    for (size_t i = 0; i < VariablesCount * 2; i += 2) {
      // both p and ~p in resolved formula
      if (resolved[i] && resolved[i + 1]) {
        opposite_bit_index = i;
        ++opposite_count;
      }
      if (opposite_count >= 2) {
        return;
      }
    }

    if (opposite_count != 1) {
      return;
    }

    resolved[opposite_bit_index] = false;
    resolved[opposite_bit_index + 1] = false;

    if (disjunctions_.contains(resolved)) {
      return;
    }

    queue_.emplace(resolved);
  }

  void FillQueue() {
    for (auto first_itr = disjunctions_.begin();
         first_itr != disjunctions_.end(); ++first_itr) {
      for (auto second_itr = std::next(first_itr);
           second_itr != disjunctions_.end(); ++second_itr) {
        AddPairToQueue(*first_itr, *second_itr);
      }
    }
  }

 public:
  Resolver(const std::unordered_set<DisjunctionT>& disjunctions)
      : disjunctions_(disjunctions) {}

  Resolver(const ConjuctiveNormalForm<VariablesCount>& formula)
      : disjunctions_(formula.GetDisjunctions()) {}

  bool IsSatisfiable() {
    FillQueue();

    while (!queue_.empty()) {
      DisjunctionPair shortest = *queue_.begin();
      queue_.erase(queue_.begin());

      // if we obtained empty conjunction
      if (shortest.resolved_size == 0) {
        return false;
      }

      for (auto itr = disjunctions_.begin(); itr != disjunctions_.end();
           ++itr) {
        AddPairToQueue(shortest.resolution_result, *itr);
      }

      disjunctions_.insert(shortest.resolution_result);
    }

    return true;
  }
};

int main() {
  ConjuctiveNormalForm<15> formula;

  auto variables = formula.GetVariables();


  Resolver resolver(formula);
  std::cout << resolver.IsSatisfiable() << std::endl;
}
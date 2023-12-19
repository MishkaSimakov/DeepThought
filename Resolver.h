#ifndef RESOLVER_H
#define RESOLVER_H

#include <unordered_set>

#include "ConjunctiveNormalForm.h"

template <size_t VariablesCount>
class Resolver {
  static constexpr std::bitset<VariablesCount * 2> kEmptyDisjunct = 0;

  static_assert(2 * VariablesCount <= sizeof(unsigned long long) * 8);

  using DisjunctionT =
      typename ConjunctiveNormalForm<VariablesCount>::DisjunctionT;

  struct DisjunctionHash {
    size_t operator()(const DisjunctionT& disjunction) const {
      return std::hash<unsigned long long>{}(disjunction.to_ullong());
    }
  };

  std::unordered_set<DisjunctionT, DisjunctionHash> queue_;
  std::unordered_set<DisjunctionT> disjunctions_;

  void AddPairToQueue(const DisjunctionT& first, const DisjunctionT& second) {
    DisjunctionT resolved;

    bool is_correct = Resolver::Resolve(first, second, resolved);

    if (!is_correct) {
      return;
    }

    if (disjunctions_.contains(resolved)) {
      return;
    }

    queue_.emplace(resolved);
  }

  void FillQueue() {
    for (auto& disjunction : disjunctions_) {
      queue_.emplace(disjunction);
    }
  }

 public:
  Resolver(const std::unordered_set<DisjunctionT>& disjunctions)
      : disjunctions_(disjunctions) {}

  Resolver(const ConjunctiveNormalForm<VariablesCount>& formula)
      : disjunctions_(formula.GetDisjunctions()) {}

  static bool Resolve(const DisjunctionT& first, const DisjunctionT& second,
                      DisjunctionT& resolved) {
    resolved = first | second;

    size_t opposite_count = 0;
    size_t opposite_bit_index = 0;

    for (size_t i = 0; i < VariablesCount * 2; i += 2) {
      // both p and ~p in resolved formula
      if (resolved[i] && resolved[i + 1]) {
        opposite_bit_index = i;
        ++opposite_count;
      }
      if (opposite_count >= 2) {
        return false;
      }
    }

    if (opposite_count != 1) {
      return false;
    }

    resolved[opposite_bit_index] = false;
    resolved[opposite_bit_index + 1] = false;

    return true;
  }

  bool IsSatisfiable() {
    FillQueue();

    while (!queue_.empty() && !queue_.contains(kEmptyDisjunct)) {
      auto current = queue_.begin();
      DisjunctionT resolved = *current;
      queue_.erase(current);

      // if we obtained empty conjunction
      if (resolved == 0) {
        return false;
      }

      for (auto& disjunction : disjunctions_) {
        AddPairToQueue(resolved, disjunction);
      }

      disjunctions_.insert(resolved);
    }

    return queue_.empty();
  }
};

#endif  // RESOLVER_H
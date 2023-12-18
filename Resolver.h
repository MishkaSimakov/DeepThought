#ifndef RESOLVER_H
#define RESOLVER_H

#include <set>
#include <unordered_set>

#include "ConjunctiveNormalForm.h"

template <size_t VariablesCount>
class Resolver {
  static_assert(2 * VariablesCount <= sizeof(unsigned long long) * 8);

  using DisjunctionT =
      typename ConjunctiveNormalForm<VariablesCount>::DisjunctionT;

  struct DisjunctionPair {
    size_t resolved_size;
    DisjunctionT resolution_result;

    explicit DisjunctionPair(const DisjunctionT& disjunction)
        : resolved_size(disjunction.count()), resolution_result(disjunction) {}

    bool operator==(const DisjunctionPair& other) const {
      return resolved_size == other.resolved_size &&
             resolution_result == other.resolution_result;
    }
  };

  struct DisjunctionPairHash {
    size_t operator()(const DisjunctionPair& first) const {
      return std::hash<unsigned long long>{}(
          first.resolution_result.to_ullong());
    }
  };

  std::unordered_set<DisjunctionPair, DisjunctionPairHash> queue_;
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

  Resolver(const ConjunctiveNormalForm<VariablesCount>& formula)
      : disjunctions_(formula.GetDisjunctions()) {}

  bool IsSatisfiable() {
    FillQueue();

    while (!queue_.empty()) {
      auto current = queue_.begin();
      DisjunctionPair disjunction = *current;
      queue_.erase(current);

      // if we obtained empty conjunction
      if (disjunction.resolved_size == 0) {
        return false;
      }

      for (auto itr = disjunctions_.begin(); itr != disjunctions_.end();
           ++itr) {
        AddPairToQueue(disjunction.resolution_result, *itr);
      }

      disjunctions_.insert(disjunction.resolution_result);
    }

    return true;
  }
};

#endif  // RESOLVER_H
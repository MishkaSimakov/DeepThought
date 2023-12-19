#ifndef CONJUNCTIVENORMALFORM_H
#define CONJUNCTIVENORMALFORM_H

#include <bitset>
#include <iostream>
#include <unordered_set>

template <size_t VariablesCount>
class ConjunctiveNormalForm {
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
  ConjunctiveNormalForm() = default;

  bool operator==(const ConjunctiveNormalForm&) const = default;

  void AddDisjunction(const DisjunctionT& disjunction) {
    disjunctions_.insert(disjunction);
  }

  void AddDisjunctions(const ConjunctiveNormalForm& other) {
    for (const DisjunctionT& disjunction : other.GetDisjunctions()) {
      AddDisjunction(disjunction);
    }
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
std::ostream& operator<<(std::ostream& os,
                         const std::bitset<VariablesCount>& disjunct) {
  static_assert(VariablesCount % 2 == 0);

  bool first_variable = true;

  for (size_t i = 0; i < VariablesCount; i += 2) {
    if (!disjunct[i] && !disjunct[i + 1]) {
      continue;
    }

    if (!first_variable) {
      os << " | ";
    }

    if (disjunct[i + 1]) {
      os << "~";
    }

    os << i / 2;
    first_variable = false;
  }

  if (first_variable) {
    os << "false";
  }

  return os;
}

template <size_t VariablesCount>
std::ostream& operator<<(std::ostream& os,
                         const ConjunctiveNormalForm<VariablesCount>& cnf) {
  if (cnf.GetDisjunctions().empty()) {
    os << "true"
       << "\n";
    return os;
  }

  for (const auto& disjunction : cnf.GetDisjunctions()) {
    os << "& " << disjunction << "\n";
  }

  return os;
}

#endif  // CONJUNCTIVENORMALFORM_H

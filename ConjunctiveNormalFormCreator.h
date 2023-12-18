#ifndef CONJUNCTIVENORMALFORMCREATOR_H
#define CONJUNCTIVENORMALFORMCREATOR_H

#include <bitset>
#include <functional>

#include "ConjunctiveNormalForm.h"

template <size_t VariablesCount>
class ConjunctiveNormalFormCreator {
 protected:
  using FunctionT =
      std::function<bool(const std::array<bool, VariablesCount>&)>;

  std::bitset<VariablesCount> used_variables_;
  FunctionT function_;

  bool NextSubstitution(std::array<bool, VariablesCount>& values) const {
    for (size_t i = 0; i < VariablesCount; ++i) {
      if (!used_variables_[i]) {
        continue;
      }

      if (!values[i]) {
        values[i] = true;
        return true;
      }

      values[i] = false;
    }

    return false;
  }

 public:
  ConjunctiveNormalFormCreator(
      const FunctionT& function,
      const std::bitset<VariablesCount>& used_variables)
      : used_variables_(used_variables), function_(function) {}

  ConjunctiveNormalForm<VariablesCount> Create() const {
    ConjunctiveNormalForm<VariablesCount> cnf;
    auto variables = cnf.GetVariables();

    std::array<bool, VariablesCount> values{false};

    do {
      if (function_(values)) {
        continue;
      }

      std::bitset<VariablesCount * 2> disjunct;
      for (size_t i = 0; i < VariablesCount; ++i) {
        if (!used_variables_[i]) {
          continue;
        }

        disjunct[2 * i + (values[i] ? 1 : 0)] = true;
      }

      cnf.AddDisjunction(disjunct);
    } while (NextSubstitution(values));

    return cnf;
  }
};

#endif  // CONJUNCTIVENORMALFORMCREATOR_H

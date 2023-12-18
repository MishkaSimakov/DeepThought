#include <iostream>

#include "ConjunctiveNormalForm.h"
#include "ConjunctiveNormalFormCreator.h"
#include "Resolver.h"
#include "ResolverTester.h"

using std::cin, std::cout, std::endl;

int main() {
  ResolverTests::doResolverTests();

  cout << "All tests passed successfully!" << endl;
  cout << endl;

  // 10 variables for edges
  constexpr size_t kVariablesCount = 10;

  // array that contains variable index for each edge
  const std::array<std::array<int, 5>, 5> kEdgeIndices = {
      0, 0, 1, 2, 3, 0, 0, 4, 5, 6, 1, 4, 0,
      7, 8, 2, 5, 7, 0, 9, 3, 6, 8, 9, 0};

  // generate cnf
  ConjunctiveNormalForm<kVariablesCount> cnf;

  for (size_t vertex = 0; vertex < 5; ++vertex) {
    std::bitset<kVariablesCount> used_variables;

    for (size_t other = 0; other < 5; ++other) {
      if (other == vertex) {
        continue;
      }

      used_variables[kEdgeIndices[vertex][other]] = true;
    }

    ConjunctiveNormalFormCreator<kVariablesCount> cnf_creator(
        [vertex, kEdgeIndices](
            const std::array<bool, kVariablesCount>& values) -> bool {
          bool is_odd = false;

          for (size_t other = 0; other < 5; ++other) {
            if (other == vertex) {
              continue;
            }

            is_odd = is_odd != values[kEdgeIndices[vertex][other]];
          }

          // without loss of generality we assume that vertex[0] = 1
          // and \A v \in 1..4: vertex[v] = 0
          return (vertex != 0) != is_odd;
        },
        used_variables);
    cnf.AddDisjunctions(cnf_creator.Create());
  }

  // substitute cnf and check
  Resolver resolver(cnf);
  cout << "Misha:    O Deep Thought Computer the task I have designed you to "
          "perform is this. I want you to tell me answer to my homework."
       << endl;
  cout << "Computer: Tricky..." << endl;
  cout << "Misha:    But can you do it?" << endl;
  cout << "Computer: Yes, I can do it." << endl;
  cout << "Misha:    There is an answer? A simple answer?" << endl;
  cout << "Computer: Yes" << endl;
  cout << "Computer: And here it is: " << std::boolalpha
       << resolver.IsSatisfiable() << endl;
}
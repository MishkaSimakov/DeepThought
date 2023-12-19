#ifndef RESOLVERTESTER_H
#define RESOLVERTESTER_H

#include "Resolver.h"

namespace ResolverTests {
bool impl(bool a, bool b) { return !a || b; }

void cnfFromVariablesTest() {
  ConjunctiveNormalForm<5> cnf;
  auto vars = cnf.GetVariables();
  auto x1 = vars[0];
  auto x2 = vars[1];
  auto x3 = vars[2];
  auto x4 = vars[3];
  auto x5 = vars[4];

  assert((x1 | x2 | x3 | x4 | x5) == std::bitset<10>(0b0101010101));
  assert((x1 | x2 | x3 | x4 | ~x5) == std::bitset<10>(0b1001010101));
  assert((x1 | x2 | x3 | ~x4 | x5) == std::bitset<10>(0b0110010101));
  assert((x1 | x2 | ~x3 | x4 | x5) == std::bitset<10>(0b0101100101));
  assert((x1 | ~x2 | x3 | x4 | x5) == std::bitset<10>(0b0101011001));
  assert((~x1 | x2 | x3 | x4 | x5) == std::bitset<10>(0b0101010110));
  assert((~x1 | ~x2 | ~x3 | ~x4 | ~x5) == std::bitset<10>(0b1010101010));
  assert((~x1 | ~x2 | ~x3 | ~x4 | ~x5) == std::bitset<10>(0b1010101010));
  assert((x2 | x3) == std::bitset<10>(0b0000010100));
  assert((x3 | ~x5) == std::bitset<10>(0b1000010000));
}

void cnfCreatorTest() {
  // or
  ConjunctiveNormalFormCreator<5> or_creator(
      [](const std::array<bool, 5>& values) -> bool {
        return values[0] || values[1] || values[2] || values[3] || values[4];
      },
      0b11111);

  ConjunctiveNormalForm<5> or_cnf;
  auto vars = or_cnf.GetVariables();
  or_cnf.AddDisjunction(vars[0] | vars[1] | vars[2] | vars[3] | vars[4]);

  assert(or_creator.Create() == or_cnf);

  // (x1 & x2) | (x3 & x4) == (x1 | x3) & (x1 | x4) & (x2 | x3) & (x2 | x4)
  ConjunctiveNormalFormCreator<5> distribute_creator(
      [](const std::array<bool, 5>& values) -> bool {
        return (values[0] && values[1]) || (values[2] && values[3]);
      },
      0b01111);

  // should generate truth table:
  // 0 0 0 0 -> 0
  // 0 0 0 1 -> 0
  // 0 0 1 0 -> 0
  // 0 0 1 1 -> 1
  // 0 1 0 0 -> 0
  // 0 1 0 1 -> 0
  // 0 1 1 0 -> 0
  // 0 1 1 1 -> 1
  // 1 0 0 0 -> 0
  // 1 0 0 1 -> 0
  // 1 0 1 0 -> 0
  // 1 0 1 1 -> 1
  // 1 1 0 0 -> 1
  // 1 1 0 1 -> 1
  // 1 1 1 0 -> 1
  // 1 1 1 1 -> 1

  // to make cnf from truth table we select rows where result is 0
  // and create disjunct for this row using this rule:
  //  if variable x value is true in row we add ~x in disjunct
  //  otherwise we add x into disjunct

  // so the resulting cnf should be
  ConjunctiveNormalForm<5> distribute_cnf;
  vars = distribute_cnf.GetVariables();

  distribute_cnf.AddDisjunction(vars[0] | vars[1] | vars[2] | vars[3]);
  distribute_cnf.AddDisjunction(vars[0] | vars[1] | vars[2] | ~vars[3]);
  distribute_cnf.AddDisjunction(vars[0] | vars[1] | ~vars[2] | vars[3]);
  distribute_cnf.AddDisjunction(vars[0] | ~vars[1] | vars[2] | vars[3]);
  distribute_cnf.AddDisjunction(vars[0] | ~vars[1] | vars[2] | ~vars[3]);
  distribute_cnf.AddDisjunction(vars[0] | ~vars[1] | ~vars[2] | vars[3]);
  distribute_cnf.AddDisjunction(~vars[0] | vars[1] | vars[2] | vars[3]);
  distribute_cnf.AddDisjunction(~vars[0] | vars[1] | vars[2] | ~vars[3]);
  distribute_cnf.AddDisjunction(~vars[0] | vars[1] | ~vars[2] | vars[3]);

  assert(distribute_creator.Create() == distribute_cnf);
}

void resolveMethodTest() {
  ConjunctiveNormalForm<5> cnf;
  auto vars = cnf.GetVariables();
  auto x1 = vars[0];
  auto x2 = vars[1];
  auto x3 = vars[2];
  auto x4 = vars[3];
  auto x5 = vars[4];

  // 1
  auto first_disjunct = x1 | x2 | x3 | x4 | x5;
  auto second_disjunct = ~x1 | x2 | x3 | x4 | x5;
  ConjunctiveNormalForm<5>::DisjunctionT result;

  assert(Resolver<5>::Resolve(first_disjunct, second_disjunct, result));
  assert(result == (x2 | x3 | x4 | x5));

  // 2
  first_disjunct = x1 | x2 | x3 | x5;
  second_disjunct = ~x5;

  assert(Resolver<5>::Resolve(first_disjunct, second_disjunct, result));
  assert(result == (x1 | x2 | x3));

  // 3
  first_disjunct = x1 | x2 | x3 | x5;
  second_disjunct = ~x5 | ~x2;

  assert(!Resolver<5>::Resolve(first_disjunct, second_disjunct, result));

  // 4
  first_disjunct = x1 | x2 | x3 | x5;
  second_disjunct = x5 | x4;

  assert(!Resolver<5>::Resolve(first_disjunct, second_disjunct, result));

  // 5
  first_disjunct = ~x3 | x5 | x2;
  second_disjunct = x3 | x5 | ~x4;

  assert(Resolver<5>::Resolve(first_disjunct, second_disjunct, result));
  assert(result == (x5 | x2 | ~x4));
}

void cnf1Test() {
  ConjunctiveNormalForm<4> cnf;
  auto vars = cnf.GetVariables();
  auto x1 = vars[0];
  auto x2 = vars[1];
  auto A = vars[2];
  auto B = vars[3];

  cnf.AddDisjunction(x1 | ~A);
  cnf.AddDisjunction(x1 | ~B);
  cnf.AddDisjunction(~x1 | A | B);
  cnf.AddDisjunction(~x2 | A | B);
  cnf.AddDisjunction(x2 | ~A);
  cnf.AddDisjunction(x2 | ~B);
  cnf.AddDisjunction(~x1);
  cnf.AddDisjunction(x2);

  Resolver resolver(cnf);
  assert(!resolver.IsSatisfiable());
}

void cnf2Test() {
  ConjunctiveNormalForm<3> cnf;
  auto vars = cnf.GetVariables();
  auto p = vars[0];
  auto q = vars[1];
  auto r = vars[2];

  cnf.AddDisjunction(p | q | r);
  cnf.AddDisjunction(~p | q);
  cnf.AddDisjunction(~q | r);
  cnf.AddDisjunction(~r);

  Resolver resolver(cnf);
  assert(!resolver.IsSatisfiable());
}

void cnf3Test() {
  ConjunctiveNormalForm<3> cnf;
  auto vars = cnf.GetVariables();
  auto p = vars[0];
  auto q = vars[1];
  auto r = vars[2];

  cnf.AddDisjunction(p | q | ~r);
  cnf.AddDisjunction(~p | ~q | r);
  cnf.AddDisjunction(q | r);
  cnf.AddDisjunction(~p | ~r);
  cnf.AddDisjunction(p | ~q);

  Resolver resolver(cnf);
  assert(!resolver.IsSatisfiable());
}

void cnf4Test() {
  ConjunctiveNormalForm<3> cnf;
  auto vars = cnf.GetVariables();
  auto p = vars[0];
  auto q = vars[1];
  auto r = vars[2];

  cnf.AddDisjunction(p | q | r);
  cnf.AddDisjunction(~p | ~q | ~r);
  cnf.AddDisjunction(p | ~q);
  cnf.AddDisjunction(~p | q);
  cnf.AddDisjunction(p | ~r);
  cnf.AddDisjunction(~p | r);

  Resolver resolver(cnf);
  assert(!resolver.IsSatisfiable());
}

void cnf5Test() {
  ConjunctiveNormalFormCreator<2> cnf_creator(
      [](const std::array<bool, 2>& values) -> bool {
        return !impl(values[0], impl(values[1], values[0]));
      },
      1 + 2);

  Resolver resolver(cnf_creator.Create());
  assert(!resolver.IsSatisfiable());
}

void cnf6Test() {
  ConjunctiveNormalFormCreator<2> cnf_creator(
      [](const std::array<bool, 2>& values) -> bool {
        return !impl(impl(values[0], values[1]), impl(!values[1], !values[0]));
      },
      1 + 2);

  Resolver resolver(cnf_creator.Create());
  assert(!resolver.IsSatisfiable());
}

void cnf7Test() {
  ConjunctiveNormalFormCreator<2> cnf_creator(
      [](const std::array<bool, 2>& values) -> bool {
        return !impl(impl(impl(values[0], values[1]), values[0]), values[0]);
      },
      1 + 2);

  Resolver resolver(cnf_creator.Create());
  assert(!resolver.IsSatisfiable());
}

void cnf8Test() {
  ConjunctiveNormalFormCreator<2> cnf_creator(
      [](const std::array<bool, 2>& values) -> bool {
        return !impl(!(values[0] && values[1]), !values[0] || !values[1]);
      },
      1 + 2);

  Resolver resolver(cnf_creator.Create());
  assert(!resolver.IsSatisfiable());
}

void cnf9Test() {
  ConjunctiveNormalForm<5> cnf;
  auto vars = cnf.GetVariables();
  auto p = vars[0];
  auto q = vars[1];
  auto r = vars[2];
  auto s = vars[3];
  auto t = vars[4];

  cnf.AddDisjunction(p | q | r);
  cnf.AddDisjunction(~p | ~q | r);
  cnf.AddDisjunction(~p | q | ~r);
  cnf.AddDisjunction(p | ~q | ~r);
  cnf.AddDisjunction(~p | s);
  cnf.AddDisjunction(~p | s);
  cnf.AddDisjunction(p | ~s);
  cnf.AddDisjunction(~r | t);
  cnf.AddDisjunction(r | ~t);
  cnf.AddDisjunction(~q | s | t);
  cnf.AddDisjunction(q | ~s | t);
  cnf.AddDisjunction(q | s | ~t);
  cnf.AddDisjunction(~q | ~s | ~t);

  Resolver resolver(cnf);
  assert(!resolver.IsSatisfiable());
}

void cnf10Test() {
  ConjunctiveNormalForm<2> cnf;
  auto vars = cnf.GetVariables();
  auto p = vars[0];
  auto q = vars[1];

  cnf.AddDisjunction(p | q);
  cnf.AddDisjunction(p);
  cnf.AddDisjunction(q);

  Resolver resolver(cnf);
  assert(resolver.IsSatisfiable());
}

void cnf11Test() {
  ConjunctiveNormalForm<2> cnf;
  auto vars = cnf.GetVariables();
  auto p = vars[0];
  auto q = vars[1];

  cnf.AddDisjunction(p | q);
  cnf.AddDisjunction(~p);
  cnf.AddDisjunction(q);

  Resolver resolver(cnf);
  assert(resolver.IsSatisfiable());
}

void cnf12Test() {
  ConjunctiveNormalForm<3> cnf;
  auto vars = cnf.GetVariables();
  auto p = vars[0];
  auto q = vars[1];
  auto r = vars[2];

  cnf.AddDisjunction(p);
  cnf.AddDisjunction(q);
  cnf.AddDisjunction(~r);
  cnf.AddDisjunction(p | q);
  cnf.AddDisjunction(p | ~q);
  cnf.AddDisjunction(r | q);

  Resolver resolver(cnf);
  assert(resolver.IsSatisfiable());
}

void cnf13Test() {
  // asserts that a * b = c where a, b, c - 3 bit numbers
  auto product_equals = [](const std::array<bool, 9>& values) -> bool {
    bool third_digit = (values[2] && values[5]);
    bool second_digit = (values[1] && values[5] != values[2] && values[4]);
    bool first_digit = (values[0] && values[5] != values[1] &&
                        values[4] != values[2] && values[3]);

    return (first_digit == values[6] && second_digit == values[7] &&
            third_digit == values[8]);
  };
  std::bitset<9> used_vars = 0b111111111;

  {
    ConjunctiveNormalFormCreator<9> cnf_creator(product_equals, used_vars);
    auto cnf = cnf_creator.Create();
    auto vars = cnf.GetVariables();

    cnf.AddDisjunction(vars[6]);
    cnf.AddDisjunction(~vars[7]);
    cnf.AddDisjunction(vars[8]);

    // solve a * b == 5
    Resolver resolver(cnf);
    assert(resolver.IsSatisfiable());
  }

  {
    ConjunctiveNormalFormCreator<9> cnf_creator(product_equals, used_vars);
    auto cnf = cnf_creator.Create();
    auto vars = cnf.GetVariables();

    cnf.AddDisjunction(vars[6]);
    cnf.AddDisjunction(~vars[7]);
    cnf.AddDisjunction(vars[8]);

    cnf.AddDisjunction(~vars[2]);

    // prove that if a * b == 5 then a % 2 == 1
    Resolver resolver(cnf);
    assert(!resolver.IsSatisfiable());
  }
}

void doResolverTests() {
  // test cnf creation using variables
  cnfFromVariablesTest();

  // test CnfCreator class
  cnfCreatorTest();

  // test Resolve method of Resolver
  resolveMethodTest();

  // №39 from my homework
  cnf1Test();

  // №4 from seminar tasks
  cnf2Test();
  cnf3Test();
  cnf4Test();

  // №6 from seminar tasks
  cnf5Test();
  cnf6Test();
  cnf7Test();
  cnf8Test();

  // №7 from seminar tasks
  cnf9Test();

  // satisfiability tests
  cnf10Test();
  cnf11Test();
  cnf12Test();

  // other tests
  cnf13Test();
}
}  // namespace ResolverTests

#endif  // RESOLVERTESTER_H

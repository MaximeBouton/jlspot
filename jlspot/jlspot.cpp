#include <jlcxx/jlcxx.hpp>
#include <jlcxx/stl.hpp>

#include <iostream>
#include <vector>
#include <tuple>

#include <spot/tl/formula.hh>
// #include <spot/tl/environment.hh>
#include <spot/tl/parse.hh>
#include <spot/tl/print.hh>

JLCXX_MODULE define_julia_module(jlcxx::Module& mod)
{
  mod.add_type<spot::formula>("Formula")
    .method("is_ltl_formula", [] (spot::formula& f) {return f.is_ltl_formula(); })
    .method("is_eventual", [](spot::formula &f) { return f.is_eventual(); });
  // .method("is_ltl_formula", static_cast<bool (spot::formula::*)()>(&spot::formula::is_ltl_formula))
  // static_cast<void (config::*)(char const *, char const *)>(&config::set)
      // mod.add_type<spot::environment>("Environment");
  // mod.append_function
    // .method("ap", [] (spot::formula& f, const std::string& name) {f.ap(name); });
    // .method("ap", [](spot::formula &f, const spot::formula &a) { f.ap(a); });
  mod.method("parse_formula", [](const std::string &ltl_string) { return spot::parse_formula(ltl_string); });
}

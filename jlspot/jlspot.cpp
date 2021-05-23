#include <jlcxx/jlcxx.hpp>
#include <jlcxx/stl.hpp>

#include <iostream>
#include <vector>
#include <tuple>

#include <spot/tl/formula.hh>
#include <spot/tl/parse.hh>
#include <spot/tl/print.hh>

JLCXX_MODULE define_julia_module(jlcxx::Module& mod)
{
  // mod.add_type<spot::formula>("Formula");
  mod.method("parse_formula", spot::parse_formula);
  // mod.append_function
    // .method("ap", [] (spot::formula& f, const std::string& name) {f.ap(name); });
    // .method("ap", [](spot::formula &f, const spot::formula &a) { f.ap(a); });
}

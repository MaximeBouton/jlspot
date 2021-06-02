#include <jlcxx/jlcxx.hpp>
#include <jlcxx/stl.hpp>
#include <jlcxx/tuple.hpp>

#include <spot/tl/formula.hh>
#include <spot/tl/parse.hh>
#include <spot/tl/print.hh>
#include <spot/tl/apcollect.hh>

#include <spot/twa/formula2bdd.hh>
#include <spot/twa/acc.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twaalgos/dot.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/split.hh>
#include <spot/twaalgos/totgba.hh>

// #include <spot/tl/environment.hh>

bool is_reachability(spot::formula &f)
{
  return f.is(spot::op::F) && f[0].is_boolean();
}

bool is_constrained_reachability(spot::formula &f)
{
  return f.is(spot::op::U) && f[0].is_boolean() && f[1].is_boolean();
}

std::vector<spot::formula> atomic_prop_collect(spot::formula f)
{
        spot::atomic_prop_set* sap = spot::atomic_prop_collect(f);
        std::vector<spot::formula> v(sap->begin(), sap->end());
        return v;
}       

std::vector<std::vector<unsigned int>> get_edges(spot::twa_graph &aut)
{
        std::vector<std::vector<unsigned int>> edge_list;
        for (auto &e : aut.edges())
        {
                std::vector<unsigned int> edge;
                edge.push_back(e.src);
                edge.push_back(e.dst);
                edge_list.push_back(edge);
        }
        return edge_list;
}

std::vector<spot::formula> get_labels(spot::twa_graph &aut)
{
        std::vector<spot::formula> labs_list;
        spot::bdd_dict_ptr bdict = aut.get_dict();
        for (auto& e: aut.edges())
        {
                spot::formula f = spot::bdd_to_formula(e.cond, bdict);
                labs_list.push_back(f);
        }
        return labs_list;
}

std::vector<spot::formula> positve_atomic_propositions(spot::formula &f)
{
        std::vector<spot::formula> positive_aps;
        for (auto ap : f)
        {
                if (!ap.is(spot::op::Not))
                {
                        positive_aps.push_back(ap);
                }
        }
        return positive_aps;
}

std::set<unsigned> container_to_set(const spot::internal::mark_container &mc)
{
        std::set<unsigned> set;
        for (unsigned s: mc)
        {
                set.insert(s);
        }
        return set;
}

using fin_inf_tuple = std::vector<std::vector<unsigned>>;

std::vector<fin_inf_tuple> get_rabin_acceptance(spot::twa_graph &aut)
{
        spot::acc_cond acc = aut.acc();
        std::vector<fin_inf_tuple> fin_inf_vec;
        std::vector<spot::acc_cond::rs_pair> pairs;
        bool israbin = acc.is_rabin_like(pairs);
        // for (auto p: pairs){
        //         std::cout << "hello pair" << std::endl;
        //         std::cout << p.fin << std::endl;
        //         std::cout << p.inf << std::endl;
        // }
        for (auto p: pairs)
        {
                std::vector<unsigned> statefinset;
                std::vector<unsigned> stateinfset;
        
                auto finset = container_to_set(p.fin.sets());
                auto infset = container_to_set(p.inf.sets());

                // std::cout << "print finset" << std::endl;
                // for (auto i : p.fin.sets())
                // {
                //         std::cout << i << std::endl;
                // }
                // std::cout << "print infset" << std::endl;
                // for (auto i : infset)
                // {
                //         std::cout << i << std::endl;
                // }
                // std::cout << "num states" << ns << std::endl;
                for (int s=0; s < aut.num_states(); s++)
                {       
                        // std::cout << "hello state " << s << std::endl;
                        auto stateset = container_to_set(aut.state_acc_sets(s).sets());
                        std::vector<unsigned int> it1;
                        std::vector<unsigned int> it2;

                        std::set_intersection(finset.begin(),
                                              finset.end(),
                                              stateset.begin(),
                                              stateset.end(),
                                              std::inserter(it1, it1.begin()));

                        std::set_intersection(infset.begin(),
                                              infset.end(),
                                              stateset.begin(),
                                              stateset.end(),
                                              std::inserter(it2, it2.begin()));

                        // std::cout << "print it1" << std::endl;
                        // for (auto i : it1)
                        // {
                                // std::cout << i << std::endl;
                        // }
                        if (!it1.empty()){
                                statefinset.push_back(s);
                        } 
                        else if (!it2.empty()){
                                stateinfset.push_back(s);
                        }
                }
                fin_inf_tuple fin_inf{ statefinset, stateinfset };
                fin_inf_vec.push_back(fin_inf);
                
        }
        return fin_inf_vec;
}

void print_dot(spot::const_twa_graph_ptr &aut)
{
        spot::print_dot(std::cout, aut);
}

JLCXX_MODULE define_julia_module(jlcxx::Module& mod)
{
        // add ltl operator enum
        mod.add_bits<spot::op>("LTLOp", jlcxx::julia_type("CppEnum"));
        mod.set_const("ff", spot::op::ff);       ///< False
        mod.set_const("tt", spot::op::tt);       ///< True
        mod.set_const("eword", spot::op::eword); ///< Empty word
        mod.set_const("ap", spot::op::ap);       ///< Atomic proposition
        //unary operators
        mod.set_const("Not", spot::op::Not);                           ///< Negation
        mod.set_const("X", spot::op::X);                               ///< Next
        mod.set_const("F", spot::op::F);                               ///< Eventually
        mod.set_const("G", spot::op::G);                               ///< Globally
        mod.set_const("Closure", spot::op::Closure);                   ///< PSL Closure
        mod.set_const("NegClosure", spot::op::NegClosure);             ///< Negated PSL Closure
        mod.set_const("NegClosureMarked", spot::op::NegClosureMarked); ///< marked version of the Negated PSL Closure
        // binary  operators
        mod.set_const("Xor", spot::op::Xor);                     ///< Exclusive Or
        mod.set_const("Implies", spot::op::Implies);             ///< Implication
        mod.set_const("Equiv", spot::op::Equiv);                 ///< Equivalence
        mod.set_const("U", spot::op::U);                         ///< until
        mod.set_const("R", spot::op::R);                         ///< release (dual of until)
        mod.set_const("W", spot::op::W);                         ///< weak until
        mod.set_const("M", spot::op::M);                         ///< strong release (dual of weak until)
        mod.set_const("EConcat", spot::op::EConcat);             ///< Seq
        mod.set_const("EConcatMarked", spot::op::EConcatMarked); ///< Seq, Marked
        mod.set_const("UConcat", spot::op::UConcat);             ///< Triggers
        // n-ary operators
        mod.set_const("Or", spot::op::Or);         ///< (omega-Rational) Or
        mod.set_const("OrRat", spot::op::OrRat);   ///< Rational Or
        mod.set_const("And", spot::op::And);       ///< (omega-Rational) And
        mod.set_const("AndRat", spot::op::AndRat); ///< Rational And
        mod.set_const("AndNLM", spot::op::AndNLM); ///< Non-Length-Matching Rational-And
        mod.set_const("Concat", spot::op::Concat); ///< Concatenation
        mod.set_const("Fusion", spot::op::Fusion); ///< Fusion
        // star-like operators
        mod.set_const("Star", spot::op::Star);               ///< Star
        mod.set_const("FStar", spot::op::FStar);             ///< Fustion Star
        mod.set_const("first_match", spot::op::first_match); ///< first_match(sere)

        // formula
        mod.add_type<spot::formula>("Formula")
            .method("is_ltl_formula", [](spot::formula &f)
                    { return f.is_ltl_formula(); })
            .method("is_eventual", [](spot::formula &f)
                    { return f.is_eventual(); })
            .method("is_sugar_free_ltl", [](spot::formula &f)
                    { return f.is_sugar_free_ltl(); })
            .method("is_literal", [](spot::formula &f)
                    { return f.is_literal(); })
            .method("is_boolean", [](spot::formula &f)
                    { return f.is_boolean(); })
            .method("size", [](spot::formula &f)
                    { return f.size(); })
            .method("is_tt", [](spot::formula &f)
                    { return f.is_tt(); })
            .method("is", [](spot::formula &f, spot::op op)
                    { return f.is(op); });
        mod.method("parse_formula", [](const std::string &ltl_string)
                   { return spot::parse_formula(ltl_string); });
        mod.method("is_reachability", is_reachability);
        mod.method("is_constrained_reachability", is_constrained_reachability);
        mod.method("atomic_prop_collect", atomic_prop_collect);
        mod.method("positive_atomic_propositions", positve_atomic_propositions);
        mod.method("str_psl", spot::str_psl);
        mod.method("str_latex_psl", spot::str_latex_psl);

        // automata
        mod.add_type<spot::twa_graph>("TWAGraph")
            .method("num_states", [](spot::twa_graph &aut)
                    { return aut.num_states(); })
            .method("num_edges", [](spot::twa_graph &aut)
                    { return aut.num_edges(); })
            .method("get_init_state_number", [](spot::twa_graph &aut)
                    { return aut.get_init_state_number(); })
            .method("atomic_propositions", [](spot::twa_graph &aut)
                    { return aut.ap(); });
        //     .method("acc", [](spot::twa_graph &aut)
                // { return aut.acc(); });
        // mod.add_type<spot::acc_cond>("AccCond");
        mod.method("split_edges", spot::split_edges);
        mod.method("is_deterministic", spot::is_deterministic);
        mod.method("to_generalized_rabin", [](spot::const_twa_graph_ptr &aut)
                {return spot::to_generalized_rabin(aut); });
        mod.method("get_edges", get_edges);
        mod.method("get_labels", get_labels);
        mod.method("get_rabin_acceptance", get_rabin_acceptance);
        mod.add_type<spot::twa>("TWA");
        mod.method("print_dot", print_dot);


        // post processor
        //   mod.add_type<spot::postprocessor::output_type>("PostporcessorOutputType");
        mod.add_bits<spot::postprocessor::output_type>("PostprocessorOutputType", jlcxx::julia_type("CppEnum"));
        mod.set_const("TGBA", spot::postprocessor::output_type::TGBA);
        mod.set_const("BA", spot::postprocessor::output_type::BA);
        mod.set_const("Monitor", spot::postprocessor::output_type::Monitor);
        mod.set_const("Generic", spot::postprocessor::output_type::Generic);
        mod.set_const("Parity", spot::postprocessor::output_type::Parity);

        mod.set_const("Small", static_cast<int>(spot::postprocessor::Small));
        mod.set_const("Deterministic", static_cast<int>(spot::postprocessor::Deterministic));
        mod.set_const("Complete", static_cast<int>(spot::postprocessor::Complete));
        mod.set_const("SBAcc", static_cast<int>(spot::postprocessor::SBAcc));
        mod.set_const("Unambiguous", static_cast<int>(spot::postprocessor::Unambiguous));
        mod.set_const("Colored", static_cast<int>(spot::postprocessor::Colored));

        // translator
        mod.add_type<spot::translator>("Translator")
            .method("set_type", [](spot::translator &trans, spot::postprocessor::output_type type)
                    { trans.set_type(type); })
            .method("set_pref", [](spot::translator &trans, spot::postprocessor::output_pref pref)
                    { return trans.set_pref(pref); })
            .method("run_translator", [](spot::translator &trans, spot::formula &f)
                    { return trans.run(f); });
        //     .method("TGBA", spot::postprocessor::output_type::TGBA)
        //       .method("BA", spot::postprocessor::output_type::BA)
        //       .method("Monitor", spot::postprocessor::output_type::Monitor)
        //       .method("Small", spot::postprocessor::Small)
        //       .method("Deterministic", spot::postprocessor::Deterministic)
        //       .method("Complete", spot::postprocessor::Complete)
        //       .method("SBAcc", spot::postprocessor::SBAcc)
        //       .method("Unambiguous", spot::postprocessor::Unambiguous)
        //       .method("Colored", spot::postprocessor::Colored);

        //   mod.add_type<spot::postprocessor::TGBA>("TGBA");
        //   mod.add_type<spot::postprocessor::BA>("BA");

        // .method("tgba", spot::translator::);

        // .method("is_ltl_formula", static_cast<bool (spot::formula::*)()>(&spot::formula::is_ltl_formula))
        // static_cast<void (config::*)(char const *, char const *)>(&config::set)
        // mod.add_type<spot::environment>("Environment");
        // mod.append_function
        // .method("ap", [] (spot::formula& f, const std::string& name) {f.ap(name); });
        // .method("ap", [](spot::formula &f, const spot::formula &a) { f.ap(a); });
}

#include "Xfa.hpp"
#include "xfa-parser/ast.hh"
#include "wali/domains/binrel/ProgramBddContext.hpp"
#include "wali/domains/binrel/BinRel.hpp"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <vector>

#include "../timer.hpp"

using namespace xfa_parser;

namespace details
{
extern
void
interleave_all_fdds();

extern
void
print_bdd_variable_order(std::ostream & os);
}


namespace wali {
    namespace xfa {

        struct ReadTransitionException {
            BinaryRelation back_weight, bit1_weight, init_weight;

            ReadTransitionException(BinaryRelation init, BinaryRelation back, BinaryRelation bit1)
                : back_weight(back)
                , bit1_weight(bit1)
                , init_weight(init)                  
            {}
        };

        struct WeightedTransition {
            State source;
            Symbol symbol;
            State target;
            BinaryRelation weight;
            
            WeightedTransition(State src, Symbol sym, State tgt, BinaryRelation w)
                : source(src), symbol(sym), target(tgt), weight(w)
            {
                assert (weight.get_ptr());
            }
        };

        typedef std::vector<WeightedTransition> TransList;
        

        std::string
        var_name(int id,
                 std::string const & domain_var_name_prefix)
        {
            return domain_var_name_prefix + "var" + boost::lexical_cast<std::string>(id);
        }    

        void
        register_vars(xfa_parser::Xfa const & ast,
                      wali::domains::binrel::ProgramBddContext & voc,
                      int fdd_size,
                      std::string const & prefix)
        {
            static bool has_run = false;
            assert(!has_run);
            
            std::cerr << "Registering variables with size " << fdd_size << "\n";
            std::set<std::string> registered;
            for (auto ast_trans = ast.transitions.begin(); ast_trans != ast.transitions.end(); ++ast_trans) {
                ActionList & acts = (*ast_trans)->actions;
                for (auto act_it = acts.begin(); act_it != acts.end(); ++act_it) {
                    auto & act = **act_it;
                    if (act.action_type == "fire"
                        || act.action_type == "alert"
                        || act.action_type == "reject")
                    {
                        continue;
                    }
                    assert (act.action_type == "ctr2");
                    if (registered.find(var_name(act.operand_id, prefix)) == registered.end()) {
                        if (has_run) {
                            voc.addIntVar(var_name(act.operand_id, prefix), fdd_size);
                        }
                        std::cerr << "    " << var_name(act.operand_id, prefix) << "\n";
                        registered.insert(var_name(act.operand_id, prefix));
                    }
                }
            }


            if (!has_run) {
                std::map<std::string, int> vars;
                for (auto var = registered.begin(); var != registered.end(); ++var) {
                    //voc.addIntVar(*var, fdd_size);
                    vars[*var] = fdd_size;
                }

                //voc.addIntVar("left_current_state", ast.states.size());
                vars["left_current_state"] = ast.states.size() * 2;
                std::cout << "Adding left_current_state with size " << vars["left_current_state"];
                
                voc.setIntVars(vars);
                details::interleave_all_fdds();
details::print_bdd_variable_order(std::cout);
                has_run = true;
            }
        }

        BinaryRelation
        get_relation(xfa_parser::Action const & act,
                     wali::domains::binrel::ProgramBddContext & voc,
                     BinaryRelation zero,
                     bdd ident,
                     std::string const & prefix)
        {
            using namespace wali::domains::binrel;

            if (act.action_type == "fire"
                || act.action_type == "alert")
            {
                std::cerr << "    fire or alert\n";
                // Hmmm.
                return zero;
            }

            assert(act.command);
            auto cmd = *act.command;

            if (cmd.name == "read") {
                BinaryRelation times2, plus1, init;
                {
                    // x = 0
                    BlockTimer tim("read: =0");
                    bdd b = voc.Assign(var_name(act.operand_id, prefix),
                                       voc.Const(0));
                    init = new BinRel(&voc, b);
                    std::cerr << "> read: =0 bdd node count " << bdd_nodecount(b) << "\n";
                }
                {
                    // x = x + x (where x is the __io_return being read into)
                    BlockTimer tim("read: *2");
                    bdd b = voc.Assign(var_name(act.operand_id, prefix),
                                       voc.Plus(voc.From(var_name(act.operand_id, prefix)),
                                                voc.From(var_name(act.operand_id, prefix))));
                    times2 = new BinRel(&voc, b);
                    std::cerr << "> read: *2 bdd node count " << bdd_nodecount(b) << "\n";
                }
                {
                    // x = x + 1 (where x is the __io_return being read into)
                    BlockTimer tim("read: +1");
                    bdd b = voc.Assign(var_name(act.operand_id, prefix),
                                       voc.Plus(voc.From(var_name(act.operand_id, prefix)),
                                                voc.Const(1)));
                    plus1 = new BinRel(&voc, b);
                    std::cerr << "> read: +1 bdd node count " << bdd_nodecount(b) << "\n";
                }
                throw ReadTransitionException(init, times2, plus1);
            }

            if (cmd.name == "reset") {
                BlockTimer tim("reset");
                assert(cmd.arguments.size() == 1u);
                int val = boost::lexical_cast<int>(cmd.arguments[0]);
                BinaryRelation ret = new BinRel(&voc, voc.Assign(var_name(act.operand_id, prefix),
                                                                 voc.Const(val)));
                return ret;
            }

            if (cmd.name == "incr") {
                BlockTimer tim("incr");
                return new BinRel(&voc, voc.Assign(var_name(act.operand_id, prefix),
                                                   voc.Plus(voc.From(var_name(act.operand_id, prefix)),
                                                            voc.Const(1))));
            }

            if (cmd.name == "testnectr2") {
                BlockTimer tim("testnectr2");
                assert(cmd.arguments.size() == 1u);
                assert(cmd.consequent);
                assert(!cmd.alternative);
                assert(cmd.consequent->action_type == "reject");

                int rhs_id = boost::lexical_cast<int>(cmd.arguments[0]);
                std::string lhs_name = var_name(act.operand_id, prefix);
                std::string rhs_name = var_name(rhs_id, prefix);
                
                int lhs_fdd = voc[lhs_name]->baseLhs;
                int rhs_fdd = voc[rhs_name]->baseLhs;
                bdd eq = fdd_equals(lhs_fdd, rhs_fdd);
                binrel_t enforce_eq = new BinRel(&voc, eq & ident);
                return enforce_eq;
                    
                bdd kill_counter = voc.Assign(rhs_name, voc.Const(0));
                binrel_t kill_k = new BinRel(&voc, kill_counter);

                return enforce_eq->Compose(kill_k.get_ptr());              
            }

            assert(false);
        }
        

        TransList
        get_transitions(xfa_parser::Transition const & trans,
                        wali::domains::binrel::ProgramBddContext & voc,
                        BinaryRelation zero,
                        bdd ident,
                        std::string const & prefix)
        {
            State source = getState(trans.source);
            State dest = getState(trans.dest);
            Symbol eps(wali::WALI_EPSILON);            

            using wali::domains::binrel::BinRel;
            BinaryRelation rel;

            TransList ret;            

            try {
                if (trans.actions.size() == 0u) {
                    rel = new BinRel(&voc, ident);
                    //rel->is_effectively_one = true;
                }
                else if (trans.actions.size() == 1u) {
                    rel = get_relation(*trans.actions[0], voc, zero, ident, prefix);
                }
                else {
                    assert(false);
                }

                auto const & syms = trans.symbols;
                for (auto sym = syms.begin(); sym != syms.end(); ++sym) {
                    auto name = dynamic_cast<xfa_parser::Name*>(sym->get());
                    assert(name);
                    if (name->name == "epsilon") {
                        ret.push_back(WeightedTransition(source, eps, dest, rel));
                    }
                    else {
                        ret.push_back(WeightedTransition(source, getSymbol(name->name), dest, rel));
                    }
                }
            }
            catch (ReadTransitionException & e) {
                auto const & syms = trans.symbols;
                for (auto sym = syms.begin(); sym != syms.end(); ++sym) {
                    auto name = dynamic_cast<xfa_parser::Name*>(sym->get());
                    assert(name->name != "epsilon");

                    // source ---> intermediate_name ---> dest
                    //                               <---
                    std::stringstream intermediate_name;
                    intermediate_name << trans.source << "__" << name->name;
                    std::stringstream bit0_name, bit1_name;;
                    bit0_name << name->name << "__bit_is_0";
                    bit1_name << name->name << "__bit_is_1";
                    Symbol startbits = getSymbol("__startbits");
                    Symbol bit0 = getSymbol(bit0_name.str());
                    Symbol bit1 = getSymbol(bit1_name.str());
                    State intermediate = getState(intermediate_name.str());

                    // source --> intermediate has identity weight, symbol '__startbits'
                    assert (e.init_weight.get_ptr());
                    ret.push_back(WeightedTransition(source, startbits, intermediate, e.init_weight));

                    // intermediate --> dest has two transitions:
                    //     '__bit_0' has identity weight
                    //     '__bit_1' has +1 weight
                    ret.push_back(WeightedTransition(intermediate, bit0, dest, new BinRel(&voc, ident)));
                    ret.push_back(WeightedTransition(intermediate, bit1, dest, e.bit1_weight));

                    // dest --> intermediate has epsilon and weight *2
                    ret.push_back(WeightedTransition(dest, eps, intermediate, e.back_weight));
                }
            }

            return ret;
        }


        Xfa
        from_parser_ast(xfa_parser::Xfa const & ast,
                        wali::domains::binrel::ProgramBddContext & voc,
                        int fdd_size,
                        std::string const & domain_var_name_prefix)
        {
            register_vars(ast, voc, fdd_size, domain_var_name_prefix);

            using namespace wali::domains::binrel;
            BinaryRelation zero = new BinRel(&voc, voc.False());
            bdd ident = voc.Assume(voc.True(), voc.True());
            Symbol eps(wali::WALI_EPSILON);
            
            wali::xfa::Xfa ret;

            for (auto ast_state = ast.states.begin(); ast_state != ast.states.end(); ++ast_state) {
                State state = getState((*ast_state)->name);
                ret.addState(state, zero);
            }

            for (auto ast_trans = ast.transitions.begin(); ast_trans != ast.transitions.end(); ++ast_trans) {
                TransList transs = get_transitions(**ast_trans, voc, zero, ident, domain_var_name_prefix);
                for (auto trans = transs.begin(); trans != transs.end(); ++trans) {
                    ret.addTrans(trans->source, trans->symbol, trans->target, trans->weight);
                }
            }

            ret.setInitialState(getState(ast.start_state));

            return ret;
        }
    }
}

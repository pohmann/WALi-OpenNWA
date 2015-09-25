// This program will walk through creating the NWA shown in Figure 3 of the
// associated documentation, and reverse it (to get the result of Figure 11).
// We then make a NestedWord of the one word in Figure 3's language and
// another NestedWord of the one word in Figure 11's language, then test that
// each is a member of just the appropriate NWA.

#include <iostream>

using std::cout;

#include "opennwa/Nwa.hpp"
#include "opennwa/NestedWord.hpp"
#include "opennwa/construct/reverse.hpp"
#include "opennwa/query/language.hpp"

using wali::getKey;
using opennwa::Nwa;
using opennwa::State;
using opennwa::Symbol;
using opennwa::NwaRefPtr;
using opennwa::NestedWord;
using opennwa::construct::reverse;
using opennwa::query::languageContains;

// These symbols are used in the NWA and both words, so I'll make them
// global.
Symbol const sym_a    = getKey("a");
Symbol const sym_b    = getKey("b");
Symbol const sym_call = getKey("call");
Symbol const sym_ret  = getKey("ret");



/// Creates the NWA shown in Figure 3 of the Wali NWA documentation, storing
/// it in 'out'.
Nwa
create_figure_3()
{
    Nwa out;
    
    // Translate the names of the states then symbols to Wali identifiers
    State start   = getKey("Start");
    State call    = getKey("Call");
    State entry   = getKey("Entry");
    State state   = getKey("State");
    State exit    = getKey("Exit");
    State return_ = getKey("Return");
    State finish  = getKey("Finish");

    // Add the transitions
    out.addInternalTrans(start, sym_a, call);
    out.addCallTrans(call, sym_call, entry);
    out.addInternalTrans(entry, sym_b, state);
    out.addInternalTrans(state, sym_b, exit);
    out.addReturnTrans(exit, call, sym_ret, return_);
    out.addInternalTrans(return_, sym_a, finish);

    // Set the initial and final states
    out.addInitialState(start);
    out.addFinalState(finish);

    return out;
}


///
/// Creates a (the one) word in the language of Figure 3's NWA
NestedWord
create_forwards_word()
{
    NestedWord out;
    out.appendInternal(sym_a);
    out.appendCall(sym_call);
    out.appendInternal(sym_b);
    out.appendInternal(sym_b);
    out.appendReturn(sym_ret);
    out.appendInternal(sym_a);

    return out;
}


/// Creates a (the one) word in the language of the reverse of Figure 3's NWA
/// (i.e. that's in Figure 11's NWA).
NestedWord
create_backwards_word()
{
    NestedWord out;
    out.appendInternal(sym_a);
    out.appendCall(sym_ret);
    out.appendInternal(sym_b);
    out.appendInternal(sym_b);
    out.appendReturn(sym_call);
    out.appendInternal(sym_a);

    return out;
}




int main()
{
    // Create the NWA
    Nwa fig3 = create_figure_3();

    // Reverse it
    NwaRefPtr fig3_reversed = reverse(fig3);

    // These are the words we are testing
    NestedWord forwards_word = create_forwards_word();
    NestedWord backwards_word = create_backwards_word();

    // Now do the tests
    cout << "fig3 contains:\n"
         << "     forwards_word  [expect 1] : " << languageContains(fig3, forwards_word) << "\n"
         << "     backwards_word [expect 0] : " << languageContains(fig3, backwards_word) << "\n"
         << "fig3_reversed contains:\n"
         << "     forwards_word  [expect 0] : " << languageContains(*fig3_reversed, forwards_word) << "\n"
         << "     backwards_word [expect 1] : " << languageContains(*fig3_reversed, backwards_word) << "\n";
    
    return 0;
}

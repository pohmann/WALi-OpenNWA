#include "gtest/gtest.h"

#include "wali/nwa/NWA.hpp"

#include "Tests/nwa/Source/fixtures.hpp"
#include "Tests/nwa/Source/class-NWA/supporting.hpp"

namespace wali
{
    namespace nwa
    {
        //////////////////////////////////
        // Supporting stuff

        
        /// All of these functions add 'state' to 'nwa', and checks that it
        /// is present in the appropriate sets and only those sets. (Symbol
        /// does the same with 'symbol'.) 'expect_addition' controls whether
        /// the state should be added.

        void
        add_to_states_and_check(NWA * nwa, State state, bool expect_addition)
        {
            size_t original_size_trans = nwa->sizeTrans();
            
            EXPECT_EQ(expect_addition, nwa->addState(state));

            EXPECT_TRUE(nwa->isState(state));
            EXPECT_FALSE(nwa->isInitialState(state));
            EXPECT_FALSE(nwa->isFinalState(state));
            EXPECT_FALSE(nwa->isSymbol(state));

            EXPECT_EQ(original_size_trans, nwa->sizeTrans());
        }
        
        void
        add_to_initial_states_and_check(NWA * nwa, State state, bool expect_addition)
        {
            size_t original_size_trans = nwa->sizeTrans();
            
            EXPECT_EQ(expect_addition, nwa->addInitialState(state));

            EXPECT_TRUE(nwa->isState(state));
            EXPECT_TRUE(nwa->isInitialState(state));
            EXPECT_FALSE(nwa->isFinalState(state));
            EXPECT_FALSE(nwa->isSymbol(state));

            EXPECT_EQ(original_size_trans, nwa->sizeTrans());
        }

        void
        add_to_final_states_and_check(NWA * nwa, State state, bool expect_addition)
        {
            size_t original_size_trans = nwa->sizeTrans();
            
            EXPECT_EQ(expect_addition, nwa->addFinalState(state));

            EXPECT_TRUE(nwa->isState(state));
            EXPECT_FALSE(nwa->isInitialState(state));
            EXPECT_TRUE(nwa->isFinalState(state));
            EXPECT_FALSE(nwa->isSymbol(state));

            EXPECT_EQ(original_size_trans, nwa->sizeTrans());
        }
        
        void
        add_to_symbols_and_check(NWA * nwa, Symbol symbol, bool expect_addition)
        {
            size_t original_size_trans = nwa->sizeTrans();
            
            EXPECT_EQ(expect_addition, nwa->addSymbol(symbol));

            EXPECT_FALSE(nwa->isState(symbol));
            EXPECT_FALSE(nwa->isInitialState(symbol));
            EXPECT_FALSE(nwa->isFinalState(symbol));
            EXPECT_TRUE(nwa->isSymbol(symbol));

            EXPECT_EQ(original_size_trans, nwa->sizeTrans());
        }
        

        /// Checks that, in 'nwa', the sizes returned by 'sizeBlah()' is the
        /// same as the number of elements between 'beginBlah()' and
        /// 'endBlah()'.
        void expect_size_consistent_with_range(NWA const & nwa)
        {
#define CHECK_CONSISTENCY(Attribute)                                             \
            EXPECT_EQ(nwa.size##Attribute(),                                     \
                      static_cast<size_t>(std::distance(nwa.begin##Attribute(),  \
                                                        nwa.end##Attribute())))
            
            CHECK_CONSISTENCY(States);
            CHECK_CONSISTENCY(InitialStates);
            CHECK_CONSISTENCY(FinalStates);
            CHECK_CONSISTENCY(Symbols);
            CHECK_CONSISTENCY(InternalTrans);
            CHECK_CONSISTENCY(CallTrans);
            CHECK_CONSISTENCY(ReturnTrans);
#undef CHECK_CONSISTENCY
        }



        
        /////////////////////////////////
        // Now begin the actual tests
        

        // For XXX in each of States, InitialStates, FinalStates, InternalTrans,
        // CallTrans, ReturnTrans:
        // 
        //   getXXX()
        //     - I don't think we can really do anything with this separately. This
        //       is tested in concert with other functions except as follows:
        // 
        //     - For getSymbols(), check in an automaton that uses epsilon and wild on
        //       transitions. Make sure that neither is present in the given set.
        TEST(wali$nwa$NWA$getSymbols, epsilonAndWildAreNotPresent)
        {
            NWA nwa;
            Key q = getKey('q');
            nwa.addInternalTrans(q, WALI_EPSILON, q);
            nwa.addInternalTrans(q, WALI_WILD, q);

            // Sanity check
            EXPECT_EQ(2u, nwa.sizeInternalTrans());

            // Actual check (see checkSizeOfEmptyIsConsistent below)
            EXPECT_EQ(0u, nwa.sizeSymbols());
        }


        //////////////////
        //   sizeXXX()
        //     - Make sure this agrees with the size of getXXX()       
        TEST(wali$nwa$NWA$getXXX, checkSizeConsistentForEmptyNwa)
        {
            NWA empty;
            expect_size_consistent_with_range(empty);
        }

        TEST(wali$nwa$NWA$getXXX, checkSizeConsistentForBigNwa)
        {
            OddNumEvenGroupsNwa fixture;
            expect_size_consistent_with_range(fixture.nwa);
        }


        //////////////////
        //   isXXX() and addXXX() for states and symbols
        //
        //     - Check if an XXX is a member of the empty NWA       
        TEST(wali$nwa$NWA$isXXX, checkNothingMemberOfEmpty)
        {
            NWA empty;
            SomeElements::expect_not_present(empty);
        }

        //     - Add unrelated items of each type; check.
        TEST(wali$nwa$NWA$isXXX, checkNothingMemberOfStuff)
        {
            OddNumEvenGroupsNwa fixture;
            SomeElements::expect_not_present(fixture.nwa);
        }

        
        //     - Add item being checked to an empty NWA; check
        //     - Check that adding an XXX adds it to the set returned by
        //       getXXX and returns true       
        TEST(wali$nwa$NWA$isState$$and$addState, addToEmptyAndCheck)
        {
            NWA empty;
            SomeElements e;
            add_to_states_and_check(&empty, e.state, true);
        }
        
        TEST(wali$nwa$NWA$isInitialState$$and$addInitialState, addToEmptyAndCheck)
        {
            NWA empty;
            SomeElements e;
            add_to_initial_states_and_check(&empty, e.state, true);
        }

        TEST(wali$nwa$NWA$isFinalState$$and$addFinalState, addToEmptyAndCheck)
        {
            NWA empty;
            SomeElements e;
            add_to_final_states_and_check(&empty, e.state, true);
        }

        TEST(wali$nwa$NWA$isSymbol$$and$addSymbol, addToEmptyAndCheck)
        {
            NWA empty;
            SomeElements e;
            add_to_symbols_and_check(&empty, e.symbol, true);
        }

        
        //     - Add both unrelated and checked item, then more unrelated
        //       items; check. (I'm not doing the "then more
        //       unrelated". TODO?)
        TEST(wali$nwa$NWA$isState$$and$addState, addToBusyAndCheck)
        {
            OddNumEvenGroupsNwa fixture;
            SomeElements e;
            add_to_states_and_check(&fixture.nwa, e.state, true);
        }
        
        TEST(wali$nwa$NWA$isInitialState$$and$addInitialState, addToBusyAndCheck)
        {
            OddNumEvenGroupsNwa fixture;
            SomeElements e;
            add_to_initial_states_and_check(&fixture.nwa, e.state, true);
        }

        TEST(wali$nwa$NWA$isFinalState$$and$addFinalState, addToBusyAndCheck)
        {
            OddNumEvenGroupsNwa fixture;
            SomeElements e;
            add_to_final_states_and_check(&fixture.nwa, e.state, true);
        }

        TEST(wali$nwa$NWA$isSymbol$$and$addSymbol, addToBusyAndCheck)
        {
            OddNumEvenGroupsNwa fixture;
            SomeElements e;
            add_to_symbols_and_check(&fixture.nwa, e.symbol, true);
        }

        
        //     - For isSymbol(), make sure epsilon and wild return false both when they
        //       are used in transitions and not.
        // This is done inline with the above
        

        //   addXXX()
        //     - Check that adding it again leaves the return from getXXX
        //       unchanged, and returns false
        TEST(wali$nwa$NWA$addState, addTwiceToEmptyAndCheck)
        {
            NWA nwa;
            SomeElements e;
            add_to_states_and_check(&nwa, e.state, true);

            NWA copy = nwa;
            add_to_states_and_check(&nwa, e.state, false);
            
            expect_nwas_are_equal(nwa, copy);
        }
        
        TEST(wali$nwa$NWA$addInitialState, addTwiceToEmptyAndCheck)
        {
            NWA nwa;
            SomeElements e;
            add_to_initial_states_and_check(&nwa, e.state, true);

            NWA copy = nwa;
            add_to_initial_states_and_check(&nwa, e.state, false);

            expect_nwas_are_equal(nwa, copy);
        }

        TEST(wali$nwa$NWA$addFinalState, addTwiceToNwaAndCheck)
        {
            NWA nwa;
            SomeElements e;
            add_to_final_states_and_check(&nwa, e.state, true);

            NWA copy = nwa;
            add_to_final_states_and_check(&nwa, e.state, false);

            expect_nwas_are_equal(nwa, copy);
        }

        TEST(wali$nwa$NWA$addSymbol, addTwiceToEmptyAndCheck)
        {
            NWA nwa;
            SomeElements e;
            add_to_symbols_and_check(&nwa, e.symbol, true);

            NWA copy = nwa;
            add_to_symbols_and_check(&nwa, e.symbol, false);

            expect_nwas_are_equal(nwa, copy);
        }

        
        //     - Check that the sets that should not be modified are not
        //       modified.
        //
        // Done inline, mostly. (I don't check that transitions are unmodified.)

        
        //     - For addInitialState and addFinalState, try from both the
        //       situation where the state was and wasn't present in the NWA
        //       at all. If they weren't present, check that they are after
        //       the addition.
        //
        // The above checks the situation where it was not present, and
        // checks that it was added.
        TEST(wali$nwa$NWA$addInitialState, addInitialStateOfAStateAlreadyPresent)
        {
            NWA nwa;
            SomeElements e;
            EXPECT_TRUE(nwa.addState(e.state));

            // Sanity checks; these should hold from above
            EXPECT_TRUE(nwa.isState(e.state));
            EXPECT_FALSE(nwa.isInitialState(e.state));

            // ** Now the real thing. Add two initial states.
            EXPECT_TRUE(nwa.addInitialState(e.state));
            EXPECT_TRUE(nwa.addInitialState(e.state2));

            // ** Make sure both are initial states
            EXPECT_TRUE(nwa.isInitialState(e.state));
            EXPECT_TRUE(nwa.isInitialState(e.state2));

            EXPECT_EQ(2u, nwa.sizeInitialStates());
        }
        

        //     - For addSymbol(), make sure adding epsilon or wild returns false even if
        //       they are the first things done
        TEST(wali$nwa$NWA$addSymbol, addingEpsilonOrWildShouldFail)
        {
            NWA nwa;

            EXPECT_FALSE(nwa.addSymbol(WALI_EPSILON));
            EXPECT_FALSE(nwa.addSymbol(WALI_WILD));
        }

        
        //   removeXXX()
        //     - Try from empty automaton, one containing given item, one not
        //       containing given item. Check correct value is returned.
        TEST(wali$nwa$NWA$removeXXX, removingFromEmptyShouldFail)
        {
            NWA nwa;
            SomeElements e;

            EXPECT_FALSE(nwa.removeState(e.state));
            EXPECT_FALSE(nwa.removeInitialState(e.state));
            EXPECT_FALSE(nwa.removeFinalState(e.state));
            EXPECT_FALSE(nwa.removeSymbol(e.symbol));
            EXPECT_FALSE(nwa.removeInternalTrans(e.internal));
            EXPECT_FALSE(nwa.removeCallTrans(e.call));
            EXPECT_FALSE(nwa.removeReturnTrans(e.ret));
        }

        TEST(wali$nwa$NWA$removeXXX, removingSingleItemsShouldLeaveEmpty)
        {
            NWA nwa;
            SomeElements e;
            SomeElements::add_to_nwa(&nwa);

            // Couple sanity checks to make sure the SomeElements NWA is what
            // I think it is.
            ASSERT_TRUE(nwa.isInitialState(e.state));
            ASSERT_TRUE(nwa.isFinalState(e.state2));

            // Have to remove transitions first so they don't remove states
            EXPECT_TRUE(nwa.removeInternalTrans(e.internal));
            EXPECT_EQ(0u, nwa.sizeInternalTrans());
            EXPECT_EQ(1u, nwa.sizeCallTrans());
            EXPECT_EQ(1u, nwa.sizeReturnTrans());

            EXPECT_TRUE(nwa.removeCallTrans(e.call));
            EXPECT_EQ(0u, nwa.sizeInternalTrans());
            EXPECT_EQ(0u, nwa.sizeCallTrans());
            EXPECT_EQ(1u, nwa.sizeReturnTrans());

            EXPECT_TRUE(nwa.removeReturnTrans(e.ret));
            EXPECT_EQ(0u, nwa.sizeInternalTrans());
            EXPECT_EQ(0u, nwa.sizeCallTrans());
            EXPECT_EQ(0u, nwa.sizeReturnTrans());

            // Check up on stuff.
            EXPECT_EQ(0u, nwa.sizeTrans());
            EXPECT_EQ(1u, nwa.sizeSymbols());
            EXPECT_EQ(1u, nwa.sizeInitialStates());
            EXPECT_EQ(1u, nwa.sizeFinalStates());
            EXPECT_EQ(3u, nwa.sizeStates());

            // Remove symbol
            EXPECT_TRUE(nwa.removeSymbol(e.symbol));
            EXPECT_EQ(0u, nwa.sizeSymbols());

            // Have to remove initial and final first so removing a state
            // doesn't disturb anything. 
            EXPECT_TRUE(nwa.removeInitialState(e.state));
            EXPECT_EQ(0u, nwa.sizeInitialStates());
            EXPECT_EQ(1u, nwa.sizeFinalStates());
            EXPECT_EQ(3u, nwa.sizeStates());

            EXPECT_TRUE(nwa.removeFinalState(e.state2));
            EXPECT_EQ(0u, nwa.sizeInitialStates());
            EXPECT_EQ(0u, nwa.sizeFinalStates());
            EXPECT_EQ(3u, nwa.sizeStates());

            EXPECT_TRUE(nwa.removeState(e.state));
            EXPECT_EQ(2u, nwa.sizeStates());
            
            EXPECT_TRUE(nwa.removeState(e.state2));
            EXPECT_EQ(1u, nwa.sizeStates());

            EXPECT_TRUE(nwa.removeState(e.state3));

            expect_nwa_is_empty(nwa);
        }

        TEST(wali$nwa$NWA$removeXXX, removingItemNotInNwaShouldLeaveItUnchanged)
        {
            OddNumEvenGroupsNwa fixture;
            NWA nwa = fixture.nwa;
            SomeElements e;

            EXPECT_FALSE(nwa.removeInternalTrans(e.internal));
            EXPECT_FALSE(nwa.removeCallTrans(e.call));
            EXPECT_FALSE(nwa.removeReturnTrans(e.ret));
            EXPECT_FALSE(nwa.removeSymbol(e.symbol));
            EXPECT_FALSE(nwa.removeInitialState(e.state));
            EXPECT_FALSE(nwa.removeFinalState(e.state2));
            EXPECT_FALSE(nwa.removeState(e.state));
            EXPECT_FALSE(nwa.removeState(e.state2));
            EXPECT_FALSE(nwa.removeState(e.state3));

            expect_nwas_are_equal(fixture.nwa, nwa);
        }

        TEST(wali$nwa$NWA$removeXXX, removingItemWhenThereIsMoreThanOneOfThatKindLeavesTheOthers)
        {
            // TODO: this test doesn't really test that the right transitions
            // were removed.
            
            OddNumEvenGroupsNwa fixture;
            NWA nwa = fixture.nwa;

            // We'll add a couple more for good fun
            ASSERT_TRUE(nwa.addInitialState(fixture.q2));
            ASSERT_TRUE(nwa.addFinalState(fixture.q0));

            // TheNwa transitions first
            EXPECT_EQ(4u, nwa.sizeInternalTrans());
            EXPECT_TRUE(nwa.removeInternalTrans(fixture.q2, fixture.zero, fixture.q3));
            EXPECT_EQ(3u, nwa.sizeInternalTrans());
            EXPECT_TRUE(nwa.removeInternalTrans(fixture.q2, WALI_EPSILON, fixture.dummy));
            EXPECT_EQ(2u, nwa.sizeInternalTrans());

            EXPECT_EQ(3u, nwa.sizeCallTrans());
            EXPECT_TRUE(nwa.removeCallTrans(fixture.q0, fixture.call, fixture.q2));
            EXPECT_EQ(2u, nwa.sizeCallTrans());

            EXPECT_EQ(3u, nwa.sizeReturnTrans());
            EXPECT_TRUE(nwa.removeReturnTrans(fixture.q3, fixture.q1, fixture.ret, fixture.q0));
            EXPECT_EQ(2u, nwa.sizeReturnTrans());

            // Symbols
            EXPECT_EQ(3u, nwa.sizeSymbols());
            EXPECT_TRUE(nwa.removeSymbol(fixture.zero)); // **
            EXPECT_EQ(2u, nwa.sizeSymbols());
            EXPECT_TRUE(nwa.removeSymbol(fixture.call)); // **
            EXPECT_EQ(1u, nwa.sizeSymbols());

            EXPECT_FALSE(nwa.isSymbol(fixture.zero));
            EXPECT_FALSE(nwa.isSymbol(fixture.call));
            EXPECT_TRUE(nwa.isSymbol(fixture.ret));

            // Initial states
            EXPECT_EQ(2u, nwa.sizeInitialStates());
            EXPECT_EQ(2u, nwa.sizeFinalStates());
            
            EXPECT_TRUE(nwa.removeInitialState(fixture.q0)); // **
            
            EXPECT_EQ(1u, nwa.sizeInitialStates());
            EXPECT_EQ(2u, nwa.sizeFinalStates());

            // Final states          
            EXPECT_TRUE(nwa.removeFinalState(fixture.q1)); // **
            EXPECT_EQ(1u, nwa.sizeInitialStates());
            EXPECT_EQ(1u, nwa.sizeFinalStates());

            EXPECT_TRUE(nwa.isInitialState(fixture.q2));
            EXPECT_TRUE(nwa.isFinalState(fixture.q0));

            // States
            EXPECT_EQ(5u, nwa.sizeStates());
            EXPECT_TRUE(nwa.removeState(fixture.dummy)); // **
            EXPECT_EQ(4u, nwa.sizeStates());
            EXPECT_FALSE(nwa.isState(fixture.dummy));
        }
        
        //     - In each case, check that sets that shouldn't change are not modified.
        //
        // This has kinda been done all along...

        //     - For removeState(), make sure that removing a state that is
        //       an initial and/or final state works correctly
        TEST(wali$nwa$NWA$removeState, removingAStateRemovesFromInitialsAndFinals)
        {
            OddNumEvenGroupsNwa fixture;

            ASSERT_TRUE(fixture.nwa.addInitialState(fixture.q2)); // q0 and q2 are inital
            ASSERT_TRUE(fixture.nwa.addFinalState(fixture.q0)); // q1 and q1 are accepting
            ASSERT_EQ(2u, fixture.nwa.sizeInitialStates());
            ASSERT_EQ(2u, fixture.nwa.sizeFinalStates());

            // Remove just an initial state
            EXPECT_TRUE(fixture.nwa.removeState(fixture.q2));
            EXPECT_EQ(1u, fixture.nwa.sizeInitialStates());
            EXPECT_EQ(2u, fixture.nwa.sizeFinalStates());

            // Remove just a accepting state
            EXPECT_TRUE(fixture.nwa.removeState(fixture.q1));
            EXPECT_EQ(1u, fixture.nwa.sizeInitialStates());
            EXPECT_EQ(1u, fixture.nwa.sizeFinalStates());

            // Remove a state that is both initial and accepting
            EXPECT_TRUE(fixture.nwa.removeState(fixture.q0));
            EXPECT_EQ(0u, fixture.nwa.sizeInitialStates());
            EXPECT_EQ(0u, fixture.nwa.sizeFinalStates());
        }

        //     - For removeState(), try from automaton in which the state is
        //       and is not involved in any transitions. Make sure you get
        //       the state in all applicable coordinates: source/target of
        //       internal, call/entry of call, and exit/pred/return of
        //       return.
        TEST(wali$nwa$NWA$removeState, removingAStateRemovesAssociatedTransitions)
        {
            OddNumEvenGroupsNwa fixture;

            // Add some extra stuff so we can distinguish removing stuff from
            // clearing transitions.
            ASSERT_TRUE(fixture.nwa.addInternalTrans(fixture.q0, WALI_EPSILON, fixture.q1));
            ASSERT_TRUE(fixture.nwa.addInternalTrans(fixture.q0, fixture.zero, fixture.q1));
            ASSERT_TRUE(fixture.nwa.addCallTrans(fixture.q0, fixture.call, fixture.q1));

            // Add one transition to get q2 as the predecessor of a return
            // and one to get it as the target of a return
            ASSERT_TRUE(fixture.nwa.addReturnTrans(fixture.q0, fixture.q2, fixture.zero, fixture.q3));
            ASSERT_TRUE(fixture.nwa.addReturnTrans(fixture.q0, fixture.q1, fixture.zero, fixture.q2));

            ASSERT_EQ(6u, fixture.nwa.sizeInternalTrans());
            ASSERT_EQ(4u, fixture.nwa.sizeCallTrans());
            ASSERT_EQ(5u, fixture.nwa.sizeReturnTrans());

            // This removes transitions where q2 is the:
            //
            // Internal: source      (q2 ---> q3 on zero, and q2 ---> dummy on epsilon and wild)
            //           target      (q3 ---> q2 on zero)
            // Call:     source      (q2 ---> dummy)
            //           target      (q0 ---> q2 on call, q1 ---> q2 on call)
            // Return:   source      (q2 ---> dummy on ret (q1 pred))
            //           predecessor (q0 ---> q3 on zero (q2 pred); this was added here)
            //           target      (q0 ---> q2 on zero (q1 pred); this was added here)
            EXPECT_TRUE(fixture.nwa.removeState(fixture.q2));

            // We removed 4 internal, 3 call, and 3 return transitions
            EXPECT_EQ(2u, fixture.nwa.sizeInternalTrans());
            EXPECT_EQ(1u, fixture.nwa.sizeCallTrans());
            EXPECT_EQ(2u, fixture.nwa.sizeReturnTrans());

            
            ///////
            // 'dummy' is no longer involved in any transitions. Removing it
            // should not change anything except the set of states. We'll
            // test this by making a copy of the current NWA, then removing
            // 'dummy' and reinserting it.
            NWA copy = fixture.nwa;
            
            EXPECT_TRUE(fixture.nwa.removeState(fixture.dummy));

            // Some extra checks...
            EXPECT_EQ(2u, fixture.nwa.sizeInternalTrans());
            EXPECT_EQ(1u, fixture.nwa.sizeCallTrans());
            EXPECT_EQ(2u, fixture.nwa.sizeReturnTrans());

            // Reinsert dummy
            EXPECT_TRUE(fixture.nwa.addState(fixture.dummy));
            expect_nwas_are_equal(copy, fixture.nwa);
        }

        
        //     - For removeSymbol(), try from automaton in which the symbol is and is not
        //       involved in any transtions. Test all three kinds of transitions.
        // 
        //   clearXXX()
        //     - Make sure that the set of XXX is empty after.
        //     - Make sure that sets that shouldn't be different are not different.
        // 
        //     - For clearStates(), make sure initial/accepting states and transitions
        //       have all been cleared too.
        //     - For clearSymbols(), make sure transitions are all clear too.

        
        // TODO: transition functions. In particular:
        //
        //     - For addYYYTrans, try from the situation where the states and symbol are
        //       and are not present. If they were not, check that they are after the
        //       addition.
        //     - For addYYYTrans, add more than one transition with the given source/sym,
        //       source/tgt, and sym/tgt.
        // 
    }
}
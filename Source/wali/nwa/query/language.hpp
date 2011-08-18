#ifndef WALI_NWA_QUERY_LANGUAGE_HPP
#define WALI_NWA_QUERY_LANGUAGE_HPP

#include "wali/nwa/NWAFwd.hpp"
#include "wali/nwa/NestedWord.hpp"

namespace wali {
  namespace nwa {
    namespace query {

      /// @brief Determines whether word is in the language of the given NWA.
      ///
      /// @returns true if 'word' is in L(this), and false otherwise.
      bool
      languageContains(NWA const & nwa, NestedWord const & word);


      /**
       * @brief tests whether the language of the first NWA is included in the language of 
       *        the second NWA
       *
       * This method tests whether the language of the first NWA is included in the language
       * of the second NWA.
       *
       * @param - first: the proposed subset
       * @param - second: the proposed superset
       * @return true if the language of the first NWA is included in the language of the 
       *          second NWA, false otherwise
       *
       */
      bool
      languageSubsetEq(NWA const & left, NWA const & right);


      /**
       *
       * @brief tests whether the language accepted by this NWA is empty
       *
       * This method tests whether the language accepted by this NWA is empty.
       *
       * @return true if the language accepted by this NWA is empty
       *
       */
      bool
      languageIsEmpty(NWA const & nwa);


      /**
       *
       * @brief Returns some word accepted by 'nwa'
       *
       * If there is not such a word (L(nwa)={}), returns the empty
       * string. (There is no way to distinguish "empty language" from
       * "here's epsilon; it's in the language" from just this call; use
       * languageContains to make that determination.)
       *
       * @return A word accepted by 'nwa'
       *
       */
      extern
      NestedWord
      getSomeAcceptedWord(NWA const & nwa);





      /**
       *
       * @brief tests whether the languages of the given NWAs are equal
       *
       * This method tests the equivalence of the languages accepted by the given NWAs.
       *
       * @param - first: one of the NWAs whose language to test
       * @param - second: one of the NWAs whose language to test
       * @return true if the languages accepted by the given NWAs are equal, false otherwise
       *
       */
      bool
      languageEquals(NWA const & first, NWA const & second);
      
    }
  }
}


// Yo, Emacs!
// Local Variables:
//   c-file-style: "ellemtel"
//   c-basic-offset: 2
// End:

#endif

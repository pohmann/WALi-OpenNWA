#ifndef wali_wpds_ewpds_ETRANS_GUARD
#define wali_wpds_ewpds_ETRANS_GUARD 1

/*!
 * @author Nick Kidd
 */

#include "wali/wfa/ITrans.hpp"
#include "wali/wfa/DecoratorTrans.hpp"
#include "wali/wpds/ewpds/MergeFunction.hpp"
#include "wali/wpds/ewpds/ERule.hpp"

namespace wali {
  namespace wpds {
    namespace ewpds {
      class ETrans : public ::wali::wfa::DecoratorTrans {

        public:
          ETrans(
              Key from, Key stack, Key to,
              sem_elem_t wAtCall, //!< Weight on path to the call transition
              sem_elem_t wAfterCall, //!< For call rule R, wAtCall->extend(R->weight())
              erule_t erule //!< The ERule
              );

          ETrans(
              ITrans* d,          //!< Trans that is being decorated
              sem_elem_t wAtCall, //!< Weight on path to the call transition
              erule_t erule //!< The merge function
              );

          virtual ~ETrans();

          /*! @return the merge function of this ETrans */
          merge_fn_t getMergeFn() const;

          erule_t getERule() const;
          /*! @return A <b>deep</b> copy of this */
          virtual ITrans* copy() const;

          /*!
           * Override to apply the merge function, i.e.,
           * 
           *   >>>   mf->apply_f(wAtCall,se)
           */
          virtual sem_elem_t poststar_eps_closure( sem_elem_t se );

          /*!
           * Override Trans::combineTrans to recover
           * the new value for wAtCall.
           *
           * Note: Trans* tp should <b>always</b> be an ETrans.
           */
          virtual void combineTrans( ITrans* tp );

        protected:
          sem_elem_t wAtCall;
          erule_t erule;

      }; // class ETrans
    } // namespace ewpds
  } // namespace wpds
} // namespace wali
#endif  // wali_wpds_ewpds_ETRANS_GUARD

/* Yo, Emacs!
   ;;; Local Variables: ***
   ;;; tab-width: 2 ***
   ;;; End: ***
*/


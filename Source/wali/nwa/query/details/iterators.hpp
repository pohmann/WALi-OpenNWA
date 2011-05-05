#ifndef wali_nwa_query_details_ITERATORS_HPP
#define wali_nwa_query_details_ITERATORS_HPP

#include "wali/nwa/NWA.hpp"

#include <iterator>
#include <utility>
#include <algorithm>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/shared_ptr.hpp>

namespace wali
{
  namespace nwa
  {
    namespace query
    {
      namespace details
      {

        namespace iterators
        {

          /// The following adapts any iterator with the same element type so
          /// that the iterator can be used
          template<typename ValueType>
          class iterator_base
            : public boost::iterator_facade<iterator_base<ValueType>,
                                            ValueType,
                                            boost::forward_traversal_tag>
          {
          protected:
            friend class boost::iterator_core_access;
           
            virtual void increment() = 0;
            virtual ValueType & dereference() const = 0;

          public:
            virtual bool equal(iterator_base const & other) const = 0;
            virtual iterator_base* clone() const = 0;
          };
          

          template<typename CPlusPlusIterator>
          class cplusplus_iterator_adapter
            : public iterator_base<typename std::iterator_traits<CPlusPlusIterator>::value_type>
          {
          private:
            CPlusPlusIterator backing;
            typedef typename std::iterator_traits<CPlusPlusIterator>::value_type ValueType;

          public:
            cplusplus_iterator_adapter()
            {}

            cplusplus_iterator_adapter(cplusplus_iterator_adapter const & other)
              : backing(other.backing)
            {}

            explicit cplusplus_iterator_adapter(CPlusPlusIterator iter)
              : backing(iter)
            {}

            
            virtual cplusplus_iterator_adapter* clone() const
            {
              return new cplusplus_iterator_adapter(*this);
            }


            virtual void increment()
            {
              ++backing;
            }

            virtual bool equal(iterator_base<ValueType> const & other) const
            {
              const cplusplus_iterator_adapter * p =
                dynamic_cast<cplusplus_iterator_adapter const*>(&other);
              
              if (p) {
                return backing == p->backing;
              }
              else {
                return false;
              }
            }

            ValueType & dereference() const
            {
              return *backing;
            }
          };
          


          template<typename ValueType>
          class iterator_base_wrapper
            : public boost::iterator_facade<iterator_base_wrapper<ValueType>,
                                            ValueType,
                                            boost::forward_traversal_tag>
          {
          private:
            // iterator_base_wrapper acts as the RAII class for this. I
            // really want a cloning smart pointer, but there isn't one
            // standard. So I'll do it myself.
            iterator_base<ValueType> * backing;

          public:
            iterator_base_wrapper()
              : backing(0)
            {}

            iterator_base_wrapper(iterator_base_wrapper const & other)
              : backing(other.backing->clone())
            {}

            explicit iterator_base_wrapper(iterator_base<ValueType> const & base)
              : backing(base.clone())
            {}

            /////////////////////////////
            // Functions to support RAII

          private:
            void swap(iterator_base_wrapper & other) const // throw()
            {
              std::swap(backing, other.backing);
            }

          public:
            iterator_base_wrapper & operator=(iterator_base_wrapper const & other)
            {
              // Copy-and-swap
              iterator_base_wrapper copy(other);
              swap(copy);
              return *this;
            }

            ~iterator_base_wrapper()
            {
              delete backing;
            }

            
          private:
            friend class boost::iterator_core_access;
            
            ////////////////////////////////
            // Iterator functions
            virtual void increment()
            {
              ++(*backing);
            }

            virtual bool equal(iterator_base_wrapper const & other) const
            {
              return backing->equal(*other.backing);
            }

            ValueType & dereference() const
            {
              return **backing;
            }
          };


          template <typename BackingIterator>
          class iterator_sequence
            : public boost::iterator_facade<iterator_sequence<BackingIterator>,
                                            typename std::iterator_traits<BackingIterator>::value_type,
                                            boost::forward_traversal_tag>
          {
          public:
            typedef std::pair<BackingIterator, BackingIterator> IteratorRange;
            typedef typename std::iterator_traits<BackingIterator>::value_type ValueType;

          private:
            boost::shared_ptr<std::deque<IteratorRange> > ranges;

            void dump_empty_ranges()
            {
              while (!ranges->empty() && ranges->front().first == ranges->front().second) {
                ranges->pop_front();
              }
            }

          protected:
            friend class boost::iterator_core_access;

            void increment()
            {
              // 'current' is ranges[0].first, so increment it
              ++(ranges->front().first);
              dump_empty_ranges();
            }

            ValueType & dereference() const
            {
              return *(ranges->front().first);
            }

            bool equal(iterator_sequence const & other) const
            {
              if (ranges->empty() && other.ranges->empty()) {
                return true;
              }
              else if (ranges->empty() && other.ranges->empty()) {
                return false;
              }
              else {
                return ranges->front().first == other.ranges->front().first;
              }
            }

          public:
            iterator_sequence()
              : ranges(new std::deque<IteratorRange>())
            {}
            
            iterator_sequence(iterator_sequence const & other)
              : ranges(other.ranges)
            {}

            void append_range(BackingIterator left, BackingIterator right)
            {
              if (left != right) {
                ranges->push_back(IteratorRange(left, right));
              }
            }
          };

          
        }
      }
    }
  }
}

// Yo, Emacs!
// Local Variables:
//   c-file-style: "ellemtel"
//   c-basic-offset: 2
// End:

#endif

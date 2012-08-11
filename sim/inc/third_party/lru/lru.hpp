#ifndef _LRU_HPP_
#define _LRU_HPP_ 
 
#include <boost/bimap.hpp> 
#include <boost/bimap/list_of.hpp> 
#include <boost/bimap/unordered_set_of.hpp> 

// Include global exception class
#include "exception.hpp"
#include "utils.h"
 
// Class providing fixed-size (by number of records) 
// LRU-replacement cache of a function with signature 
// V f(K). 
// SET is expected to be one of boost::bimaps::set_of 
// or boost::bimaps::unordered_set_of 
// Optional info can be defined ( by default int )
template < typename K, typename V, typename I = int > class lru_cache
{
    public: 
    typedef K key_type; 
    typedef V value_type;
    typedef I info_type;
 
    typedef boost::bimaps::bimap<
        boost::bimaps::unordered_set_of<key_type>,
        boost::bimaps::list_of<value_type>,
        boost::bimaps::with_info<info_type>
      > container_type; 

    // Default constructor
    lru_cache() : _capacity(256) { } 

    // Constuctor specifies the cached function and 
    // the maximum number of records to be stored. 
    lru_cache(size_t c) : _capacity(c) 
    { 
        assert_and_throw(_capacity != 0, sim_except_fatal("lru capcity shouldn't be zero")); 
    }

    // @func  : operator[] 
    // @brief : Obtain value of the cached function for k
    //          If the key is not found, an exception is thrown and it's the responsibility of caller
    //          to update calculate new value and update the cache.
    value_type operator[](const key_type& k) { 
 
        // Attempt to find existing record 
        const typename container_type::left_iterator it = _container.left.find(k); 
        if (it == _container.left.end()) { 
            // We don't have it: 
            // Throw an exception. It's the responsibility of user to manually call insert now
            throw sim_except(SIM_EXCEPT_EINVAL, "Illegal key");
 
        } else { 
 
            // We do have it: 
            // Update the access record view. 
            _container.right.relocate(_container.right.end(), _container.project_right(it)); 
 
            // Return the retrieved value 
            return it->second; 
        } 
    } 

    // @func  : info_at
    // @brief : Obtain value of the additional info at key k
    //          If the key is not found, an exception is thrown and it's the responsibility of caller
    //          to update calculate new value and update the cache.
    info_type info_at(const key_type& k){
        // Attempt to find existing record 
        const typename container_type::left_iterator it = _container.left.find(k); 
        if (it == _container.left.end()) { 
            // We don't have it: 
            // Throw an exception. It's the responsibility of user to manually call insert now
            throw sim_except(SIM_EXCEPT_EINVAL, "Illegal key");
 
        }
        // Return the retrieved value 
        return it->info; 
    }
 
    // @func  : insert
    // @brief : insert new key-value pair into the cache
    void insert(const key_type& k, const value_type& v, const info_type i = info_type()) { 
    
        assert_and_throw(_container.size() <= _capacity, sim_except_fatal("lru cache size shouldn't grow beyond it's capacity"));

        // If necessary, make space 
        if (_container.size() == _capacity) { 
            // by purging the least-recently-used element 
            _container.right.erase(_container.right.begin()); 
        } 
      
        // Create a new record from the key and the value 
        // bimap's list_view defaults to inserting this at 
        // the list tail (considered most-recently-used). 
        _container.insert(typename container_type::value_type(k,v, i));
    }

    // @func  : clear
    // @brief : clears the cache
    void clear(){
        _container.clear();
    }

    // @func  : set_size
    // @brief : set size
    void set_size(size_t c){
        _capacity = c;
    }

    size_t size(){ return _capacity; }

    private: 
    size_t _capacity; 
    container_type _container; 
}; 
 
#endif

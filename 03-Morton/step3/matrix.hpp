#ifndef MORTON_MATRIX_HPP
#define MORTON_MATRIX_HPP

#include <memory>
#include <cassert>
#include <iterator>

#include "bits.hpp"

namespace morton {
  
 
  // Forward declare the matrix
  template<class T>
  class matrix;

  // Note we inherit from std::iterator<stuff>.
  // This basically ensures our iterator has the right traits to work
  // efficiently with the standard library.
  // See http://en.cppreference.com/w/cpp/iterator/iterator

  // I've decided this should be a random access iterator - i.e. you
  // can jump direct to any valid value. You only need to implement a
  // handful of the methods, the rest follow from code I've done.
  template<class T>
  class matrix_iterator :
    public std::iterator<std::bidirectional_iterator_tag,
			 T, int64_t, T*, T&> {
  public:
    // TODO
    // Default constructor
    matrix_iterator();

    // TODO
    // Get the x/y coordinates of the current element
    uint32_t x() const {
      auto z = _ptr - _start;
      return pack(z);
    }
			
    uint32_t y() const {
      auto z = _ptr - _start;
      return pack(z >> 1);
    }
    
    // Default copy/move c'tor and assign are fine
    // Same for d'tor
    
    // Comparison operators. Note these are inline non-member friend
    // functions.
    friend bool operator==(const matrix_iterator& a, const matrix_iterator& b) {
      return a._ptr == b._ptr;
    }
    // This can be done in terms of the above
    friend bool operator!=(const matrix_iterator& a, const matrix_iterator& b) {
      return !(a == b);
    }
    
    // Dereference operator
    T& operator*() {
      return *_ptr;
    }
    
    // Preincrement operator
    matrix_iterator& operator++() {
      ++_ptr;
      return *this;
    }
    // Predecrement operator
    matrix_iterator& operator--() {
      --_ptr;
      return *this;
    }
    
  private:
    // Construct from a data pointer
    matrix_iterator(T* start, T* pos) : _ptr(pos), _start(start) {
    }
    
    // Other constructors should probably not be publicly visible, so
    // we need to allow matrix<T> access.
    
    friend matrix<T>;

    T* _ptr;
    T* _start;
  };

  
  // 2D square matrix that stores data in Morton order
  
  // This first implementation requires that the size be a power of 2
  // (or zero)
  template<class T>
  class matrix {
  public:
    typedef matrix_iterator<T> iterator;
    typedef matrix_iterator<const T> const_iterator;
    
    matrix() : _rank(0) {
    }
    
    // Design decision - matrix is not resizeable
    matrix(uint32_t r) : _rank(r), _data(new T[r*r]) {
      // Would consider throwing an exception, but these are not in
      // the syllabus!
      assert((r & (r-1)) == 0);
    }

    // Implicit copying is not allowed - must use explicit function
    matrix(const matrix& other) = delete;
    matrix& operator=(const matrix& other) = delete;

    // Moving is allowed - default's ok because of choice to use
    // unique_ptr to manage data storage
    matrix(matrix&& other) = default;
    matrix& operator=(matrix&& other) = default;

    // Destructor - default ok because of unique_ptr
    ~matrix() = default;

    // Create a new matrix with contents copied from this one
    matrix duplicate() const {
      matrix ans(_rank);
      std::copy(begin(), end(), ans.begin());
      return ans;
    }
    
    // Get rank size
    uint32_t rank() const {
      return _rank;
    }
    
    // Get total size
    uint64_t size() const {
      return uint64_t(_rank) * uint64_t(_rank);
    }

    // Const element access
    const T& operator()(uint32_t i, uint32_t j) const {
      auto z = encode(i, j);
      return _data[z];
    }
    
    // Mutable element access
    T& operator()(uint32_t i, uint32_t j) {
      auto z = encode(i, j);
      return _data[z];
    }

    // Raw data access (const and mutable)
    const T* data() const {
      return _data.get();
    }
    T* data() {
      return _data.get();
    }

    // Mutable iterators
    iterator begin() {
      return iterator(data(), data());
    }
    iterator end() {
      return iterator(data(), data() + size());
    }

    // Const iterators
    const_iterator begin() const {
      return const_iterator(data(), data());
    }
    const_iterator end() const {
      return const_iterator(data(), data() + size());
    }
    
  private:
    // rank of matrix
    uint32_t _rank;
    // Data storage - note using array version of unique_ptr
    std::unique_ptr<T[]> _data;
  };
  
}
#endif

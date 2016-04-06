#ifndef CPP14_H
#define CPP14_H

#include <utility>
#include <chrono>

namespace cpp14 {

  template<bool B, class T = void>
  struct enable_if {};
 
  template<class T>
  struct enable_if<true, T> { typedef T type; };

  template <bool B, typename T = void>
  using enable_if_t = typename enable_if<B, T>::type;

  template< class T >
  using decay_t = typename std::decay<T>::type;
};


#endif

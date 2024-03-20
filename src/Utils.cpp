#include "Utils.h"
#include <cstdlib>
#include <iostream>
#include <ostream>

template <typename T> Option<T> Option<T>::Some(T val) {
  if (val == nullptr) {
    std::cerr << "[ERROR] Panic: Tried to wrap null into Some. This is a "
                 "programmer error, please report it."
              << std::endl;
    std::exit(1);
  }
  auto ret = Option<T>();
  ret.variant = Option<T>::SOME;
  ret.value = val;
  return ret;
}
template <typename T> Option<T> Option<T>::None(void) {
  auto ret = Option<T>();
  ret.variant = Option<T>::NONE;
  ret.value = nullptr;
  return ret;
}

template <typename T> T Option<T>::unwrap() {
  if (!this->is_some()) {
    std::cerr << "[ERROR] Panic: Tried to unwrap a None value. This is a "
                 "programmer error, please report it."
              << std::endl;
    std::exit(1);
  }
  return this->value;
}

template <typename T> bool Option<T>::is_some() {
  return this->variant == Option<T>::SOME;
}

template <typename T>
template <typename E>
Result<T, E> Option<T>::to_result(E err) {
  if (this->is_some()) {
    return Result<T, E>::Ok(this->unwrap());
  } else {
    return Result<T, E>::Err(err);
  }
}

template <typename T>
template <typename A>
Option<A> Option<T>::map(std::function<A(T)> f) {
  if (this->is_some()) {
    auto inside = this->unwrap();
    auto mapped = f(inside);
    return Option<A>::Some(mapped);
  } else {
    return Option<A>::None();
  }
}

template <typename T, typename E> Result<T,E>  Result<T,E>::Ok(T val) {
  if (val == nullptr) {
    std::cerr << "[ERROR] Panic: Tried to wrap null into Ok. This is a "
                 "programmer error, please report it."
              << std::endl;
    std::exit(1);
  }
  auto ret = Result<T,E>();
  ret.variant = Result<T, E>::OK;
  ret.value.ok = val;
  return ret;
}

template <typename T, typename E> Result<T,E> Result<T,E>::Err(E err) {
  if (err == nullptr) {
    std::cerr << "[ERROR] Panic: Tried to wrap null into Err. This is a "
                 "programmer error, please report it."
              << std::endl;
    std::exit(1);
  }
  auto ret = Result<T,E>();
  ret.variant = Result<T, E>::ERR;
  ret.value.err = err;
  return ret;
}
template<typename T, typename E> T Result<T,E>::unwrap() {
  if (this->is_err()) {
    std::cerr << "[ERROR] Panic: Tried to unwrap Err variant. This is a "
                 "programmer error, please report it."
              << std::endl;
    std::exit(1);
  } 
  return this->value.ok; 
}
template<typename T, typename E> E Result<T,E>::unwrap_err() {
  if (this->is_ok()) {
    std::cerr << "[ERROR] Panic: Tried to unwrap_err Ok variant. This is a "
                 "programmer error, please report it."
              << std::endl;
    std::exit(1);
  }
  return this->value.err;
}
template<typename T, typename E> bool Result<T,E>::is_ok() {
  return this->variant == Result<T,E>::OK;
}
template<typename T, typename E> bool Result<T,E>::is_err() {
  return this->variant == Result<T,E>::ERR;
}
template<typename T, typename E> Option<T> Result<T,E>::to_option() {
  if (this->is_ok()) {
    return Option<T>::Some(this->unwrap());
  }
  return Option<T>::None();
}
template<typename T, typename E> Option<E> Result<T,E>::to_option_err() {
  if (this->is_err()) {
    return Option<E>::Some(this->unwrap_err());
  }
  return Option<E>::None();
}
template<typename T, typename E> template<typename A> Result<A,E> Result<T,E>::map(std::function<A (T)> f) {
  if (this->is_ok()) {
    return Result<A, E>::Ok(f(this->unwrap()));
  } 
  return Result<A, E>::Err(this->unwrap_err());
}

template<typename T, typename E> template<typename F> Result<T,F> Result<T,E>::map_err(std::function<F (E)> f) {
  if (this->is_err()) {
    return Result<T,F>::Err(f(this->unwrap_err()));
  }
  return Result<T, F>::Ok(this->unwrap());
}



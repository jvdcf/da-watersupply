#ifndef UTILS
#define UTILS
#include <functional>

/* Forward Declare */
template <typename T, typename E> class Result;
template <typename T> class Option;


/* Formal Declare */
template <typename T> class Option {
private:
  enum {
    SOME,
    NONE,
  } variant;
  T value;
  Option();
public:
  static Option<T> Some(T val);
  static Option<T> None(void);
  T unwrap();
  bool is_some();
  template<typename E> Result<T,E> to_result(E err);
  template<typename A> Option<A> map(std::function<A (T)> f);
};

template <typename T, typename E> class Result {
private:
  enum {
    OK,
    ERR,
  } variant; 
  union {
    T ok;
    E err;
  } value;
  Result();
public:
  static Result<T,E> Ok(T val);
  static Result<T,E> Err(E err);
  T unwrap();
  E unwrap_err();
  bool is_ok();
  bool is_err();
  Option<T> to_option();
  Option<E> to_option_err();
  template<typename A> Result<A,E> map(std::function<A (T)> f);
  template<typename F> Result<T,F> map_err(std::function<F (E)> f);
};


#endif // !UTILS

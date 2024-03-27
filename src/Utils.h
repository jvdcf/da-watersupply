#ifndef UTILS
#define UTILS
#include <functional>
#include <iostream>
#include <ostream>

// /* Forward Declare */
// template <typename T, typename E> class Result;
// template <typename T> class Option;

// /* Formal Declare */
// template <typename T> class Option {
// private:
//   enum {
//     SOME,
//     NONE,
//   } variant;
//   T value;
//   Option() {
//     this->variant = NONE;
//     this->value = T();
//   };

// public:
//   static Option<T> Some(T val) {
//     auto ret = Option<T>();
//     ret.variant = Option<T>::SOME;
//     ret.value = val;
//     return ret;
//   };
//   static Option<T> None(void) {
//     auto ret = Option<T>();
//     ret.variant = Option<T>::NONE;
//     return ret;
//   };
//   T unwrap() {
//     if (!this->is_some()) {
//       std::cerr << "[ERROR] Panic: Tried to unwrap a None value. This is a "
//                    "programmer error, please report it."
//                 << std::endl;
//       std::exit(1);
//     }
//     return this->value;
//   };
//   bool is_some() { return this->variant == Option<T>::SOME; };
//   template <typename E> Result<T, E> to_result(E err) {
//     if (this->is_some()) {
//       return Result<T, E>::Ok(this->unwrap());
//     } else {
//       return Result<T, E>::Err(err);
//     }
//   };
//   template <typename A> Option<A> map(std::function<A(T)> f) {
//     if (this->is_some()) {
//       auto inside = this->unwrap();
//       auto mapped = f(inside);
//       return Option<A>::Some(mapped);
//     } else {
//       return Option<A>::None();
//     }
//   }
// };

// template <typename T, typename E> class Result {
// private:
//   enum {
//     OK,
//     ERR,
//   } variant;
//   union {
//     T ok;
//     E err;
//   } value;
//   Result() {
//     this->variant = ERR;
//   };

// public:
//   static Result<T, E> Ok(T val) {
//     auto ret = Result<T, E>();
//     ret.variant = Result<T, E>::OK;
//     ret.value.ok = val;
//     return ret;
//   };
//   static Result<T, E> Err(E err) {
//     auto ret = Result<T, E>();
//     ret.variant = Result<T, E>::ERR;
//     ret.value.err = err;
//     return ret;
//   };
//   T unwrap() {
//     if (this->is_err()) {
//       std::cerr << "[ERROR] Panic: Tried to unwrap Err variant. This is a "
//                    "programmer error, please report it."
//                 << std::endl;
//       std::exit(1);
//     }
//     return this->value.ok;
//   };
//   E unwrap_err() {
//     if (this->is_ok()) {
//       std::cerr << "[ERROR] Panic: Tried to unwrap_err Ok variant. This is a "
//                    "programmer error, please report it."
//                 << std::endl;
//       std::exit(1);
//     }
//     return this->value.err;
//   };
//   bool is_ok() { return this->variant == Result<T, E>::OK; };
//   bool is_err() { return this->variant == Result<T, E>::ERR; };
//   Option<T> to_option() {
//     if (this->is_ok()) {
//       return Option<T>::Some(this->unwrap());
//     }
//     return Option<T>::None();
//   };
//   Option<E> to_option_err() {
//     if (this->is_err()) {
//       return Option<E>::Some(this->unwrap_err());
//     }
//     return Option<E>::None();
//   };
//   template <typename A> Result<A, E> map(std::function<A(T)> f) {
//     if (this->is_ok()) {
//       return Result<A, E>::Ok(f(this->unwrap()));
//     }
//     return Result<A, E>::Err(this->unwrap_err());
//   }
//   template <typename F> Result<T, F> map_err(std::function<F(E)> f) {
//     if (this->is_err()) {
//       return Result<T, F>::Err(f(this->unwrap_err()));
//     }
//     return Result<T, F>::Ok(this->unwrap());
//   };
// };



#endif // !UTILS

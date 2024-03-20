#include "Parser.h"
#include "Utils.h"
// TODO

Parser<char> char_p(char c) {
  return Parser<char>([c](std::string s) {
    if (s.size() == 0)
      return Option<parsertup<char>>::None();
    if (c == s[0]) {
      if (s.size() > 1) {
        return Option<parsertup<char>>::Some(std::tuple(s.substr(1), c));
      } else {
        return Option<parsertup<char>>::Some(std::tuple("", c));
      }
    } else {
      return Option<parsertup<char>>::None();
    }
  });
}

Parser<std::string> string_p(std::string s) {
  std::vector<Parser<char>> pp;
  for (auto c : s) {
    pp.push_back(char_p(c));
  }
  return Parser<std::string>([pp](auto other) {
    std::string res;
    std::string o = other;
    for (auto p : pp) {
      auto resu = p.next(o);
      if (!resu.is_some()) {
        return Option<parsertup<std::string>>::None();
      } else {
        auto [in, out] = resu.unwrap();
        o = in;
        res.push_back(out);
      }
    }
    return Option<parsertup<std::string>>::Some(std::tuple(o, res));
  });
}

template <typename T, typename U>
Parser<std::tuple<T, U>> pair(Parser<T> fst, Parser<U> snd) {
  return Parser<std::tuple<T, U>>([fst, snd](auto input) {
    auto res1 = fst.next(input);
    if (res1->is_some()) {
      auto [rest1, result1] = res1->unwrap();
      auto res2 = snd.next(rest1);
      if (res2->is_some()) {
        auto [rest2, result2] = res2->unwrap();
        return Option<parsertup<std::tuple<T, U>>>::Some(
            std::tuple(rest2, std::tuple(result1, result2)));
      }
    }
  });
}

template <typename T, typename U, typename V>
Parser<std::tuple<T, U, V>> sequence3_p(Parser<T> fst, Parser<U> tgt,
                                        Parser<V> snd) {
  return Parser<std::tuple<T, U, V>>([fst, tgt, snd](auto s) {
    auto res1 = fst.next(s);
    if (res1->is_some()) {
      auto [rest1, result1] = res1->unwrap();
      auto res2 = tgt.next(rest1);
      if (res2->is_some()) {
        auto [rest2, result2] = res2->unwrap();
        auto res3 = snd.next(rest2);
        if (res3->is_some()) {
          auto [rest3, result3] = res3->unwrap();
          return Option<parsertup<std::tuple<T, U, V>>>::Some(
              std::tuple(rest3, std::tuple(result1, result2, result3)));
        }
      }
    }
    return Option<parsertup<std::tuple<T, U, V>>>::None();
  });
}

template <typename A, typename I>
Parser<std::vector<A>> take_while_p(Parser<I> f) {
  return Parser<std::vector<A>>([f](auto s) {
    auto status = f.next(s);
    if (!status->is_some())
      return Option<A>::None();
    std::vector<A> res;
    while (status->is_some()) {
      auto [rest, val] = status.unwrap();
      res.push_back(val);
      status = f.next(rest);
    }
    return Option<A>::Some(res);
  });
}

template<typename T, typename U> Parser<T> ends_with_fst(Parser<T> tgt, Parser<U> delimiter) {
  return Parser<T>([tgt,delimiter](auto input) {
    auto rest = input;
    auto fnd_del = delimiter.next(rest);
    std::string collected;
    while (!fnd_del->is_some()) {
      if (input.size() == 0) return Option<parsertup<T>>::None();
      collected.push_back(input[0]);
      rest = input.substr(1, input.size() - 1);
      fnd_del = delimiter.next(rest);
    }
    auto collected_parsed = tgt.next(collected);
    if (!collected_parsed->is_some()) return collected_parsed;
    auto [empty, parsed] = collected_parsed->unwrap();
    if (empty != "") return Option<parsertup<T>>::None();
    return Option<parsertup<T>>::Some(rest, parsed);
  });
}





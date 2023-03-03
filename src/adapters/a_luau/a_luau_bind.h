#ifndef A_LUAU_BIND_H
#define A_LUAU_BIND_H

#include <a_luau_state.h>

#include <iostream>
#include <tuple>

namespace a_luau {

template <typename Res, typename... ArgTypes>
class Bind {
 public:
  using Function = std::function<Res(ArgTypes...)>;

  Bind(Res (*fn)(ArgTypes...)) : m_func(fn) {}

  Bind(const Function& fn) : m_func(fn) {}

  int operator()(a_luau::State& state) {
    std::tuple<ArgTypes...> args;
    tovalues(state, 1, args);  // function arguments start at index 1
    return pushvalue(state, m_func, args);
  }

 private:
  Function m_func;
};

}  // namespace a_luau

#endif
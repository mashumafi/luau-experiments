#ifndef A_LUAU_BIND_H
#define A_LUAU_BIND_H

#include <a_luau_state.h>

#include <iostream>
#include <tuple>

namespace a_luau {

template <typename T>
T tovalue(State& state, int index);

template <>
double tovalue(State& state, int index) {
  return state.tonumber(index);
}

template <size_t I = 0, typename... Ts>
typename std::enable_if<I == sizeof...(Ts), void>::type tovalues(
    State& state,
    std::tuple<Ts...>& tup) {
  return;
}

template <size_t I = 0, typename... Ts>
typename std::enable_if<(I < sizeof...(Ts)), void>::type tovalues(
    State& state,
    std::tuple<Ts...>& tup) {
  std::get<I>(tup) =
      tovalue<std::tuple_element_t<I, std::tuple<Ts...>>>(state, I + 1);
  tovalues<I + 1>(state, tup);
}

template <typename T>
int pushvalue(State& state, const T& value);

template <>
int pushvalue(State& state, const double& num) {
  state.pushnumber(num);
  return 1;
}

template <size_t I = 0, typename... Ts>
typename std::enable_if<I == sizeof...(Ts), void>::type pushvalues(
    State& state,
    const std::tuple<Ts...>& tup) {
  return;
}

template <size_t I = 0, typename... Ts>
typename std::enable_if<(I < sizeof...(Ts)), void>::type pushvalues(
    State& state,
    const std::tuple<Ts...>& tup) {
  pushvalue<std::tuple_element_t<I, std::tuple<Ts...>>>(state,
                                                        std::get<I>(tup));
  pushvalues<I + 1>(state, tup);
}

template <typename... T>
int pushvalue(State& state, const std::tuple<T...>& tup) {
  pushvalues(state, tup);
  return sizeof...(T);
}

template <typename Res, typename... ArgTypes>
int pushvalue(State& state,
              const std::function<Res(ArgTypes...)>& fn,
              const std::tuple<ArgTypes...>& args) {
  return pushvalue(state, std::apply(fn, args));
}

template <typename... ArgTypes>
int pushvalue(State& state,
              const std::function<void(ArgTypes...)>& fn,
              const std::tuple<ArgTypes...>& args) {
  std::apply(fn, args);
  return 0;
}

template <typename Res, typename... ArgTypes>
class Bind {
 public:
  using Function = std::function<Res(ArgTypes...)>;

  Bind(Res (*fn)(ArgTypes...)) : m_func(fn) {}

  Bind(const Function& fn) : m_func(fn) {}

  int operator()(a_luau::State& state) {
    std::tuple<ArgTypes...> args;
    tovalues(state, args);
    return pushvalue(state, m_func, args);
  }

 private:
  Function m_func;
};

}  // namespace a_luau

#endif
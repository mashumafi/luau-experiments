#ifndef A_LUAU_STATE_H
#define A_LUAU_STATE_H

#include <cstddef>
#include <functional>
#include <iostream>
#include <memory>
#include <string_view>

struct lua_State;

namespace a_luau {

class State;

// to value
template <typename T>
void tovalue(State& state, int index, T& result);

template <size_t I = 0, typename... Ts>
typename std::enable_if<I == sizeof...(Ts), void>::type
tovalues(State& state, int index, std::tuple<Ts...>& tup) {
  return;
}

template <size_t I = 0, typename... Ts>
typename std::enable_if<(I < sizeof...(Ts)), void>::type
tovalues(State& state, int index, std::tuple<Ts...>& tup) {
  tovalue(state, index, std::get<I>(tup));
  tovalues<I + 1>(state, index + 1, tup);
}

template <typename... Ts>
void tovalue(State& state, int index, std::tuple<Ts...>& tup) {
  tovalues(state, index, tup);
}

// count
template <typename Tuple>
struct CountArgs;

template <>
struct CountArgs<std::tuple<>> {
    static const size_t value = 0;
};

template <typename T, typename... Ts>
struct CountArgs<std::tuple<T, Ts...>> {
    static const size_t value = CountArgs<T>::value + CountArgs<std::tuple<Ts...>>::value;
};

template <>
struct CountArgs<double> {
    static const size_t value = 1;
};

template <typename T>
size_t countargs = CountArgs<T>::value;

// push value
template <typename T>
int pushvalue(State& state, const T& value);

template <size_t I = 0, typename... Ts>
typename std::enable_if<I == sizeof...(Ts), int>::type pushvalues(
    State& state,
    const std::tuple<Ts...>& tup) {
  return 0;
}

template <size_t I = 0, typename... Ts>
typename std::enable_if<(I < sizeof...(Ts)), int>::type pushvalues(
    State& state,
    const std::tuple<Ts...>& tup) {
  pushvalue(state, std::get<I>(tup));
  return pushvalues<I + 1>(state, tup) + 1;
}

template <typename... T>
int pushvalue(State& state, const std::tuple<T...>& tup) {
  return pushvalues(state, tup);
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

class State {
 public:
  using Closure = std::function<int(State&)>;
  State();
  State(lua_State* L);
  virtual ~State();

  void sandbox();
  void sandboxthread();
  const char* debugtrace();
  int load(std::string_view chunkname, std::string_view data, int env);

  void openlibs();

  const char* tolstring(int index, size_t* len);
  const char* tostring(int index);
  double tonumber(int index);
  int isnumber(int index);

  std::string get_typename(int index);

  void pop(int n);
  void pushvalue(int index);
  void remove(int index);

  State newthread();
  void xmove(State& to, int n);
  int resume(int nargs);
  int resume(State& from, int nargs);
  int gettop();
  void insert(int index);
  int isnil(int index);

  void pushnil();
  void pushnumber(double num);

  template <class Ret, class... Args>
  Ret call(Args... args) {
    const int nargs = pushvalues(*this, std::tuple<Args...>(args...));
    Ret result;
    int status = pcall(nargs, countargs<Ret>, 0);
    tovalue(*this, -countargs<Ret>, result);
    return result;
  }
  int pcall(int nargs, int nresults, int msgh);

  int getglobal(const char* name);
  void setglobal(const char* name);

  void checkstack(int sz, const char* msg);

  void pushcclosure(const Closure& fn, std::string_view debugname, int nup);
  void pushcclosure(const Closure& fn, int nup);
  void pushcfunction(const Closure& fn, std::string_view debugname = "");
  void pushlightuserdata(void* p);
  void setfield(int index, const char* k);
  const void* topointer(int index);

  int newmetatable(std::string_view tname);
  void newtable();
  void setmetatable(int index);
  void createtable(int narr, int nrec);
  void* newuserdata(int sz);
  template <typename T, typename... ArgTypes>
  T* newuserdata(ArgTypes... args) {
    return *reinterpret_cast<T**>(newuserdata(sizeof(T))) = new T(args...);
  }
  void* touserdata(int index);
  template <typename T>
  T* touserdata(int index) {
    return *reinterpret_cast<T**>(touserdata(index));
  }
  void* newuserdatadtor(int sz, void (*dtor)(void*));
  template <typename T, typename... ArgTypes>
  T* newuserdatadtor(void (*dtor)(void*), ArgTypes... args) {
    return *reinterpret_cast<T**>(newuserdatadtor(sizeof(T*), dtor)) =
               new T(args...);
  }
  void pushboolean(int b);

  int gc_collect();

 private:
  State(std::shared_ptr<lua_State>&& state);

  std::shared_ptr<lua_State> m_state;
};

}  // namespace a_luau

#endif

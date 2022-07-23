#ifndef A_LUAU_STATE_H
#define A_LUAU_STATE_H

#include <cstddef>
#include <functional>
#include <memory>
#include <string_view>

struct lua_State;

namespace a_luau {

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

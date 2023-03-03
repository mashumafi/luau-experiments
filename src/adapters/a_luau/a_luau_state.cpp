#include <a_luau_state.h>
#include <lua.h>
#include <luacode.h>
#include <lualib.h>

#include <iostream>

namespace a_luau {

namespace {
void close(lua_State* state) {
  lua_close(state);
}

void noop(lua_State* state) {}

int closure(lua_State* L) {
  State state(L);
  auto* f = state.touserdata<State::Closure>(lua_upvalueindex(1));
  return (*f)(state);
}

void gc_closure(void* userdata) {
  free(*reinterpret_cast<State::Closure**>(userdata));
}
}  // namespace

State::State() : State(std::shared_ptr<lua_State>(luaL_newstate(), close)) {}

State::State(lua_State* L) : State(std::shared_ptr<lua_State>(L, noop)) {}

State::State(std::shared_ptr<lua_State>&& state) : m_state(std::move(state)) {}

State::~State() {}

void State::sandbox() {
  luaL_sandbox(m_state.get());
}

void State::sandboxthread() {
  luaL_sandboxthread(m_state.get());
}

const char* State::debugtrace() {
  return lua_debugtrace(m_state.get());
}

int State::load(std::string_view chunkname, std::string_view data, int env) {
  return luau_load(m_state.get(), chunkname.data(), data.data(), data.size(),
                   env);
}

void State::openlibs() {
  luaL_openlibs(m_state.get());
}

const char* State::tolstring(int index, size_t* len) {
  return lua_tolstring(m_state.get(), index, len);
}

const char* State::tostring(int index) {
  return lua_tostring(m_state.get(), index);
}

double State::tonumber(int index) {
  return lua_tonumber(m_state.get(), index);
}

int State::isnumber(int index) {
  return lua_isnumber(m_state.get(), index);
}

std::string State::get_typename(int index) {
  return lua_typename(m_state.get(), index);
}

void State::pop(int n) {
  lua_pop(m_state.get(), n);
}

void State::pushvalue(int index) {
  lua_pushvalue(m_state.get(), index);
}

State State::newthread() {
  return State(lua_newthread(m_state.get()));
}

void State::remove(int index) {
  lua_remove(m_state.get(), index);
}

void State::xmove(State& to, int n) {
  lua_xmove(m_state.get(), to.m_state.get(), n);
}

int State::resume(State& from, int nargs) {
  return lua_resume(m_state.get(), from.m_state.get(), nargs);
}

int State::resume(int nargs) {
  return lua_resume(m_state.get(), nullptr, nargs);
}

int State::gettop() {
  return lua_gettop(m_state.get());
}

void State::insert(int index) {
  return lua_insert(m_state.get(), index);
}

int State::pcall(int nargs, int nresults, int msgh) {
  return lua_pcall(m_state.get(), nargs, nresults, msgh);
}

int State::getglobal(const char* name) {
  return lua_getglobal(m_state.get(), name);
}

void State::setglobal(const char* name) {
  lua_setglobal(m_state.get(), name);
}

int State::isnil(int index) {
  return lua_isnil(m_state.get(), index);
}

void State::pushnil() {
  lua_pushnil(m_state.get());
}

void State::pushnumber(double num) {
  lua_pushnumber(m_state.get(), num);
}

void State::checkstack(int sz, const char* msg) {
  luaL_checkstack(m_state.get(), sz, msg);
}

void State::pushcclosure(const Closure& fn, int nup) {
  pushcclosure(fn, "", nup);
}

void State::pushcclosure(const Closure& fn,
                         std::string_view debugname,
                         int nup) {
  newuserdatadtor<Closure>(gc_closure, fn);
  if (nup > 1) {
    insert(nup);
  }
  lua_pushcclosure(m_state.get(), closure, debugname.data(), nup + 1);
}

void State::pushcfunction(const Closure& fn, std::string_view debugname) {
  pushcclosure(fn, debugname.data(), 0);
}

void State::pushlightuserdata(void* p) {
  lua_pushlightuserdata(m_state.get(), p);
}

void State::setfield(int index, const char* k) {
  lua_setfield(m_state.get(), index, k);
}

const void* State::topointer(int index) {
  return lua_topointer(m_state.get(), index);
}

int State::newmetatable(std::string_view tname) {
  return luaL_newmetatable(m_state.get(), tname.data());
}

void State::newtable() {
  lua_newtable(m_state.get());
}

void State::setmetatable(int index) {
  lua_setmetatable(m_state.get(), index);
}

void State::createtable(int narr, int nrec) {
  lua_createtable(m_state.get(), narr, nrec);
}

void* State::newuserdata(int sz) {
  return lua_newuserdata(m_state.get(), sz);
}

void* State::touserdata(int index) {
  return lua_touserdata(m_state.get(), index);
}

void* State::newuserdatadtor(int sz, void (*dtor)(void*)) {
  return lua_newuserdatadtor(m_state.get(), sz, dtor);
}

void State::pushboolean(int b) {
  lua_pushboolean(m_state.get(), b);
}

int State::gc_collect() {
  return lua_gc(m_state.get(), LUA_GCCOLLECT, 0);
}

template <>
void tovalue(State& state, int index, double& result) {
  result = state.tonumber(index);
}

template <>
int pushvalue(State& state, const double& num) {
  state.pushnumber(num);
  return 1;
}

}  // namespace a_luau

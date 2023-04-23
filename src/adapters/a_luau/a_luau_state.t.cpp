#include <a_luau_bind.h>
#include <a_luau_state.h>

#include <catch2/catch_all.hpp>

#include <Luau/Compiler.h>
#include <lua.h>

#include <iostream>
#include <optional>

static Luau::CompileOptions copts() {
  Luau::CompileOptions result = {};
  result.optimizationLevel = 1;
  result.debugLevel = 1;
  result.coverageLevel = 0;

  return result;
}

int threadFunction(a_luau::State& state,
                   const char* script,
                   const char* variable) {
  std::string bytecode = Luau::compile(script, copts());

  if (state.load("", bytecode, 0) != 0) {
    fprintf(stdout, "Err: %s", state.tolstring(-1, 0));
    state.pop(1);
    return {};
  }

  a_luau::State thread = state.newthread();
  int error = thread.resume(0);
  if (error) {
    std::cerr << "Error starting thread: " << thread.tostring(-1) << std::endl;
    state.pop(2);
    return -1;
  }

  thread.getglobal(variable);
  if (thread.isnil(-1)) {
    std::cerr << "Error: " << variable << " is not defined in thread"
              << std::endl;
    thread.pop(1);
    state.pop(2);
    return -1;
  }
  const char* variableValue = thread.tostring(-1);

  thread.pop(1);
  state.pop(1);
  return std::stoi(variableValue);
}

static void setupState(a_luau::State& state) {
  state.openlibs();
  state.sandbox();
}

double sub(double a, double b) {
  std::cout << "subtracting " << b << " from " << a << std::endl;
  return a - b;
}

using Direction = std::tuple<double, double>;
Direction east() {
  return {0.0, 1.0};
}

using Directions = std::tuple<Direction, Direction>;
Directions directions() {
  return {{1.0, 2.0}, {3.0, 4.0}};
}

TEST_CASE("State can be created", "[a_luau_state]") {
  a_luau::State state;
}

TEST_CASE("Test binding") {
  a_luau::Bind testFn(sub);
  a_luau::Bind eastFn(east);
  a_luau::Bind directionsFn(directions);

  a_luau::State state;
  setupState(state);
  state.sandboxthread();

  state.pushcfunction(testFn, "sub");
  state.setglobal("sub");

  state.pushcfunction(eastFn, "east");
  state.setglobal("east");

  state.pushcfunction(directionsFn, "directions");
  state.setglobal("directions");

  REQUIRE(threadFunction(state, "local result = sub(1,2)", "result") == -1);

  state.getglobal("sub");
  CHECK(state.call<double>(1.0, 4.0) == -3.0);

  state.getglobal("east");
  Direction dir = state.call<Direction>();
  CHECK(std::get<0>(dir) == 0.0);
  CHECK(std::get<1>(dir) == 1.0);

  state.getglobal("directions");
  REQUIRE(a_luau::countargs<Directions> == 4);
  Directions dirs = state.call<Directions>();
  CHECK(std::get<0>(std::get<0>(dirs)) == 1.0);
  CHECK(std::get<1>(std::get<0>(dirs)) == 2.0);

  CHECK(std::get<0>(std::get<1>(dirs)) == 3.0);
  CHECK(std::get<1>(std::get<1>(dirs)) == 4.0);
}
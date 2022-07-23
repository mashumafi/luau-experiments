#include <Luau/Compiler.h>
#include <a_luau_bind.h>
#include <a_luau_state.h>
#include <lua.h>

#include <iostream>

static Luau::CompileOptions copts() {
  Luau::CompileOptions result = {};
  result.optimizationLevel = 1;
  result.debugLevel = 1;
  result.coverageLevel = 0;

  return result;
}

std::string runCode(a_luau::State& state, const std::string& source) {
  std::string bytecode = Luau::compile(source, copts());

  if (state.load("test123", bytecode, 0) != 0) {
    size_t len;
    const char* msg = state.tolstring(-1, &len);

    std::string error(msg, len);
    state.pop(1);

    return error;
  }

  a_luau::State thread = state.newthread();

  state.pushvalue(-2);
  state.remove(-3);
  state.xmove(thread, 1);

  int status = thread.resume(0);

  if (status == 0) {
    int n = thread.gettop();

    if (n != 0) {
      thread.checkstack(LUA_MINSTACK, "too many results to print");
      thread.getglobal("_PRETTYPRINT");
      // If _PRETTYPRINT is nil, then use the standard print function
      // instead
      if (thread.isnil(-1) != 0) {
        thread.pop(1);
        thread.getglobal("print");
      }
      thread.insert(1);
      thread.pcall(n, 0, 0);
    }
  } else {
    std::string error;

    if (status == -1) {
      error = "thread yielded unexpectedly";
    } else if (const char* str = thread.tostring(-1)) {
      error = str;
    }

    error += "\nstack backtrace:\n";
    error += thread.debugtrace();

    fprintf(stdout, "%s", error.c_str());
  }

  state.pop(1);
  return std::string();
}

static void setupState(a_luau::State& state) {
  state.openlibs();
  state.sandbox();
}

double sub(double a, double b) {
  std::cout << "subtracting " << b << " from " << a << std::endl;
  return a - b;
}

void empty() {
  std::cout << "Doing nothing.." << std::endl;
}

std::tuple<double, double> east() {
  return {0.0, 1.0};
}

int main(int argc, const char* argv[]) {
  a_luau::Bind testFn(sub);
  a_luau::Bind emptyFn(empty);
  a_luau::Bind eastFn(east);

  a_luau::State state;
  setupState(state);
  state.sandboxthread();

  state.pushcfunction(testFn, "sub");
  state.setglobal("sub");

  state.pushcfunction(emptyFn, "empty");
  state.setglobal("empty");

  state.pushcfunction(eastFn, "east");
  state.setglobal("east");

  while (true) {
    std::string source;
    std::cin >> source;
    std::cout << runCode(state, source);
    state.pushnil();
    state.setglobal("sub");
    state.gc_collect();
  }

  return 0;
}

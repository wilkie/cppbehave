#ifndef HARNESS_H
#define HARNESS_H

#include <stdio.h>

class Units {
public:
	static void addTest(void (*test)) {
		if (_testCount == _testMax) {
		}
	}

	static void runTests() {
	}

private:
	static unsigned int _testCount;
	static unsigned int _testMax;
	static void (**_tests);
};

unsigned int Units::_testCount = 0;
unsigned int Units::_testMax   = 0;
void (**Units::_tests)         = NULL;

class Eff {
public:
	Eff(void (*test)) {
		Units::addTest(test);
		printf("eff");
	}
};

#define STRINGIFY_2(x) \
  #x

#define STRINGIFY(x) \
  STRINGIFY_2(x)

#define before \
  void do_before()

#define after \
  void do_after()

#define describe(x) \
  class Base##x {\
    public: \
      Base##x() { passes = 0; fails = 0; } \
      const char* name() { return STRINGIFY(x); } \
      void pass() { passes++; } \
      void fail() { fails++;  } \
	  void do_before() {} \
	  void do_after() {} \
    private: \
      int passes; \
      int fails; \
  };\
  Eff run##x (); \
  \
  class Test##x : public Base##x

#define it(str) \
  void do_test_1() { \
    printf("error %s " str, name()); \
  } \
  void test_1()

#define should(expr) \
  (expr) ? pass() : fail()

int main() {
	return 0;
}

#endif

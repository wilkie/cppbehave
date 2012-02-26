#ifndef HARNESS_H
#define HARNESS_H

#include <stdio.h>
#include <string.h>

typedef void (*Test)();

struct TestDetails;

class NewDescribeBase {
  public:
    NewDescribeBase(NewDescribeBase* parent, const char* name);
    void addDescribe(NewDescribeBase* nd);
    void addTest(void (*test)(), void(*before)(), void(*after)(), const char* str, const char* filename, int line);
    void pass();
    void fail();
    void runTests();

    int base;
    const char* name;
    NewDescribeBase* parent;
    unsigned int _testCount;
    unsigned int _testMax;
    TestDetails* _tests;
    static unsigned int totalPasses, totalFails;
    unsigned int passes, fails;
    unsigned int _childCount, _childMax;
    NewDescribeBase** _children;
    unsigned int _currentTest;

    static unsigned int _failedMax;
    static TestDetails* _failedTests;
};

void do_before();
void do_after();

class NewDescribe : public NewDescribeBase {
  public:
    NewDescribe(const char* name);
};

class NewTest {
  public:
    NewTest(NewDescribeBase* parent, void(*before)(), void(*after)(), void(*test)(), const char* str, const char* filename, int line);
};

extern NewDescribe _describe;

// This macro will place quotes around the given token
#define STRINGIFY_2(x) #x
#define STRINGIFY(x) STRINGIFY_2(x)

#define before \
  void do_before()

#define after \
  void do_after()

#define MERGE(a,b) a##b
#define TEST_LABEL(a) MERGE(test_, a)
#define TESTER_LABEL(a) MERGE(tester_, a)
#define UNIQUE_TEST(x) TEST_LABEL(__LINE__)
#define UNIQUE_TESTER(x) TESTER_LABEL(__LINE__)

#define it(str)                                                    \
  void UNIQUE_TEST(test)();                                        \
  NewTest UNIQUE_TESTER(test)(&_describe, &do_before, &do_after, &UNIQUE_TEST(test), str, __FILE__, __LINE__); \
  void UNIQUE_TEST(test)()

#define should(expr)   \
  if (expr) {          \
	  _describe.pass();  \
  }                    \
  else {               \
    _describe.fail();  \
    return;            \
  } do {} while(0)

#define describe(x)                                                         \
  namespace Describe##x {                                                   \
    class NewDescribeBaseA : public NewDescribeBase {                       \
      public:                                                               \
        NewDescribeBaseA(::NewDescribeBase* parent, const char* name)       \
           : NewDescribeBase(parent, name) {}                               \
    };                                                                      \
    class NewDescribeBase : public NewDescribeBaseA {                       \
      public:                                                               \
        NewDescribeBase(::NewDescribeBase* parent, const char* name)        \
           : NewDescribeBaseA(parent, name) { base++; }                     \
    };                                                                      \
    class NewDescribe : public NewDescribeBase {                            \
      public:                                                               \
        NewDescribe(const char* name) : NewDescribeBase(&_describe, name) { \
          parent->addDescribe(this);                                        \
        }                                                                   \
    };                                                                      \
    NewDescribe _describe(STRINGIFY(x));                                    \
  }                                                                         \
namespace Describe##x

#endif

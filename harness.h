#ifndef HARNESS_H
#define HARNESS_H

#include <stdio.h>
#include <string.h>

typedef void (*Test)();

class NewDescribeBase;

struct TestDetails {
  void  (*test)();
  void  (*before)();
  void  (*after)();
  const char* name;

  const char* filename;
  int   line;

  NewDescribeBase* parent;
};

class NewDescribeBase {
  public:
    NewDescribeBase(NewDescribeBase* parent, const char* name) {
      base = 1;
      this->parent = parent;
      this->name = name;
      this->_testCount = 0;
      this->_testMax = 32;
      this->_tests = NULL;
      this->_childCount = 0;
      this->_childMax = 32;
      this->_children = NULL;
    }

    void addDescribe(NewDescribeBase* nd) {
      if (_children == NULL) {
        _children = new NewDescribeBase*[_childMax];
      }
      if (_childCount == _childMax) {
        _childMax *= 2;
        NewDescribeBase** old = _children;
        _children = new NewDescribeBase*[_childMax];
        memcpy(_children, old, sizeof(NewDescribeBase*) * _childCount);
        delete [] old;
      }
      _children[_childCount] = nd;
      _childCount++;
    }

    void addTest(void (*test)(), void(*before)(), void(*after)(), const char* str, const char* filename, int line) {
      if (_tests == NULL) {
        _tests = new TestDetails[_testMax];
      }
      if (_testCount == _testMax) {
        _testMax *= 2;
        TestDetails* old = _tests;
        _tests = new TestDetails[_testMax];
        memcpy(_tests, old, sizeof(TestDetails) * _testCount);
        delete [] old;
      }
      _tests[_testCount].test = test;
      _tests[_testCount].name = str;
      _tests[_testCount].before = before;
      _tests[_testCount].after = after;
      _tests[_testCount].line = line;
      _tests[_testCount].filename = filename;
      _tests[_testCount].parent = this;
      _testCount++;
    }

    void pass() {
      printf(".");
      passes++;
      totalPasses++;
    }

    void fail() {
      printf("F");
      if (_failedTests == NULL) {
        _failedTests = new TestDetails[_failedMax];
      }
      if (totalFails == _failedMax) {
        _failedMax *= 2;
        TestDetails* old = _failedTests;
        _failedTests = new TestDetails[_failedMax];
        memcpy(_failedTests, old, sizeof(TestDetails) * totalFails);
        delete [] old;
      }
      _failedTests[totalFails] = _tests[_currentTest];

      fails++;
      totalFails++;
    }

    void runTests() {
      // Run each child describe
      for(unsigned int i = 0; i < _childCount; i++) {
        NewDescribeBase* base = _children[i];
        base->runTests();
      }

      // Run each test
      for(unsigned int i = 0; i < _testCount; i++) {
        _currentTest = i;
        _tests[i].before();
        _tests[i].test();
        _tests[i].after();
      }
    }
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

void do_before() {}
void do_after()  {}

unsigned int NewDescribeBase::totalPasses = 0;
unsigned int NewDescribeBase::totalFails  = 0;

unsigned int NewDescribeBase::_failedMax = 0;
TestDetails* NewDescribeBase::_failedTests = NULL;

class NewDescribe : public NewDescribeBase {
  public:
    NewDescribe(const char* name) : NewDescribeBase(NULL, name) {
    }
};

class NewTest {
  public:
    NewTest(NewDescribeBase* parent, void(*before)(), void(*after)(), void(*test)(), const char* str, const char* filename, int line) {
      parent->addTest(test, before, after, str, filename, line);
    }
};

NewDescribe describe("{}");

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
  NewTest UNIQUE_TESTER(test)(&describe, &do_before, &do_after, &UNIQUE_TEST(test), str, __FILE__, __LINE__); \
  void UNIQUE_TEST(test)()

#define should(expr)   \
  if (expr) {          \
	  describe.pass(); \
  }                    \
  else {               \
    describe.fail();   \
    return;            \
  } do {} while(0)

#define describe(x)                                                        \
  namespace Describe##x {                                                  \
    class NewDescribeBaseA : public NewDescribeBase {                      \
      public:                                                              \
        NewDescribeBaseA(::NewDescribeBase* parent, const char* name)      \
           : NewDescribeBase(parent, name) {}                              \
    };                                                                     \
    class NewDescribeBase : public NewDescribeBaseA {                      \
      public:                                                              \
        NewDescribeBase(::NewDescribeBase* parent, const char* name)       \
           : NewDescribeBaseA(parent, name) { base++; }                    \
    };                                                                     \
    class NewDescribe : public NewDescribeBase {                           \
      public:                                                              \
        NewDescribe(const char* name) : NewDescribeBase(&describe, name) { \
          parent->addDescribe(this);                                       \
        }                                                                  \
    };                                                                     \
    NewDescribe describe(STRINGIFY(x));                                    \
  }                                                                        \
  namespace Describe##x

int main() {
  describe.runTests();
  printf("\n");
  printf("Passes: %d\n", describe.totalPasses);
  printf("Failures: %d\n", describe.totalFails);
  printf("\n");

  for(unsigned int i = 0; i < describe.totalFails; i++) {
    TestDetails test = describe._failedTests[i];
    NewDescribeBase* current = test.parent;
    int maxChars = 0;
    while(current != NULL) {
      maxChars += strlen(current->name) + 2;
      current = current->parent;
    }
    char* testRoot = new char[maxChars+1];
    strcpy(testRoot, "");

    current = test.parent;
    while(current != NULL) {
      // Shift array over current->name + 2
      int shiftAmount = strlen(current->name) + 2;
      for (int i = shiftAmount + strlen(testRoot) + 1; i >= shiftAmount; i--) {
        testRoot[i] = testRoot[i - shiftAmount];
      }
      // Copy (without \0) over string
      for (int i = 0; i < strlen(current->name); i++) {
        testRoot[i] = current->name[i];
      }
      testRoot[shiftAmount-1] = ':';
      testRoot[shiftAmount-2] = ':';
      current = current->parent;
    }
    // Go past the {}:: beginning
    testRoot += 4;

    // Remove last ::
    testRoot[strlen(testRoot)-2] = '\0';

    printf("Failed: %s:%d %s %s\n", test.filename, test.line, testRoot, test.name);
  }

  return 0;
}

#endif

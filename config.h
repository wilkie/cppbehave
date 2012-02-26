#include "harness.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void do_before() {}
void do_after()  {}

struct TestDetails {
  void  (*test_func)();
  void  (*before_func)();
  void  (*after_func)();
  const char* name;

  const char* filename;
  int   line;

  NewDescribeBase* parent;
};

void NewDescribeBase::addTest(void (*test_func)(), void(*before_func)(), void(*after_func)(), const char* str, const char* filename, int line) {
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
  _tests[_testCount].test_func = test_func;
  _tests[_testCount].name = str;
  _tests[_testCount].before_func = before_func;
  _tests[_testCount].after_func = after_func;
  _tests[_testCount].line = line;
  _tests[_testCount].filename = filename;
  _tests[_testCount].parent = this;
  _testCount++;
}
unsigned int NewDescribeBase::totalPasses = 0;
unsigned int NewDescribeBase::totalFails  = 0;

unsigned int NewDescribeBase::_failedMax = 0;
TestDetails* NewDescribeBase::_failedTests = NULL;

NewDescribeBase::NewDescribeBase(NewDescribeBase* parent, const char* name) {
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

void NewDescribeBase::addDescribe(NewDescribeBase* nd) {
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

void NewDescribeBase::pass() {
  printf(".");
  passes++;
  totalPasses++;
}

void NewDescribeBase::fail() {
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

void NewDescribeBase::runTests() {
  // Run each child describe
  for(unsigned int i = 0; i < _childCount; i++) {
    NewDescribeBase* base = _children[i];
    base->runTests();
  }

  // Run each test
  for(unsigned int i = 0; i < _testCount; i++) {
    _currentTest = i;
    _tests[i].before_func();
    _tests[i].test_func();
    _tests[i].after_func();
  }
}

NewDescribe::NewDescribe(const char* name) : NewDescribeBase(NULL, name) {
}

NewTest::NewTest(NewDescribeBase* parent, void(*before_func)(), void(*after_func)(), void(*test_func)(), const char* str, const char* filename, int line) {
  parent->addTest(test_func, before_func, after_func, str, filename, line);
}

NewDescribe _describe("{}");

char* yieldDescribeName(NewDescribeBase* ds) {
  NewDescribeBase* current = ds;
  int maxChars = 0;
  while(current != NULL) {
    maxChars += strlen(current->name) + 2;
    current = current->parent;
  }
  char* testRoot = new char[maxChars+1];
  char* origRoot = testRoot;
  strcpy(testRoot, "");

  current = ds;
  while(current != NULL) {
    // Shift array over current->name + 2
    // If it is not the root
    if (strcmp(current->name, "{}") != 0) {
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
    }
    current = current->parent;
  }

  // Remove last ::
  testRoot[strlen(testRoot)-2] = '\0';

  return testRoot;
}

void printFailure(TestDetails test) {
  char* describeName = yieldDescribeName(test.parent);

  printf("Failed: %s:%d %s %s\n", test.filename, test.line, describeName, test.name);

  delete [] describeName;
}

void printDescribe(NewDescribeBase* root) {
  char* describeName = yieldDescribeName(root);

  if (strlen(describeName) > 0) {
    printf("%s\n", describeName);

    for(unsigned int t = 0; t < root->_testCount; t++) {
      printf("  %s\n", root->_tests[t].name);
    }
  }

  delete [] describeName;

  printf("\n");

  for(unsigned int c = 0; c < root->_childCount; c++) {
    printDescribe(root->_children[c]);
  }
}

void printDocumentation() {
  printDescribe(&_describe);
}

void reportTests() {
  printf("\n");
  printf("Passes: %d\n", _describe.totalPasses);
  printf("Failures: %d\n", _describe.totalFails);
  printf("\n");

  for(unsigned int i = 0; i < _describe.totalFails; i++) {
    printFailure(_describe._failedTests[i]);
  }
}

int main(int argc, char** argv) {
  unsigned int seed = 0;
  if (argc > 1) {
    if (strncmp(argv[1], "--docs", 6) == 0) {
      printDocumentation();
      return 0;
    }
    else if (strncmp(argv[1], "--seed", 6) == 0) {
      if (argc > 2) {
        seed = atoi(argv[2]);
      }
      else {
        printf("No seed specified\n");
        return 0;
      }
    }
    else {
      printf("Unknown arguments\n");
      return 0;
    }
  }

  srand(seed);

  _describe.runTests();
  reportTests();

  printf("Seed: %d\n", seed);

  return 0;
}

namespace Configuration {
}

#define configuration     \
  namespace Configuration

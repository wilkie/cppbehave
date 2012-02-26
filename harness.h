#ifndef HARNESS_H
#define HARNESS_H

#include <stdio.h>
#include <string.h>

typedef void (*Test)();

class NewDescribeBase {
	public:
		NewDescribeBase(NewDescribeBase* parent, const char* name) {
			base = 1;
			this->parent = parent;
			this->name = name;
			this->_testCount = 0;
			this->_testMax = 32;
			this->_tests = NULL;
			this->_strings = NULL;
			this->_childCount = 0;
			this->_childMax = 32;
			this->_children = NULL;
		}

		void addDescribe(NewDescribeBase* nd) {
			printf("I see ");
			NewDescribeBase* current = nd;
			while(current != NULL) {
				printf("->%s", current->name);
				current = current->parent;
			}
			printf("\n");
			
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

		void addTest(void (*test)(), const char* str) {
			if (_tests == NULL) {
				_tests = new void*[_testMax];
				_strings = new char*[_testMax];
			}
			if (_testCount == _testMax) {
				_testMax *= 2;
				void** old = _tests;
				char** old_strings = _strings;
				_tests = new void*[_testMax];
				_strings = new char*[_testMax];
				memcpy(_tests, old, sizeof(void*) * _testCount);
				memcpy(_strings, old_strings, sizeof(char*) * _testCount);
				delete [] old;
				delete [] old_strings;
			}
			_tests[_testCount] = (void*)test;
			_strings[_testCount] = new char[strlen(str)+1];
			strcpy(_strings[_testCount], str);
			_testCount++;
		}

		void pass() {
			printf(".");
			passes++;
			totalPasses++;
		}

		void fail() {
			printf("F");
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
				Test test_func = (Test)_tests[i];
				test_func();
			}
		}

		int base;
		const char* name;
		NewDescribeBase* parent;

		unsigned int _testCount;
		unsigned int _testMax;
		void** _tests;
		char** _strings;

		static unsigned int totalPasses;
		static unsigned int totalFails;

		unsigned int passes;
		unsigned int fails;

		unsigned int _childCount;
		unsigned int _childMax;
		NewDescribeBase** _children;
};

unsigned int NewDescribeBase::totalPasses = 0;
unsigned int NewDescribeBase::totalFails  = 0;

class NewDescribe : public NewDescribeBase {
	public:
		NewDescribe(const char* name) : NewDescribeBase(NULL, name) {
		}
};

class NewTest {
	public:
		NewTest(NewDescribeBase* parent, void(*test)(), const char* str) {
			printf("Registering test %s in ", str);
			NewDescribeBase* current = parent;
			while(current != NULL) {
				printf("->%s", current->name);
				current = current->parent;
			}
			printf("\n");

			parent->addTest(test, str);
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
  NewTest UNIQUE_TESTER(test)(&describe, &UNIQUE_TEST(test), str); \
  void UNIQUE_TEST(test)()

#define should(expr)   \
  if (expr) {          \
	  describe.pass(); \
  }                    \
  else {               \
    describe.fail();   \
    return;            \
  }      do {} while(0)

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
	printf("Passes: %d\nFailures: %d\n", describe.totalPasses, describe.totalFails);
	return 0;
}

#endif

# CppBehave

A C++ behavioral testing framework in the spirit of rspec, etc. Provides a simple DSL that compiles down to a test executable, and a very detailed and customizable report.

## Using

Copy the two header files into your project. Write a configuration cpp file that includes config.h. It may be empty for a default configuration. Write a cpp file for each specification you wish to provide that each includes harness.h. Compile with the config.cpp first in the compile order:

    g++ -o tester config.cpp test_1.cpp test_2.cpp ...

Then just run the tester:

    ./tester

## The Most Basic Failed Test

```
// config.cpp
#include "config.h"
```

```
// test.cpp
#include "harness.h"

describe(Foo) {
  it ("should work") {
    should(1 == 0);
  }
}
```

Compile:
```
g++ -o tester config.cpp test.cpp
```

Run:
```
./tester
```

Outputs:

```
F

  Passes: 0
Failures: 1

Failed: bar.cpp:8 Foo should work

Seed: 0

Time: 11ms
```

## Configuration

The configuration block can contain several options and parameters you can use to customize the test executable's behavior.

    // config.cpp
    #include "config.h"

    configuration {
      settings {
        // Whether or not to report the seed at the end of the tests
        printSeed = false;

        // Whether or not to report the results (# of passes, etc)
        printResults = true;

        // Whether or not to report the time elapsed
        printTime = true;
      }
    }

## Command-Line Arguments

The tester application automatically supports several test arguments.

* --docs - Prints the tests and modules in an easy-to-read format.
* --seed - Uses the given seed to randomize the tests. Good for re-running a previous test order to mitigate nondeterminism.

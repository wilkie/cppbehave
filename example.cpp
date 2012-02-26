#include "harness.h"

describe (Foo) {
  int x;

  before {
    x = 3;
  }

  it ("should do a thing") {
    should(x == 3);
    x = 4;
  }

  it ("should do another thing") {
    should(x == 4);
  }

  describe (Else) {
    int x = 5;

    describe (Bar) {
      it ("should do something crazy") {
        should(x == 4);
      }

      it ("should work like it's ducking supposed to") {
        should(x == 5);
      }
    }
  }
}

/* Outputs:
 *
 * F..F
 * Passes: 2
 * Failures: 2
 *
 * Failed: example.cpp:23 Foo::Else::Bar should do something crazy
 * Failed: example.cpp:15 Foo should do another thing
 *
 */

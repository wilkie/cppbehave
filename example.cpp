#include "harness.h"

describe (Foo) {
	int x = 5;

	it ("should do a thing") {
		should(x == 5);
		should(x == 6);
	}

	describe (Else) {
		describe (Bar) {
		}
	}
}

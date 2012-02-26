#include "harness.h"

describe (Foo) {
	it ("should do a thing") {
		should(3 == 27);
	}
};

describe (World) {
	describe (addActor) {
		int x;

		before {
			x = 5;
		}

		after {
			x = 0;
		}

		it ("should really do a thing") {
			should(x == 5);
		}
	};
};

// Simple vector class example C++ file
#include <stdio.h>
#include "vectorclass.h"
int main() {
	// define and initialize integer vectors a and b
	Vec4i a(10,11,12,13);
	Vec4i b(20,21,22,23);
	// add the two vectors
	Vec4i c = a + b;
	// Print the results
	for (int i = 0; i < c.size(); i++) {
		printf(" %5i", c[i]);
	}
	printf("\n");
	return 0;
}

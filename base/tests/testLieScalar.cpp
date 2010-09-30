/**
 * @file testLieScalar.cpp
 * @author Kai Ni
 */

#include <gtsam/CppUnitLite/TestHarness.h>

#include <gtsam/base/LieScalar.h>

using namespace gtsam;

/* ************************************************************************* */
TEST( testLieScalar, construction ) {
	double d = 2.;
	LieScalar lie1(d), lie2(d);

	EXPECT(lie1.dim() == 1);
	EXPECT(assert_equal(lie1, lie2));
}

/* ************************************************************************* */
TEST( testLieScalar, logmap ) {
	LieScalar lie1(1.), lie2(3.);

	EXPECT(assert_equal(Vector_(1, 2.), lie1.logmap(lie2)));
}

/* ************************************************************************* */
int main() { TestResult tr; return TestRegistry::runAllTests(tr); }
/* ************************************************************************* */

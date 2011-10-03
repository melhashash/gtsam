/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation, 
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 *  @file  testNonlinearFactor.cpp
 *  @brief Unit tests for Non-Linear Factor, 
 *  create a non linear factor graph and a values structure for it and
 *  calculate the error for the factor.
 *  @author Christian Potthast
 **/

/*STL/C++*/
#include <iostream>

#include <CppUnitLite/TestHarness.h>

// TODO: DANGEROUS, create shared pointers
#define GTSAM_MAGIC_GAUSSIAN 2

// Magically casts strings like "x3" to a Symbol('x',3) key, see Key.h
#define GTSAM_MAGIC_KEY

#include <gtsam/base/Matrix.h>
#include <gtsam/slam/smallExample.h>
#include <gtsam/slam/simulated2D.h>
#include <gtsam/linear/GaussianFactor.h>
#include <gtsam/nonlinear/NonlinearFactorGraph-inl.h>

using namespace std;
using namespace gtsam;
using namespace example;

typedef boost::shared_ptr<NonlinearFactor<VectorValues> > shared_nlf;

/* ************************************************************************* */
TEST( NonlinearFactor, equals )
{
	SharedNoiseModel sigma(noiseModel::Isotropic::Sigma(2,1.0));

	// create two nonlinear2 factors
	Point2 z3(0.,-1.);
	simulated2D::Measurement f0(z3, sigma, 1,1);

	// measurement between x2 and l1
	Point2 z4(-1.5, -1.);
	simulated2D::Measurement f1(z4, sigma, 2,1);

	CHECK(assert_equal(f0,f0));
	CHECK(f0.equals(f0));
	CHECK(!f0.equals(f1));
	CHECK(!f1.equals(f0));
}

/* ************************************************************************* */
TEST( NonlinearFactor, equals2 )
{
  // create a non linear factor graph
  Graph fg = createNonlinearFactorGraph();

  // get two factors
  Graph::sharedFactor f0 = fg[0], f1 = fg[1];

  CHECK(f0->equals(*f0));
// SL-FIX  CHECK(!f0->equals(*f1));
// SL-FIX  CHECK(!f1->equals(*f0));
}

/* ************************************************************************* */
TEST( NonlinearFactor, NonlinearFactor )
{
  // create a non linear factor graph
  Graph fg = createNonlinearFactorGraph();

  // create a values structure for the non linear factor graph
  Values cfg = createNoisyValues();

  // get the factor "f1" from the factor graph
  Graph::sharedFactor factor = fg[0];

  // calculate the error_vector from the factor "f1"
  // error_vector = [0.1 0.1]
  Vector actual_e = boost::dynamic_pointer_cast<NoiseModelFactor<Values> >(factor)->unwhitenedError(cfg);
  CHECK(assert_equal(0.1*ones(2),actual_e));

  // error = 0.5 * [1 1] * [1;1] = 1
  double expected = 1.0; 

  // calculate the error from the factor "f1"
  double actual = factor->error(cfg);
  DOUBLES_EQUAL(expected,actual,0.00000001);
}

/* ************************************************************************* */
TEST( NonlinearFactor, linearize_f1 )
{
  Values c = createNoisyValues();

  // Grab a non-linear factor
  Graph nfg = createNonlinearFactorGraph();
  Graph::sharedFactor nlf = nfg[0];

  // We linearize at noisy config from SmallExample
  GaussianFactor::shared_ptr actual = nlf->linearize(c, *c.orderingArbitrary());

  GaussianFactorGraph lfg = createGaussianFactorGraph(*c.orderingArbitrary());
  GaussianFactor::shared_ptr expected = lfg[0];

  CHECK(assert_equal(*expected,*actual));

  // The error |A*dx-b| approximates (h(x0+dx)-z) = -error_vector
  // Hence i.e., b = approximates z-h(x0) = error_vector(x0)
	//CHECK(assert_equal(nlf->error_vector(c),actual->get_b()));
}

/* ************************************************************************* */
TEST( NonlinearFactor, linearize_f2 )
{
  Values c = createNoisyValues();

  // Grab a non-linear factor
  Graph nfg = createNonlinearFactorGraph();
  Graph::sharedFactor nlf = nfg[1];

  // We linearize at noisy config from SmallExample
  GaussianFactor::shared_ptr actual = nlf->linearize(c, *c.orderingArbitrary());

  GaussianFactorGraph lfg = createGaussianFactorGraph(*c.orderingArbitrary());
  GaussianFactor::shared_ptr expected = lfg[1];

  CHECK(assert_equal(*expected,*actual));
}

/* ************************************************************************* */
TEST( NonlinearFactor, linearize_f3 )
{
  // Grab a non-linear factor
  Graph nfg = createNonlinearFactorGraph();
  Graph::sharedFactor nlf = nfg[2];

  // We linearize at noisy config from SmallExample
  Values c = createNoisyValues();
  GaussianFactor::shared_ptr actual = nlf->linearize(c, *c.orderingArbitrary());

  GaussianFactorGraph lfg = createGaussianFactorGraph(*c.orderingArbitrary());
  GaussianFactor::shared_ptr expected = lfg[2];

  CHECK(assert_equal(*expected,*actual));
}

/* ************************************************************************* */
TEST( NonlinearFactor, linearize_f4 )
{
  // Grab a non-linear factor
  Graph nfg = createNonlinearFactorGraph();
  Graph::sharedFactor nlf = nfg[3];

  // We linearize at noisy config from SmallExample
  Values c = createNoisyValues();
  GaussianFactor::shared_ptr actual = nlf->linearize(c, *c.orderingArbitrary());

  GaussianFactorGraph lfg = createGaussianFactorGraph(*c.orderingArbitrary());
  GaussianFactor::shared_ptr expected = lfg[3];

  CHECK(assert_equal(*expected,*actual));
}

/* ************************************************************************* */
TEST( NonlinearFactor, size )
{
	// create a non linear factor graph
	Graph fg = createNonlinearFactorGraph();
	
	// create a values structure for the non linear factor graph
	Values cfg = createNoisyValues();
	
	// get some factors from the graph
	Graph::sharedFactor factor1 = fg[0], factor2 = fg[1],
			factor3 = fg[2];
	
	CHECK(factor1->size() == 1);
	CHECK(factor2->size() == 2);
	CHECK(factor3->size() == 2);
}

/* ************************************************************************* */
TEST( NonlinearFactor, linearize_constraint1 )
{
	Vector sigmas = Vector_(2, 0.2, 0.0);
	SharedDiagonal constraint = noiseModel::Constrained::MixedSigmas(sigmas);

	Point2 mu(1., -1.);
	Graph::sharedFactor f0(new simulated2D::Prior(mu, constraint, 1));

	Values config;
	config.insert(simulated2D::PoseKey(1), Point2(1.0, 2.0));
	GaussianFactor::shared_ptr actual = f0->linearize(config, *config.orderingArbitrary());

	// create expected
	Ordering ord(*config.orderingArbitrary());
	Vector b = Vector_(2, 0., -3.);
	JacobianFactor expected(ord["x1"], eye(2), b, constraint);
	CHECK(assert_equal((const GaussianFactor&)expected, *actual));
}

/* ************************************************************************* */
TEST( NonlinearFactor, linearize_constraint2 )
{
	Vector sigmas = Vector_(2, 0.2, 0.0);
	SharedDiagonal constraint = noiseModel::Constrained::MixedSigmas(sigmas);

	Point2 z3(1.,-1.);
	simulated2D::Measurement f0(z3, constraint, 1,1);

	Values config;
	config.insert(simulated2D::PoseKey(1), Point2(1.0, 2.0));
	config.insert(simulated2D::PointKey(1), Point2(5.0, 4.0));
	GaussianFactor::shared_ptr actual = f0.linearize(config, *config.orderingArbitrary());

	// create expected
	Ordering ord(*config.orderingArbitrary());
	Vector b = Vector_(2, -3., -3.);
	JacobianFactor expected(ord["x1"], -1*eye(2), ord["l1"], eye(2), b, constraint);
	CHECK(assert_equal((const GaussianFactor&)expected, *actual));
}

/* ************************************************************************* */
int main() { TestResult tr; return TestRegistry::runAllTests(tr);}
/* ************************************************************************* */

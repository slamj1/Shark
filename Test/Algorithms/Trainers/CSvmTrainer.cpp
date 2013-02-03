//===========================================================================
/*!
 *  \file CSvmTrainer.cpp
 *
 *  \brief test case for the CSvmTrainer
 *
 *
 *  \author T. Glasmachers
 *  \date 2011
 *
 *  \par Copyright (c) 1998-2011:
 *      Institut f&uuml;r Neuroinformatik<BR>
 *      Ruhr-Universit&auml;t Bochum<BR>
 *      D-44780 Bochum, Germany<BR>
 *      Phone: +49-234-32-25558<BR>
 *      Fax:   +49-234-32-14209<BR>
 *      eMail: Shark-admin@neuroinformatik.ruhr-uni-bochum.de<BR>
 *      www:   http://www.neuroinformatik.ruhr-uni-bochum.de<BR>
 *      <BR>
 *
 *
 *  <BR><HR>
 *  This file is part of Shark. This library is free software;
 *  you can redistribute it and/or modify it under the terms of the
 *  GNU General Public License as published by the Free Software
 *  Foundation; either version 3, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this library; if not, see <http://www.gnu.org/licenses/>.
 *
 */
//===========================================================================
#include <iostream>
#include <boost/numeric/ublas/io.hpp>
#define BOOST_TEST_MODULE ALGORITHMS_TRAINERS_CSVMTRAINER
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>


#define SHARK_COUNT_KERNEL_LOOKUPS //in this example, we want to count the kernel lookups


#include <shark/Algorithms/Trainers/CSvmTrainer.h>
#include <shark/Models/Kernels/LinearKernel.h>


using namespace shark;

// This test case consists of training SVMs with
// analytically computable solution. This known
// solution is used to validate the trainer.
BOOST_AUTO_TEST_CASE( CSVM_TRAINER_TEST )
{
	// simple 5-point dataset
	std::vector<RealVector> input(5);
	std::vector<unsigned int> target(5);
	size_t i;
	for (i=0; i<5; i++) input[i].resize(2);
	input[0](0) =  0.0; input[0](1) =  0.0; target[0] = 0;
	input[1](0) =  2.0; input[1](1) =  2.0; target[1] = 1;
	input[2](0) = -1.0; input[2](1) = -8.0; target[2] = 0;
	input[3](0) = -1.0; input[3](1) = -1.0; target[3] = 0;
	input[4](0) =  3.0; input[4](1) =  3.0; target[4] = 1;
	ClassificationDataset dataset(input, target);

	// hard-margin training with linear kernel
	{
		std::cout << "C-SVM hard margin" << std::endl;
		LinearKernel<> kernel;
		KernelExpansion<RealVector> svm(true);
		CSvmTrainer<RealVector> trainer(&kernel, 1e100);
		trainer.sparsify() = false;
		trainer.stoppingCondition().minAccuracy = 1e-8;
		trainer.train(svm, dataset);
		RealVector param = svm.parameterVector();
		BOOST_REQUIRE_EQUAL(param.size(), 6u);
		std::cout << "alpha: "
			<< param(0) << " "
			<< param(1) << " "
			<< param(2) << " "
			<< param(3) << " "
			<< param(4) << std::endl;
		std::cout << "    b: " << param(5) << std::endl;
		std::cout << "kernel computations: " << trainer.accessCount() << std::endl;
		
		// test against analytically known solution
		BOOST_CHECK_SMALL(param(0) + 0.25, 1e-6);
		BOOST_CHECK_SMALL(param(1) - 0.25, 1e-6);
		BOOST_CHECK_SMALL(param(2), 1e-6);
		BOOST_CHECK_SMALL(param(3), 1e-6);
		BOOST_CHECK_SMALL(param(4), 1e-6);
		BOOST_CHECK_SMALL(param(5) + 1.0, 1e-6);
	}

	// soft-margin training with linear kernel
	{
		std::cout << "C-SVM soft margin" << std::endl;
		LinearKernel<> kernel;
		KernelExpansion<RealVector> svm(true);
		CSvmTrainer<RealVector> trainer(&kernel, 0.1);
		trainer.sparsify() = false;
		trainer.stoppingCondition().minAccuracy = 1e-8;
		trainer.train(svm, dataset);
		RealVector param = svm.parameterVector();
		BOOST_REQUIRE_EQUAL(param.size(), 6u);
		std::cout << "alpha: "
			<< param(0) << " "
			<< param(1) << " "
			<< param(2) << " "
			<< param(3) << " "
			<< param(4) << std::endl;
		std::cout << "    b: " << param(5) << std::endl;
		std::cout << "kernel computations: " << trainer.accessCount() << std::endl;

		// test against analytically known solution
		BOOST_CHECK_SMALL(param(0) + 0.1, 1e-6);
		BOOST_CHECK_SMALL(param(1) - 0.1, 1e-6);
		BOOST_CHECK_SMALL(param(2), 1e-6);
		BOOST_CHECK_SMALL(param(3) + 0.0125, 1e-6);
		BOOST_CHECK_SMALL(param(4) - 0.0125, 1e-6);
		BOOST_CHECK_SMALL(param(5) + 0.5, 1e-6);
	}
}
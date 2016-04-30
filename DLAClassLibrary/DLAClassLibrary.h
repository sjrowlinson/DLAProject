// DLAClassLibrary.h

#pragma once

#include "DLAContainer.h"
#include "DLA_2d.h"
#include "DLA_3d.h"

using namespace System;

namespace DLAClassLibrary {

	//public ref class Class1
	//{
		// TODO: Add your methods for this class here.
	//};

	public ref class ManagedDLA2DContainer {

		DLAContainer* native_DLA_container_ptr;

	public:

		ManagedDLA2DContainer() : native_DLA_container_ptr(new DLA_2d()) {}

		ManagedDLA2DContainer(double _coeff_stick) : native_DLA_container_ptr(new DLA_2d(_coeff_stick)) {}

		ManagedDLA2DContainer(LatticeType _lattice_type, AttractorType _attractor_type) : native_DLA_container_ptr(new DLA_2d(_lattice_type, _attractor_type)) {}

		ManagedDLA2DContainer(LatticeType _lattice_type, AttractorType _attractor_type, double _coeff_stick) : native_DLA_container_ptr(new DLA_2d(_lattice_type, _attractor_type, _coeff_stick)) {}

		ManagedDLA2DContainer() {
			delete native_DLA_container_ptr;
		}

		double get_coeff_stick() {
			return native_DLA_container_ptr->get_coeff_stick();
		}

		void set_coeff_stick(const double& _coeff_stick) {
			native_DLA_container_ptr->set_coeff_stick(_coeff_stick);
		}

		size_t size() {
			return native_DLA_container_ptr->size();
		}

		void clear() {
			native_DLA_container_ptr->clear();
		}

		void generate(size_t _n) {
			native_DLA_container_ptr->generate(_n);
		}

		double estimate_fractal_dimension() {
			native_DLA_container_ptr->estimate_fractal_dimension();
		}

	};

	public ref class ManagedDLA3DContainer {

		DLAContainer* native_DLA_container_ptr;

	public:

		ManagedDLA3DContainer() : native_DLA_container_ptr(new DLA_3d()) {}

		ManagedDLA3DContainer(double _coeff_stick) : native_DLA_container_ptr(new DLA_3d(_coeff_stick)) {}

		ManagedDLA3DContainer(LatticeType _lattice_type, AttractorType _attractor_type) : native_DLA_container_ptr(new DLA_3d(_lattice_type, _attractor_type)) {}

		ManagedDLA3DContainer(LatticeType _lattice_type, AttractorType _attractor_type, double _coeff_stick) : native_DLA_container_ptr(new DLA_3d(_lattice_type, _attractor_type, _coeff_stick)) {}

		~ManagedDLA3DContainer() {
			delete native_DLA_container_ptr;
		}

		double get_coeff_stick() {
			return native_DLA_container_ptr->get_coeff_stick();
		}

		void set_coeff_stick(const double& _coeff_stick) {
			native_DLA_container_ptr->set_coeff_stick(_coeff_stick);
		}

		size_t size() {
			return native_DLA_container_ptr->size();
		}

		void clear() {
			native_DLA_container_ptr->clear();
		}

		void generate(size_t _n) {
			native_DLA_container_ptr->generate(_n);
		}

		double estimate_fractal_dimension() {
			native_DLA_container_ptr->estimate_fractal_dimension();
		}

	};

}

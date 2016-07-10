// DLAClassLibrary.h

#pragma once
#include "DLAContainer.h"
#include "DLA_2d.h"
#include "DLA_3d.h"
#include <fstream>

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Concurrent;
using namespace System::Collections::Generic;
using namespace System::Threading;

namespace DLAClassLibrary {

	public enum class ManagedLatticeType {
		Square,
		Triangle,
	};
	
	public enum class ManagedAttractorType {
		Point,
		Line,
		Plane,
	};

	/**
	 * \class ManagedDLA2DContainer
	 *
	 * \brief Managed wrapper for DLA_2d native C++ code class.
	 *
	 * \author Samuel Rowlinson
	 * \date April, 2016
	 */
	public ref class ManagedDLA2DContainer {

		// handle to DLA_2d class
		DLA_2d* native_dla_2d_ptr;

	public:
		/**
		 * \brief Initialises a ManagedDLA2DContainer which has default properties and is initially empty.
		 */
		ManagedDLA2DContainer() : native_dla_2d_ptr(new DLA_2d()) {}

		/**
		 * \brief Initialises a ManagedDLA2DContainer which has a given stickiness coefficient and is initially empty.
		 *
		 * \param _coeff_stick Coefficient of stickiness of aggregrate, in interval (0,1].
		 */
		ManagedDLA2DContainer(double _coeff_stick) : native_dla_2d_ptr(new DLA_2d(_coeff_stick)) {}

		/**
		 * \brief Initialises a ManagedDLA2DContainer with given lattice and attractor types and is initially empty.
		 *
		 * \param _lattice_type Type of lattice aggregate is to be built upon.
		 * \param _attractor_type Type of attractor for initial system seed.
		 */
		ManagedDLA2DContainer(ManagedLatticeType _lattice_type, ManagedAttractorType _attractor_type) : native_dla_2d_ptr(new DLA_2d(static_cast<LatticeType>(_lattice_type), static_cast<AttractorType>(_attractor_type))) {}

		/**
		 * \brief Initialises a ManagedDLA2DContainer with given lattice and attractor types and a given stickiness
		 *        coefficient, and is initially empty.
		 *
		 * \param _lattice_type Type of lattice aggregrate is to be built upon.
		 * \param _attractor_type Type of attractor for initial system seed.
		 * \param _coeff_stick Coefficient of stickiness of aggregrate, in interval (0,1].
		 */
		ManagedDLA2DContainer(ManagedLatticeType _lattice_type, ManagedAttractorType _attractor_type, double _coeff_stick) : 
			native_dla_2d_ptr(new DLA_2d(static_cast<LatticeType>(_lattice_type), static_cast<AttractorType>(_attractor_type), _coeff_stick)) {}

		ManagedDLA2DContainer(ManagedDLA2DContainer^ _other) : native_dla_2d_ptr(new DLA_2d(*_other->native_dla_2d_ptr)) {}

		/**
		 * \brief Destructor, deletes native DLA class handle.
		 */
		~ManagedDLA2DContainer() {
			delete native_dla_2d_ptr;
		}

		/**
		 * \brief Gets the coefficient of stickiness of the aggregate.
		 *
		 * \return Aggregrate stickiness coefficient.
		 */
		double GetCoeffStick() {
			return native_dla_2d_ptr->get_coeff_stick();
		}

		/**
		 * \brief Sets the coefficient of stickiness of the aggregrate.
		 *
		 * \param _coeff_stick Coefficient of stickiness of aggregrate, in interval (0,1].
	     * \throw Throws ArgumentException if _coeff_stick not in (0,1].
		 */
		void SetCoeffStick(double _coeff_stick) {
			try {
				native_dla_2d_ptr->set_coeff_stick(_coeff_stick);
			}
			catch (const std::invalid_argument& ex) {
				String^ err_msg = gcnew String(ex.what());
				throw gcnew ArgumentException(err_msg);
			}
		}

		/**
		 * \brief Sets the type of lattice.
		 *
		 * \param _lattice_type LatticeType to update to.
		 */
		void SetLatticeType(ManagedLatticeType _lattice_type) {
			native_dla_2d_ptr->set_lattice_type(static_cast<LatticeType>(_lattice_type));
		}

		/**
		 * \brief Sets the type of attractor.
		 *
		 * \param _attractor_type AttractorType to update to.
		 */
		void SetAttractorType(ManagedAttractorType _attractor_type) {
			native_dla_2d_ptr->set_attractor_type(static_cast<AttractorType>(_attractor_type));
		}

		/**
		 * \brief Gets the size of the aggregrate.
		 *
		 * \return Size of the DLA structure.
		 */
		std::size_t Size() {
			return native_dla_2d_ptr->size();
		}

		std::size_t GetAggregateMisses() {
			return native_dla_2d_ptr->aggregate_misses();
		}

		/**
		 * \brief Gets the batch_queue from the DLA_2d pointer and processes the data, saving
		 *        each co-ordinate point to a BlockingCollection and dequeuing the batch_queue
		 *        ready to receive next block of aggregate data.
		 *
		 * \return BlockingCollection containing co-ordinates held in current state of batch_queue.
		 */
		BlockingCollection<KeyValuePair<int, int>>^ ProcessBatchQueue() {
			// stores particles in a BlockingQueue configuration
			BlockingCollection<KeyValuePair<int, int>>^ blocking_queue = gcnew BlockingCollection<KeyValuePair<int, int>>();
			// get reference to batch_queue of DLA_2d
			std::queue<std::pair<int,int>>& batch_queue_ref = native_dla_2d_ptr->batch_queue_handle();
			// loop over batch_queue transferring particles to blocking_queue
			while (!batch_queue_ref.empty()) {
				blocking_queue->Add(KeyValuePair<int, int>(batch_queue_ref.front().first, batch_queue_ref.front().second));
				batch_queue_ref.pop();
			}
			return blocking_queue;
		}

		/**
		 * \brief Raises an abort signal stopping any current execution of aggregate generation.
		 */
		void RaiseAbortSignal() {
			native_dla_2d_ptr->raise_abort_signal();
		}

		/**
		 * \brief Clears the aggregrate of all particles.
		 */
		void Clear() {
			native_dla_2d_ptr->clear();
		}

		/**
		 * \brief Generates an aggregrate structure of size _n.
		 *
		 * \param _n Size of aggregrate to produce.
		 */
		void Generate(std::size_t _n) {
			native_dla_2d_ptr->generate(_n);
			//std::string file_path = "C:/Users/Sam/Documents/MATLAB/NMProject/DLASquare2D.txt";
			//std::ofstream of(file_path);
			//native_dla_2d_ptr->write(of, true);
			//of.close();
		}

		/**
		 * \brief Estimates the fractal dimension of the aggregrate.
		 *
		 * \return An estimate of fractal dimension of the DLA structure.
		 */
		double EstimateFractalDimension() {
			return native_dla_2d_ptr->estimate_fractal_dimension();
		}

	};

	/**
	 * \class ManagedDLA3DContainer
	 *
	 * \brief Managed wrapper for DLA_3d native C++ code class.
	 *
	 * \author Samuel Rowlinson
	 * \date April, 2016
	 */
	public ref class ManagedDLA3DContainer {

		// handle to DLAContainer abstract class
		DLA_3d* native_dla_3d_ptr;

	public:
		/**
		 * \brief Initialises a ManagedDLA3DContainer which has default properties and is initially empty.
		 */
		ManagedDLA3DContainer() : native_dla_3d_ptr(new DLA_3d()) {}

		/**
		 * \brief Initialises a ManagedDLA3DContainer which has a given stickiness coefficient and is initially empty.
		 *
		 * \param _coeff_stick Coefficient of stickiness of aggregrate, in interval (0,1].
		 */
		ManagedDLA3DContainer(double _coeff_stick) : native_dla_3d_ptr(new DLA_3d(_coeff_stick)) {}

		/**
		 * \brief Initialises a ManagedDLA3DContainer with given lattice and attractor types and is initially empty.
		 *
		 * \param _lattice_type Type of lattice aggregate is to be built upon.
		 * \param _attractor_type Type of attractor for initial system seed.
		 */
		ManagedDLA3DContainer(ManagedLatticeType _lattice_type, ManagedAttractorType _attractor_type) : native_dla_3d_ptr(new DLA_3d(static_cast<LatticeType>(_lattice_type), static_cast<AttractorType>(_attractor_type))) {}

		/**
		 * \brief Initialises a ManagedDLA3DContainer with given lattice and attractor types and a given stickiness
		 *        coefficient, and is initially empty.
		 *
		 * \param _lattice_type Type of lattice aggregrate is to be built upon.
		 * \param _attractor_type Type of attractor for initial system seed.
		 * \param _coeff_stick Coefficient of stickiness of aggregrate, in interval (0,1].
		 */
		ManagedDLA3DContainer(ManagedLatticeType _lattice_type, ManagedAttractorType _attractor_type, double _coeff_stick) : 
			native_dla_3d_ptr(new DLA_3d(static_cast<LatticeType>(_lattice_type), static_cast<AttractorType>(_attractor_type), _coeff_stick)) {}

		ManagedDLA3DContainer(ManagedDLA3DContainer^ _other) : native_dla_3d_ptr(new DLA_3d(*_other->native_dla_3d_ptr)) {}

		~ManagedDLA3DContainer() {
			delete native_dla_3d_ptr;
		}

		/**
		 * \brief Gets the coefficient of stickiness of the aggregate.
		 *
		 * \return Aggregrate stickiness coefficient.
		 */
		double GetCoeffStick() {
			return native_dla_3d_ptr->get_coeff_stick();
		}

		/**
		 * \brief Sets the coefficient of stickiness of the aggregrate.
		 *
		 * \param _coeff_stick Coefficient of stickiness of aggregrate, in interval (0,1].
		 * \throw Throws ArgumentException if _coeff_stick not in (0,1].
		 */
		void SetCoeffStick(double _coeff_stick) {
			try {
				native_dla_3d_ptr->set_coeff_stick(_coeff_stick);
			}
			catch (const std::invalid_argument& ex) {
				String^ err_msg = gcnew String(ex.what());
				throw gcnew ArgumentException(err_msg);
			}
		}

		/**
		 * \brief Sets the type of lattice.
		 *
		 * \param _lattice_type LatticeType to update to.
		 */
		void SetLatticeType(ManagedLatticeType _lattice_type) {
			native_dla_3d_ptr->set_lattice_type(static_cast<LatticeType>(_lattice_type));
		}

		/**
		 * \brief Sets the type of attractor.
		 *
		 * \param _attractor_type AttractorType to update to.
		 */
		void SetAttractorType(ManagedAttractorType _attractor_type) {
			native_dla_3d_ptr->set_attractor_type(static_cast<AttractorType>(_attractor_type));
		}

		/**
		 * \brief Gets the size of the aggregrate.
		 *
		 * \return Size of the DLA structure.
		 */
		std::size_t Size() {
			return native_dla_3d_ptr->size();
		}

		std::size_t GetAggregateMisses() {
			return native_dla_3d_ptr->aggregate_misses();
		}

		/**
		 * \brief Clears the aggregrate of all particles.
		 */
		void Clear() {
			native_dla_3d_ptr->clear();
		}

		/**
		 * \brief Gets the batch_queue from the DLA_2d pointer and processes the data, saving
		 *        each co-ordinate point to a BlockingCollection and dequeuing the batch_queue
		 *        ready to receive next block of aggregate data.
		 *
		 * \return BlockingCollection containing co-ordinates held in current state of batch_queue.
		 */
		BlockingCollection<Tuple<int,int,int>^>^ ProcessBatchQueue() {
			// stores particles in a BlockingQueue configuration
			BlockingCollection<Tuple<int,int,int>^>^ blocking_queue = gcnew BlockingCollection<Tuple<int,int,int>^>();
			// get reference to batch_queue of DLA_2d
			std::queue<utl::triple<int,int,int>>& batch_queue_ref = native_dla_3d_ptr->batch_queue_handle();
			// loop over batch_queue transferring particles to blocking_queue
			while (!batch_queue_ref.empty()) {
				blocking_queue->Add(gcnew Tuple<int,int,int>(batch_queue_ref.front().first, batch_queue_ref.front().second, batch_queue_ref.front().third));
				batch_queue_ref.pop();
			}
			return blocking_queue;
		}

		/**
		 * \brief Raises an abort signal stopping any current execution of aggregate generation.
		 */
		void RaiseAbortSignal() {
			native_dla_3d_ptr->raise_abort_signal();
		}

		/**
		 * \brief Generates an aggregrate structure of size _n.
		 *
		 * \param _n Size of aggregrate to produce.
		 */
		void Generate(std::size_t _n) {
			native_dla_3d_ptr->generate(_n);
		}

		/**
		 * \brief Estimates the fractal dimension of the aggregrate.
		 *
		 * \return An estimate of fractal dimension of the DLA structure.
		 */
		double EstimateFractalDimension() {
			return native_dla_3d_ptr->estimate_fractal_dimension();
		}

	};

}

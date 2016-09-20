// DLAClassLibrary.h
#pragma once
#include "DLAContainer.h"
#include "DLA_2d.h"
#include "DLA_3d.h"
#include <fstream>

namespace DLAClassLibrary {
	/**
	 * \enum ManagedLatticeType
	 *
	 * \brief Geometry of lattice.
	 */
	public enum class ManagedLatticeType {
		Square,
		Triangle,
	};
	/**
	 * \enum ManagedAttractorType
	 *
	 * \brief Geometry of attractor seed.
	 */
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
	private:
		// handle to DLA_2d class
		DLA_2d* native_dla_2d_ptr;
		// lock object used for critical section locking on ProcessBatchQueue()
		System::Object^ lock_obj = gcnew System::Object();
	public:
		// CONSTRUCTION/ASSIGNMENT
		/**
		 * \brief Initialises a ManagedDLA2DContainer which has default properties and is initially empty.
		 */
		ManagedDLA2DContainer() : native_dla_2d_ptr(new DLA_2d()) {}
		/**
		 * \brief Initialises a ManagedDLA2DContainer which has a given stickiness coefficient and is initially empty.
		 *
		 * \param coeff_stick Coefficient of stickiness of aggregrate, in interval (0,1].
		 */
		ManagedDLA2DContainer(double coeff_stick) : native_dla_2d_ptr(new DLA_2d(coeff_stick)) {}
		/**
		 * \brief Initialises a ManagedDLA2DContainer with given lattice and attractor types and is initially empty.
		 *
		 * \param lattice_type Type of lattice aggregate is to be built upon.
		 * \param attractor_type Type of attractor for initial system seed.
		 */
		ManagedDLA2DContainer(ManagedLatticeType ltt, ManagedAttractorType att, std::size_t att_size) 
			: native_dla_2d_ptr(new DLA_2d(static_cast<lattice_type>(ltt), static_cast<attractor_type>(att), att_size)) {}
		/**
		 * \brief Initialises a ManagedDLA2DContainer with given lattice and attractor types and a given stickiness
		 *        coefficient, and is initially empty.
		 *
		 * \param lattice_type Type of lattice aggregrate is to be built upon.
		 * \param attractor_type Type of attractor for initial system seed.
		 * \param coeff_stick Coefficient of stickiness of aggregrate, in interval (0,1].
		 */
		ManagedDLA2DContainer(ManagedLatticeType ltt, ManagedAttractorType att, std::size_t att_size, double coeff_stick) : 
			native_dla_2d_ptr(new DLA_2d(static_cast<lattice_type>(ltt), static_cast<attractor_type>(att), att_size, coeff_stick)) {}
		/**
		 * \brief Copy constructor. Initialises a ManagedDLA2DContainer with a copy of the contents of `other`.
		 *
		 * \param other `ManagedDLA2DContainer` instance to use as data source.
		 */
		ManagedDLA2DContainer(ManagedDLA2DContainer^ other) : native_dla_2d_ptr(new DLA_2d(*other->native_dla_2d_ptr)) {}
		/**
		 * \brief Destructor, deletes native DLA class handle.
		 */
		~ManagedDLA2DContainer() { delete native_dla_2d_ptr; }
		// AGGREGATE PROPERTIES
		/**
		 * \brief Gets the coefficient of stickiness of the aggregate.
		 *
		 * \return Aggregrate stickiness coefficient.
		 */
		double GetCoeffStick() { return native_dla_2d_ptr->get_coeff_stick(); }
		/**
		 * \brief Sets the coefficient of stickiness of the aggregrate.
		 *
		 * \param coeff_stick Coefficient of stickiness of aggregrate, in interval (0,1].
	     * \throw Throws ArgumentException if _coeff_stick not in (0,1].
		 */
		void SetCoeffStick(double coeff_stick) {
			try {
				native_dla_2d_ptr->set_coeff_stick(coeff_stick);
			}
			catch (const std::invalid_argument& ex) {
				System::String^ err_msg = gcnew System::String(ex.what());
				throw gcnew System::ArgumentException(err_msg);
			}
		}
		/**
		 * \brief Sets the type of lattice.
		 *
		 * \param lattice_type LatticeType to update to.
		 */
		void SetLatticeType(ManagedLatticeType ltt) { native_dla_2d_ptr->set_lattice_type(static_cast<lattice_type>(ltt)); }
		/**
		 * \brief Sets the type of attractor.
		 *
		 * \param attractor_type AttractorType to update to.
		 */
		void SetAttractorType(ManagedAttractorType att, std::size_t att_size) { native_dla_2d_ptr->set_attractor_type(static_cast<attractor_type>(att), att_size); }
		/**
		 * \brief Gets the size of the aggregrate.
		 *
		 * \return Size of the DLA structure.
		 */
		std::size_t Size() { return native_dla_2d_ptr->size(); }
		/**
		 * \brief Returns the square of the radius of the aggregate.
		 *
		 * \return Radius squared of the diffusion limited aggregate.
		 */
		std::size_t GetAggregateSpanningDistance() { return native_dla_2d_ptr->aggregate_spanning_distance(); }
		/**
		 * \brief Returns the number of times a particle failed to stick to the aggregate.
		 *
		 * \return Number of occurrences of failed particle sticking.
		 */
		std::size_t GetAggregateMisses() { return native_dla_2d_ptr->aggregate_misses(); }
		/**
		 * \brief Estimates the fractal dimension of the aggregrate.
		 *
		 * \return An estimate of fractal dimension of the DLA structure.
		 */
		double EstimateFractalDimension() { return native_dla_2d_ptr->estimate_fractal_dimension(); }
		// SIGNAL/EVENT FIRING
		/**
		 * \brief Raises an abort signal stopping any current execution of aggregate generation.
		 */
		void RaiseAbortSignal() { native_dla_2d_ptr->raise_abort_signal(); }
		/**
		 * \brief Alters the state of the continuous flag, determining whether aggregate generation
		 *        is continuous or finite.
		 *
		 * \param continuous Flag value to set.
		 */
		void ChangeContinuousFlag(bool continuous) { native_dla_2d_ptr->change_continuous_flag(continuous); }
		// AGGREGATE GENERATION / PROCESSING
		/**
		 * \brief Configures the spawn location of random walking particles used for aggregate generation.
		 * \param above_below Pair determining spawn locations.
		 */
		void SetRandomWalkParticleSpawnSource(System::Collections::Generic::KeyValuePair<bool, bool> above_below) {
			native_dla_2d_ptr->set_random_walk_particle_spawn_source(std::make_pair(above_below.Key, above_below.Value));
		}
		/**
		 * \brief Clears the aggregrate of all particles.
		 */
		void Clear() { native_dla_2d_ptr->clear(); }
		/**
		 * \brief Generates an aggregrate structure of size _n.
		 *
		 * \param n Size of aggregrate to produce.
		 */
		void Generate(std::size_t n) { native_dla_2d_ptr->generate(n); }
		/**
		 * \brief Fetches the buffer from the DLA_3d pointer and consumes the buffer data in-order, adding
		 *        each particle from the buffer into a List of KVPs to be used by .NET code for rendering.
		 *
		 * \param marked_index Index to which consumer thread read to in the buffer in the last polling interval.
		 * \return List of KVPs containing co-ordinates of current batch of particles to render.
		 */
		System::Collections::Generic::List<System::Collections::Generic::KeyValuePair<int, int>>^ ConsumeBuffer(std::size_t marked_index) {
			System::Collections::Generic::List<System::Collections::Generic::KeyValuePair<int, int>>^ buffer =
				gcnew System::Collections::Generic::List<System::Collections::Generic::KeyValuePair<int, int>>();
			if (native_dla_2d_ptr->aggregate_buffer().empty()) return buffer;
			System::Threading::Monitor::Enter(lock_obj);	// define critical section start
			try {	// execute critical section
				// read from last marked buffer index up to size of buffer and write these data to batch list
				for (int i = marked_index; i < native_dla_2d_ptr->aggregate_buffer().size(); ++i) {
					buffer->Add(System::Collections::Generic::KeyValuePair<int, int>(
						native_dla_2d_ptr->aggregate_buffer()[i].first,
						native_dla_2d_ptr->aggregate_buffer()[i].second
						)
					);
				}
			}
			finally { System::Threading::Monitor::Exit(lock_obj); } // exit critical section by releasing exclusive lock
			return buffer;
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
	private:
		// handle to DLAContainer abstract class
		DLA_3d* native_dla_3d_ptr;
		System::Object^ lock_obj = gcnew System::Object();
	public:
		// CONSTRUCTION/ASSIGNMENT
		/**
		 * \brief Initialises a ManagedDLA3DContainer which has default properties and is initially empty.
		 */
		ManagedDLA3DContainer() : native_dla_3d_ptr(new DLA_3d()) {}
		/**
		 * \brief Initialises a ManagedDLA3DContainer which has a given stickiness coefficient and is initially empty.
		 *
		 * \param coeff_stick Coefficient of stickiness of aggregrate, in interval (0,1].
		 */
		ManagedDLA3DContainer(double coeff_stick) : native_dla_3d_ptr(new DLA_3d(coeff_stick)) {}
		/**
		 * \brief Initialises a ManagedDLA3DContainer with given lattice and attractor types and is initially empty.
		 *
		 * \param lattice_type Type of lattice aggregate is to be built upon.
		 * \param attractor_type Type of attractor for initial system seed.
		 */
		ManagedDLA3DContainer(ManagedLatticeType ltt, ManagedAttractorType att, std::size_t att_size) 
			: native_dla_3d_ptr(new DLA_3d(static_cast<lattice_type>(ltt), static_cast<attractor_type>(att), att_size)) {}
		/**
		 * \brief Initialises a ManagedDLA3DContainer with given lattice and attractor types and a given stickiness
		 *        coefficient, and is initially empty.
		 *
		 * \param lattice_type Type of lattice aggregrate is to be built upon.
		 * \param attractor_type Type of attractor for initial system seed.
		 * \param coeff_stick Coefficient of stickiness of aggregrate, in interval (0,1].
		 */
		ManagedDLA3DContainer(ManagedLatticeType ltt, ManagedAttractorType att, std::size_t att_size, double coeff_stick) : 
			native_dla_3d_ptr(new DLA_3d(static_cast<lattice_type>(ltt), static_cast<attractor_type>(att), att_size, coeff_stick)) {}
		/**
		 * \brief Copy constructor. Initialises a `ManagedDLA3DContainer` with a copy of the contents of `other`.
		 *
		 * \param other `ManagedDLA3DContainer` instance to use as data source.
		 */
		ManagedDLA3DContainer(ManagedDLA3DContainer^ other) : native_dla_3d_ptr(new DLA_3d(*other->native_dla_3d_ptr)) {}
		/**
		 * \brief Destructor, deletes native DLA class handle.
		 */
		~ManagedDLA3DContainer() { delete native_dla_3d_ptr; }
		// AGGREGATE PROPERTIES
		/**
		 * \brief Gets the coefficient of stickiness of the aggregate.
		 *
		 * \return Aggregrate stickiness coefficient.
		 */
		double GetCoeffStick() { return native_dla_3d_ptr->get_coeff_stick(); }
		/**
		 * \brief Sets the coefficient of stickiness of the aggregrate.
		 *
		 * \param coeff_stick Coefficient of stickiness of aggregrate, in interval (0,1].
		 * \throw Throws ArgumentException if _coeff_stick not in (0,1].
		 */
		void SetCoeffStick(double coeff_stick) {
			try {
				native_dla_3d_ptr->set_coeff_stick(coeff_stick);
			}
			catch (const std::invalid_argument& ex) {
				System::String^ err_msg = gcnew System::String(ex.what());
				throw gcnew System::ArgumentException(err_msg);
			}
		}
		/**
		 * \brief Sets the type of lattice.
		 *
		 * \param lattice_type LatticeType to update to.
		 */
		void SetLatticeType(ManagedLatticeType ltt) { native_dla_3d_ptr->set_lattice_type(static_cast<lattice_type>(ltt)); }
		/**
		 * \brief Sets the type of attractor.
		 *
		 * \param attractor_type AttractorType to update to.
		 */
		void SetAttractorType(ManagedAttractorType att, std::size_t att_size) { native_dla_3d_ptr->set_attractor_type(static_cast<attractor_type>(att), att_size); }
		/**
		 * \brief Gets the size of the aggregrate.
		 *
		 * \return Size of the DLA structure.
		 */
		std::size_t Size() { return native_dla_3d_ptr->size(); }
		/**
		 * \brief Returns the square of the radius of the aggregate.
		 *
		 * \return Radius squared of the diffusion limited aggregate.
		 */
		std::size_t GetAggregateSpanningDistance() { return native_dla_3d_ptr->aggregate_spanning_distance(); }
		/**
		 * \brief Returns the number of times a particle failed to stick to the aggregate.
		 *
		 * \return Number of occurrences of failed particle sticking.
		 */
		std::size_t GetAggregateMisses() { return native_dla_3d_ptr->aggregate_misses(); }
		/**
		 * \brief Estimates the fractal dimension of the aggregrate.
		 *
		 * \return An estimate of fractal dimension of the DLA structure.
		 */
		double EstimateFractalDimension() { return native_dla_3d_ptr->estimate_fractal_dimension(); }
		// SIGNAL/EVENT FIRING
		/**
		 * \brief Raises an abort signal stopping any current execution of aggregate generation.
		 */
		void RaiseAbortSignal() { native_dla_3d_ptr->raise_abort_signal(); }
		/**
		 * \brief Alters the state of the continuous flag, determining whether aggregate generation
		 *        is continuous or finite.
		 *
		 * \param continuous Flag value to set.
		 */
		void ChangeContinuousFlag(bool _continuous) { native_dla_3d_ptr->change_continuous_flag(_continuous); }
		// AGGREGATE GENERATION / PROCESSING
		/**
		 * \brief Configures the spawn location of random walking particles used for aggregate generation.
		 * \param above_below Pair determining spawn locations.
		 */
		void SetRandomWalkParticleSpawnSource(System::Collections::Generic::KeyValuePair<bool, bool> above_below) {
			native_dla_3d_ptr->set_random_walk_particle_spawn_source(std::make_pair(above_below.Key, above_below.Value));
		}
		/**
		 * \brief Clears the aggregrate of all particles.
		 */
		void Clear() { native_dla_3d_ptr->clear(); }
		/**
		 * \brief Generates an aggregrate structure of size `n`.
		 *
		 * \param n Size of aggregrate to produce.
		 */
		void Generate(std::size_t n) { native_dla_3d_ptr->generate(n); }
		/**
		 * \brief Fetches the buffer from the DLA_3d pointer and consumes the buffer data in-order, adding
		 *        each particle from the buffer into a List of Tuples to be used by .NET code for rendering.
		 *
		 * \param marked_index Index to which consumer thread read to in the buffer in the last polling interval.
		 * \return List of Tuples containing co-ordinates of current batch of particles to render.
		 */
		System::Collections::Generic::List<System::Tuple<int, int, int>^>^ ConsumeBuffer(std::size_t marked_index) {
			System::Collections::Generic::List<System::Tuple<int,int,int>^>^ buffer =
				gcnew System::Collections::Generic::List<System::Tuple<int, int, int>^>();
			if (native_dla_3d_ptr->aggregate_buffer().empty()) return buffer;
			System::Threading::Monitor::Enter(lock_obj);	// define critical section start
			try {	// execute critical section
				// read from last marked buffer index up to size of buffer and write these data to batch list
				for (int i = marked_index; i < native_dla_3d_ptr->aggregate_buffer().size(); ++i) {
					buffer->Add(gcnew System::Tuple<int, int, int>(
						std::get<0>(native_dla_3d_ptr->aggregate_buffer()[i]),
						std::get<1>(native_dla_3d_ptr->aggregate_buffer()[i]),
						std::get<2>(native_dla_3d_ptr->aggregate_buffer()[i])
						)
					);
				}
			}
			finally { System::Threading::Monitor::Exit(lock_obj); }	// exit critical section by releasing exclusive lock
			return buffer;
		}
	};
}

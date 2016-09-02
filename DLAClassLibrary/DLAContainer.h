#pragma once
#include "mutable_comp_priority_queue.h"
#include "utilities.h"
#include <algorithm>
#include <cmath>
#include <iterator>
#include <ostream>
#include <stdexcept>
#include <utility>
#include <vector>

/**
 * \class DLAContainer
 *
 * \brief Abstract class containing data about a diffusion limited aggregate with virtual methods for generating an aggregate, finding
 *        its properties and writing to a stream. This class is overridden by DLA_2d and DLA_3d for generating 2D and 3D aggregates.
 *
 * \author Samuel Rowlinson
 * \date April, 2016
 */
class DLAContainer {
public:
	// CONSTRUCTION/ASSIGNMENT
	/**
	 * \brief Default constructor, initialises empty aggregate with default lattice type
	 *        of LatticeType::SQUARE and attractor type of AttractorType::POINT.
	 *
	 * \param _coeff_stick [= 1.0] Coefficient of stickiness.
	 * \throw Throws std::invalid_argument exception if _coeff_stick not in (0,1].
	 */
	DLAContainer(const double& _coeff_stick = 1.0);
	/**
	 * \brief Constructor with parameters for type of lattice and type of attractor.
	 *
	 * \param ltt Type of lattice for DLA construction.
	 * \param att Type of attractor for DLA construction.
	 * \throw Throws std::invalid_argument exception if _coeff_stick not in (0,1].
	 */
	DLAContainer(lattice_type ltt, attractor_type att, std::size_t att_size, const double& _coeff_stick = 1.0);
	/**
	 * \brief Copy constructor, copies contents of parameterised DLAContainer to this.
	 *
	 * \param other const reference to DLAContainer instance.
	 */
	DLAContainer(const DLAContainer& other);
	/**
	 * \brief Move constructor, uses move-semantics for constructing a DLAContainer 
	 *        from an rvalue reference of a DLAContainer - leaving that container
	 *        in a valid but unspecified state.
	 *
 	 * \param other rvalue reference to DLAContainer instance.
	 */
	DLAContainer(DLAContainer&& other);
	/**
	 * \brief Destructs the container.
	 */
	virtual ~DLAContainer();
	// AGGREGATE PROPERTIES
	/**
	 * \brief Gets the size of the aggregate.
	 *
	 * \return Size of the aggregate.
	 */
	virtual std::size_t size() const noexcept = 0;
	/**
	 * \brief Gets the stickiness coefficient of the aggregate.
	 *
	 * \return Coefficient of stickiness of this aggregate.
	 */
	double get_coeff_stick() const noexcept;
	/**
	 * \brief Sets the coefficient of stickiness of the aggregrate.
	 *
	 * \param _coeff_stick Coefficient of stickiness to set.
	 * \throw Throws std::invalid_argument exception if _coeff_stick not in (0,1].
	 */
	void set_coeff_stick(const double& _coeff_stick);
	/**
	 * \brief Gets the type of lattice.
	 *
	 * \return lattice_type of this instance.
	 */
	lattice_type get_lattice_type() const noexcept;
	/**
	 * \brief Gets the type of attractor and its associated size.
	 *
	 * \return `std::pair` of the `attractor_type` and its size.
	 */
	std::pair<attractor_type, std::size_t> get_attractor_type() const noexcept;
	/**
	 * \brief Sets the type of lattice.
	 *
	 * \param _lattice_type LatticeType to update to.
	 */
	void set_lattice_type(lattice_type ltt) noexcept;
	/**
	 * \brief Sets the type of attractor.
	 *
	 * \param _attractor_type AttractorType to update to.
	 */
	virtual void set_attractor_type(attractor_type att, std::size_t att_size);
	/**
	 * \brief Initialises the attractor data structure corresponding to the
	 *        `attractor_type` and size of the attractor.
	 */
	virtual void initialise_attractor_structure() = 0;
	/**
	 * \brief Gets the distance of the aggregate from its origin attractor.
	 *
	 * Returns: - the radius squared of furthest point if the aggregate attractor type
	 * is `attractor_type::POINT`,
	 * - the distance of the furthest point from the attractor line if the attractor
	 * type is `attractor_type::LINE`.
	 *
	 * \return Distance of further point in the aggregate.
	 */
	std::size_t aggregate_spanning_distance() const noexcept;
	/**
	 * \brief Gets the number of misses corresponding to when a particle collides with
	 *        the aggregate but fails to stick to it. If `_coeff_stick = 1.0` then this
	 *        will always return zero.
	 *
	 * \return Number of aggregate misses.
	 */
	std::size_t aggregate_misses() const noexcept;
	// MODIFIERS
	/**
	 * \brief Raises an abort signal, stopping any current aggregate generation.
	 */
	void raise_abort_signal() noexcept;
	/**
	 * \brief Changes the value of the continous_flag field to specified argument.
	 *
	 * \param _continuous Boolean value to change `continuous_flag` to.
	 */
	void change_continuous_flag(bool _continuous) noexcept;
	/**
	 * \brief Clears the aggregrate structure.
	 */
	virtual void clear();
	/**
	 * \brief Generates a diffusion limited aggregate consisting of the parameterised
	 *        number of particles.
	 *
	 * \param _n Number of particles to generate in the DLA.
	 */
	virtual void generate(std::size_t n) = 0;
	/**
	 * \brief Computes an estimate of the fractal dimension for the aggregrate.
	 *
	 * \return Estimate of aggregrate fractal dimension.
	 */
	virtual double estimate_fractal_dimension() const = 0;
	/**
	 * \brief Write aggregate to an output stream std::ostream, defaults to "write-as-is" where
	 *        the aggregate data is written without any sorting.
	 *
	 * Secondary default bool argument allows for sorting of the aggregate by the order in which
	 * particles were added to the aggregate before writing these data to the output stream.
	 *
	 * \param os Instance of std::ostream for writing to.
	 * \param sort_by_gen_order [= false] Flag to signal sorting of data 
	 *        by order of generation before writing.
	 */
	virtual std::ostream& write(std::ostream& os, bool sort_by_gen_order = false) const = 0;
protected:
	lattice_type lattice;
	attractor_type attractor;
	double coeff_stick;
	utl::uniform_random_probability_generator<> pr_gen;
	bool abort_signal = false;
	bool continuous = false;
	std::size_t aggregate_span = 0;
	std::size_t aggregate_misses_ = 0;
	std::size_t attractor_size;
	/**
	 * \brief Updates position of random walking particle.
	 *
	 * \param _current Current position, to be updated.
	 * \param _movement_choice Double in [0,1] for direction choice.
	 */
	void update_particle_position(std::pair<int,int>& current, const double& movement_choice) const noexcept;
	/**
	 * \copydoc DLAContainer::update_particle_position(std::pair<int,int>,const double&)
	 */
	void update_particle_position(std::tuple<int,int,int>& current, const double& movement_choice) const noexcept;
	/**
	 * \brief Checks for collision with boundary of lattice and reflects.
	 *
	 * \param _current Current position, updated if collision occurred.
	 * \param _previous Previous position.
	 * \param _spawn_diam Current diameter of spawning box.
	 */
	bool lattice_boundary_collision(std::pair<int,int>& current, const std::pair<int,int>& previous, const int& spawn_diam) const noexcept;
	/**
	 * \copydoc DLAContainer::lattice_boundary_collision(std::pair<int,int>&,const std::pair<int,int>&,const int&)
	 */
	bool lattice_boundary_collision(std::tuple<int,int,int>& current, const std::tuple<int,int,int>& previous, const int& spawn_diam) const noexcept;
};

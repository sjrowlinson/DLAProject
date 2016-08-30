#pragma once
#include "utilities.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iterator>
#include <ostream>
#include <stdexcept>
#include <utility>
#include <vector>

/**
 * \enum LatticeType
 *
 * \brief Defines several types of lattice applicable to both 2D and 3D DLA systems, for example
 *        LatticeType::SQUARE will be interpreted as a square lattice in a 2D system and cubic 
 *        lattice in a 3D system.
 */
enum class LatticeType {
	SQUARE, // square for 2D, cubic for 3D
	TRIANGLE, // triangular for 2D, hexagonal for 3D
};

/**
 * \brief Defines several types of attractor geometry applicable to both 2D and 3D DLA systems. Note
 *        that using some attractor geometries defined for 3D systems (i.e. AttractorType::PLANE) is
 *        not allowed when applying to a DLA_2d object.
 */
enum class AttractorType {
	POINT,
	LINE,
	PLANE, // not applicable for 2D
};

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
	 * \param _lattice_type Type of lattice for DLA construction.
	 * \param _attractor_type Type of attractor for DLA construction.
	 * \throw Throws std::invalid_argument exception if _coeff_stick not in (0,1].
	 */
	DLAContainer(LatticeType _lattice_type, AttractorType _attractor_type, const double& _coeff_stick = 1.0);
	/**
	 * \brief Copy constructor, copies contents of parameterised DLAContainer to this.
	 *
	 * \param _other const reference to DLAContainer instance.
	 */
	DLAContainer(const DLAContainer& _other);
	/**
	 * \brief Move constructor, uses move-semantics for constructing a DLAContainer 
	 *        from an rvalue reference of a DLAContainer - leaving that container
	 *        in a valid but unspecified state.
	 *
 	 * \param _other rvalue reference to DLAContainer instance.
	 */
	DLAContainer(DLAContainer&& _other);
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
	LatticeType get_lattice_type() const noexcept;
	/**
	 * \brief Gets the type of attractor.
	 *
	 * \return attractor_type of this instance.
	 */
	AttractorType get_attractor_type() const noexcept;
	/**
	 * \brief Sets the type of lattice.
	 *
	 * \param _lattice_type LatticeType to update to.
	 */
	void set_lattice_type(LatticeType _lattice_type) noexcept;
	/**
	 * \brief Sets the type of attractor.
	 *
	 * \param _attractor_type AttractorType to update to.
	 */
	virtual void set_attractor_type(AttractorType _attractor_type);
	/**
	 * \brief Gets the radial distance squared of the aggregate from its origin.
	 *
	 * \return Radial distance squared of the aggregate.
	 */
	std::size_t aggregate_radius_sqd() const noexcept;
	/**
	 * \brief Gets the number of misses corresponding to when a particle collides with
	 *        the aggregate but fails to stick to it. If `_coeff_stick = 1.0` then this
	 *        will always return zero.
	 *
	 * \return Number of aggregate misses.
	 */
	std::size_t aggregate_misses() const noexcept;
	/**
	 * \briefs Returns the rate at which particles are being added to the aggregate structure
	 *         in units of particles per second.
	 *
	 * \return Rate of aggregrate generation.
	 */
	std::size_t generation_rate() const noexcept;
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
	 * \param _os Instance of std::ostream for writing to.
	 * \param _sort_by_map_value [= false] Flag to signal sorting of data before writing.
	 */
	virtual std::ostream& write(std::ostream& _os, bool _sort_by_map_value = false) const = 0;
protected:
	LatticeType lattice_type;
	AttractorType attractor_type;
	double coeff_stick;
	utl::uniform_random_probability_generator<> pr_gen;
	bool abort_signal = false;
	bool continuous = false;
	std::size_t aggregate_radius_sqd_ = 0;
	std::size_t aggregate_misses_ = 0;
	std::size_t gen_rate = 0;
	/**
	 * \brief Updates position of random walking particle.
	 *
	 * \param _current Current position, to be updated.
	 * \param _movement_choice Double in [0,1] for direction choice.
	 */
	void update_particle_position(std::pair<int,int>& _current, const double& _movement_choice) const noexcept;
	/**
	 * \copydoc DLAContainer::update_particle_position(std::pair<int,int>,const double&)
	 */
	void update_particle_position(std::tuple<int,int,int>& _current, const double& _movement_choice) const noexcept;
	/**
	 * \brief Checks for collision with boundary of lattice and reflects.
	 *
	 * \param _current Current position, updated if collision occurred.
	 * \param _previous Previous position.
	 * \param _spawn_diam Current diameter of spawning box.
	 */
	bool lattice_boundary_collision(std::pair<int,int>& _current, const std::pair<int,int>& _previous, const int& _spawn_diam) const noexcept;
	/**
	 * \copydoc DLAContainer::lattice_boundary_collision(std::pair<int,int>&,const std::pair<int,int>&,const int&)
	 */
	bool lattice_boundary_collision(std::tuple<int,int,int>& _current, const std::tuple<int,int,int>& _previous, const int& _spawn_diam) const noexcept;
};

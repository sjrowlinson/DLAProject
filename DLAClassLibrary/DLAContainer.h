#pragma once
#include "utilities.h"
#include <algorithm>
#include <cmath>
#include <iterator>
#include <ostream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

enum class LatticeType {
	SQUARE,
	TRIANGLE,
};

enum class AttractorType {
	POINT,
	LINE,
	PLANE,
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

	virtual ~DLAContainer();

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
	void set_attractor_type(AttractorType _attractor_type) noexcept;

	/**
	 * \brief Sets the number of data points to generate for bounding radii data
	 *
	 * \param _npoints Number of data points to generate.
	 */
	void set_bound_radii_npoints(size_t _npoints) noexcept;

	/**
	 * \brief Raises an abort signal, stopping any current aggregate generation.
	 */
	void raise_abort_signal() noexcept;

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
	virtual void generate(size_t n) = 0;

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

	/**
	 * \brief Writes the bounding radii for a predetermined number of intervals of particles
	 *        in the aggregate to a std::ostream instance.
	 *
	 * Data are written in the following format:
	 *
	 \begin{verbatim}
	 N1	R1
	 N2	R2
	 ...	...
	 \end{verbatim}
	 *
	 * where N1 is the number of particles in the aggregate and R1 is the corresponding minimal bounding
	 * radius which contains this aggregate state (written in \t delimited columns). The number of data
	 * points is default set to 50, but can be altered with the method set_bound_radii_npoints.
	 *
	 * \param _os Instance of std::ostream for writing to.
	 * \return modified std::ostream instance _os containing bounding_radii_vec data.
	 */
	std::ostream& write_bounding_radii_data(std::ostream& _os) const;

protected:
	LatticeType lattice_type;
	AttractorType attractor_type;
	double coeff_stick;
	std::mt19937 mt_eng;
	bool abort_signal = false;
	std::vector<std::pair<size_t, double>> bounding_radii_vec;
	size_t bound_radii_npoints = 50;

	/**
	 * \brief Updates position of random walking particle.
	 *
	 * \param _current Current position, to be updated.
	 * \param _movement_choice Double in [0,1] for direction choice.
	 */
	void update_particle_position(std::pair<int,int>& _current, const double& _movement_choice) const noexcept;
	void update_particle_position(utl::triple<int,int,int>& _current, const double& _movement_choice) const noexcept;

	/**
	 * \brief Checks for collision with boundary of lattice and reflects.
	 *
	 * \param _current Current position, updated if collision occurred.
	 * \param _previous Previous position.
	 * \param _spawn_diam Current diameter of spawning box.
	 */
	bool lattice_boundary_collision(std::pair<int,int>& _current, const std::pair<int,int>& _previous, const int& _spawn_diam) const noexcept;
	bool lattice_boundary_collision(utl::triple<int,int,int>& _current, const utl::triple<int,int,int>& _previous, const int& _spawn_diam) const noexcept;

};

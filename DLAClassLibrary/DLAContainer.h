#pragma once

#include <algorithm>
#include <cmath>
#include <iterator>
#include <ostream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "utilities.h"

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
* @class DLAContainer
*
* @brief Abstract class containing data about a diffusion limited aggregates with virtual methods for generating an aggregate, finding
*        its properties and writing to a stream. This class is overridden by DLA_2d and DLA_3d for generating 2D and 3D aggregates.
*
* @author Samuel Rowlinson
* @date April, 2016
*/
class DLAContainer {

public:

	/**
	* @brief Default constructor, initialises empty aggregate with default lattice type
	*        of LatticeType::SQUARE and attractor type of AttractorType::POINT.
	*
	* @param _coeff_stick [= 1.0] Coefficient of stickiness
	* @throw Throws std::invalid_argument exception if _coeff_stick not in (0,1]
	*/
	DLAContainer(const double& _coeff_stick = 1.0);

	/**
	* @brief Constructor with parameters for type of lattice and type of attractor
	*
	* @param _lattice_type Type of lattice for DLA construction
	* @param _attractor_type Type of attractor for DLA construction
	* @throw Throws std::invalid_argument exception if _coeff_stick not in (0,1]
	*/
	DLAContainer(LatticeType _lattice_type, AttractorType _attractor_type, const double& _coeff_stick = 1.0);

	/**
	* @brief Copy constructor
	*
	* @param _other const reference to DLAContainer instance
	*/
	DLAContainer(const DLAContainer& _other);

	/**
	* @brief Move constructor
	*
	* @param _other rvalue reference to DLAContainer instance
	*/
	DLAContainer(DLAContainer&& _other);

	virtual ~DLAContainer();

	virtual size_t size() const noexcept = 0;

	double get_coeff_stick() const noexcept;

	/**
	* @brief Sets the coefficient of stickiness of the aggregrate
	*
	* @param _coeff_stick Coefficient of stickiness to set
	*/
	void set_coeff_stick(const double& _coeff_stick);

	/**
	* @brief Sets the number of data points to generate for bounding radii data
	*
	* @param _npoints Number of data points to generate
	*/
	void set_bound_radii_npoints(size_t _npoints) noexcept;

	/**
	* @brief Clears the aggregrate structure
	*/
	virtual void clear();

	/**
	* @brief Generates a diffusion limited aggregate consisting of the parameterised
	*        number of particles using a stickiness coefficient given.
	*
	* @param _n Number of particles to generate in the DLA
	*/
	virtual void generate(size_t n) = 0;

	/**
	* @brief Computes an estimate of the fractal dimension for the aggregrate
	*
	* @return Estimate of aggregrate fractal dimension
	*/
	virtual double estimate_fractal_dimension() const = 0;

	/**
	* @brief Write aggregate to an output stream std::ostream, defaults to "write-as-is" where
	*        the aggregate data is written without any sorting.
	*
	* Secondary default bool argument allows for sorting of the aggregate by the order in which
	* particles were added to the aggregate before writing these data to the output stream.
	*
	* @param _os Instance of std::ostream
	* @param _sort_by_map_value [= false] Flag to signal sorting of data before writing
	*/
	virtual std::ostream& write(std::ostream& _os, bool _sort_by_map_value = false) const = 0;

	/**
	* @brief Writes the bounding radii for a predetermined number of intervals of particles
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
	* @param _os Instance of std::ostream
	* @return modified std::ostream instance _os containing bounding_radii_vec data
	*/
	std::ostream& write_bounding_radii_data(std::ostream& _os) const;

protected:

	LatticeType lattice_type;
	AttractorType attractor_type;
	double coeff_stick;
	std::mt19937 mt_eng;

	std::vector<std::pair<size_t, double>> bounding_radii_vec;
	size_t bound_radii_npoints = 50;

	/**
	 * @brief Spawns particle in a random boundary position dependent upon attractor type and current size of aggregate
	 *
	 * @param _x Position to set spawn in x co-ordinate
	 * @param _y Position to set spawn in y co-ordinate
	 * @param _spawn_diam Variable to set for allowed spawning diameter
	 * @param _dist std::uniform_real_distribution<> for prng
	 */
	virtual void spawn_particle(int& _x, int& _y, int& _spawn_diam, std::uniform_real_distribution<>& _dist) noexcept = 0;
	/**
	 * @brief Spawns particle in a random boundary position dependent upon attractor type and current size of aggregate
	 *
	 * @param _x Position to set spawn in x co-ordinate
	 * @param _y Position to set spawn in y co-ordinate
	 * @param _z Position to set spawn in z co-ordinate
	 * @param _spawn_diam Variable to set for allowed spawning diameter
	 * @param _dist std::uniform_real_distribution<> for prng
	 */
	virtual void spawn_particle(int& _x, int& _y, int& _z, int& _spawn_diam, std::uniform_real_distribution<>& _dist) noexcept = 0;

	/**
	 * @brief Update the position of a particle via unbiased random walk motion
	 *
	 * @param _x Position in x co-ordinate
	 * @param _y Position in y co-ordinate
	 * @param _movement_choice Probability determining direction of movement
	 */
	void update_particle_position(int& _x, int& _y, const double& _movement_choice) const noexcept;
	/**
	 * @brief Update the position of a particle via unbiased random walk motion
	 *
	 * @param _x Position in x co-ordinate
	 * @param _y Position in y co-ordinate
	 * @param _z Position in z co-ordinate
	 * @param _movement_choice Probability determining direction of movement
	 */
	void update_particle_position(int& _x, int& _y, int& _z, const double& _movement_choice) const noexcept;

	/**
	 * @brief Check for collision of a particle with a lattice boundary and reflect if true
	 *
	 * @param _x Position in x co-ordinate
	 * @param _y Position in y co-ordinate
	 * @param _prev_x Previous position in x
	 * @param _prev_y Previous position in y
	 * @param _spawn_diam Current diameter of spawning zone
	 * @return true if boundary collision occurred, false otherwise
	 */
	bool lattice_boundary_collision(int& _x, int& _y, const int& _prev_x, const int& _prev_y, const int& _spawn_diam) const noexcept;
	/**
	 * @brief Check for collision of a particle with a lattice boundary and reflect if true
	 *
	 * @param _x Position in x co-ordinate
	 * @param _y Position in y co-ordinate
	 * @param _z Position in z co-ordinate
	 * @param _prev_x Previous position in x
	 * @param _prev_y Previous position in y
	 * @param _prev_z Previous position in z
	 * @param _spawn_diam Current diameter of spawning zone
	 * @return true if boundary collision occurred, false otherwise
	 */
	bool lattice_boundary_collision(int& _x, int& _y, int& _z, const int& _prev_x, const int& _prev_y, const int& _prev_z, const int& _spawn_diam) const noexcept;

	/**
	 * @brief Check for collision of a particle with the aggregate and add particle to
	 *        aggregrate structure if true (subject to _sticky_pr <= coeff_stick)
	 *
	 * @param _x Position in x co-ordinate
	 * @param _y Position in y co-ordinate
	 * @param _prev_x Previous position in x
	 * @param _prev_y Previous position in y
	 * @param _sticky_pr Constraint on stickiness, if <= coeff_stick then sticking occurs
 	 * @param _count Current number of particles in aggregate
 	 * @return true if aggregrate collision occurred and particle was added, false otherwise
	 */
	virtual bool aggregate_collision(const int& _x, const int& _y, const int& _prev_x, const int& _prev_y, const double& _sticky_pr, size_t& _count) = 0;
	/**
	 * @brief Check for collision of a particle with the aggregate and add particle to
	 *        aggregrate structure if true (subject to _sticky_pr <= coeff_stick)
	 *
	 * @param _x Position in x co-ordinate
	 * @param _y Position in y co-ordinate
	 * @param _z Position in z co-ordinate
	 * @param _prev_x Previous position in x
	 * @param _prev_y Previous position in y
	 * @param _prev_z Previous position in z
	 * @param _sticky_pr Constraint on stickiness, if <= coeff_stick then sticking occurs
	 * @param _count Current number of particles in aggregate
	 * @return true if aggregrate collision occurred and particle was added, false otherwise
	 */
	virtual bool aggregate_collision(const int& _x, const int& _y, const int& _z, const int& _prev_x, const int& _prev_y, const int& _prev_z, const double& _sticky_pr, size_t& _count) = 0;

};

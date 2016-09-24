#pragma once
#include "DLAContainer.h"
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/**
 * \class DLA_2d
 *
 * \brief Defines a diffusion limited aggregate on a two-dimensional lattice.
 *
 * Provides a class which allows for the generation, manipulation and statistical processing
 * of a Diffusion Limited Aggregate (DLA) on a 2D lattice. This DLA can be produced on a 
 * variety of 2D lattice types, given by LatticeType enumerated constants, with a choice of
 * different attractor geometries, given by AttractorType enumerated constants.
 *
 * \author Samuel Rowlinson
 * \date May, 2016
 */
class DLA_2d : public DLAContainer {
	typedef std::unordered_map<std::pair<int, int>,
		std::size_t,
		utl::tuple_hash> aggregate2d_unordered_map;
	typedef std::unordered_set<std::pair<int, int>,
		utl::tuple_hash> attractor2d_unordered_set;
	typedef utl::mutable_comp_priority_queue<std::pair<int, int>,
		std::vector<std::pair<int, int>>,
		utl::distance_comparator> aggregate2d_priority_queue;
	typedef std::vector<std::pair<int, int>> aggregate2d_buffer_vector;
public:
	/**
	 * \brief Default constructor, initialises empty 2d aggregate with given stickiness coefficient.
	 *
	 * \param _coeff_stick [= 1.0] Coefficient of stickiness.
	 * \throw Throws std::invalid_argument exception if _coeff_stick not in (0,1].
	 */
	DLA_2d(const double& _coeff_stick = 1.0);
	/**
	 * \brief Initialises empty 2d aggregate with specified lattice and attractor types.
	 *
	 * \param _lattice_type Type of lattice for construction.
	 * \param _attractor_type Type of attractor of initial aggregate.
	 * \param _coeff_stick [= 1.0] Coefficient of stickiness.
	 */
	DLA_2d(lattice_type ltt, attractor_type att, std::size_t att_size, const double& _coeff_stick = 1.0);
	/**
	 * \brief Copy constructor, copies contents of parameterised DLA_2d to this.
	 *
	 * \param _other const reference to DLA_2d instance.
	 */
	DLA_2d(const DLA_2d& other);
	/**
	 * \brief Move constructor, uses move-semantics for constructing a DLA_2d.
	 *        from an rvalue reference of a DLA_2d - leaving that container.
 	 *        in a valid but unspecified state.
	 *
	 * \param _other rvalue reference to DLA_2d instance
	 */
	DLA_2d(DLA_2d&& other) noexcept;
	/**
	 * \copydoc DLAContainer::~DLAContainer()
	 */
	~DLA_2d() override = default;
	/**
	 * \copydoc DLAContainer::size()
	 */
	std::size_t size() const noexcept override;
	/**
	 * \brief Returns a const reference to the buffer vector of the aggregate, storing
	 *        the particles of the aggregate structure in the order in which they were
	 *        added.
	 * \return const reference to buffer of 2d aggregate.
	 */
	const aggregate2d_buffer_vector& aggregate_buffer() const noexcept;
	/**
	 * \copydoc DLAContainer::set_attractor_type(attractor_type)
	 * \throw Throws std::invalid_argument exception if _attractor_type is invalid for 2D lattice.
	 */
	void set_attractor_type(attractor_type att, std::size_t att_size) override;
	/**
	 * \copydoc DLAContainer::initialise_attractor_structure()
	 */
	void initialise_attractor_structure() override;
	/**
	 * \copydoc DLAContainer::clear()
	 */
	void clear() override;
	/**
	 * \brief Generates a 2D diffusion limited aggregate consisting of the parameterised
	 *        number of particles.
	 *
	 * \param n Number of particles to generate in the 2D DLA.
	 */
	void generate(std::size_t n) override;
	/**
	 * \copydoc DLAContainer::estimate_fractal_dimension()
	 */
	double estimate_fractal_dimension() const override;
	/**
	 * \copydoc DLAContainer::write(std::ostream&,bool)
	 */
	std::ostream& write(std::ostream& os, bool sort_by_gen_order = false) const override;
private:
	// map to store aggregate point co-ordinates as Keys and
	// order of adding to the container as Values
	aggregate2d_unordered_map aggregate_map;
	// set storing point co-ordinates of attractor
	attractor2d_unordered_set attractor_set;
	// priority queue for retrieving co-ordinates of aggregate
	// particle furthest from origin in constant time
	aggregate2d_priority_queue aggregate_pq;
	// buffer vector holding co-ordinates of aggregate structure in
	// order in which they were added
	aggregate2d_buffer_vector buffer;
	/**
	 * \brief Spawns a particle at a random position on the lattice boundary.
	 *
	 * \param spawn_pos Position of spawn.
	 * \param spawn_diam Diameter of spawn zone.
	 * \param dist Uniform real distribution for probability generation.
	 */
	void spawn_particle(std::pair<int,int>& spawn_pos, int& spawn_diam) noexcept;
	/**
	 * \brief Checks for collision of random-walking particle with aggregate structure
	 *        and adds this particles' previous position to aggregate if collision occurred.
	 *
	 * \param current Current co-ordinates of particle.
	 * \param previous Previous co-ordinates of particle.
	 * \param sticky_pr |coeff_stick - _sticky_pr| = |1 - probability of sticking to aggregate|.
	 * \param count Current number of particles generated in aggregate.
	 */
	bool aggregate_collision(const std::pair<int,int>& current, const std::pair<int,int>& previous, const double& sticky_pr, std::size_t& count);
	/**
	 * \brief Pushes a particle into the aggregate, inserting the co-ordinates `p` into
	 *        all necessary data structures used to contain the aggregate particles.
	 *
	 * \param p Co-ordinates of particle to insert.
	 * \param count Index number of particle in aggregate.
	 */
	void push_particle(const std::pair<int, int>& p, std::size_t count);
};
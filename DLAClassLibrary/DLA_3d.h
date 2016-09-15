#pragma once
#include "DLAContainer.h"
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/**
 * \class DLA_3d
 *
 * \brief Defines a diffusion limited aggregate on a three-dimensional lattice.
 *
 * Provides a class which allows for the generation, manipulation and statistical processing
 * of a Diffusion Limited Aggregate (DLA) on a 3D lattice. This DLA can be produced on a 
 * variety of 3D lattice types, given by LatticeType enumerated constants, with a choice of
 * different attractor geometries, given by AttractorType enumerated constants.
 *
 * \author Samuel Rowlinson
 * \date May, 2016
 */
class DLA_3d : public DLAContainer {
	typedef std::unordered_map<std::tuple<int, int, int>,
		std::size_t,
		utl::tuple_hash> aggregate3d_unordered_map;
	typedef std::unordered_set<std::tuple<int, int, int>,
		utl::tuple_hash> attractor3d_unordered_set;
	typedef utl::mutable_comp_priority_queue<std::tuple<int, int, int>,
		std::vector<std::tuple<int, int, int>>,
		utl::distance_comparator> aggregate3d_priority_queue;
	typedef std::deque<std::tuple<int, int, int>> aggregate3d_batch_queue;
public:
	/**
	 * \brief Default constructor, initialises empty 3d aggregate with given sticky coefficient.
	 *
	 * \param _coeff_stick [= 1.0] Coefficient of stickiness.
	 * \throw Throws std::invalid_argument exception if _coeff_stick not in (0,1].
	 */
	DLA_3d(const double& _coeff_stick = 1.0);
	/**
	 * \brief Initialises empty 3d aggregate with specified lattice and attractor types.
	 *
	 * \param _lattice_type Type of lattice for construction.
	 * \param _attractor_type Type of attractor of initial aggregate.
	 * \param _coeff_stick [= 1.0] Coefficient of stickiness.
	 */
	DLA_3d(lattice_type ltt, attractor_type att, std::size_t att_size, const double& _coeff_stick = 1.0);
	/**
	 * \brief Copy constructor, copies contents of parameterised DLA_3d to this.
	 *
	 * \param _other const reference to DLA_3d instance.
	 */
	DLA_3d(const DLA_3d& other);
	/**
	 * \brief Move constructor, uses move-semantics for constructing a DLA_3d.
	 *        from an rvalue reference of a DLA_3d - leaving that container.
 	 *        in a valid but unspecified state.
	 *
	 * \param _other rvalue reference to DLA_2d instance.
	 */
	DLA_3d(DLA_3d&& other);
	/**
	 * \copydoc DLAContainer::~DLAContainer()
	 */
	~DLA_3d() override = default;
	/**
	 * \copydoc DLAContainer::size()
	 */
	std::size_t size() const noexcept override;
	/**
	 * \brief Gets a non-const reference to the batch_queue of the aggregate, used
	 *        in C++/CLI ManagedDLA3DContainer::ProcessBatchQueue for GUI updating.
	 *
	 * \return reference to batch_queue of 2d aggregate.
	 */
	aggregate3d_batch_queue& batch_queue_handle() noexcept;
	/**
	 * \copydoc DLAContainer::set_attractor_type(attractor_type)
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
	 * \brief Generates a 3D diffusion limited aggregate consisting of the parameterised
	 *        number of particles.
	 *
	 * \param _n Number of particles to generate in the 3D DLA.
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
	aggregate3d_unordered_map aggregate_map;
	// set storing point co-ordinates of attractor
	attractor3d_unordered_set attractor_set;
	// priority queue for retrieving co-ordinates of aggregate
	// particle furthest from origin in constant time
	aggregate3d_priority_queue aggregate_pq;
	// queue for multi-thread batching - holds a buffer of aggregate
	// points to be consumed by aggregate listening thread
	aggregate3d_batch_queue batch_queue;
	/**
	 * \brief Spawns a particle at a random position on the lattice boundary.
	 *
	 * \param _current Position of spawn.
	 * \param _spawn_diam Diameter of spawn zone.
	 * \param _dist Uniform real distribution for probability generation.
	 */
	void spawn_particle(std::tuple<int,int,int>& current, int& spawn_diam) noexcept;
	/**
	 * \brief Checks for collision of random-walking particle with aggregate structure
	 *        and adds this particles' previous position to aggregate if collision occurred.
	 *
	 * \param _current Current co-ordinates of particle.
	 * \param _previous Previous co-ordinates of particle.
	 * \param _sticky_pr |coeff_stick - _sticky_pr| = |1 - probability of sticking to aggregate|.
	 * \param _count Current number of particles generated in aggregate.
	 */
	bool aggregate_collision(const std::tuple<int,int,int>& current, const std::tuple<int,int,int>& previous, const double& sticky_pr, std::size_t& count);
	/**
	 * \brief Pushes a particle into the aggregate, inserting the co-ordinates `p` into
	 *        all necessary data structures used to contain the aggregate particles.
	 *
	 * \param p Co-ordinates of particle to insert.
	 * \param count Index number of particle in aggregate.
	 */
	void push_particle(const std::tuple<int, int, int>& p, std::size_t count);
};
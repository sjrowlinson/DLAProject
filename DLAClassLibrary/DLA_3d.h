#pragma once
#include "DLAContainer.h"
#include <queue>
#include <unordered_map>
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
	DLA_3d(LatticeType _lattice_type, AttractorType _attractor_type, const double& _coeff_stick = 1.0);
	/**
	 * \brief Copy constructor, copies contents of parameterised DLA_3d to this.
	 *
	 * \param _other const reference to DLA_3d instance.
	 */
	DLA_3d(const DLA_3d& _other);
	/**
	 * \brief Move constructor, uses move-semantics for constructing a DLA_3d.
	 *        from an rvalue reference of a DLA_3d - leaving that container.
 	 *        in a valid but unspecified state.
	 *
	 * \param _other rvalue reference to DLA_2d instance.
	 */
	DLA_3d(DLA_3d&& _other);
	~DLA_3d();

	/**
	 * \copydoc DLAContainer::size()
	 */
	size_t size() const noexcept override;
	/**
	 * \brief Gets a non-const reference to the batch_queue of the aggregate, used
	 *        in C++/CLI ManagedDLA3DContainer::ProcessBatchQueue for GUI updating.
	 *
	 * \return reference to batch_queue of 2d aggregate.
	 */
	std::queue<std::tuple<int, int, int>>& batch_queue_handle() noexcept;
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
	std::unordered_map<std::tuple<int, int, int>, std::size_t, utl::tuple_hash> aggregate_map;
	// priority queue for retrieving co-ordinates of aggregate
	// particle furthest from origin in constant time
	std::priority_queue<std::tuple<int, int, int>, std::vector<std::tuple<int, int, int>>, utl::distance_comparator> aggregate_pq;
	// queue for multi-thread batching - holds a buffer of aggregate
	// points to be consumed by aggregate listening thread
	std::queue<std::tuple<int, int, int>> batch_queue;
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
};
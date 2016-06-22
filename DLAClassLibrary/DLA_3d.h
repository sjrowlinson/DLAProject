#pragma once
#include "DLAContainer.h"
#include <queue>
#include <unordered_map>
#include <vector>

/**
 * \struct triple_hash
 *
 * \brief Implements a hash function object for a triple of generic types.
 */
struct triple_hash {
	template<typename _Ty1, typename _Ty2, typename _Ty3> std::size_t operator()(const utl::triple<_Ty1, _Ty2, _Ty3>& _t) const {
		return 51 + std::hash<_Ty1>()(_t.first) * 51 + std::hash<_Ty2>()(_t.second) + std::hash<_Ty3>()(_t.third);
	}
};

/**
 * \struct distance_comparator_3d
 *
 * \brief Implements a comparator function object for triple<int,int,int> objects which can be used to
 *        to choose the instance of a triple which has a greater distance from the origin.
 */
struct distance_comparator_3d {
	bool operator()(const utl::triple<int, int, int>& _lhs, const utl::triple<int, int, int>& _rhs) {
		return (_lhs.first*_lhs.first + _lhs.second*_lhs.second + _lhs.third*_lhs.third) <
			(_rhs.first*_rhs.first + _rhs.second*_rhs.second + _rhs.third*_rhs.third);
	}
};

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
	std::queue<utl::triple<int, int, int>>& batch_queue_handle() noexcept;

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
	void generate(size_t _n) override;

	/**
	 * \copydoc DLAContainer::estimate_fractal_dimension()
	 */
	double estimate_fractal_dimension() const override;

	/**
	 * \copydoc DLAContainer::write(std::ostream&,bool)
	 */
	std::ostream& write(std::ostream& _os, bool _sort_by_map_value = false) const override;

private:
	// map to store aggregate point co-ordinates as Keys and
	// order of adding to the container as Values
	std::unordered_map<utl::triple<int, int, int>, size_t, triple_hash> aggregate_map;
	// priority queue for retrieving co-ordinates of aggregate
	// particle furthest from origin in constant time
	std::priority_queue<utl::triple<int, int, int>, std::vector<utl::triple<int, int, int>>, distance_comparator_3d> aggregate_pq;
	std::queue<utl::triple<int, int, int>> batch_queue;

	/**
	 * \brief Spawns a particle at a random position on the lattice boundary.
	 *
	 * \param[out] _current Position of spawn.
	 * \param[out] _spawn_diam Diameter of spawn zone.
	 * \param[in] _dist Uniform real distribution for probability generation.
	 */
	void spawn_particle(utl::triple<int,int,int>& _current, int& _spawn_diam, std::uniform_real_distribution<>& _dist) noexcept;

	/**
	 * \brief Checks for collision of random-walking particle with aggregate structure
	 *        and adds this particles' previous position to aggregate if collision occurred.
	 *
	 * \param _current Current co-ordinates of particle.
	 * \param _previous Previous co-ordinates of particle.
	 * \param _sticky_pr |coeff_stick - _sticky_pr| = |1 - probability of sticking to aggregate|.
	 * \param _count Current number of particles generated in aggregate.
	 */
	bool aggregate_collision(const utl::triple<int,int,int>& _current, const utl::triple<int,int,int>& _previous, const double& _sticky_pr, size_t& count);

};
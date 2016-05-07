#pragma once

#include <queue>
#include <unordered_map>
#include <vector>

#include "DLAContainer.h"

/**
 * @struct pair_hash
 *
 * @brief Implements a hash function for a std::pair of generic types.
 */
struct pair_hash {
	template<typename _Ty1, typename _Ty2> std::size_t operator()(const std::pair<_Ty1, _Ty2>& _p) const {
		return 51 + std::hash<_Ty1>()(_p.first) * 51 + std::hash<_Ty2>()(_p.second);
	}
};

/**
 * @struct distance_comparator
 *
 * @brief Implements a comparator function object for std::pair<int,int> objects which can be used to
 *        to choose the instance of a std::pair which has a greater distance from the origin.
 */
struct distance_comparator {
	bool operator()(const std::pair<int, int>& _lhs, const std::pair<int, int>& _rhs) const {
		return (_lhs.first*_lhs.first + _lhs.second*_lhs.second) < (_rhs.first*_rhs.first + _rhs.second*_rhs.second);
	}
};

/**
 * @class DLA_2d
 *
 * @brief Defines a diffusion limited aggregate on a two-dimensional lattice.
 *
 * @author Samuel Rowlinson
 * @date May, 2016
 */
class DLA_2d : public DLAContainer {

public:

	/**
	 * @brief Default constructor, initialises empty 2d aggregate with given stickiness coefficient.
	 *
	 * @param _coeff_stick [= 1.0] Coefficient of stickiness
	 * @throw Throws std::invalid_argument exception if _coeff_stick not in (0,1]
	 */
	DLA_2d(const double& _coeff_stick = 1.0);

	/**
	 * @brief Initialises empty 2d aggregate with specified lattice and attractor types.
	 *
	 * @param _lattice_type Type of lattice for construction 
	 * @param _attractor_type Type of attractor of initial aggregate
	 * @param _coeff_stick [= 1.0] Coefficient of stickiness
	 */
	DLA_2d(LatticeType _lattice_type, AttractorType _attractor_type, const double& _coeff_stick = 1.0);

	/**
	 * @brief Copy constructor
	 *
	 * @param _other const reference to DLA_2d instance
	 */
	DLA_2d(const DLA_2d& _other);

	/**
	 * @brief Move constructor
	 *
	 * @param _other rvalue reference to DLA_2d instance
	 */
	DLA_2d(DLA_2d&& _other);

	~DLA_2d();

	size_t size() const noexcept override;

	const std::unordered_map<std::pair<int, int>, size_t, pair_hash>& get_aggregate_map() const noexcept;

	/**
	 * @brief Clears the aggregrate structure
	 */
	void clear() override;

	/**
	 * @brief Generates a 2D diffusion limited aggregate consisting of the parameterised
	 *        number of particles using a stickiness coefficient given.
	 *
	 * @param _n Number of particles to generate in the 2D DLA
	 */
	void generate(size_t _n) override;

	double estimate_fractal_dimension() const override;

	std::ostream& write(std::ostream& _os, bool _sort_by_map_value = false) const override;

private:
	// map to store aggregate point co-ordinates as Keys and
	// order of adding to the container as Values
	std::unordered_map<std::pair<int, int>, size_t, pair_hash> aggregate_map;
	// priority queue for retrieving co-ordinates of aggregate
	// particle furthest from origin in constant time
	std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, distance_comparator> aggregate_pq;

	void spawn_particle(int& _x, int& _y, int& _spawn_diam, std::uniform_real_distribution<>& _dist) noexcept override;
	void spawn_particle(int& _x, int& _y, int& _z, int& _spawn_diam, std::uniform_real_distribution<>& _dist) noexcept override;

	bool aggregate_collision(const int& _x, const int& _y, const int& _prev_x, const int& _prev_y, const double& _sticky_pr, size_t& _count) override;
	bool aggregate_collision(const int& _x, const int& _y, const int& _z, const int& _prev_x, const int& _prev_y, const int& _prev_z, const double& _sticky_pr, size_t& count) override;

};
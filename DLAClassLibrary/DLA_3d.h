#pragma once

#include <queue>
#include <unordered_map>
#include <vector>

#include "DLAContainer.h"

/**
 * @struct triple_hash
 *
 * @brief Implements a hash function for a triple object of generic types.
 */
struct triple_hash {
	template<typename _Ty1, typename _Ty2, typename _Ty3> std::size_t operator()(const triple<_Ty1, _Ty2, _Ty3>& _t) const {
		return 51 + std::hash<_Ty1>()(_t.first) * 51 + std::hash<_Ty2>()(_t.second) + std::hash<_Ty3>()(_t.third);
	}
};

/**
 * @struct distance_comparator_3d
 *
 * @brief Implements a comparator function object for triple<int,int,int> objects which can be used to
 *        to choose the instance of a triple which has a greater distance from the origin.
 */
struct distance_comparator_3d {
	bool operator()(const triple<int, int, int>& _lhs, const triple<int, int, int>& _rhs) {
		return (_lhs.first*_lhs.first + _lhs.second*_lhs.second + _lhs.third*_lhs.third) <
			(_rhs.first*_rhs.first + _rhs.second*_rhs.second + _rhs.third*_rhs.third);
	}
};

class DLA_3d : public DLAContainer {

public:

	DLA_3d(const double& _coeff_stick = 1.0);

	DLA_3d(LatticeType _lattice_type, AttractorType _attractor_type, const double& _coeff_stick = 1.0);

	DLA_3d(const DLA_3d& _other);

	DLA_3d(DLA_3d&& _other);

	~DLA_3d();

	size_t size() const noexcept override;

	void clear() override;

	void generate(size_t _n) override;

	double estimate_fractal_dimension() const override;

	std::ostream& write(std::ostream& _os, bool _sort_by_map_value = false) const override;

private:
	// map to store aggregate point co-ordinates as Keys and
	// order of adding to the container as Values
	std::unordered_map<triple<int, int, int>, size_t, triple_hash> aggregate_map;
	// priority queue for retrieving co-ordinates of aggregate
	// particle furthest from origin in constant time
	std::priority_queue<triple<int, int, int>, std::vector<triple<int, int, int>>, distance_comparator_3d> aggregate_pq;

	void spawn_particle(int& _x, int& _y, int& _spawn_diam, std::uniform_real_distribution<>& _dist) noexcept override;
	void spawn_particle(int& _x, int& _y, int& _z, int& _spawn_diam, std::uniform_real_distribution<>& _dist) noexcept override;

	bool aggregate_collision(const int& _x, const int& _y, const int& _prev_x, const int& _prev_y, const double& _sticky_pr, size_t& _count) override;
	bool aggregate_collision(const int& _x, const int& _y, const int& _z, const int& _prev_x, const int& _prev_y, const int& _prev_z, const double& _sticky_pr, size_t& count) override;

};
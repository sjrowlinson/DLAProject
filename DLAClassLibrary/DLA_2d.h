#pragma once

#include <queue>
#include <unordered_map>
#include <vector>

#include "DLAContainer.h"

struct pair_hash {
	template<typename _Ty1, typename _Ty2> std::size_t operator()(const std::pair<_Ty1, _Ty2>& _p) const {
		return 51 + std::hash<_Ty1>()(_p.first) * 51 + std::hash<_Ty2>()(_p.second);
	}
};

struct distance_comparator {
	bool operator()(const std::pair<int, int>& _lhs, const std::pair<int, int>& _rhs) const {
		return (_lhs.first*_lhs.first + _lhs.second*_lhs.second) < (_rhs.first*_rhs.first + _rhs.second*_rhs.second);
	}
};

class DLA_2d : public DLAContainer {

public:

	DLA_2d(const double& _coeff_stick = 1.0);

	DLA_2d(LatticeType _lattice_type, AttractorType _attractor_type, const double& _coeff_stick = 1.0);

	DLA_2d(const DLA_2d& _other);

	DLA_2d(DLA_2d&& _other);

	~DLA_2d();

	size_t size() const noexcept override;

	void clear() override;

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

	bool aggregate_collision(int& _x, int& _y, const int& _prev_x, const int& _prev_y, const double& _sticky_pr, size_t& _count) override;
	bool aggregate_collision(int& _x, int& _y, int& _z, const int& _prev_x, const int& _prev_y, const int& _prev_z, const double& _sticky_pr, size_t& count) override;

};
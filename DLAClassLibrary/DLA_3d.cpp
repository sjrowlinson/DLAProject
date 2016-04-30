#include "Stdafx.h"
#include "DLA_3d.h"

DLA_3d::DLA_3d(const double& _coeff_stick) : DLAContainer(_coeff_stick) {

}

DLA_3d::DLA_3d(LatticeType _lattice_type, AttractorType _attractor_type, const double& _coeff_stick) : DLAContainer(_lattice_type, _attractor_type, _coeff_stick) {

}

DLA_3d::DLA_3d(const DLA_3d& _other) : DLAContainer(_other) {
	// deep copy fields of _other to this
	aggregate_map = _other.aggregate_map;
	aggregate_pq = _other.aggregate_pq;
}

DLA_3d::DLA_3d(DLA_3d&& _other) : DLAContainer(_other) {
	// deep copy fields of _other to this
	aggregate_map = _other.aggregate_map;
	aggregate_pq = _other.aggregate_pq;
	// set _other container fields to default values
	_other.aggregate_map = std::unordered_map<triple<int, int, int>, size_t, triple_hash>();
	_other.aggregate_pq = std::priority_queue<triple<int, int, int>, std::vector<triple<int, int, int>>, distance_comparator_3d>();
}

DLA_3d::~DLA_3d() {

}

size_t DLA_3d::size() const noexcept {
	return aggregate_map.size();
}

void DLA_3d::clear() {
	DLAContainer::clear();
	aggregate_map.clear();
	aggregate_pq = std::priority_queue<triple<int, int, int>, std::vector<triple<int, int, int>>, distance_comparator_3d>();
}

void DLA_3d::generate(size_t _n) {
	size_t count = 0;
	triple<int, int, int> origin_sticky = make_triple(0, 0, 0);
	aggregate_map.insert(std::make_pair(origin_sticky, count));
	aggregate_pq.push(origin_sticky);

	int x = 0;
	int y = 0;
	int z = 0;
	int prev_x = x;
	int prev_y = y;
	int prev_z = z;

	bool has_next_spawned = false;
	int spawn_diameter = 0;
	const unsigned int boundary_offset = 16;

	std::uniform_real_distribution<> dist(0.0, 1.0);

	size_t fractal_data_interval = _n / bound_radii_npoints;
	size_t prev_count_taken = count;

	while (count < _n) {
		if (!has_next_spawned) {
			int rmax_sqd = aggregate_pq.top().first*aggregate_pq.top().first + aggregate_pq.top().second*aggregate_pq.top().second + aggregate_pq.top().third*aggregate_pq.top().third;
			spawn_diameter = 2 * static_cast<int>(std::sqrt(rmax_sqd)) + boundary_offset;

			double placement_pr = dist(mt_eng);

			// Spawn on negative constant z plane of bounding box
			if (placement_pr < 1.0 / 6.0) {
				x = static_cast<int>(spawn_diameter*(dist(mt_eng) - 0.5));
				y = static_cast<int>(spawn_diameter*(dist(mt_eng) - 0.5));
				z = -spawn_diameter / 2;
			}
			// Spawn on positive constant z plane of bounding box
			else if (placement_pr >= 1.0 / 6.0 && placement_pr < 2.0 / 6.0) {
				x = static_cast<int>(spawn_diameter*(dist(mt_eng) - 0.5));
				y = static_cast<int>(spawn_diameter*(dist(mt_eng) - 0.5));
				z = spawn_diameter / 2;
			}
			// Spawn on negative constant x plane of bounding box
			else if (placement_pr >= 2.0 / 6.0 && placement_pr < 3.0 / 6.0) {
				x = -spawn_diameter / 2;
				y = static_cast<int>(spawn_diameter*(dist(mt_eng) - 0.5));
				z = static_cast<int>(spawn_diameter*(dist(mt_eng) - 0.5));
			}
			// Spawn on positive constant x plane of bounding box
			else if (placement_pr >= 3.0 / 6.0 && placement_pr < 4.0 / 6.0) {
				x = spawn_diameter / 2;
				y = static_cast<int>(spawn_diameter*(dist(mt_eng) - 0.5));
				z = static_cast<int>(spawn_diameter*(dist(mt_eng) - 0.5));
			}
			// Spawn on negative constant y plane of bounding box
			else if (placement_pr >= 4.0 / 6.0 && placement_pr < 5.0 / 6.0) {
				x = static_cast<int>(spawn_diameter*(dist(mt_eng) - 0.5));
				y = -spawn_diameter / 2;
				z = static_cast<int>(spawn_diameter*(dist(mt_eng) - 0.5));
			}
			// Spawn on positive constant z plane of bounding box
			else if (placement_pr >= 5.0 / 6.0 && placement_pr < 1.0) {
				x = static_cast<int>(spawn_diameter*(dist(mt_eng) - 0.5));
				y = spawn_diameter / 2;
				z = static_cast<int>(spawn_diameter*(dist(mt_eng) - 0.5));
			}
			has_next_spawned = true;
		}
		double movement_choice = dist(mt_eng);

		prev_x = x;
		prev_y = y;
		prev_z = z;

		update_particle_position(x, y, z, movement_choice);

		lattice_boundary_collision(x, y, z, prev_x, prev_y, prev_z, spawn_diameter);

		double stick_pr = dist(mt_eng);

		if (aggregate_collision(x, y, z, prev_x, prev_y, prev_z, stick_pr, count)) {
			has_next_spawned = false;
		}

		if (!(size() % fractal_data_interval) && size() != prev_count_taken) {
			double rmax_sqd = aggregate_pq.top().first*aggregate_pq.top().first + aggregate_pq.top().second*aggregate_pq.top().second + aggregate_pq.top().third*aggregate_pq.top().third;
			bounding_radii_vec.push_back(std::make_pair(aggregate_map.size(), std::sqrt(rmax_sqd)));
			prev_count_taken = size();
		}

	}

}

double DLA_3d::estimate_fractal_dimension() const {
	int rmax_sqd = aggregate_pq.top().first*aggregate_pq.top().first + aggregate_pq.top().second*aggregate_pq.top().second + aggregate_pq.top().third*aggregate_pq.top().third;
	double bounding_radius = std::sqrt(rmax_sqd);

	return std::log(aggregate_map.size()) / std::log(bounding_radius);
}

std::ostream& DLA_3d::write(std::ostream& _os, bool _sort_by_map_value) const {
	// sort by order particles were added to the aggregate
	if (_sort_by_map_value) {
		// std::vector container to store aggregate map values
		std::vector<std::pair<size_t, triple<int, int, int>>> agg_vec;
		for (auto it = aggregate_map.cbegin(); it != aggregate_map.cend(); ++it) {
			agg_vec.push_back(std::make_pair(it->second, it->first));
		}

		struct {
			bool operator()(const std::pair<size_t, triple<int, int, int>>& _lhs, const std::pair<size_t, triple<int, int, int>>& _rhs) const {
				return _lhs.first < _rhs.first;
			}
		} sort_aggregate;

		std::sort(agg_vec.begin(), agg_vec.end(), sort_aggregate);

		for (auto it = agg_vec.cbegin(); it < agg_vec.cend(); ++it) {
			_os << it->second << "\n";
		}
	}
	else {
		for (auto it = aggregate_map.cbegin(); it != aggregate_map.cend(); ++it) {
			_os << it->second << "\t" << it->first << "\n";
		}
	}
	return _os;
}

bool DLA_3d::aggregate_collision(int&, int&, const int&, const int&, const double&, size_t&) {
	return false;
}

bool DLA_3d::aggregate_collision(int& _x, int& _y, int& _z, const int& _prev_x, const int& _prev_y, const int& _prev_z, const double& _sticky_pr, size_t& _count) {
	auto search = aggregate_map.find(make_triple(_x, _y, _z));
	// co-ordinates _x, _y, _z occur in the aggregate, collision occurred
	if (search != aggregate_map.end() && _sticky_pr <= coeff_stick) {
		aggregate_map.insert(std::make_pair(make_triple(_prev_x, _prev_y, _prev_z), ++_count));
		aggregate_pq.push(make_triple(_prev_x, _prev_y, _prev_z));
		return true;
	}
	return false;
}
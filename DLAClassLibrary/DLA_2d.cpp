#include "DLA_2d.h"

DLA_2d::DLA_2d(const double& _coeff_stick) : DLAContainer(_coeff_stick) {

}

DLA_2d::DLA_2d(LatticeType _lattice_type, AttractorType _attractor_type, const double& _coeff_stick) : DLAContainer(_lattice_type, _attractor_type, _coeff_stick) {

}

DLA_2d::DLA_2d(const DLA_2d& _other) : DLAContainer(_other) {
	// deep copy fields of _other to this
	aggregate_map = _other.aggregate_map;
	aggregate_pq = _other.aggregate_pq;
}

DLA_2d::DLA_2d(DLA_2d&& _other) : DLAContainer(_other) {
	// deep copy fields of _other to this
	aggregate_map = _other.aggregate_map;
	aggregate_pq = _other.aggregate_pq;
	// set _other container fields to default values
	_other.aggregate_map = std::unordered_map<std::pair<int, int>, size_t, pair_hash>();
	_other.aggregate_pq = std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, distance_comparator>();
}

DLA_2d::~DLA_2d() {

}

size_t DLA_2d::size() const noexcept {
	return aggregate_map.size();
}

void DLA_2d::clear() {
	DLAContainer::clear();
	aggregate_map.clear();
	aggregate_pq = std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, distance_comparator>();
}

void DLA_2d::generate(size_t _n) {
	// clear any existing aggregrate data
	clear();

	// push original aggregate point to map and priority queue
	size_t count = 0;
	std::pair<int, int> origin_sticky = std::make_pair(0, 0);
	aggregate_map.insert(std::make_pair(origin_sticky, count));
	aggregate_pq.push(origin_sticky);

	// initialise particle position (altered during spawn routine)
	int x = 0;
	int y = 0;
	int prev_x = x;
	int prev_y = y;

	bool has_next_spawned = false;
	int spawn_diameter = 0;
	// offset to prevent particles spawning directly onto aggregate
	const unsigned int boundary_offset = 16;

	// uniform distribution in [0,1] for probability generation
	std::uniform_real_distribution<> dist(0.0, 1.0);

	size_t fractal_data_interval = _n / bound_radii_npoints;
	size_t prev_count_taken = count;

	while (count < _n) {
		// spawn the next particle if previous particle
		// successfully stuck to aggregate structure
		if (!has_next_spawned) {
			// set diameter of spawn zone to double the maximum of the largest distance co-ordinate
			// pair currently in the aggregate structure plus an offset to avoid direct sticking spawns
			spawn_diameter = 2 * static_cast<int>(std::sqrt(aggregate_pq.top().first*aggregate_pq.top().first + aggregate_pq.top().second*aggregate_pq.top().second)) + boundary_offset;

			// generate random probability for spawn placement
			double placement_pr = dist(mt_eng);

			// spawn on upper line of lattice boundary
			if (placement_pr < 0.25) {
				x = static_cast<int>(spawn_diameter*(dist(mt_eng) - 0.5));
				y = spawn_diameter / 2;
			}
			// spawn on lower line of lattice boundary
			else if (placement_pr >= 0.25 && placement_pr < 0.5) {
				x = static_cast<int>(spawn_diameter*(dist(mt_eng) - 0.5));
				y = -spawn_diameter / 2;
			}
			// spawn on right line of lattice boundary
			else if (placement_pr >= 0.5 && placement_pr < 0.75) {
				x = spawn_diameter / 2;
				y = static_cast<int>(spawn_diameter*(dist(mt_eng) - 0.5));
			}
			// spawn on left line of lattice boundary
			else {
				x = -spawn_diameter / 2;
				y = static_cast<int>(spawn_diameter*(dist(mt_eng) - 0.5));
			}
			has_next_spawned = true;
		}

		double movement_choice = dist(mt_eng);

		prev_x = x;
		prev_y = y;

		// update position of particle via unbiased random walk
		update_particle_position(x, y, movement_choice);
		//std::cout << x << "\t" << y << std::endl;
		// check for collision with particle walls and reflect if true
		lattice_boundary_collision(x, y, prev_x, prev_y, spawn_diameter);

		double stick_pr = dist(mt_eng);

		// check for collision with aggregate structure and add particle to 
		// the aggregate (both to map and pq) if true, set flag ready for
		// next particle spawn
		if (aggregate_collision(x, y, prev_x, prev_y, stick_pr, count)) {
			has_next_spawned = false;
		}

		if (!(size() % fractal_data_interval) && size() != prev_count_taken) {
			double rmax_sqd = aggregate_pq.top().first*aggregate_pq.top().first + aggregate_pq.top().second*aggregate_pq.top().second;
			bounding_radii_vec.push_back(std::make_pair(aggregate_map.size(), std::sqrt(rmax_sqd)));
			prev_count_taken = size();
		}

	}
}

double DLA_2d::estimate_fractal_dimension() const {
	int rmax_sqd = aggregate_pq.top().first*aggregate_pq.top().first + aggregate_pq.top().second*aggregate_pq.top().second;
	double bounding_radius = std::sqrt(rmax_sqd);

	return std::log(aggregate_map.size()) / std::log(bounding_radius);
}

std::ostream& DLA_2d::write(std::ostream& _os, bool _sort_by_map_value) const {
	// sort by order particles were added to the aggregate
	if (_sort_by_map_value) {
		// std::vector container to store aggregate_map values
		std::vector<std::pair<size_t, std::pair<int, int>>> agg_vec;
		//agg_vec.resize(aggregate_map.size()); // resize agg_vec to size of aggregate_map
		// deep copy elements of aggregate_map to agg_vec
		for (auto it = aggregate_map.cbegin(); it != aggregate_map.cend(); ++it) {
			agg_vec.push_back(std::make_pair(it->second, it->first));
		}
		// custom function object for sorting aggregate via order
		// in which particles were added to structure
		struct {
			bool operator()(const std::pair<size_t, std::pair<int, int>>& _lhs, const std::pair<size_t, std::pair<int, int>>& _rhs) const {
				return _lhs.first < _rhs.first;
			}
		} sort_aggregate;

		// sort agg_vec using the custom function object sort_aggregate
		std::sort(agg_vec.begin(), agg_vec.end(), sort_aggregate);

		for (auto it = agg_vec.cbegin(); it < agg_vec.cend(); ++it) {
			_os << it->second << "\n";
		}
	}
	// output aggregate data "as-is" without sorting
	else {
		for (auto it = aggregate_map.cbegin(); it != aggregate_map.cend(); ++it) {
			_os << it->second << "\t" << it->first << "\n";
		}
	}
	return _os;
}

bool DLA_2d::aggregate_collision(int& _x, int& _y, const int& _prev_x, const int& _prev_y, const double& _sticky_pr, size_t& _count) {
	auto search = aggregate_map.find(std::make_pair(_x, _y));
	// co-ordinates _x, _y occur in the aggregate, collision occurred
	if (search != aggregate_map.end() && _sticky_pr <= coeff_stick) {
		aggregate_map.insert(std::make_pair(std::make_pair(_prev_x, _prev_y), ++_count));
		aggregate_pq.push(std::make_pair(_prev_x, _prev_y));
		return true;
	}
	return false;
}

bool DLA_2d::aggregate_collision(int&, int&, int&, const int&, const int&, const int&, const double&, size_t&) {
	return false;
}
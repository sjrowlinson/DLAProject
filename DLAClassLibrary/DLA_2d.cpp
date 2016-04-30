#include "Stdafx.h"
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
	// TODO: alter original sticky seed code for different attractor types (2D)
	size_t count = 0;
	std::pair<int, int> origin_sticky = std::make_pair(0, 0);
	aggregate_map.insert(std::make_pair(origin_sticky, count));
	aggregate_pq.push(origin_sticky);

	// initialise variables for particle position, altered immediately in generation loop
	int x = 0;
	int y = 0;
	int prev_x = x;
	int prev_y = y;

	bool has_next_spawned = false;
	// variable to store current allowed size of bounding
	// box spawning zone
	int spawn_diameter = 0;

	// uniform distribution in [0,1] for probability generation
	std::uniform_real_distribution<> dist(0.0, 1.0);

	// intervals to record bounding radii data at
	size_t fractal_data_interval;
	// prevents division by zero in generation loop interval checking
	if (_n > bound_radii_npoints)
		fractal_data_interval = _n / bound_radii_npoints;
	else
		fractal_data_interval = _n;
	size_t prev_count_taken = count;
	// aggregate generation loop
	while (count < _n) {
		// spawn the next particle if previous particle
		// successfully stuck to aggregate structure
		if (!has_next_spawned) {
			spawn_particle(x, y, spawn_diameter, dist);
			has_next_spawned = true;
		}

		double movement_choice = dist(mt_eng);

		prev_x = x;
		prev_y = y;

		// update position of particle via unbiased random walk
		update_particle_position(x, y, movement_choice);
		// check for collision with bounding walls and reflect if true
		lattice_boundary_collision(x, y, prev_x, prev_y, spawn_diameter);

		double stick_pr = dist(mt_eng);

		// check for collision with aggregate structure and add particle to 
		// the aggregate (both to map and pq) if true, set flag ready for
		// next particle spawn
		if (aggregate_collision(x, y, prev_x, prev_y, stick_pr, count)) {
			has_next_spawned = false;
		}
		// record no. of particles in aggregate and corresponding minimal
		// bounding radii to the field bounding_radii_vec
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

void DLA_2d::spawn_particle(int& _x, int& _y, int& _spawn_diam, std::uniform_real_distribution<>& _dist) noexcept {
	const int boundary_offset = 16;
	// set diameter of spawn zone to double the maximum of the largest distance co-ordinate
	// pair currently in the aggregate structure plus an offset to avoid direct sticking spawns
	_spawn_diam = 2 * static_cast<int>(std::sqrt(aggregate_pq.top().first*aggregate_pq.top().first + aggregate_pq.top().second*aggregate_pq.top().second)) + boundary_offset;
	
	double placement_pr = _dist(mt_eng);
	// spawn on upper line of lattice boundary
	if (placement_pr < 0.25) {
		_x = static_cast<int>(_spawn_diam*(_dist(mt_eng) - 0.5));
		_y = _spawn_diam / 2;
	}
	// spawn on lower line of lattice boundary
	else if (placement_pr >= 0.25 && placement_pr < 0.5) {
		_x = static_cast<int>(_spawn_diam*(_dist(mt_eng) - 0.5));
		_y = -_spawn_diam / 2;
	}
	// spawn on right line of lattice boundary
	else if (placement_pr >= 0.5 && placement_pr < 0.75) {
		_x = _spawn_diam / 2;
		_y = static_cast<int>(_spawn_diam*(_dist(mt_eng) - 0.5));
	}
	// spawn on left line of lattice boundary
	else {
		_x = -_spawn_diam / 2;
		_y = static_cast<int>(_spawn_diam*(_dist(mt_eng) - 0.5));
	}
}

void DLA_2d::spawn_particle(int&, int&, int&, int&, std::uniform_real_distribution<>&) noexcept {
	return;
}

bool DLA_2d::aggregate_collision(const int& _x, const int& _y, const int& _prev_x, const int& _prev_y, const double& _sticky_pr, size_t& _count) {
	// find the given point in the aggregrete, yields aggregrate_map.end() if not in container
	auto search = aggregate_map.find(std::make_pair(_x, _y));
	// co-ordinates _x, _y occur in the aggregate, collision occurred
	if (search != aggregate_map.end() && _sticky_pr <= coeff_stick) {
		// insert previous position of particle to aggregrate_map and aggregrate priority queue
		aggregate_map.insert(std::make_pair(std::make_pair(_prev_x, _prev_y), ++_count));
		aggregate_pq.push(std::make_pair(_prev_x, _prev_y));
		return true;
	}
	return false;
}

bool DLA_2d::aggregate_collision(const int&, const int&, const int&, const int&, const int&, const int&, const double&, size_t&) {
	return false;
}
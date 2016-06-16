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

std::queue<triple<int, int, int>>& DLA_3d::batch_queue_handle() noexcept {
	return batch_queue;
}

void DLA_3d::clear() {
	DLAContainer::clear();
	aggregate_map.clear();
	aggregate_pq = std::priority_queue<triple<int, int, int>, std::vector<triple<int, int, int>>, distance_comparator_3d>();
	batch_queue = std::queue<triple<int, int, int>>();
}

void DLA_3d::generate(size_t _n) {
	// push original sticky point to map and priority queue
	// TODO: alter original sticky seed code for different attractor types (3D)
	size_t count = 0;
	triple<int, int, int> origin_sticky = make_triple(0, 0, 0);
	aggregate_map.insert(std::make_pair(origin_sticky, count));
	aggregate_pq.push(origin_sticky);
	batch_queue.push(origin_sticky);
	// initialise current and previous co-ordinate containers
	triple<int, int, int> current = { 0,0,0 };
	triple<int, int, int> prev = current;

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
	while (size() < _n) {
		if (abort_signal) {
			abort_signal = false;
			return;
		}
		// spawn the next particle if previous particle
		// successfully stuck to aggregate structure
		if (!has_next_spawned) {
			spawn_particle(current, spawn_diameter, dist);
			has_next_spawned = true;
		}
		// generate random double in [0,1] for movement choice direction
		double movement_choice = dist(mt_eng);
		prev = current;
		// update position of particle via unbiased random walk
		update_particle_position(current, movement_choice);
		// check for collision with bounding walls and reflect if true
		lattice_boundary_collision(current, prev, spawn_diameter);

		double stick_pr = dist(mt_eng);
		// check for collision with aggregate structure and add particle to 
		// the aggregate (both to map and pq) if true, set flag ready for
		// next particle spawn
		if (aggregate_collision(current, prev, stick_pr, count)) {
			has_next_spawned = false;
		}
		// record no. of particles in aggregate and corresponding minimal
		// bounding radii to the field bounding_radii_vec
		if (!(size() % fractal_data_interval) && size() != prev_count_taken) {
			double rmax_sqd = aggregate_pq.top().first*aggregate_pq.top().first + aggregate_pq.top().second*aggregate_pq.top().second + aggregate_pq.top().third*aggregate_pq.top().third;
			bounding_radii_vec.push_back(std::make_pair(aggregate_map.size(), std::sqrt(rmax_sqd)));
			prev_count_taken = size();
		}
	}
}

double DLA_3d::estimate_fractal_dimension() const {
	// find radius which minimally bounds the aggregate
	int rmax_sqd = aggregate_pq.top().first*aggregate_pq.top().first + aggregate_pq.top().second*aggregate_pq.top().second + aggregate_pq.top().third*aggregate_pq.top().third;
	double bounding_radius = std::sqrt(rmax_sqd);
	// compute fractal dimension via ln(N)/ln(rmin)
	return std::log(aggregate_map.size()) / std::log(bounding_radius);
}

std::ostream& DLA_3d::write(std::ostream& _os, bool _sort_by_map_value) const {
	// sort by order particles were added to the aggregate
	if (_sort_by_map_value) {
		// std::vector container to store aggregate map values
		std::vector<std::pair<size_t, triple<int, int, int>>> agg_vec;
		// deep copy elements of aggregate_map to agg_vec
		for (auto it = aggregate_map.cbegin(); it != aggregate_map.cend(); ++it) {
			agg_vec.push_back(std::make_pair(it->second, it->first));
		}
		// lambda for sorting aggregate via order in which particles were generated
		auto sort_agg = [](const std::pair<size_t, triple<int, int, int>>& _lhs, const std::pair<size_t, triple<int, int, int>>& _rhs) {return _lhs.first < _rhs.first; };
		// sort agg_vec using lambda sort_agg
		std::sort(agg_vec.begin(), agg_vec.end(), sort_agg);
		// write sorted data to stream
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

void DLA_3d::spawn_particle(triple<int,int,int>& _current, int& _spawn_diam, std::uniform_real_distribution<>& _dist) noexcept {
	const int boundary_offset = 16;
	// set diameter of spawn zone to double the maximum of the largest distance co-ordinate
	// triple currently in the aggregate structure plus an offset to avoid direct sticking spawns
	int rmax_sqd = aggregate_pq.top().first*aggregate_pq.top().first + aggregate_pq.top().second*aggregate_pq.top().second + aggregate_pq.top().third*aggregate_pq.top().third;
	_spawn_diam = 2 * static_cast<int>(std::sqrt(rmax_sqd)) + boundary_offset;
	double placement_pr = _dist(mt_eng);
	// Spawn on negative constant z plane of bounding box
	if (placement_pr < 1.0 / 6.0) {
		_current.first = static_cast<int>(_spawn_diam*(_dist(mt_eng) - 0.5));
		_current.second = static_cast<int>(_spawn_diam*(_dist(mt_eng) - 0.5));
		_current.third = -_spawn_diam / 2;
	}
	// Spawn on positive constant z plane of bounding box
	else if (placement_pr >= 1.0 / 6.0 && placement_pr < 2.0 / 6.0) {
		_current.first = static_cast<int>(_spawn_diam*(_dist(mt_eng) - 0.5));
		_current.second = static_cast<int>(_spawn_diam*(_dist(mt_eng) - 0.5));
		_current.third = _spawn_diam / 2;
	}
	// Spawn on negative constant x plane of bounding box
	else if (placement_pr >= 2.0 / 6.0 && placement_pr < 3.0 / 6.0) {
		_current.first = -_spawn_diam / 2;
		_current.second = static_cast<int>(_spawn_diam*(_dist(mt_eng) - 0.5));
		_current.third = static_cast<int>(_spawn_diam*(_dist(mt_eng) - 0.5));
	}
	// Spawn on positive constant x plane of bounding box
	else if (placement_pr >= 3.0 / 6.0 && placement_pr < 4.0 / 6.0) {
		_current.first = _spawn_diam / 2;
		_current.second = static_cast<int>(_spawn_diam*(_dist(mt_eng) - 0.5));
		_current.third = static_cast<int>(_spawn_diam*(_dist(mt_eng) - 0.5));
	}
	// Spawn on negative constant y plane of bounding box
	else if (placement_pr >= 4.0 / 6.0 && placement_pr < 5.0 / 6.0) {
		_current.first = static_cast<int>(_spawn_diam*(_dist(mt_eng) - 0.5));
		_current.second = -_spawn_diam / 2;
		_current.third = static_cast<int>(_spawn_diam*(_dist(mt_eng) - 0.5));
	}
	// Spawn on positive constant z plane of bounding box
	else if (placement_pr >= 5.0 / 6.0 && placement_pr < 1.0) {
		_current.first = static_cast<int>(_spawn_diam*(_dist(mt_eng) - 0.5));
		_current.second = _spawn_diam / 2;
		_current.third = static_cast<int>(_spawn_diam*(_dist(mt_eng) - 0.5));
	}
}

bool DLA_3d::aggregate_collision(const triple<int,int,int>& _current, const triple<int,int,int>& _previous, const double& _sticky_pr, size_t& _count) {
	// find the given point in the aggregrete, yields aggregrate_map.end() if not in container
	auto search = aggregate_map.find(_current);
	// co-ordinates _x, _y, _z occur in the aggregate, collision occurred
	if (search != aggregate_map.end() && _sticky_pr <= coeff_stick) {
		// insert previous position of particle to aggregrate_map and aggregrate priority queue
		aggregate_map.insert(std::make_pair(_previous, ++_count));
		aggregate_pq.push(_previous);
		batch_queue.push(_previous);
		return true;
	}
	return false;
}
#include "Stdafx.h"
#include "DLA_2d.h"

DLA_2d::DLA_2d(const double& _coeff_stick) : DLAContainer(_coeff_stick), 
	aggregate_pq(utl::distance_comparator(attractor_type::POINT, 1U)) {}

DLA_2d::DLA_2d(lattice_type ltt, attractor_type att, std::size_t att_size, const double& _coeff_stick) : DLAContainer(ltt, att, att_size, _coeff_stick),
aggregate_pq(utl::distance_comparator(att, att_size)) {	initialise_attractor_structure(); }

DLA_2d::DLA_2d(const DLA_2d& other) : DLAContainer(other),
	aggregate_map(other.aggregate_map), aggregate_pq(other.aggregate_pq), batch_queue(other.batch_queue) {}

DLA_2d::DLA_2d(DLA_2d&& other) : DLAContainer(std::move(other)),
	aggregate_map(std::move(other.aggregate_map)), aggregate_pq(std::move(other.aggregate_pq)), batch_queue(std::move(other.batch_queue)) {}

DLA_2d::~DLA_2d() {}

std::size_t DLA_2d::size() const noexcept {
	return aggregate_map.size();
}

std::queue<std::pair<int,int>>& DLA_2d::batch_queue_handle() noexcept {
	return batch_queue;
}

void DLA_2d::set_attractor_type(attractor_type attr, std::size_t att_size) {
	// invalid attractor type for 2D lattice
	if (attr == attractor_type::PLANE)
		throw std::invalid_argument("Cannot set attractor type of 2D DLA to PLANE.");
	DLAContainer::set_attractor_type(attr, att_size);
	aggregate_pq.comparator().att = attr;	// get handle to comparator of pq and alter its attractor_type field
	if (!aggregate_pq.empty()) aggregate_pq.reheapify(); // perform reordering of pq based on new attractor_type
}

void DLA_2d::initialise_attractor_structure() {
	attractor_set.clear();
	attractor_set.reserve(attractor_size);
	switch (attractor) {
	case attractor_type::POINT:	// insert single point at origin to attractor_set
		attractor_set.insert(std::make_pair(0, 0));
		break;
	case attractor_type::LINE:	// insert line extending from [-att_size/2, +att_size/2] to attractor_set
		for (int i = -static_cast<int>(attractor_size) / 2; i < static_cast<int>(attractor_size) / 2; ++i)
			attractor_set.insert(std::make_pair(i, 0));
		break;
	default:
		break;
	}
}

void DLA_2d::clear() {
	DLAContainer::clear();
	aggregate_map.clear();
	aggregate_pq.clear();
	batch_queue = aggregate2d_batch_queue();
}

void DLA_2d::generate(std::size_t n) {
	// compute attractor geometry inserting points to attractor_set
	initialise_attractor_structure();
	aggregate_map.reserve(n);	// pre-allocate n memory slots in agg map
	std::size_t count = 0U;
	// initialise current and previous co-ordinate containers
	std::pair<int, int> current = std::make_pair(0, 0);
	std::pair<int, int> prev = current;
	bool has_next_spawned = false;
	// variable to store current allowed size of bounding
	// box spawning zone
	int spawn_diameter = 0;
	// aggregate generation loop 
	while (size() < n || continuous) {
		if (abort_signal) {
			abort_signal = false;
			return;
		}
		// spawn the next particle if previous particle
		// successfully stuck to aggregate structure
		if (!has_next_spawned) {
			spawn_particle(current, spawn_diameter);
			has_next_spawned = true;
		}
		prev = current;
		// update position of particle via unbiased random walk
		update_particle_position(current, pr_gen());
		// check for collision with bounding walls and reflect if true
		lattice_boundary_collision(current, prev, spawn_diameter);
		// check for collision with aggregate structure and add particle to 
		// the aggregate (both to map and pq) if true, set flag ready for
		// next particle spawn
		if (aggregate_collision(current, prev, pr_gen(), count)) has_next_spawned = false;
	}
}

double DLA_2d::estimate_fractal_dimension() const {
	// find radius which minimally bounds the aggregate
	double bounding_radius = 0.0;
	switch (attractor) {
	case attractor_type::POINT:
		bounding_radius = std::hypot(aggregate_pq.top().first, aggregate_pq.top().second);
		break;
	case attractor_type::LINE:
		bounding_radius = aggregate_pq.top().second;
		break;
	default:
		return -1.0;
	}
	// compute fractal dimension via ln(N)/ln(rmin)
	return std::log(aggregate_map.size()) / std::log(bounding_radius);
}

std::ostream& DLA_2d::write(std::ostream& os, bool sort_by_gen_order) const {
	using utl::operator<<;
	// sort by order particles were added to the aggregate
	if (sort_by_gen_order) {
		// std::vector container to store aggregate_map values
		std::vector<std::pair<std::size_t, std::pair<int, int>>> agg_vec;
		// deep copy elements of aggregate_map to agg_vec
		for (const auto& el : aggregate_map)
			agg_vec.push_back(std::make_pair(el.second, el.first));
		// sort agg_vec using a lambda based on order of particle generation
		std::sort(agg_vec.begin(), agg_vec.end(), [](auto& _lhs, auto& _rhs) {return _lhs.first < _rhs.first; });
        // write sorted data to stream
		for (const auto& el : agg_vec)
			os << el.second << '\n';
	}
	// output aggregate data "as-is" without sorting
	else {
		for (const auto& el : aggregate_map)
			os << el.second << '\t' << el.first << '\n';
	}
	return os;
}

void DLA_2d::spawn_particle(std::pair<int,int>& spawn_pos, int& spawn_diam) noexcept {
	const int boundary_offset = 16;
	// generate random double in [0,1]
	double placement_pr = pr_gen();
	// set diameter of spawn zone to double the maximum of the largest distance co-ordinate
	// pair currently in the aggregate structure plus an offset to avoid direct sticking spawns
	switch (attractor) {
	case attractor_type::POINT:
		spawn_diam = (aggregate_pq.empty() ? 0 : 2 * static_cast<int>(std::hypot(aggregate_pq.top().first, aggregate_pq.top().second))) 
			+ boundary_offset;
		// spawn on upper line of lattice boundary
		if (placement_pr < 0.25) {
			spawn_pos.first = static_cast<int>(spawn_diam*(pr_gen() - 0.5));
			spawn_pos.second = spawn_diam / 2;
		}
		// spawn on lower line of lattice boundary
		else if (placement_pr >= 0.25 && placement_pr < 0.5) {
			spawn_pos.first = static_cast<int>(spawn_diam*(pr_gen() - 0.5));
			spawn_pos.second = -spawn_diam / 2;
		}
		// spawn on right line of lattice boundary
		else if (placement_pr >= 0.5 && placement_pr < 0.75) {
			spawn_pos.first = spawn_diam / 2;
			spawn_pos.second = static_cast<int>(spawn_diam*(pr_gen() - 0.5));
		}
		// spawn on left line of lattice boundary
		else {
			spawn_pos.first = -spawn_diam / 2;
			spawn_pos.second = static_cast<int>(spawn_diam*(pr_gen() - 0.5));
		}
		break;
	case attractor_type::LINE:
		spawn_diam = (aggregate_pq.empty() ? 0 : aggregate_pq.top().second) + boundary_offset;
		// spawn on upper line of lattice boundary
		if (placement_pr < 0.5) {
			spawn_pos.first = static_cast<int>(attractor_size*(pr_gen() - 0.5));
			spawn_pos.second = spawn_diam;
		}
		// spawn on lower line of lattice boundary
		else {
			spawn_pos.first = static_cast<int>(attractor_size*(pr_gen() - 0.5));
			spawn_pos.second = -spawn_diam;
		}
		break;
	default:
		break;
	}
}

void DLA_2d::push_particle(const std::pair<int, int>& p, std::size_t count) {
	aggregate_map.insert(std::make_pair(p, count));
	aggregate_pq.push(p);
	batch_queue.push(p);
}

bool DLA_2d::aggregate_collision(const std::pair<int,int>& current, const std::pair<int,int>& previous, const double& sticky_pr, std::size_t& count) {
	// particle did not stick to aggregate, increment aggregate_misses counter
	if (sticky_pr > coeff_stick)
		++aggregate_misses_;
	// else, if current co-ordinates of particle exist in aggregate
	// or attractor then collision and successful sticking occurred
	else if (aggregate_map.find(current) != aggregate_map.end() || attractor_set.find(current) != attractor_set.end()) {
		// insert previous position of particle to aggregrate_map and aggregrate priority queue
		push_particle(previous, ++count);
		switch (attractor) {
		case attractor_type::POINT: // compute r^2 of furthest point from origin
			aggregate_span = aggregate_pq.top().first*aggregate_pq.top().first + aggregate_pq.top().second*aggregate_pq.top().second;
			break;
		case attractor_type::LINE: // compute furthest y distance from origin line
			aggregate_span = aggregate_pq.top().second;
			break;
		}
		return true;
	}
	return false;
}

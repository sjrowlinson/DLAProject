#include "Stdafx.h"
#include "DLAContainer.h"

DLAContainer::DLAContainer(const double& _coeff_stick) 
	: lattice(lattice_type::SQUARE), attractor(attractor_type::POINT), pr_gen(), attractor_size(1U) { set_coeff_stick(_coeff_stick); }

DLAContainer::DLAContainer(lattice_type ltt, attractor_type att, std::size_t att_size, const double& _coeff_stick) 
	: lattice(ltt), attractor(att), pr_gen(), attractor_size(att_size) { set_coeff_stick(_coeff_stick); }

DLAContainer::DLAContainer(const DLAContainer& other)
	: lattice(other.lattice), attractor(other.attractor),
		coeff_stick(other.coeff_stick), pr_gen(other.pr_gen), attractor_size(other.attractor_size) {}

DLAContainer::DLAContainer(DLAContainer&& other)
	: lattice(std::move(other.lattice)), attractor(std::move(other.attractor)),
	coeff_stick(std::move(other.coeff_stick)), pr_gen(std::move(other.pr_gen)), attractor_size(std::move(other.attractor_size)) {}

double DLAContainer::get_coeff_stick() const noexcept {
	return coeff_stick;
}

void DLAContainer::set_coeff_stick(const double& _coeff_stick) {
	// if _coeff_stick not in (0,1] => throw std::invalid_argument
	if (_coeff_stick <= 0.0 || _coeff_stick > 1.0)
		throw std::invalid_argument("_coeff_stick must be in interval (0,1]");
	coeff_stick = _coeff_stick;
}

lattice_type DLAContainer::get_lattice_type() const noexcept {
	return lattice;
}

void DLAContainer::set_lattice_type(lattice_type ltt) noexcept {
	lattice = ltt;
}

std::pair<attractor_type, std::size_t> DLAContainer::get_attractor_type() const noexcept {
	return { attractor, attractor_size };
}

void DLAContainer::set_attractor_type(attractor_type att, std::size_t att_size) {
	attractor = att;
	attractor_size = att_size;
}

std::size_t DLAContainer::aggregate_spanning_distance() const noexcept {
	return aggregate_span;
}

std::size_t DLAContainer::aggregate_misses() const noexcept {
	return aggregate_misses_;
}

void DLAContainer::raise_abort_signal() noexcept {
	abort_signal = true;
}

void DLAContainer::change_continuous_flag(bool _continuous) noexcept {
	continuous = _continuous;
}

void DLAContainer::set_random_walk_particle_spawn_source(const std::pair<bool, bool>& above_below) {
	if (!above_below.first && !above_below.second) throw std::invalid_argument("must have at least one spawn source.");
	is_spawn_source_above = above_below.first;
	is_spawn_source_below = above_below.second;
}

void DLAContainer::clear() {
	aggregate_misses_ = 0U;
	aggregate_span = 0U;
	pr_gen.reset_distribution_state();	// reset prng distribution state such that next generated values independent
}

void DLAContainer::update_particle_position(std::pair<int,int>& current, const double& movement_choice) noexcept {
	// perform different translations dependent upon type of lattice
	switch (lattice) {
	case lattice_type::SQUARE:
		// translate (+1,0) or (-1,0)
		if (movement_choice < 0.5) (movement_choice < 0.25) ? ++current.first : --current.first;
		// translate (0,+1) or (0,-1)
		else (movement_choice < 0.75) ? ++current.second : --current.second;
		break;
	case lattice_type::TRIANGLE:
		// translate (+1,0) or (-1,0)
		if (movement_choice < 1.0 / 5.0) (movement_choice < 1.0 / 10.0) ? ++current.first : --current.first;
		// translate (+1,+1)
		else if (movement_choice >= 1.0 / 5.0 && movement_choice < 2.0 / 5.0) {
			++current.first;
			++current.second;
		}
		// translate (+1,-1)
		else if (movement_choice >= 2.0 / 5.0 && movement_choice < 3.0 / 5.0) {
			++current.first;
			--current.second;
		}
		// translate (-1,+1)
		else if (movement_choice >= 3.0 / 5.0 && movement_choice < 4.0 / 5.0) {
			--current.first;
			++current.second;
		}
		// translate (-1,-1)
		else {
			--current.first;
			--current.second;
		}
		break;
	}
}

void DLAContainer::update_particle_position(std::tuple<int,int,int>& current, const double& movement_choice) noexcept {
	// perform different translations dependent upon type of lattice
	switch (lattice) {
	case lattice_type::SQUARE:
		// translate (+1,0,0) or (-1,0,0)
		if (movement_choice < 1.0 / 3.0) (movement_choice < 1.0/6.0) ? ++std::get<0>(current) : --std::get<0>(current);
		// translate (0,+1,0) or (0,-1,0)
		else if (movement_choice >= 1.0 / 3.0 && movement_choice < 2.0 / 3.0) (movement_choice < 0.5) ? ++std::get<1>(current) : --std::get<1>(current);
		// translate (0,0,+1) or (0,0,-1)
		else (movement_choice < 5.0/6.0) ? ++std::get<2>(current) : --std::get<2>(current);
		break;
	case lattice_type::TRIANGLE:
		// translate (+1,0,0) or (-1,0,0)
		if (movement_choice < 1.0 / 6.0) (movement_choice < 1.0/12.0) ? ++std::get<0>(current) : --std::get<0>(current);
		// translate (+1,+1,0)
		else if (movement_choice >= 1.0 / 6.0 && movement_choice < 2.0 / 6.0) {
			++std::get<0>(current);
			++std::get<1>(current);
		}
		// translate (+1,-1,0)
		else if (movement_choice >= 2.0 / 6.0 && movement_choice < 3.0 / 6.0) {
			++std::get<0>(current);
			--std::get<1>(current);
		}
		// translate (-1,+1,0)
		else if (movement_choice >= 3.0 / 6.0 && movement_choice < 4.0 / 6.0) {
			--std::get<0>(current);
			++std::get<1>(current);
		}
		// translate (-1,-1,0)
		else if (movement_choice >= 4.0 / 6.0 && movement_choice < 5.0 / 6.0) {
			--std::get<0>(current);
			--std::get<1>(current);
		}
		// translate (0,0,+1) or (0,0,-1)
		else (movement_choice < 11.0/12.0) ? ++std::get<2>(current) : --std::get<2>(current);
		break;
	}
}

bool DLAContainer::lattice_boundary_collision(std::pair<int,int>& current, const std::pair<int,int>& previous, const int& spawn_diam) const noexcept {
	// small offset for correction on boundaries
	const int epsilon = 2;
	// choose correct boundary collision detection based on type of attractor
	switch (attractor) {
	case attractor_type::POINT:
		// reflect particle from boundary
		if (std::abs(current.first) > ((spawn_diam / 2) + epsilon) || std::abs(current.second) > ((spawn_diam / 2) + epsilon)) {
			current = previous;
			return true;
		}
		break;
	case attractor_type::LINE:
		if (std::abs(current.first) > ((static_cast<int>(attractor_size) / 2) + epsilon) || std::abs(current.second) > (spawn_diam + epsilon)) {
			current = previous;
			return true;
		}
		break;
	}
	return false;
}

bool DLAContainer::lattice_boundary_collision(std::tuple<int,int,int>& current, const std::tuple<int,int,int>& previous, const int& spawn_diam) const noexcept {
	// small offset for correction on boundaries
	const int epsilon = 2;
	// choose correct boundary collision detection based on type of attractor
	switch (attractor) {
	case attractor_type::POINT:
		// reflect particle from boundary
		if (std::abs(std::get<0>(current)) > ((spawn_diam / 2) + epsilon) 
			|| std::abs(std::get<1>(current)) > ((spawn_diam / 2) + epsilon) 
			|| std::abs(std::get<2>(current)) > ((spawn_diam / 2) + epsilon)) {
			current = previous;
			return true;
		}
		break;
	case attractor_type::LINE:
		if (std::abs(std::get<0>(current)) > (static_cast<int>(attractor_size) / 2 + epsilon)
			|| std::abs(std::get<1>(current)) > (spawn_diam + epsilon)
			|| std::abs(std::get<2>(current)) > (spawn_diam + epsilon)) {
			current = previous;
			return true;
		}
		break;
	case attractor_type::PLANE:
		if (std::abs(std::get<0>(current)) > (static_cast<int>(attractor_size) / 2 + epsilon)
			|| std::abs(std::get<1>(current)) > ((static_cast<int>(attractor_size) / 2 + epsilon))
			|| std::abs(std::get<2>(current)) > (spawn_diam + epsilon)) {
			current = previous;
			return true;
		}
		break;
	}
	return false;
}

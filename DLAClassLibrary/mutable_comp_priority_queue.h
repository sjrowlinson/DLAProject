#pragma once
#include <iterator>
#include <queue>
#include <vector>

namespace utl {
	/**
	 * \class mutable_comp_priority_queue
	 *
	 * \brief Provides an unstable interface to a `std::priority_queue` allowing mutability of the underlying
	 *        comparator function object. The heap invariant of the priority queue can be broken via this
	 *        mutability, in order to preserve the heap a call to `reheapify()` must be made upon altering
	 *        the comparator.
	 */
	template<class Ty,
		class Cntr = std::vector<Ty>,
		class Pr = std::less<Ty>
	> class mutable_comp_priority_queue
		: private std::priority_queue<Ty, Cntr, Pr> {
		using base_pq = std::priority_queue<Ty, Cntr, Pr>;
	public:
		// export constructor
		using base_pq::base_pq;
		// export reqd methods
		using base_pq::empty;
		using base_pq::size;
		using base_pq::top;
		using base_pq::push;
		using base_pq::emplace;
		using base_pq::pop;
		// access reference to comparator function-object
		auto& comparator() { return base_pq::comp; }
		// clear underlying container
		void clear() { base_pq::c.clear(); }
		// perform heapification of underlying container to preserve heap-invariant
		void reheapify() { std::make_heap(std::begin(base_pq::c), std::end(base_pq::c), base_pq::comp); }
		// enable reserve method for underlying container
		void reserve(std::size_t new_cap) {
			base_pq::c.reserve(new_cap);
		}
	};
}
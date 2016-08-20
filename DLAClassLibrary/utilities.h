#pragma once
#include <ostream>
#include <type_traits>
#include <utility>
#include <vector>

namespace utl {
	// utl::triple
	/**
	 * \struct triple
	 *
	 * \brief Defines a triplet of generic types.
	 */
	template<typename _Ty1, typename _Ty2, typename _Ty3> struct triple {
		_Ty1 first;
		_Ty2 second;
		_Ty3 third;
		/**
		 * \brief Default constructor, initialises the container with default-constructed values.
		 */
		template<class _Uty1 = _Ty1,
			class _Uty2 = _Ty2,
			class _Uty3 = _Ty3,
			class = std::enable_if_t<std::is_default_constructible<_Uty1>::value
				&& std::is_default_constructible<_Uty2>::value
				&& std::is_default_constructible<_Uty3>::value> 
		> constexpr triple() : first(), second(), third() {}
		/**
		 * \brief Construct a `triple` object from specified values using copy construction.
		 */
		template<class _Uty1 = _Ty1,
			class _Uty2 = _Ty2,
			class _Uty3 = _Ty3,
			class = std::enable_if_t<std::is_copy_constructible<_Uty1>::value
				&& std::is_copy_constructible<_Uty2>::value
				&& std::is_copy_constructible<_Uty3>::value> 
		> triple(const _Ty1& _first, const _Ty2& _second, const _Ty3& _third) : first(_first), second(_second), third(_third) {}
		/**
		 * \brief Construct a `triple` object from specified values using move construction.
		 */
		template<class _Uty1 = _Ty1,
			class _Uty2 = _Ty2,
			class _Uty3 = _Ty3,
			class = std::enable_if_t<std::is_move_constructible<_Uty1>::value
				&& std::is_move_constructible<_Uty2>::value
				&& std::is_move_constructible<_Uty3>::value> 
		> triple(_Ty1&& _first, _Ty2&& _second, _Ty3&& _third) : first(std::move(_first)), second(std::move(_second)), third(std::move(_third)) {}
	};
	/**
	 * \brief Makes a `triple` object with given values.
	 *
	 * \param _val1 Value with which to initialise first field of `triple`.
	 * \param _val2 Value with which to initialise second field of `triple`.
	 * \param _val3 Value with which to initialise third field of `triple`.
	 * \return `triple` object comprised of specified parameters.
	 */
	template<typename _Ty1, typename _Ty2, typename _Ty3> 
	inline constexpr triple<typename std::_Unrefwrap<_Ty1>::type, typename std::_Unrefwrap<_Ty2>::type, typename std::_Unrefwrap<_Ty3>::type> make_triple(_Ty1&& _val1, _Ty2&& _val2, _Ty3&& _val3) {
		typedef triple<typename std::_Unrefwrap<_Ty1>::type, typename std::_Unrefwrap<_Ty2>::type, typename std::_Unrefwrap<_Ty3>::type> _ret_triple;
		return _ret_triple(std::forward<_Ty1>(_val1), std::forward<_Ty2>(_val2), std::forward<_Ty3>(_val3));
	}
	/**
	 * \brief Equality operator for `triple`.
	 *
	 * \param lhs First instance of `triple`.
	 * \param rhs Second instance of `triple`.
	 * \return `true` if `lhs == rhs`, `false` otherwise.
	 */
	template<typename _Ty1, typename _Ty2, typename _Ty3> 
	bool operator==(const triple<_Ty1, _Ty2, _Ty3>& lhs, const triple<_Ty1, _Ty2, _Ty3>& rhs) {
		return lhs.first == rhs.first && lhs.second == rhs.second && lhs.third == rhs.third;
	}
	/**
	 * \brief Inequality operator for `triple`.
	 *
	 * \param lhs First instance of `triple`.
	 * \param rhs Second instance of `triple`.
	 * \return `true` if `lhs != rhs`, `false` otherwise.
	 */
	template<typename _Ty1, typename _Ty2, typename _Ty3> 
	bool operator!=(const triple<_Ty1, _Ty2, _Ty3>& lhs, const triple<_Ty1, _Ty2, _Ty3>& rhs) {
		return !(lhs == rhs);
	}
	/**
	 * \brief Stream insertion operator for `triple`.
	 *
	 * \param os Instance of stream type to insert to.
	 * \param trp `triple` instance to insert to data stream.
	 * \return Modified `std::ostream` instance `os` containing data of `trp`.
	 */
	template<typename _Ty1, typename _Ty2, typename _Ty3> 
	std::ostream& operator<<(std::ostream& os, const triple<_Ty1, _Ty2, _Ty3>& trp) {
		os << trp.first << "\t" << trp.second << "\t" << trp.third;
		return os;
	}
	// std::pair INSERTION OPERATOR
	/**
	 * \brief Stream insertion operator for `std::pair`.
	 *
	 * \param os Instance of stream type to insert to.
	 * \param p `std::pair` instance to insert to data stream.
	 * \return Modified `std::ostream` instance `os` containing data of `p`.
	 */
	template<typename _Ty1, typename _Ty2> 
	std::ostream& operator<<(std::ostream& os, const std::pair<_Ty1, _Ty2>& p) {
		os << p.first << "\t" << p.second;
		return os;
	}
	/*************** std::vector INSERTION OPERATOR ***************/
	/**
	 * \brief Stream insertion operator for `std::vector`.
	 *
	 * \param os Instance of stream type to insert to.
	 * \param vec `std::vector` instance to insert to data stream.
	 * \return Modified `std::ostream` instance `os` containing data of `vec`.
	 */
	template<typename _Ty1, typename _Ty2> 
	std::ostream& operator<<(std::ostream& os, const std::vector<std::pair<_Ty1, _Ty2>>& vec) {
		for (const auto& it : vec)
			os << it << '\n';
		return os;
	}

}
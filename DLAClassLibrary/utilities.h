#pragma once
#include <ostream>
#include <type_traits>
#include <utility>
#include <vector>

namespace utl {

	/*************** triple STRUCT ***************/

	/**
	 * \struct Triple
	 *
	 * \brief Defines a triplet of generic types.
	 *
	 * \author Samuel Rowlinson
	 * \date April, 2016
	 */
	template<typename _Ty1, typename _Ty2, typename _Ty3> struct triple {
		_Ty1 first;
		_Ty2 second;
		_Ty3 third;

		/**
		 * \brief Default constructor, fields are initialised with default values.
		 */
		template<class _Uty1 = _Ty1,
			class _Uty2 = _Ty2,
			class _Uty3 = _Ty3,
			class = std::enable_if_t<std::is_default_constructible<_Uty1>::value
			&& std::is_default_constructible<_Uty2>::value
			&& std::is_default_constructible<_Uty3>::value> >
			constexpr triple() : first(), second(), third() {}

		/**
		 * \brief Construct a triple object from specified values using copy construction.
		 */
		template<class _Uty1 = _Ty1,
			class _Uty2 = _Ty2,
			class _Uty3 = _Ty3,
			class = std::enable_if_t<std::is_copy_constructible<_Uty1>::value
			&& std::is_copy_constructible<_Uty2>::value
			&& std::is_copy_constructible<_Uty3>::value> >
			triple(const _Ty1& _first, const _Ty2& _second, const _Ty3& _third) : first(_first), second(_second), third(_third) {}

		/**
		 * \brief Construct a triple object from specified values using move construction.
		 */
		template<class _Uty1 = _Ty1,
			class _Uty2 = _Ty2,
			class _Uty3 = _Ty3,
			class = std::enable_if_t<std::is_move_constructible<_Uty1>::value
			&& std::is_move_constructible<_Uty2>::value
			&& std::is_move_constructible<_Uty3>::value> >
			triple(_Ty1&& _first, _Ty2&& _second, _Ty3&& _third) : first(std::move(_first)), second(std::move(_second)), third(std::move(_third)) {}
	};

	/**
	 * \brief Makes a triple object with given values.
	 *
	 * \param _val1 rvalue reference to first value
	 * \param _val2 rvalue reference to second value
	 * \param _val3 rvalue reference to third value
	 * \return triple object comprised of params
	 */
	template<typename _Ty1, typename _Ty2, typename _Ty3> inline constexpr
		triple<typename std::_Unrefwrap<_Ty1>::type, typename std::_Unrefwrap<_Ty2>::type, typename std::_Unrefwrap<_Ty3>::type> make_triple(_Ty1&& _val1, _Ty2&& _val2, _Ty3&& _val3) {
		typedef triple<typename std::_Unrefwrap<_Ty1>::type, typename std::_Unrefwrap<_Ty2>::type, typename std::_Unrefwrap<_Ty3>::type> _ret_triple;
		return _ret_triple(std::forward<_Ty1>(_val1), std::forward<_Ty2>(_val2), std::forward<_Ty3>(_val3));
	}

	/**
	 * \brief Overloaded operator== for triple object.
	 *
	 * \param _lhs Instance of triple
	 * \param _rhs Instance of triple same type as _lhs
	 * \return true if _lhs == _rhs, false otherwise
	 */
	template<typename _Ty1, typename _Ty2, typename _Ty3> bool operator==(const triple<_Ty1, _Ty2, _Ty3>& _lhs, const triple<_Ty1, _Ty2, _Ty3>& _rhs) {
		return _lhs.first == _rhs.first && _lhs.second == _rhs.second && _lhs.third == _rhs.third;
	}

	/**
	 * \brief Overloaded operator<< for triple object.
	 *
	 * \param _os std::ostream instance
	 * \param _t triple instance
	 * \return modified std::ostream instance _os containing data of _t
	 */
	template<typename _Ty1, typename _Ty2, typename _Ty3> std::ostream& operator<<(std::ostream& _os, const triple<_Ty1, _Ty2, _Ty3>& _t) {
		_os << _t.first << "\t" << _t.second << "\t" << _t.third;
		return _os;
	}

	/*************** std::pair OPERATORS ***************/

	/**
	 * \brief Overloaded operator<< for std::pair object.
	 *
	 * \param _os std::ostream instance
	 * \param _p std::pair instance
	 * \return modified std::ostream instance _os containing data of _p
	 */
	template<typename _Ty1, typename _Ty2> std::ostream& operator<<(std::ostream& _os, const std::pair<_Ty1, _Ty2>& _p) {
		_os << _p.first << "\t" << _p.second;
		return _os;
	}

	/*************** std::vector OPERATORS ***************/

	/**
	 * \brief Overloaded operator<< for std::vector object.
	 *
	 * \param _os std::ostream instance
	 * \param _vec std::vector instance
	 * \return modified std::ostream instance _os containing data of _vec
	 */
	template<typename _Ty1, typename _Ty2> std::ostream& operator<<(std::ostream& _os, const std::vector<std::pair<_Ty1, _Ty2>>& _vec) {
		for (auto it = _vec.cbegin(); it < _vec.cend(); ++it) {
			_os << *it << "\n";
		}
		return _os;
	}

}
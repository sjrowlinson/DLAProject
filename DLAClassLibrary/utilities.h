#pragma once
#include <ostream>
#include <random>
#include <type_traits>
#include <tuple>
#include <utility>

/**
 * \enum lattice_type
 *
 * \brief Defines several types of lattice applicable to both 2D and 3D DLA systems, for example
 *        LatticeType::SQUARE will be interpreted as a square lattice in a 2D system and cubic
 *        lattice in a 3D system.
 */
enum class lattice_type {
	SQUARE, // square for 2D, cubic for 3D
	TRIANGLE, // triangular for 2D, hexagonal for 3D
};

/** 
 * \enum attractor_type
 *
 * \brief Defines several types of attractor geometry applicable to both 2D and 3D DLA systems. Note
 *        that using some attractor geometries defined for 3D systems (i.e. AttractorType::PLANE) is
 *        not allowed when applying to a DLA_2d object.
 */
enum class attractor_type {
	POINT,
	LINE,
	PLANE, // not applicable for 2D
};

namespace utl {
	// DISTANCE_COMPARATOR
	/**
	 * \struct tuple_distance_compute
	 *
	 * \brief Helper recersive function-object for computing squared radius
	 *        of a `std::tuple` co-ordinate from the origin.
	 */
	template<class Tuple, std::size_t N>
	struct tuple_distance_t {
		static auto tuple_distance(const Tuple& t) {
			return tuple_distance_t<Tuple, N - 1>::tuple_distance(t)
				+ std::get<N - 1>(t)*std::get<N - 1>(t);
		}
	};
	// base-helper
	template<class Tuple>
	struct tuple_distance_t<Tuple, 1> {
		static auto tuple_distance(const Tuple& t) {
			return std::get<0>(t)*std::get<0>(t);
		}
	};
	/**
	 * \struct distance_comparator
	 *
	 * \brief Defines a distance comparator function object for a `std::tuple` (incl. `std::pair`)
	 *        of generic types, used for determining tuple which has greater distance from origin.
	 */
	struct distance_comparator {
		template<class... Args>
		bool operator()(const std::tuple<Args...>& lhs, const std::tuple<Args...>& rhs) const {
			return tuple_distance_t<decltype(lhs), sizeof...(Args)>::tuple_distance(lhs)
				< tuple_distance_t<decltype(rhs), sizeof...(Args)>::tuple_distance(rhs);
		}
		template<class Ty1, class Ty2>
		bool operator()(const std::pair<Ty1, Ty2>& lhs, const std::pair<Ty1, Ty2>& rhs) const {
			return tuple_distance_t<decltype(lhs), 2>::tuple_distance(lhs)
				< tuple_distance_t<decltype(rhs), 2>::tuple_distance(rhs);
		}
	};
	// TUPLE_HASH
	/**
	 * \struct tuple_hash_compute
	 *
	 * \brief Helper recersive function-object for computing hash function of a `std::tuple` instance.
	 */
	template<class Tuple, std::size_t N>
	struct tuple_hash_compute {
		static std::size_t hash_compute(const Tuple& t) {
			using type = typename std::tuple_element<N - 1, Tuple>::type;
			return tuple_hash_compute<Tuple, N - 1>::hash_compute(t) + std::hash<type>()(std::get<N - 1>(t));
		}
	};
	// base-helper
	template<class Tuple>
	struct tuple_hash_compute<Tuple, 1> {
		static std::size_t hash_compute(const Tuple& t) {
			using type = typename std::tuple_element<0, Tuple>::type;
			return 51 + std::hash<type>()(std::get<0>(t)) * 51;
		}
	};
	/**
	 * \struct tuple_hash
	 *
	 * \brief Defines a hash function object for a `std::tuple` (incl. `std::pair`)
	 *        of generic types.
	 */
	struct tuple_hash {
		template<class... Args>
		std::size_t operator()(const std::tuple<Args...>& t) const {
			return tuple_hash_compute<std::tuple<Args...> , sizeof...(Args) > ::hash_compute(t);
		}
		template<class Ty1, class Ty2>
		std::size_t operator()(const std::pair<Ty1, Ty2>& p) const {
			return tuple_hash_compute<std::pair<Ty1, Ty2>, 2>::hash_compute(p);
		}
	};
	// std::pair INSERTION OPERATOR
	/**
	 * \brief Writes a `std::pair` to an output stream `os`.
	 *
	 * \param os Instance of output stream to write to.
	 * \param p `std::pair` instance to write to `os`.
	 * \return `os`.
	 */
	template<typename _Ty1, typename _Ty2> 
	std::ostream& operator<<(std::ostream& os, const std::pair<_Ty1, _Ty2>& p) {
		os << p.first << "\t" << p.second;
		return os;
	}
	// std::tuple INSERTION OPERATOR 
	/**
	 * \struct tuple_write_t
	 *
	 * \brief Recursive function-object for printing a generic `std::tuple`
	 *        type to a `std::ostream` instance.
	 */
	template<class Tuple, std::size_t N>
	struct tuple_write_t {
		static void tuple_write(std::ostream& os, const Tuple& t) {
			tuple_write_t<Tuple, N - 1>::tuple_write(os, t);
			os << std::get<N - 1>(t);
		}
	};
	// base-helper
	template<class Tuple>
	struct tuple_write_t<Tuple, 1> {
		static void tuple_write(std::ostream& os, const Tuple& t) {
			os << std::get<0>(t);
		}
	};
	/**
	 * \brief Writes a `std::tuple` to an output stream `os`.
	 *
	 * \param os Instance of output stream to write to.
	 * \param t `std::tuple` instance to write to `os`.
	 * \return `os`.
	 */
	template<class... Args>
	std::ostream& operator<<(std::ostream& os, const std::tuple<Args...>& t) {
		tuple_write_t<std::tuple<Args...>, sizeof...(Args)>::tuple_write(os, t);
		return os;
	}
#ifndef RANDOM_NUMBER_GENERATOR_H
#define RANDOM_NUMBER_GENERATOR_H
	/**
	 * \class random_number_generator
	 *
	 * \brief Pseudo-random number generator for random values over a specified `Distribution`
	 *        using a given `Generator` engine.
	 *
	 * A convenience wrapper around a generator engine and random number distribution used for
	 * generating random values quickly and simply. Any pre-defined generator from the C++ `<random>`
	 * header may be used as the `Generator` type-param and any distribution from this header
	 * may be used for the `Distribution` type-param. The next value in the random distribution
	 * is generated via a call to `random_number_generator::operator()`. Resetting the internal
	 * state of the distribution such that the next generating call is not dependent upon the last
	 * call is achieved via a call to `random_number_generator::reset_distribution_state()`.
	 *
	 * Note that the type `Ty` (referenced as `result_type` in the class API) must match the type of
	 * distribution `Distribution` (referenced as `distribution_type` in the class API) used, e.g.
  	 * if the `result_type` is `int` then it is undefined behaviour to use a distribution type
	 * intended for floating point types.
	 * 
	 * \tparam Ty The type of the values to generate, must satisfy `std::is_arithmetic<Ty>`. Defaults
	 *         to the integral type `int`.
	 * \tparam Generator The type of the generator engine to use for pseudo-random generation, must
	 *         meet the requirement of `UniformRandomBitGenerator` (see C++ Concepts). Defaults to
	 *         the engine type `std::mt19937`.
	 * \tparam Distribution The type of distribution over which to calculate the random numbers. The
	 *         value type of the distribution must match the value type `Ty` of this class. Must meet
	 *         the requirement of `RandomNumberDistribution` (see C++ Conecpts). Defaults to the
 	 *         distribution type `std::uniform_int_distribution<Ty>`.
	 */
	template<class Ty = int,
		class Generator = std::mt19937,
		class Distribution = std::uniform_int_distribution<Ty>,
		class = std::enable_if_t<std::is_arithmetic<Ty>::value>
	> class random_number_generator {
	public:
		// PUBLIC API TYPE DEFINITIONS
		typedef Ty result_type;
		typedef Generator generator_type;
		typedef Distribution distribution_type;
		// CONSTRUCTION/ASSIGNMENT
		/**
		 * \brief Move constructs the generator with the values of the engine `_eng` and the
		 *        distribution `_dist`. This is also the default constructor.
		 *
		 * \param _eng Generator engine to use.
		 * \param _dist Distribution for random numbers.
		 */
		explicit random_number_generator(Generator&& _eng = Generator{ std::random_device{}() }, Distribution&& _dist = Distribution())
			: eng(std::move(_eng)), dist(std::move(_dist)) {
		}
		/**
		 * \brief Constructs the generator with a copy of the values of the engine `_eng` and
		 *        the distribution `_dist`.
		 *
		 * \param _eng Generator engine to use.
		 * \param _dist Distribution for random numbers.
		 */
		explicit random_number_generator(const Generator& _eng, const Distribution& _dist)
			: eng(_eng), dist(_dist) {
		}
		/**
		 * \brief Copy constructor. Constructs the generator with a copy of the fields of `other`.
		 *
		 * \param other `random_number_generator` instance to use as data source.
		 */
		random_number_generator(const random_number_generator& other)
			: eng(other.eng), dist(other.dist) {
		}
		/**
		 * \brief Move constructor. Constructs the generator with the fields of `other` using
		 *        move-semantics such that `other` is left in a valid but unspecified state.
		 *
		 * \param other `random_number_generator` instance to use as data source.
		 */
		random_number_generator(random_number_generator&& other)
			: eng(std::move(other.eng)), dist(std::move(other.dist)) {
		}
		/**
		 * \brief Copy-assignment operator. Replaces the generator with a copy of the fields of `other`.
		 * \param other `random_number_generator` instance to use as data source.
		 * \return `*this`.
		 */
		random_number_generator& operator=(const random_number_generator& other) {
			if (this != &other)
				random_number_generator(other).swap(*this); // copy-and-swap
			return *this;
		}
		/**
		 * \brief Move-assignment operator. Replaces the generator with the field of `other`
		 *        using move-semantics.
		 * \param other `random_number_generator` instance to use as data source.
		 * \return `*this`.
		 */
		random_number_generator& operator=(random_number_generator&& other) {
			if (this != &other)
				swap(*this, std::move(other));
			return *this;
		}
		// GENERATING OPERATOR()
		/**
		 * \brief Generates the next random number in the distribution.
		 *
		 * \return The generated random number.
		 */
		result_type operator()() { return dist(eng); }
		// GENERATOR AND DISTRIBUTION OBJECT ACCESS
		/**
		 * \brief Returns a copy of the underlying generator engine.
		 * \return A copy of the underlying engine used for random number generation.
		 */
		generator_type get_generator() const noexcept { return eng; }
		/**
		 * \brief Returns a copy of the underlying distribution.
		 * \return A copy of the underlying distribution over which the random
		 *         numbers are generated.
		 */
		distribution_type get_distribution() const noexcept { return dist; }
		// PROPERTIES
		/**
		 * \brief Returns the minimum potentially generated value.
		 * \return The minimum value potentially generated by the underlying distribution.
		 */
		result_type min() const { return dist.min(); }
		/**
		 * \brief Returns the maximum potentially generated value.
		 * \return The maximum value potentially generated by the underlying distribution.
	     */
		result_type max() const { return dist.max(); }
		// MODIFIERS
		/**
		 * \brief Resets the internal state of the underlying distribution object. After calling this function,
		 *        the next call to `operator()` on the generator will not be dependent upon previous calls
		 *        to `operator()`.
		 */
		void reset_distribution_state() { dist.reset(); }
		/**
		 * \brief Exchanges the fields of the generator with those of `other`.
		 *
		 * \param other `random_number_generator` object to swap with.
		 */
		void swap(random_number_generator& other) {
			std::swap(eng, other.eng);
			std::swap(dist, other.dist);
		}
		/**
		 * \brief Exchanges the fields of the generator `lhs` with those of `rhs`.
		 *
		 * \param lhs `random_number_generator` object to swap with `rhs`.
		 * \param rhs `random_number_generator` object to swap with `lhs`.
		 */
		static void swap(random_number_generator& lhs, random_number_generator& rhs) { lhs.swap(rhs); }
	private:
		generator_type eng;
		distribution_type dist;
	};
#endif // !RANDOM_NUMBER_GENERATOR_H
#ifndef UNIFORM_RANDOM_PROBABILITY_GENERATOR_H
#define UNIFORM_RANDOM_PROBABILITY_GENERATOR_H
	/**
	 * \class uniform_random_probability_generator
	 *
	 * \brief Pseudo-random number generator for random floating point values distributed
	 *        uniformly over the range [0.0, 1.0] using a given `Generator` engine.
	 *
	 * A convenience wrapper around a template specialisation of `crsc::random_number_generator` providing
	 * a class to produce uniformly distributed floating point values in the range [0.0, 1.0]. Any pre-defined
	 * generator from the C++ `<random>` header may be used as the `Generator` type-param. The next value in
	 * the random distribution is generated via a call to `uniform_random_probability_generator::operator()`.
	 * Resetting the internal state of the distribution such that the next generating call is not dependent upon
	 * the last call is achieved via a call to `uniform_random_probability_generator::reset_distribution_state()`.
	 *
	 * \tparam FloatType The type of the probabilities to generator, must satisfy `std::is_floating_point<FloatType>`.
	 *         Defaults to the type `double`.
	 * \tparam Generator The type of the generator engine to use for pseudo-random generation, must
	 *         meet the requirement of `UniformRandomBitGenerator` (see C++ Concepts). Defaults to
	 *         the engine type `std::mt19937`.
	 */
	template<class FloatType = double,
		class Generator = std::mt19937,
		class = std::enable_if_t<std::is_floating_point<FloatType>::value>
	> class uniform_random_probability_generator {
		typedef random_number_generator<FloatType, Generator, std::uniform_real_distribution<FloatType>> uniform_pr_gen;
	public:
		// PUBLIC API TYPE DEFINITIONS
		typedef typename uniform_pr_gen::result_type result_type;
		typedef typename uniform_pr_gen::generator_type generator_type;
		typedef typename uniform_pr_gen::distribution_type distribution_type;
		// CONSTRUCTION/ASSIGNMENT
		/**
		 * \brief Move constructs the generator with the value of the pre-defined generator `engine`. This
		 *        is also the default constructor.
		 *
		 * \param engine Generator engine to use.
		 */
		explicit uniform_random_probability_generator(Generator&& engine = Generator{ std::random_device{}() })
			: generator(std::move(engine)) {
		}
		/**
		 * \brief Constructs the gnerator with a copy of the pre-defined generator `engine`.
		 *
		 * \param engine Generator engine to use.
		 */
		explicit uniform_random_probability_generator(const Generator& engine)
			: generator(engine, distribution_type()) {
		}
		/**
		 * \brief Copy constructor. Constructs the generator with a copy of the fields of `other`.
		 *
		 * \param other `uniform_random_probability_generator` instance to use as data source.
		 */
		uniform_random_probability_generator(const uniform_random_probability_generator& other)
			: generator(other.generator) {
		}
		/**
		 * \brief Move constructor. Constructs the generator with the fields of `other` using
		 *        move-semantics such that `other` is left in a valid but unspecified state.
		 *
		 * \param other `uniform_random_probability_generator` instance to use as data source.
		 */
		uniform_random_probability_generator(uniform_random_probability_generator&& other)
			: generator(std::move(other.generator)) {
		}
		/**
		 * \brief Copy assignment operator. Replaces the generator with a copy of the fields of `other`.
		 * \param other `uniform_random_probability_generator` instance to use as data source.
		 * \return `*this`.
		 */
		uniform_random_probability_generator& operator=(const uniform_random_probability_generator& other) {
			if (this != &other)
				generator = other.generator;
			return *this;
		}
		/**
		 * \brief Move-assignment operator. Replaces the generator with the fields of `other` using move-semantics.
		 * \param other `uniform_random_probability_generator` instance to use as data source.
		 * \return `*this`.
		 */
		uniform_random_probability_generator& operator=(uniform_random_probability_generator&& other) {
			if (this != &other)
				generator = std::move(other.generator);
			return *this;
		}
		// GENERATING OPERATOR()
		/**
		 * \brief Generates the next random number in the distribution.
		 * \return The generated random number.
		 */
		result_type operator()() { return generator(); }
		// GENERATOR AND DISTRIBUTION OBJECT ACCESS
		/**
		 * \brief Returns a copy of the underlying generator engine.
		 * \return A copy of the underlying engine used for random number generation.
		 */
		generator_type get_generator() const noexcept { return generator.get_generator(); }
		/**
		 * \brief Returns a copy of the underlying distribution.
		 * \return A copy of the underlying distribution over which the random
		 *         numbers are generated.
		 */
		distribution_type get_distribution() const noexcept { return generator.get_distribution(); }
		// PROPERTIES
		/**
		 * \brief Returns the minimum potentially generated value (always 0.0).
		 * \return The minimum value potentially generated by the underlying distribution.
		 */
		constexpr result_type min() const { return generator.min(); }
		/**
		 * \brief Returns the maximum potentially generated value (always 1.0).
		 * \return The maximum value potentially generated by the underlying distribution.
		 */
		constexpr result_type max() const { return generator.max(); }
		// MODIFIERS
		/**
		 * \brief Resets the internal state of the underlying distribution object. After calling this function,
		 *        the next call to `operator()` on the generator will not be dependent upon previous calls
		 *        to `operator()`.
		 */
		void reset_distribution_state() { generator.reset_distribution_state(); }
		/**
		 * \brief Exchanges the fields of the generator with those of `other`.
		 *
		 * \param other `uniform_random_probability_generator` object to swap with.
		 */
		void swap(uniform_random_probability_generator& other) { generator.swap(other.generator); }
		/**
		 * \brief Exchanges the fields of the generator `lhs` with those of `rhs`.
		 *
		 * \param lhs `uniform_random_probability_generator` object to swap with `rhs`.
		 * \param rhs `uniform_random_probability_generator` object to swap with `lhs`.
		 */
		static void swap(uniform_random_probability_generator& lhs, uniform_random_probability_generator& rhs) { lhs.swap(rhs); }
	private:
		uniform_pr_gen generator;
	};
#endif // !UNIFORM_RANDOM_PROBABILITY_GENERATOR_H
}
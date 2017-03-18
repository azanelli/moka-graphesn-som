#ifndef MOKA_UTIL_MATH_IMPL_H
#define MOKA_UTIL_MATH_IMPL_H

#include "math.h"

#include <functional>
#include <numeric>

namespace moka {
namespace util {

/**
 * Methos maxAbs
 *
 * Given a standard container of numbers (with the method "size" and methods
 * "begin" and "end" that returns iterators to access its elements) this method
 * computes and returns the max absolute value:
 *   Max_i( |x_i| )
 */
template <typename Container>
Math::Real Math::maxAbs(const Container& container)
{
  Real max = 0.0, abs = 0.0;
  typename Container::const_iterator it;
  for (it = container.begin(); it != container.end(); ++it)
  {
    abs = std::fabs(*it);
    if (abs > max)
      max = abs;
  }
  return max;
} // method maxAbs

/**
 * Method avg
 *
 * Given a standard container of numbers (with the method "size" and methods
 * "begin" and "end" that returns iterators to access its elements) this method
 * computes and returns the average of numbers in such container:
 *   Sum_i(x_i) / N
 */
template <typename Container>
Math::Real Math::avg(const Container& container)
{
  return sum(container) / container.size();
} // method avg

/**
 * Method median
 *
 * Given a vector of numbers this method computes and returns their median
 * value.
 *
 * NOTE: the vector is sorted in order to compute the median value.
 */
template <typename ValueType>
ValueType Math::median(std::vector<ValueType>& vect)
{
  size_t v_size = vect.size();
  if (v_size == 0)
    throw moka::GenericException("Math::median: empty vector");

  std::sort(vect.begin(), vect.end());

  ValueType med;
  if (v_size % 2 == 0)
    med = (vect[v_size / 2 - 1] + vect[v_size / 2]) / 2.0;
  else
    med = vect[std::floor(v_size / 2.0)];

  return med;
} // method median

/**
 * Method norm1
 *
 * Given a standard container of numbers (with the method "size" and methods
 * "begin" and "end" that returns iterators to access its elements) this method
 * computes and returns the norm 1 of container elements:
 *   Sum_i( |x_i| )
 */
template <typename Container>
Math::Real Math::norm1(const Container& container)
{
  Real norm = 0.0;
  typename Container::const_iterator it;
  for (it = container.begin(); it != container.end(); ++it)
    norm += std::fabs(*it);
  return norm;
} // method norm1

/**
 * Method stdev
 *
 * Given a standard container of numbers (with the method "size" and methods
 * "begin" and "end" that returns iterators to access its elements) this method
 * computes and returns the standard deviation:
 *   sqrt(Sum_i((x_i - mean)^2) / N)
 */
template <typename Container>
Math::Real Math::stdev(const Container& container)
{
  return stdev(container, avg(container));
} // method standardDeviation

/**
 * Method stdev
 *
 * Given a standard container of numbers (with the method "size" and methods
 * "begin" and "end" that returns iterators to access its elements) and given
 * the average of such numbers (by suppling the average you can speed up the
 * computation) this method computes and returns the standard deviation:
 *   sqrt(Sum_i((x_i - avg)^2) / N)
 */
template <typename Container>
Math::Real Math::stdev(const Container& container, const Real& average)
{
  size_t n_elements = container.size();
  std::vector<Real> diff(n_elements);

  typename Container::const_iterator it = container.begin();
  for (size_t idx = 0; idx < n_elements; ++idx, ++it)
    diff[idx] = (*it) - average;

  Real sq_sum =
      std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);

  return std::sqrt(sq_sum / n_elements);
} // method stdev

/**
 * Method sum
 *
 * Given a standard container of numbers (with the method "size" and methods
 * "begin" and "end" that returns iterators to access its elements) this method
 * computes and returns the sum of numbers in the container:
 *   Sum_i(x_i)
 */
template <typename Container>
Math::Real Math::sum(const Container& container)
{
  return std::accumulate(container.begin(), container.end(), 0.0);
} // method sum

} // namespace util
} // namespace moka

#endif // MOKA_UTIL_MATH_IMPL_H

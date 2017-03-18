#ifndef MOKA_GLOBAL_IMPL_H
#define MOKA_GLOBAL_IMPL_H

#include "global.h"

#include <boost/bind.hpp>

namespace moka {

/**
 * Method read
 *
 * Reads a vector of readable elements (by operator >>) from the passed input
 * stream (previous written using the method write).
 */
template <typename Type>
bool Global::read(std::istream& is, std::vector<Type>& vector)
{
  size_t size = 0;
  is >> size;

  if (size != 0 && !is.good())
    return false;

  vector.resize(size);
  size_t i = 0;
  for (/* nop */; i < size && is.good(); ++i)
    is >> vector[i];

  if (!is.good() && i != size)
    return false;

  return true;
} // method read

/**
 * Method readLine
 *
 * Read a readable value (by operator >>) from the passed input stream. The
 * value must be in one line ended by a carriage return. After this method the
 * istream will be on the next line. A copy of the readed value is returned.
 * In case of error (e.g. ivalid input stream) will be thrown an exception of
 * type moka::GenericException.
 */
template <typename Type>
Type Global::readLine(std::istream& is)
{
  Type value;

  try
  {
    readLine(is, value);
  } // try
  catch (std::exception& ex)
  {
    throw moka::GenericException("Global::readLine: fail reading value");
  } // try-catch

  return value;
} // method readLine

/**
 * Method readLine
 *
 * Read a readable value (by operator >>) from the passed input stream. The
 * value must be in one line ended by a carriage return. After this method the
 * istream will be on the next line.
 * In case of error (e.g. ivalid input stream) will be thrown an exception of
 * type moka::GenericException.
 */
template <typename Type>
void Global::readLine(std::istream& is, Type& value)
{
  if (!is.good())
    throw moka::GenericException("Global::readLine: invalid input stream");

  std::string line;
  std::istringstream iss;

  try
  {
    std::getline(is, line);
    iss.str(line);
    iss >> value;
  } // try
  catch (std::exception& ex)
  {
    throw moka::GenericException("Global::readLine: fail reading value");
  } // try-catch

  return;
} // method readLine

/**
 * Method readLines
 *
 * Read a vector of readable value (by operator >>) from the passed input
 * stream. The first value must be the vector size, then each vector value
 * must be in one line ended by a carriage return. After this method the
 * istream will be on the next line.
 * In case of error (e.g. ivalid input stream) will be thrown an exception of
 * type moka::GenericException.
 */
template <typename Type>
void Global::readLines(std::istream& is, std::vector<Type>& vector)
{
  size_t size = readLine<size_t>(is);

  if (size != 0 && !is.good())
  {
    vector.clear();
    throw moka::GenericException("Global::readLines: invalid input stream");
  }

  vector.resize(size);
  try
  {
    for (size_t i = 0; i < size; ++i)
      Global::readLine(is, vector[i]);
  } // try
  catch (std::exception& ex)
  {
    vector.clear();
    throw moka::GenericException("Global::readLines: fail reading a value");
  } // try-catch

  return;
} // method readLine

/**
 * Method sortMapBySecond
 *
 * Sort the map elements by the second element returning a sorted list of pairs.
 * MAP_TO_SORT (input argument) is the map containing element to sort, while
 * SORTED_LIST (output argument) is a list of pairs filled with the map
 * element sorted by the second element. To sort is used the operator <, so
 * that the first list element is the lower element.
 */
template <typename Type1, typename Type2>
void Global::sortMapBySecond
(
    const std::map<Type1, Type2>& map_to_sort,
    std::list< std::pair<Type1, Type2> >& sorted_list
)
{
  sorted_list.clear();
  sorted_list.resize(map_to_sort.size());

  // Copy pairs in the list
  std::copy(map_to_sort.begin(), map_to_sort.end(), sorted_list.begin());

  // Sort the list
  sorted_list.sort
      (
        boost::bind(&std::pair<Type1, Type2>::second, ::_1) <
        boost::bind(&std::pair<Type1, Type2>::second, ::_2)
      );

  return;
} // method sortMapBySecond

/**
 * Method toString
 *
 * Converts a std::vector into a printable std::string separating elements
 * with the string SEP.
 */
template <typename Type>
std::string Global::toString
(
    const std::vector<Type>& vector,
    const std::string& sep
)
{
  if (vector.empty())
    return std::string("");

  std::stringstream ss;
  ss << vector[0];
  for (size_t i = 1; i < vector.size(); ++i)
    ss << sep << vector[i];

  return ss.str();
} // method toString

/**
 * Method write
 *
 * Writes a vector of writable elements (by operator <<) on output stream with
 * elements separated by a carriage return.
 */
template <typename Type>
void Global::write(std::ostream& os, const std::vector<Type>& vector)
{
  os << vector.size() << std::endl;
  for (size_t i = 0; i < vector.size(); ++i)
    os << vector[i] << std::endl;
  return;
} // method write

} // namespace moka

#endif // MOKA_GLOBAL_IMPL_H

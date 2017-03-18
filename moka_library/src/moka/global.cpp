#include "global.h"

#include <ostream>
#include <sys/time.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <moka/log.h>

namespace moka {

// ======================
// PRIVATE STATIC MEMBERS
// ======================

/**
 * Moka library version string
 */
const std::string Global::m_library_version = "1.0.2a";

/**
 * Global uniform random number generator.
 */
Global::UniformRandomGenerator Global::m_urandgen;

// ==============
// PUBLIC METHODS
// ==============

/**
 * Method getLibraryVersion
 *
 * Return a string representing the current version of this library.
 */
const std::string& Global::getLibraryVersion()
{
  return m_library_version;
} // method getLibraryVersion

/**
 * Method getRandInt
 *
 * Returns a random number (integer) uniformly distributed in the range
 * [MIN, MAX] with MIN < MAX.
 * If the range is not valid an exception of type moka::GenericException will
 * be thrown.
 */
Global::Int Global::getRandInt(Int min, Int max)
{
  if (min > max)
    throw moka::GenericException("Global::getRand<int>: invalid interval.");
  return m_urandgen.getRandInt(min, max);
} // method getRandInt

/**
 * Method getRandReal
 *
 * Returns a random number (real) uniformly distributed in the range [MIN, MAX]
 * with MIN < MAX.
 * If the range is not valid an exception of type moka::GenericException will
 * be thrown.
 */
Global::Real Global::getRandReal(Real min, Real max)
{
  if (min > max)
    throw moka::GenericException("Global::getRand<Real>: invalid interval.");
  return m_urandgen.getRandReal(min, max);
} // method getRandReal

/**
 * Method getRandSeed
 *
 * Returns the seed useds to generates random numbers.
 */
Global::Uint Global::getRandSeed()
{
  return m_urandgen.getRandSeed();
} // method getRandSeed

/**
 * Method readLine
 *
 * Read an std::string from the passed input stream. The value must be in one
 * line ended by a carriage return. After this method the istream will be on
 * the next line. A copy of the readed string is returned.
 * In case of error (e.g. ivalid input stream) will be thrown an exception of
 * type moka::GenericException .
 */
std::string Global::readLine(std::istream& is)
{
  std::string value;

  try
  {
    Global::readLine(is, value);
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
 * Read an std::string from the passed input stream. The value must be in one
 * line ended by a carriage return. After this method the istream will be on
 * the next line.
 * In case of error (e.g. ivalid input stream) will be thrown an exception of
 * type moka::GenericException .
 */
void Global::readLine(std::istream& is, std::string& str)
{
  if (!is.good())
    throw moka::GenericException("Global::readLine: invalid input stream");

  try
  {
    std::getline(is, str);
  } // try
  catch (std::exception& ex)
  {
    throw moka::GenericException("Global::readLine: fail reading value");
  } // try-catch

  return;
} // method readLine

/**
 * Method readNonEmptyLine
 *
 * Reads from istream a non empty line and returns it. If there is an error
 * reading from the input stream an exception will be thrown.
 */
std::string Global::readNonEmptyLine(std::istream& is)
{
  std::string line;
  while (line.empty() && is.good())
  {
    std::getline(is, line);
    boost::algorithm::trim(line);
  }

  if (!is.good())
    throw moka::GenericException("Global::getNonEmptyLine: input stream error");

  return line;
} // method readNonEmptyLine

/**
 * Method setRandSeed
 *
 * Initializes the global random number generator with the passed seed. If the
 * seed is not specified the default is std::time(NULL) (with precision to the
 * second).
 */
void Global::setRandSeed(Uint s)
{
  m_urandgen.setRandSeed(s);
} // method setRandSeed

/**
 * Method timeNull
 *
 * Returns a number representing the current time in millisecond. You can use
 * this value to get a different seed to initialize the random number generator.
 */
Global::Uint Global::timeNull()
{
  timeval timev;
  gettimeofday(&timev, NULL);
  return timev.tv_usec;
} // method timeNull

/**
 * Method toBool
 *
 * Converts the passed string into a bool type. The conversion follows this
 * rules:
 *   - String "true" (without difference upper or lower case) become true
 *   - String "false" (without difference upper or lower case) become false
 *   - Otherwise the string in converted as integer (see toInt):
 *       - Integer 0 become false
 *       - Integer not 0 become true
 */
bool Global::toBool(const std::string& str)
{
  std::string str_cpy = str;

  boost::algorithm::trim(str_cpy);
  boost::algorithm::to_upper(str_cpy);
  if (str_cpy == "TRUE")
    return true;
  else if (str_cpy == "FALSE")
    return false;

  Int val = toInt(str);
  if (val == 0)
    return false;
  else
    return true;
} // method toBool

/**
 * Method toInt
 *
 * Converts the passed string STR into a Int (signed integer) type. If the
 * conversion is not possible returns the value 0.
 */
Global::Int Global::toInt(const std::string& str)
{
  std::string str_cpy = str;
  boost::algorithm::trim(str_cpy);
  try
  {
    return boost::lexical_cast<Int>(str_cpy);
  }
  catch (boost::bad_lexical_cast&)
  { }
  return 0;
} // method toInt

/**
 * Method toReal
 *
 * Converts the passed string STR into a Global::Real (real number) type. If
 * the conversion is not possible returns the value 0.
 */
Global::Real Global::toReal(const std::string& str) {
  std::string str_cpy = str;
  boost::algorithm::trim(str_cpy);
  try
  {
    return boost::lexical_cast<Real>(str_cpy);
  }
  catch (boost::bad_lexical_cast&)
  { }
  return 0.0;
} // method toReal

/**
 * Method toString
 *
 * Converts a bool value into an std::string:
 *   - The value true become the string "true"
 *   - The value false become the string "false"
 */
std::string Global::toString(bool value)
{
  std::ostringstream ss;

  if (value)
    ss << "true";
  else
    ss << "false";

  return ss.str();
} // method toString

/**
 * Method toString
 *
 * Converts an integer number (Int) into a std::string.
 */
std::string Global::toString(Int value)
{
  std::ostringstream ss;
  ss << value;
  return ss.str();
} // method toString

/**
 * Method toString
 *
 * Converts a Real number into a std::string.
 */
std::string Global::toString(Real value)
{
  std::ostringstream ss;
  ss << value;
  return ss.str();
} // method toString

/**
 * Method toString
 *
 * Converts an unsigned integer number (Uint) into a std::string.
 */
std::string Global::toString(Uint value)
{
  std::ostringstream ss;
  ss << value;
  return ss.str();
} // method toString

/**
 * Method toUint
 *
 * Converts the passed string STR into a Global::Uint (unsigned integer) type.
 * If the conversion is not possible returns the value 0.
 */
Global::Uint Global::toUint(const std::string& str)
{
  std::string str_cpy = str;
  boost::algorithm::trim(str_cpy);
  try
  {
    return boost::lexical_cast<Uint>(str_cpy);
  }
  catch (boost::bad_lexical_cast&)
  { }
  return 0;
} // method toUint

// ======================================
// CLASS GLOBAL::UNIFORM_RANDOM_GENERATOR
// ======================================

/**
 * Constructor Global::UniformRandomGenerator
 *
 * The random number generator is initialized with the passed seed
 * (std::time(NULL) by default).
 */
Global::UniformRandomGenerator::UniformRandomGenerator(Global::Uint seed)
{
  setRandSeed(seed);
  return;
} // constructor Global::UniformRandomGenerator

// ==============
// PUBLIC METHODS
// ==============

/**
 * Method UniformRandomGenerator::setRandSeed
 *
 * Initializes the random number generator with the passed seed. If the seed
 * is not specified the default is std::time(NULL) (with precision to the
 * second).
 */
void Global::UniformRandomGenerator::setRandSeed(Global::Uint seed)
{
  m_seed = seed;
  m_generator.seed(seed);
  //  srand(m_seed);
  return;
} // method UniformRandomGenerator::setRandSeed

/**
 * Method UniformRandomGenerator::getRandSeed
 *
 * Returns the seed useds to generates random numbers.
 */
Global::Uint Global::UniformRandomGenerator::getRandSeed() const
{
  return m_seed;
} // method UniformRandomGenerator::getRandSeed

/**
 * Method UniformRandomGenerator::getRandInt
 *
 * Returns a random number (integer) uniformly distributed in the range
 * [MIN, MAX] with MIN < MAX.
 * If the range is not valid an exception of type moka::GenericException will
 * be thrown.
 */
Global::Int Global::UniformRandomGenerator::getRandInt
(
    Global::Int min,
    Global::Int max
)
{
  if (min > max)
    throw moka::GenericException("UniformRandomGenerator::getRandInt: "
        "invalid interval.");
  if (min == max)
    return min;
  boost::random::uniform_int_distribution<Global::Int> dist(min, max);
  //  return ( rand() % (max - min + 1) ) + min;
  return dist(m_generator);
} // method UniformRandomGenerator::getRandInt

/**
 * Method UniformRandomGenerator::getRandReal
 *
 * Returns a random number (real) uniformly distributed in the range [MIN, MAX]
 * with MIN < MAX.
 * If the range is not valid an exception of type moka::GenericException will
 * be thrown.
 */
Global::Real Global::UniformRandomGenerator::getRandReal
(
    Global::Real min,
    Global::Real max
)
{
  if (min > max)
    throw moka::GenericException("UniformRandomGenerator::getRandReal: "
        "invalid interval.");
  if (min == max)
    return min;
  boost::random::uniform_real_distribution<Global::Real> dist(min, max);
  //  return min + (rand() / (Real) RAND_MAX) * (max - min);
  return dist(m_generator);
} // method UniformRandomGenerator::getRandReal

} // namespace moka

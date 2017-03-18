#ifndef MOKA_GLOBAL_H
#define MOKA_GLOBAL_H

#include <ctime>
#include <iterator>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <utility>
#include <boost/random/mersenne_twister.hpp>
#include <moka/exception.h>

namespace moka {

/**
 * Class Global
 *
 * Contains definitions, functions and objects used globally whithin the whole
 * namespace moka.
 */
class Global
{
  public:
    // -----
    // TYPES
    // -----

    // Integer
    typedef int Int;

    // Unsigned integer
    typedef unsigned int Uint;

    // Real number
    typedef double Real;

    // -------
    // CLASSES
    // -------

    class UniformRandomGenerator;

    // -------
    // METHODS
    // -------

    static const std::string& getLibraryVersion();
    static Int getRandInt(Int min, Int max);
    static Real getRandReal(Real min, Real max);
    static Uint getRandSeed();
    static std::string readLine(std::istream& is);
    static void readLine(std::istream& is, std::string& str);
    static std::string readNonEmptyLine(std::istream& is);
    static void setRandSeed(Uint s = std::time(NULL));
    static Uint timeNull();
    static bool toBool(const std::string& str);
    static Int toInt(const std::string& str);
    static Real toReal(const std::string& str);
    static std::string toString(bool value);
    static std::string toString(Int value);
    static std::string toString(Real value);
    static std::string toString(Uint value);
    static Uint toUint(const std::string& str);

    template <typename Type>
    static bool read(std::istream& is, std::vector<Type>& vector);

    template <typename Type>
    static Type readLine(std::istream& is);

    template <typename Type>
    static void readLine(std::istream& is, Type& value);

    template <typename Type>
    static void readLines(std::istream& is, std::vector<Type>& vector);

    template <typename Type1, typename Type2>
    static void sortMapBySecond(
        const std::map<Type1, Type2>& map,
        std::list< std::pair<Type1, Type2> >& sorted_list);

    template <typename Type>
    static std::string toString(
        const std::vector<Type>& vector,
        const std::string& sep = " ");

    template <typename Type>
    static void write(std::ostream& os, const std::vector<Type>& vector);

  private:
    static const std::string m_library_version;
    static UniformRandomGenerator m_urandgen;

}; // class Global

// =================
// CLASS DEFINITIONS
// =================

/**
 * Class Global::UniformRandomGenerator
 *
 * This class allows to have an own random number generator uniform distributed.
 * You can set the seed with the method "setRandSeed" and with the same seed
 * setted the numbers generated will be the same.
 */
class Global::UniformRandomGenerator
{
  public:
    UniformRandomGenerator(Global::Uint seed = std::time(NULL));
    void setRandSeed(Global::Uint seed = std::time(NULL));
    Global::Uint getRandSeed() const;
    Global::Int getRandInt(Global::Int min, Global::Int max);
    Global::Real getRandReal(Global::Real min, Global::Real max);

  private:
    boost::random::mt19937 m_generator;
    Global::Uint m_seed;

}; // class Global::UniformRandomGenerator

} // namespace moka

#include "global_impl.h"

#endif // MOKA_GLOBAL_H

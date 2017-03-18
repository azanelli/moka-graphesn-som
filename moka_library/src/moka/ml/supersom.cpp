#include "supersom.h"

#include <cmath>
#include <fstream>
#include <limits>
#include <map>
#include <utility>
#include <boost/algorithm/string.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <moka/exception.h>
#include <moka/log.h>

namespace moka {
namespace ml {

namespace balg = ::boost::algorithm;
namespace bll  = ::boost::lambda;
namespace mut  = ::moka::util;

/**
 * Constructor
 */
SuperSOM::SuperSOM()
{
  clearObject();
}

/**
 * Copy constructor
 */
SuperSOM::SuperSOM(const SuperSOM& ssom)
{
  this->operator=(ssom);
} // copy constructor

/**
 * Destructor
 */
SuperSOM::~SuperSOM()
{
  clearNeighborsMap();
  return;
}

// ==============
// PUBLIC METHODS
// ==============

/**
 * Method activate
 *
 * Returns the unit activation function (cauchy function) result on the passed
 * data:
 *   f(x) = 1 / (1 + (x / gamma)^2)
 * where x is the distance between the passed data and the codebook of the
 * passed unit. The parameter gamma is 0.01 if you don't setted it.
 *
 * The object must be initialized (see isInitialized) otherwise this method
 * returns always the value 0.0.
 */
SuperSOM::Real SuperSOM::activate
(
    const UnitIndex& unit,
    const Data& data
) const
{
  if (!isInitialized())
    return 0.0;

  Real act_fun_res =
      1.0 /
      (
        1.0 +
        std::pow
        (
          distance(data, m_map[unit.first][unit.second]) / m_act_fun_gamma,
          2
        )
      );
  return act_fun_res;
} // method activate

/**
 * Method dectToStr
 *
 * Converts a DecayType value into an std::string. If the DecayType value
 * is not recognized throws an exception of type moka::GenericException.
 */
std::string SuperSOM::dectToStr(const DecayType& decay_type)
{
  switch (decay_type)
  {
  case linear_decay:
    return "linear";
    break;
  case inverse_decay:
    return "inverse";
    break;
  default:
    throw moka::GenericException(
        "SuperSOM::dectToStr: invalid DecayType value");
  }
  return "";
} // method dectToStr

/**
 * Method init
 *
 * Inits the map units randomly in the range of training data.
 * Following parameters must be properly setted before to call this method and
 * you must re-call this method if any of this is changed:
 *   - number of rows
 *   - number of columns
 *   - random seed
 *
 * The number of rows and the number of columns must be greater than zero,
 * otherwise this method do nothing.
 */
void SuperSOM::init(const DataContainer& training_data)
{
  if (m_nrows == 0 || m_ncols == 0)
    return;

  // Inits the map
  m_map.resize(m_nrows);
  for (size_t i = 0; i < m_nrows; ++i)
    m_map[i].resize(m_ncols);

  size_t data_size = training_data[0].size();

  // For each element get the min and the max value in the training set
  Data elem_min = arma::zeros(data_size);
  Data elem_max = arma::zeros(data_size);
  for (size_t el = 0; el < data_size; ++el)
  {
    elem_min[el] = training_data[0][el];
    elem_max[el] = training_data[0][el];
  }
  for (size_t i = 0; i < training_data.size(); ++i)
  {
    for (size_t el = 0; el < data_size; ++el)
    {
      if (training_data[i][el] < elem_min[el])
        elem_min[el] = training_data[i][el];
      if (training_data[i][el] > elem_max[el])
        elem_max[el] = training_data[i][el];
    }
  } // for i

  // Inits map values randomly
  for (size_t row = 0; row < m_nrows; ++row)
  {
    for (size_t col = 0; col < m_ncols; ++col)
    {
      m_map[row][col] = arma::zeros(data_size);

      for (size_t el = 0; el < data_size; ++el)
        m_map[row][col][el] = m_rand.getRandReal(elem_min[el], elem_max[el]);

    } // for col
  } // for row

  // Makes this object usable
  setInitialized(true);

  return;
} // method initMap

///**
// * Method loadFromFile
// *
// * Loads a SOM from the specified file (see method write).
// * After the first line, lines starting with a '#' will be skipped and for each
// * line will be read codebooks of the specified size, skipping other values
// * (like labels values) at the end of the line.
// *
// * In case of errors an exception of type moka::GenericException will be thrown
// * and the SOM will be left *uninitialized* (i.e. with the same starting
// * dimensions but you must invoke the method initMap to use the SOM).
// */
//void SuperSOM::loadFromFile(const std::string& filename)
//{
//  setInitialized(false);
//  m_map.clear();
//
//  try
//  {
//    std::fstream fs(filename.c_str(), std::fstream::in);
//    if (fs.fail())
//      throw moka::GenericException(
//          "SuperSOM::loadFromFile: fail open file " + filename);
//
//    std::string line;
//    std::vector<std::string> sline;
//
//    // Get som dimensions
//    std::getline(fs, line);
//    balg::trim(line);
//    balg::split(sline, line, balg::is_any_of("\t "), balg::token_compress_on);
//
//    if (sline.size() != 5)
//      throw moka::GenericException(
//          "SuperSOM::loadFromFile:first line must have 5 elements");
//
//    Uint cod_size = Global::toUint(sline[0]);
//    Uint no_cols = Global::toUint(sline[2]);
//    Uint no_rows = Global::toUint(sline[3]);
//
//    if (cod_size == 0 || no_cols == 0 || no_rows == 0)
//      throw moka::GenericException(
//          "SuperSOM::loadFromFile: some size is wrong");
//
//    m_map.resize(no_rows);
//    for (size_t i = 0; i < no_rows; ++i)
//      m_map[i].resize(no_cols);
//
//    for (Uint row = 0; row < no_rows; ++row)
//    {
//      Uint col = 0;
//      while (col < no_cols)
//      {
//        std::getline(fs, line);
//        balg::trim(line);
//
//        if (!line.empty() && line[0] != '#')
//        {
//          balg::split(
//              sline, line, balg::is_any_of("\t "), balg::token_compress_on);
//
//          if (sline.size() < cod_size)
//            throw moka::GenericException(
//                "SuperSOM::loadFromFile: wrong codebook size: " + line);
//
//          m_map[row][col] = arma::zeros(cod_size);
//          for (Uint cod_el = 0; cod_el < cod_size; ++cod_el)
//            m_map[row][col][cod_el] = Global::toReal(sline[cod_el]);
//
//          ++col;
//        } // if (!line.empty() && line[0] != '#')
//
//      } // (cols < no_cols)
//    } // for row
//
//    m_nrows = no_rows;
//    m_ncols = no_cols;
//    setInitialized(true);
//
//  } // try
//  catch (std::exception& ex)
//  {
//    m_map.clear();
//    throw moka::GenericException(
//        std::string("SuperSOM::loadFromFile: error: ") + ex.what());
//  }
//
//  return;
//} // method loadFromFile

/**
 * Method loadFromFile
 *
 * Loads a SOM from the specified file using the method read.
 *
 * In case of errors an exception of type moka::GenericException will be thrown
 * and the SOM will be left empty (as just created).
 */
void SuperSOM::loadFromFile(const std::string& filename)
{
  try
  {
    std::fstream fs(filename.c_str(), std::fstream::in);
    if (fs.fail())
    {
      clearObject();
      throw moka::GenericException(
          "SuperSOM::loadFromFile: fail open file " + filename);
    }

    read(fs);

    fs.close();
  } // try
  catch (std::exception& ex)
  {
    throw moka::GenericException(
        std::string("SuperSOM::loadFromFile: fail: ") + ex.what());
  }

  return;
} // method loadFromFile

/**
 * operator=
 *
 * Copy the passed SOM into this object.
 */
SuperSOM& SuperSOM::operator=(const SuperSOM& ssom)
{
  // Parameters
  m_nrows = ssom.m_nrows;
  m_ncols = ssom.m_ncols;
  m_act_fun_gamma = ssom.m_act_fun_gamma;
  m_is_initialized = ssom.m_is_initialized;

  // Random number generator
  m_rand = ssom.m_rand;

  // SOM elements
  m_map = ssom.m_map;

  // Training parameters
  m_nepochs_1 = ssom.m_nepochs_1;
  m_nepochs_2 = ssom.m_nepochs_2;
  m_nepochs_3 = ssom.m_nepochs_3;
  m_alpha_1 = ssom.m_alpha_1;
  m_alpha_2 = ssom.m_alpha_2;
  m_alpha_3 = ssom.m_alpha_3;
  m_sigma_1 = ssom.m_sigma_1;
  m_sigma_2 = ssom.m_sigma_2;
  m_sigma_3 = ssom.m_sigma_3;
  m_sigma_fin_1 = ssom.m_sigma_fin_1;
  m_sigma_fin_2 = ssom.m_sigma_fin_2;
  m_sigma_fin_3 = ssom.m_sigma_fin_3;
  m_alpha_decay_type = ssom.m_alpha_decay_type;

  // Training support structures
  clearNeighborsMap();
  m_neighbors_map.resize(ssom.m_neighbors_map.size());
  for (size_t r = 0; r < m_neighbors_map.size(); ++r)
  {
    m_neighbors_map[r].resize(ssom.m_neighbors_map[r].size());
    for (size_t c = 0; c < m_neighbors_map[r].size(); ++c)
    {
      m_neighbors_map[r][c].resize(ssom.m_neighbors_map[r][c].size());
      for (size_t n = 0; n < m_neighbors_map[r][c].size(); ++n)
        m_neighbors_map[r][c][n] =
            new std::list<UnitIndex>(*ssom.m_neighbors_map[r][c][n]);
    } // for c
  } // for r

  m_classes_map = ssom.m_classes_map;
  m_winner_unit_vector = ssom.m_winner_unit_vector;

  return (*this);
} // operator=

/**
 * Method read
 *
 * Read the SOM from an input stream, previously written using the method
 * write.
 *
 * In case of errors an exception of type moka::GenericException will be
 * thrown and this object is left "clear" (as just created).
 */
void SuperSOM::read(std::istream& is)
{
  clearObject();

  try
  {
    std::string line;

    // Parameters
    Global::readLine(is, m_nrows);
    Global::readLine(is, m_ncols);
    Global::readLine(is, m_act_fun_gamma);
    Global::readLine(is, line);
    m_is_initialized = Global::toBool(line);

    // Random number generator
    m_rand.setRandSeed(Global::readLine<Uint>(is));

    // SOM elements
    m_map.resize(m_nrows);
    for (size_t i = 0; i < m_nrows; ++i)
      m_map[i].resize(m_ncols);

    for (Uint r = 0; r < m_nrows; ++r)
      for (Uint c = 0; c < m_ncols; ++c)
        mut::Math::read(is, m_map[r][c]);

    // Training parameters
    Global::readLine(is, m_nepochs_1);
    Global::readLine(is, m_nepochs_2);
    Global::readLine(is, m_nepochs_3);
    Global::readLine(is, m_alpha_1);
    Global::readLine(is, m_alpha_2);
    Global::readLine(is, m_alpha_3);
    Global::readLine(is, m_sigma_1);
    Global::readLine(is, m_sigma_2);
    Global::readLine(is, m_sigma_3);
    Global::readLine(is, m_sigma_fin_1);
    Global::readLine(is, m_sigma_fin_2);
    Global::readLine(is, m_sigma_fin_3);
    Global::readLine(is, line);
    m_alpha_decay_type = strToDect(line);

  } // try
  catch (std::exception& ex)
  {
    clearObject();
    throw moka::GenericException(
        std::string("SuperSOM::read: fail reading the object: ") + ex.what());
  } // catch

  return;
} // method read

/**
 * Method saveOnFile
 *
 * Save the SuperSOM on file. See the method write.
 * In case of error returns false, a message will be logged out and no map is
 * written. Otherwise returns true.
 */
bool SuperSOM::saveOnFile(const std::string& filename) const
{
  if (!m_map.size())
  {
    Log::err << "SuperSOM::saveOnFile: empty SuperSOM." << Log::endl;
    return false;
  }

  std::fstream fs(filename.c_str(), std::fstream::out);
  if (fs.fail())
  {
    Log::err << "SuperSOM::saveOnFile: fail open file " << filename << "."
             << Log::endl;
    return false;
  }

  write(fs);
  fs.close();

  return true;
} // method saveOnFile

/**
 * Method setDefaultParameters
 *
 * Given the the training set size, this method deduces and set good training
 * parameters:
 *   - Alpha decay = linear
 *   - Phase 1 (rough train)
 *       - epochs_1 = ceil(10 * munits / dlen)
 *           where munits is the number of units in the map and dlen is the
 *           training set size.
 *       - alpha_1 = 0.5
 *       - sigma_ini_1 = max(1.0, ceil(ms / 4))
 *           where ms is the max(rows, cols) on the map.
 *       - sigma_fin_1 = max(1.0, sigma_ini / 4)
 *   - Phase 2 (fine tune)
 *       - epochs_2 = 4 * epochs_1
 *       - alpha_2 = alpha_1 / 10.0
 *       - sigma_ini_2 = sigma_fin_1
 *       - sigma_fin_2 = 1.0
 *   - Phase 3 (final phase / supervised train)
 *       - epochs_3 = epochs_2
 *       - alpha_3 = alpha_2
 *       - sigma_ini_3 = sigma_fin_2 / 2.0
 *       - sigma_fin_3 = 0.1
 *
 * Note that you must have already setted the SOM dimensions (number of rows
 * and number of columns).
 */
void SuperSOM::setDefaultParameters(Uint training_set_size)
{
  // Phase 1
  m_nepochs_1 = std::ceil(10.0 * getNoUnits() / (Real)training_set_size);
  m_alpha_1 = 0.5;
  m_sigma_1 = std::max(1.0, std::ceil(std::max(m_nrows, m_ncols) / 4.0));
  m_sigma_fin_1 = std::max(1.0, std::ceil(m_sigma_1 / 4.0));

  // Phase 2
  m_nepochs_2 = 4 * m_nepochs_1;
  m_alpha_2 = m_alpha_1 / 10.0;
  m_sigma_2 = m_sigma_fin_1;
  m_sigma_fin_2 = 1.0;

  // Phase 3
  m_nepochs_3 = m_nepochs_2;
  m_alpha_3 = m_alpha_2;
  m_sigma_3 = m_sigma_fin_2 / 2.0;
  m_sigma_fin_3 = 0.1;

  setInitialized(false);

  return;
} // method setDefaultParameters

/**
 * Method strToDect
 *
 * Converts a an std::string into a DecayType value. If the string is not
 * recognize throw an exception of type moka::GenericException.
 */
SuperSOM::DecayType SuperSOM::strToDect(const std::string& str)
{
  if (str == "linear")
    return linear_decay;
  else if (str == "inverse")
    return inverse_decay;
  else
    throw moka::GenericException(
        "SuperSOM::strToDect: Invalid string representation of a DecayType "
        "value");
  // return 0;
} // method strToDect

/**
 * Method supervisedTraining
 *
 * Trains the SOM using the supervised training algorithm.
 * The map should be previously initialized (see initMap). Following parameters
 * must be setted properly for each phase:
 *   - learning rate alpha
 *   - radius sigma
 *   - number of epochs
 *
 * The object must be initialized (see isInitialized) otherwise this method
 * do nothing.
 */
void SuperSOM::supervisedTraining
(
    const DataContainer& training_data,
    const std::vector<Real>& training_data_classes
)
{
  if (!isInitialized())
  {
    Log::err << "SuperSOM::supervisedTraining: the map must be initialized."
             << Log::endl;
    return;
  }

  if (training_data.size() != training_data_classes.size())
  {
    Log::err << "SuperSOM::supervisedTraining: fatal error: training data and "
             << "classes are of different sizes. The SOM is left untrained."
             << Log::endl;
    return;
  }

  // Build training support data structures
  initNeighborsMap();

  // Unsupervised rough train
  if (m_nepochs_1 != 0)
  {
    // Log::vrb << "Start SOM unsupervised rough train." << Log::endl;
    unsuperTrainProcedure
    (
        training_data,
        m_nepochs_1,
        m_alpha_1,
        m_alpha_decay_type,
        m_sigma_1,
        m_sigma_fin_1
    );
  }

  // Unsupervised fine tune
  if (m_nepochs_2 != 0)
  {
    // Log::vrb << "Start SOM unsupervised fine tune." << Log::endl;
    unsuperTrainProcedure
    (
        training_data,
        m_nepochs_2,
        m_alpha_2,
        m_alpha_decay_type,
        m_sigma_2,
        m_sigma_fin_2
    );
  }

  // Supervised train
  if (m_nepochs_3 != 0)
  {
    // Log::vrb << "Start SOM supervised train." << Log::endl;
    superTrainProcedure
    (
        training_data,
        training_data_classes,
        m_nepochs_3,
        m_alpha_3,
        m_alpha_decay_type,
        m_sigma_3,
        m_sigma_fin_3
    );
  }

  // Delete data structures
  clearNeighborsMap();

  return;
} // method supervisedTraining

/**
 * Method umatrix
 *
 * Builds the U-matrix (unified distance matrix) for the current SOM and put it
 * in UMAT.
 *
 * The resulting U-matrix is a matrix of size
 *   (2 * nrows - 1) x (2 * ncols - 1)
 * where nrows and ncols are respectively the number of rows and the number of
 * columns of the current SOM. The U-matrix is filled as follows (considering
 * the map as hexagonal):
 *   m   dx  m   dx  m   dx  ...
 *   dz  dy  dz  dy  dz  dy  ...
 *   m   dx  m   dx  m   dx  ...
 *   ...
 * where
 *   - dx is the distance between the codebook of a unit and that of the unit
 *     to its right.
 *   - dy is the distance between the codebook of a unit and that of the unit
 *     under it on the left.
 *   - dz is the distance between the codebook of a unit and that of the unit
 *     under it on the right.
 *   - m is the median of the surrounding values of a unit.
 * all the values are then normalized in [0,1] (to represents values in
 * grayscale).
 *
 * The map should first initialized (see isInitialized) otherwise an error
 * occurs and an exception of type moka::GenericException will be throw.
 */
void SuperSOM::umatrix(UMatrix& umat) const
{
  typedef typename UMatrix::value_type::value_type UMatValue;

  if (!isInitialized())
    throw moka::GenericException(
        "SuperSOM::umatrix: the map should be initialized");

  // Init the umatrix
  Uint urows = 2 * m_nrows - 1;
  Uint ucols = 2 * m_ncols - 1;

  umat.resize(urows);
  for (Uint ur = 0; ur < urows; ++ur)
    umat[ur].resize(ucols);

  // Compute distances dx, dy, dz
  for (Uint r = 0; r < m_nrows; ++r)
    for (Uint c = 0; c < m_ncols; ++c)
    {
      // Compute dx
      if (c < m_ncols - 1)
        umat[2 * r][2 * c + 1] = arma::norm(m_map[r][c] - m_map[r][c + 1], 2);

      // Compute dy
      if (r < m_nrows - 1)
      {
        if (r % 2 != 0) // odd row (right shifted)
          umat[2 * r + 1][2 * c] =
              arma::norm(m_map[r][c] - m_map[r + 1][c], 2);
        else if (c > 0) // even row, but not in the first column
          umat[2 * r + 1][2 * c - 1] =
              arma::norm(m_map[r][c] - m_map[r + 1][c - 1], 2);
      } // if

      // Compute dz
      if (r < m_nrows - 1)
      {
        if (r % 2 == 0) // even row
          umat[2 * r + 1][2 * c] =
              arma::norm(m_map[r][c] - m_map[r + 1][c], 2);
        else if (c < m_ncols - 1) // odd row (right shifted), not last column
          umat[2 * r + 1][2 * c + 1] =
              arma::norm(m_map[r][c] - m_map[r + 1][c + 1], 2);
      } // if

    } // for (r,c)

  // Compute medians values
  for (Uint ur = 0; ur < urows; ur += 2)
    for (Uint uc = 0; uc < ucols; uc += 2)
    {
      std::vector<UMatValue> med_values;

      // Unit in the middle of the map
      if (ur > 0 && uc > 0 && ur < urows - 1 && uc < ucols - 1)
      {
        med_values.resize(6);
        med_values[0] = umat[ur][uc - 1];
        med_values[1] = umat[ur][uc + 1];
        if (ur % 4 == 0)
        {
          med_values[2] = umat[ur - 1][uc - 1];
          med_values[3] = umat[ur - 1][uc];
          med_values[4] = umat[ur + 1][uc - 1];
          med_values[5] = umat[ur + 1][uc];
        }
        else
        {
          med_values[2] = umat[ur - 1][uc];
          med_values[3] = umat[ur - 1][uc + 1];
          med_values[4] = umat[ur + 1][uc];
          med_values[5] = umat[ur + 1][uc + 1];
        }
      } // if

      // Unit in the upper edge
      else if (ur == 0 && uc > 0 && uc < ucols - 1)
      {
        med_values.resize(4);
        med_values[0] = umat[ur][uc - 1];
        med_values[1] = umat[ur][uc + 1];
        med_values[2] = umat[ur + 1][uc];
        med_values[3] = umat[ur + 1][uc - 1];
      } // else if

      // Unit in the lower edge
      else if (ur == urows - 1 && uc > 0 && uc < ucols - 1)
      {
        med_values.resize(4);
        med_values[0] = umat[ur][uc - 1];
        med_values[1] = umat[ur][uc + 1];
        if (ur % 4 == 0)
        {
          med_values[2] = umat[ur - 1][uc - 1];
          med_values[3] = umat[ur - 1][uc];
        }
        else
        {
          med_values[2] = umat[ur - 1][uc];
          med_values[3] = umat[ur - 1][uc + 1];
        }
      } // else if

      // Unit in the left edge
      else if (ur > 0 && uc == 0 && ur < urows - 1)
      {
        med_values.reserve(5);
        med_values.push_back(umat[ur][uc + 1]);
        if (ur % 4 == 0)
        {
          med_values.push_back(umat[ur - 1][uc]);
          med_values.push_back(umat[ur + 1][uc]);
        }
        else
        {
          med_values.push_back(umat[ur - 1][uc]);
          med_values.push_back(umat[ur - 1][uc + 1]);
          med_values.push_back(umat[ur + 1][uc]);
          med_values.push_back(umat[ur + 1][uc + 1]);
        }
      } // else if

      // Unit in the right edge
      else if (ur > 0 && uc == ucols - 1 && ur < urows - 1)
      {
        med_values.reserve(5);
        med_values.push_back(umat[ur][uc - 1]);
        if (ur % 4 != 0)
        {
          med_values.push_back(umat[ur - 1][uc]);
          med_values.push_back(umat[ur + 1][uc]);
        }
        else
        {
          med_values.push_back(umat[ur - 1][uc]);
          med_values.push_back(umat[ur - 1][uc - 1]);
          med_values.push_back(umat[ur + 1][uc]);
          med_values.push_back(umat[ur + 1][uc - 1]);
        }
      } // else if

      // Unit in the upper left-hand corner
      else if (ur == 0 && uc == 0)
      {
        med_values.resize(2);
        med_values[0] = umat[ur][uc + 1];
        med_values[1] = umat[ur + 1][uc];
      } // else if

      // Unit in the upper right-hand corner
      else if (ur == 0 && uc == ucols - 1)
      {
        med_values.resize(3);
        med_values[0] = umat[ur][uc - 1];
        med_values[1] = umat[ur + 1][uc - 1];
        med_values[2] = umat[ur + 1][uc];
      } // else if

      // Unit in the lower left-hand corner
      else if (ur == urows - 1 && uc == 0)
      {
        if (ur % 4 == 0)
        {
          med_values.resize(2);
          med_values[0] = umat[ur][uc + 1];
          med_values[1] = umat[ur - 1][uc];
        }
        else
        {
          med_values.resize(3);
          med_values[0] = umat[ur][uc + 1];
          med_values[1] = umat[ur - 1][uc];
          med_values[2] = umat[ur - 1][uc + 1];
        }
      } // else if

      // Unit in the lower right-hand corner
      else if (ur == urows - 1 && uc == ucols - 1)
      {
        if (ur % 4 != 0)
        {
          med_values.resize(2);
          med_values[0] = umat[ur][uc - 1];
          med_values[1] = umat[ur - 1][uc];
        }
        else
        {
          med_values.resize(3);
          med_values[0] = umat[ur][uc - 1];
          med_values[1] = umat[ur - 1][uc];
          med_values[2] = umat[ur - 1][uc - 1];
        }
      } // else if

      umat[ur][uc] = mut::Math::median(med_values);

    } // for (ur,uc)

  // Find minimum and maximum values
  UMatValue umax = 0.0;
  UMatValue umin = std::numeric_limits<UMatValue>::max();

  for (Uint ur = 0; ur < urows; ur++)
    for (Uint uc = 0; uc < ucols; uc++)
    {
      if (umat[ur][uc] > umax)
        umax = umat[ur][uc];
      if (umat[ur][uc] < umin)
        umin = umat[ur][uc];
    }

  // Scale values to [0,1]
  for (Uint ur = 0; ur < urows; ur++)
    for (Uint uc = 0; uc < ucols; uc++)
      umat[ur][uc] = 1.0 - (umat[ur][uc] - umin) / (umax - umin);

  return;
} // method umatrix

/**
 * Method unsupervisedTraining
 *
 * Trains the SOM using classic algorithm (rough train + fine tune).
 * The map should be previously initialized (see initMap). Following parameters
 * must be setted properly for each phase:
 *   - learning rate alpha
 *   - radius sigma (initial and final)
 *   - number of epochs
 *
 * The object must be initialized (see isInitialized) otherwise this method
 * do nothing.
 */
void SuperSOM::unsupervisedTraining(const DataContainer& training_data)
{
  if (!isInitialized())
  {
    Log::err << "SuperSOM::unsupervisedTraining: the map must be initialized."
             << Log::endl;
    return;
  }

  // Build training support data structures
  initNeighborsMap();

  // Rough train
  if (m_nepochs_1 != 0)
  {
    // Log::vrb << "Start SOM rough train." << Log::endl;
    unsuperTrainProcedure
    (
        training_data,
        m_nepochs_1,
        m_alpha_1,
        m_alpha_decay_type,
        m_sigma_1,
        m_sigma_fin_1
    );
  }

  // Fine tune
  if (m_nepochs_2 != 0)
  {
    // Log::vrb << "Start SOM fine tune." << Log::endl;
    unsuperTrainProcedure
    (
        training_data,
        m_nepochs_2,
        m_alpha_2,
        m_alpha_decay_type,
        m_sigma_2,
        m_sigma_fin_2
    );
  }

  // Final unsupervised tune
  if (m_nepochs_3 != 0)
  {
    // Log::vrb << "Start SOM final unsupervised tune." << Log::endl;
    unsuperTrainProcedure
    (
        training_data,
        m_nepochs_3,
        m_alpha_3,
        m_alpha_decay_type,
        m_sigma_3,
        m_sigma_fin_3
    );
  }

  // Delete data structures
  clearNeighborsMap();

  return;
} // method unsupervisedTraining

/**
 * Method winnerUnit
 *
 * Computes and returns the winner unit indeces on the passed data:
 *    SuperSOM::UnitIndex wu = SuperSOM.winner(data); // get winner unit
 *    wu.first;  // row index
 *    wu.second; // column index
 *
 * The map should first initialized (see isInitialized) otherwise an error
 * occurs and an exception of type moka::GenericException will be throw.
 */
SuperSOM::UnitIndex SuperSOM::winnerUnit(const Data& data) const
{
  if (!isInitialized())
    throw moka::GenericException(
        "SuperSOM::winnerUnit: the map should be initialized");

  UnitIndex winner_unit;
  winner_unit.first = 0;
  winner_unit.second = 0;

  Real dist_min = distance(data, m_map[0][0]);

  // Get the winner unit
  for (size_t r = 0; r < m_nrows; ++r)
  {
    for (size_t c = 0; c < m_ncols; ++c)
    {
      Real dist = distance(data, m_map[r][c]);

      if (dist < dist_min)
      {
        dist_min = dist;
        winner_unit.first = r;
        winner_unit.second = c;
      } // if

    } // for c
  } // for r

  return winner_unit;
} // method winnerUnit

/**
 * Method winnerUnits
 *
 * Computes and fill the vector WINNERS with indeces of the N closest units
 * to the passed data. The indeces are also ordered in the vector to the
 * closest (at index 0) to less close unit.
 *
 * The map should first initialized (see isInitialized) otherwise an error
 * occurs and an exception of type moka::GenericException will be throw.
 */
void SuperSOM::winnerUnits
(
    const Data& data,
    Uint n,
    std::vector<UnitIndex>& winners
) const
{
  if (!isInitialized())
    throw moka::GenericException(
        "SuperSOM::winnerUnits: the map should be initialized");

  // Clear final vector
  winners.clear();
  winners.resize(n);

  // Some support structures
  std::vector< std::pair<UnitIndex, Real> > closest_unit(n);
  Real last_dist = std::numeric_limits<Real>::max();
  size_t last_index = 0;

  // Inits support structures
  for (size_t i = 0; i < n; ++i)
  {
    closest_unit[i].first.first = 0;
    closest_unit[i].first.second = 0;
    closest_unit[i].second = last_dist;
  } // for i

  // Get closest units
  UnitIndex unit;
  for (unit.first = 0; unit.first < m_nrows; ++unit.first)
    for (unit.second = 0; unit.second < m_ncols; ++unit.second)
    {
      Real dist = distance(data, getCodebook(unit));

      if (dist < last_dist)
      {
        closest_unit[last_index].first = unit;
        closest_unit[last_index].second = dist;

        // search for last distance
        last_dist = dist;
        for (size_t i = 0; i < n; ++i)
          if (closest_unit[i].second > last_dist)
          {
            last_index = i;
            last_dist = closest_unit[i].second;
          }

      } // if (dist < last_dist)

    } // for (unit.first, unit.second)

  // Sort results
  std::sort
      (
        closest_unit.begin(),
        closest_unit.end(),
        (
          bll::bind(&std::pair<UnitIndex, Real>::second, bll::_1) <
          bll::bind(&std::pair<UnitIndex, Real>::second, bll::_2)
        )
      );

  // Store results in winners vector
  for (size_t i = 0; i < n; ++i)
    winners[i] = closest_unit[i].first;

  return;
} // method winnerUnits

///**
// * Method write
// *
// * Writes the SOM on the output stream in the following format (from SOM_PAK):
// *
// *   <size> <top> <M> <N> <neigh>
// *   <codebook vector (0,0)>
// *   <codebook vector (0,1)>
// *   ...
// *   <codebook vector (0,M)>
// *   <codebook vector (1,0)>
// *   <codebook vector (1,1)>
// *   ...
// *   <codebook vector (1,M)>
// *   ...
// *   <codebook vector (N,M)>
// *
// * where:
// *   - <size>: is the codebook dimension
// *   - <top>: is the network topology (hexagonal)
// *   - <M>: is the number of columns in the map
// *   - <N>: is the number of rows in the map
// *   - <neigh>: is the neighborhood function (gaussian function)
// *
// * You can reload the map using the method read.
// */
//void SuperSOM::write(std::ostream& os) const
//{
//  const size_t nrows = m_map.size();
//  const size_t ncols = m_map[0].size();
//
//  os << m_map[0][0].size() << " hexa ";
//  os << ncols << " ";
//  os << nrows << " ";
//  os << "gaussian \n";
//
//  for (size_t r = 0; r < nrows; ++r)
//  {
//    for (size_t c = 0; c < ncols; ++c)
//    {
//      for (size_t w = 0; w < m_map[r][c].size(); ++w)
//        os << m_map[r][c][w] << " ";
//      os << std::endl;
//    } // for c
//  } // for r
//
//  return;
//} // method write

/**
 * Method write
 *
 * Writes the SOM on the output stream. You can reload the map using the method
 * read.
 *
 * Since not all the object elements are written, if you read this object
 * (through the method read) previously written using this method, by calling
 * the init method you can get a different initialization respect to call such
 * method before the write/read process.
 */
void SuperSOM::write(std::ostream& os) const
{
  os << m_nrows << std::endl;
  os << m_ncols << std::endl;
  os << m_act_fun_gamma << std::endl;
  os << Global::toString(m_is_initialized) << std::endl;

  os << m_rand.getRandSeed() << std::endl;

  // SOM elements
  for (size_t r = 0; r < m_nrows; ++r)
    for (size_t c = 0; c < m_ncols; ++c)
      m_map[r][c].save(os, arma::arma_ascii);

  // Training parameters
  os << m_nepochs_1 << std::endl;
  os << m_nepochs_2 << std::endl;
  os << m_nepochs_3 << std::endl;
  os << m_alpha_1 << std::endl;
  os << m_alpha_2 << std::endl;
  os << m_alpha_3 << std::endl;
  os << m_sigma_1 << std::endl;
  os << m_sigma_2 << std::endl;
  os << m_sigma_3 << std::endl;
  os << m_sigma_fin_1 << std::endl;
  os << m_sigma_fin_2 << std::endl;
  os << m_sigma_fin_3 << std::endl;
  os << dectToStr(m_alpha_decay_type) << std::endl;

  return;
} // method write

// ===============
// PRIVATE METHODS
// ===============

/**
 * Method alphaDecay
 *
 * Returns dacayed alpha value.
 */
inline
SuperSOM::Real SuperSOM::alphaDecay
(
    const Real& alpha_ini,
    const DecayType& decay_type,
    const Uint& total_steps,
    const Uint& step
) const
{
  Real alpha = 0.0;
  switch (decay_type)
  {
  case linear_decay:
    alpha = alpha_ini * (1.0 - (step / (Real) total_steps));
    break;
  case inverse_decay: default:
    Real b = ((total_steps - 1.0) / (100.0 - 1.0));
    Real a = alpha_ini * b;
    alpha = a / (b + step);
    break;
  }
  return alpha;
} // method alphaDecay

/**
 * Method clearNeighborsMap
 *
 * Deletes all elements in the neighors map.
 */
void SuperSOM::clearNeighborsMap()
{
  // delete all list pointers
  for (size_t i = 0; i < m_neighbors_map.size(); ++i)
    for (size_t j = 0; j < m_neighbors_map[i].size(); ++j)
      for (size_t k = 0; k < m_neighbors_map[i][j].size(); ++k)
      {
        delete m_neighbors_map[i][j][k];
        m_neighbors_map[i][j][k] = NULL;
      }

  m_neighbors_map.clear();

  return;
} // method clearNeighborsMap

/**
 * Method clearObject
 *
 * Clears this object as just created (setting members to their initial values).
 */
void SuperSOM::clearObject()
{
  // Parameters
  m_nrows = 0;
  m_ncols = 0;
  m_act_fun_gamma = 0.01;
  m_is_initialized = false;

  // Random number generator
  m_rand.setRandSeed();

  // SOM elements
  m_map.clear();

  // Training parameters
  m_nepochs_1 = 0;
  m_nepochs_2 = 0;
  m_nepochs_3 = 0;
  m_alpha_1 = 0.0;
  m_alpha_2 = 0.0;
  m_alpha_3 = 0.0;
  m_sigma_1 = 0.0;
  m_sigma_2 = 0.0;
  m_sigma_3 = 0.0;
  m_sigma_fin_1 = 0.0;
  m_sigma_fin_2 = 0.0;
  m_sigma_fin_3 = 0.0;
  m_alpha_decay_type = linear_decay;

  // Training support structures (should already be empty)
  clearNeighborsMap();
  m_classes_map.clear();
  m_winner_unit_vector.clear();

  return;
} // method clearObject

/**
 * Method distance
 *
 * Returns the euclidean distance between DATA and CODEBOOK.
 */
inline
SuperSOM::Real SuperSOM::distance
(
    const Data& data,
    const Codebook& codebook
) const
{
  return arma::norm(data - codebook, 2);
} // method distance

/**
 * Method hexTop
 *
 * Returns the distance in the hexagonal topology between two unit with
 * passed indeces.
 * The hexagonal topology is the following:
 *    (0,0) (1,0) (2,0) ...
 *       (0,1) (1,1) (2,1) ...
 *    (0,2) (1,2) (2,2) ...
 *       (0,3) (1,3) (2,3) ...
 *    (0,4) (1,4) (2,4) ...
 *    ... ...
 * that is in cartesian coordinates (x and y), and the second row offset to
 * right. The passed indeces are indeces of unit in the matrix representation,
 * then are flipped respect to above coordinates. This method for first
 * converts such indeces to cartesian coordinates.
 * The returned distance is the minimum number of unit to be traversed in order
 * to go from the first to the second.
 */
inline
SuperSOM::Uint SuperSOM::hexTop
(
    size_t index_1_1,
    size_t index_1_2,
    size_t index_2_1,
    size_t index_2_2
) const
{
  // Change matrix indeces to cartesian coordinates
  Int coord_1_x = index_1_2;
  Int coord_1_y = index_1_1;
  Int coord_2_x = index_2_2;
  Int coord_2_y = index_2_1;

  // Compute the distance
  Uint dist = 0;
  Int dy = std::abs(coord_1_y - coord_2_y);

  Int dx = 0;
  if ((coord_1_y % 2 == 0) && (coord_2_x < coord_1_x) && (dy % 2 == 1))
    dx = std::abs(coord_2_x - coord_1_x + 1);
  else if ((coord_1_y % 2 == 1) && (coord_2_x > coord_1_x) && (dy % 2 == 1))
    dx = std::abs(coord_2_x - coord_1_x - 1);
  else
    dx = std::abs(coord_1_x - coord_2_x);

  if ((double)dx < (dy / 2.0))
    dist = dy;
  else
    dist = dy + dx - std::floor(dy / 2.0);

  return dist;
} // method hexTop

/**
 * Method initNeighborsMap
 *
 * Builds the neighbors map used to speed up the training process. If you have
 * to use very large SOM you must remove the use of this support structure to
 * make use of less memory.
 * The neighbors map keeps for each unit a list of its neighbors divided by
 * distance from the unit itself. In this way the updating process will go
 * through the neighbors from nearest to farthest and stop when the update does
 * not affect most.
 */
void SuperSOM::initNeighborsMap()
{
  clearNeighborsMap();

  m_neighbors_map.resize(m_nrows);
  for (size_t i = 0; i < m_nrows; ++i)
    m_neighbors_map[i].resize(m_ncols);

  size_t half_max = (std::max(m_nrows, m_ncols) / 2) + 1;
  for (size_t r = 0; r < m_nrows; ++r)
    for (size_t c = 0; c < m_ncols; ++c)
      m_neighbors_map[r][c].reserve(half_max);

  Uint dist = 0;
  // for each unit (u_r,u_c)
  for (size_t u_r = 0; u_r < m_nrows; ++u_r)
    for (size_t u_c = 0; u_c < m_ncols; ++u_c)
    {
      // for each neighbor
      UnitIndex neigh;
      for (neigh.first = 0; neigh.first < m_nrows; ++neigh.first)
        for (neigh.second = 0; neigh.second < m_ncols; ++neigh.second)
        {
          dist = hexTop(u_r, u_c, neigh.first, neigh.second);

          if (m_neighbors_map[u_r][u_c].size() <= dist)
            m_neighbors_map[u_r][u_c].resize(dist + 1, NULL);

          if (m_neighbors_map[u_r][u_c][dist] == NULL)
            m_neighbors_map[u_r][u_c][dist] = new std::list<UnitIndex>();

          m_neighbors_map[u_r][u_c][dist]->push_back(neigh);

        } // for (neigh.first, neigh.second)
    } // for (u_r,u_c)

  return;
} // method initNeighborsMap

/**
 * Method lvq1ExtUpdate
 *
 * Updates the codebook for the passed winner unit using the LVQ1 supervised
 * procedure extended for the SOM working (by updating also map neighbors).
 */
inline
void SuperSOM::lvq1ExtUpdate
(
    const Data& data,
    const Real& data_class,
    const UnitIndex& win_unit,
    const Real& alpha,
    const Real& sigma
)
{
  // Get the class for the winner unit
  Real win_unit_class = unitClass(win_unit, data_class);

  // Update codebooks
  if (win_unit_class == data_class)
    updateCodebook(win_unit, data, alpha, sigma, true);
  else
    updateCodebook(win_unit, data, alpha, sigma, false);

  return;
} // method lvq1ExtUpdate

/**
 * Method lvq3ExtUpdate
 *
 * Updates the codebook for the passed winner unit using the LVQ3 supervised
 * procedure extended for the SOM working (by updating also map neighbors).
 */
inline
void SuperSOM::lvq3ExtUpdate
(
    const Data& data,
    const Real& data_class,
    const UnitIndex& closest_unit_1,
    const UnitIndex& closest_unit_2,
    const Real& window,
    const Real& epsilon,
    const Real& alpha,
    const Real& sigma
)
{
  // Get the classes for the two winner units
  Real closest_unit_1_class = unitClass(closest_unit_1, data_class);
  Real closest_unit_2_class = unitClass(closest_unit_2, data_class);

  // Update codebooks
  if (closest_unit_1_class != closest_unit_2_class)
  {
    // check if data fall in the window (there must not be a large
    // disproportion between distances from data and the two winning units)
    Real dist1 = arma::norm(data - getCodebook(closest_unit_1), 2);
    Real dist2 = arma::norm(data - getCodebook(closest_unit_2), 2);

    Real window_threshold = (1 - window) / (1 + window);
    if (std::min(dist1 / dist2, dist2 / dist2) > window_threshold)
    {
      if (closest_unit_1_class == data_class &&
          closest_unit_2_class != data_class)
      {
        updateCodebook(closest_unit_1, data, alpha, sigma, true);
        updateCodebook(closest_unit_2, data, alpha, sigma, false);
      }
      else if (closest_unit_1_class != data_class &&
               closest_unit_2_class == data_class)
      {
        updateCodebook(closest_unit_2, data, alpha, sigma, true);
        updateCodebook(closest_unit_1, data, alpha, sigma, false);
      }
      else
        Log::err << "SuperSOM::lvq3Extended: Warning: something wrong in "
                 << "LVQ 3 updating procedure." << Log::endl;
    } // if-else

  } // if (closest_unit_1_class != closest_unit_2_class)

  else if (closest_unit_1_class == data_class) // all have the same class
  {
    updateCodebook(closest_unit_1, data, alpha, sigma, true, epsilon);
    updateCodebook(closest_unit_2, data, alpha, sigma, true, epsilon);
  }

  return;
} // method lvq3ExtUpdate

/**
 * Method setInitialized
 *
 * By setting to false this parameter you can invalidate this object that
 * must be re-initialized.
 */
void SuperSOM::setInitialized(bool initialized)
{
  m_is_initialized = initialized;
  return;
} // method setInitialized

/**
 * Method sigmaDecay
 *
 * Returns dacayed sigma value.
 */
inline
SuperSOM::Real SuperSOM::sigmaDecay
(
    const Real& sigma_ini,
    const Real& sigma_fin,
    const Uint& total_steps,
    const Uint& step
) const
{
  return sigma_ini + step * ((sigma_fin - sigma_ini) / (total_steps - 1.0));
} // method sigmaDecay

/**
 * Method superTrainProcedure
 *
 * Trains the object m_map using passed parameters in a supervised way,
 * using the LVQ 1 SOM extended algorithm.
 * Note that the object m_neighbors_map must be already builded (used in the
 * updating procedure).
 */
void SuperSOM::superTrainProcedure
(
    const DataContainer& tr_data,
    const std::vector<Real>& tr_data_class,
    const Uint& epochs,
    const Real& alpha_ini,
    const DecayType& alpha_decay,
    const Real& sigma_ini,
    const Real& sigma_fin
)
{
  size_t tr_data_size = tr_data.size();
  Uint total_steps = epochs * tr_data_size;
  Real alpha = alpha_ini;
  Real sigma = sigma_ini;

  // Init data structures
  m_winner_unit_vector.resize(tr_data_size);
  m_classes_map.resize(m_nrows);
  for (size_t i = 0; i < m_nrows; ++i)
    m_classes_map[i].resize(m_ncols);

  // Compute initial classes for each unit and winner unit for each input
  for (size_t i = 0; i < tr_data_size; ++i)
  {
    UnitIndex win_unit = winnerUnit(tr_data[i]);
    m_winner_unit_vector[i] = win_unit;

    std::map<Real, Int>& win_unit_classes =
        m_classes_map[win_unit.first][win_unit.second];

    if (win_unit_classes.count(tr_data_class[i]) == 0)
      win_unit_classes[tr_data_class[i]] = 1;
    else
      win_unit_classes[tr_data_class[i]] += 1;

  } // for i

  // Build access vector
  std::vector<size_t> av(tr_data_size);
  for (size_t i = 0; i < tr_data_size; ++i)
    av[i] = i;

  // Start training
  Uint step = 0;
  for (Uint ep = 0; ep < epochs; ++ep)
  {
    // Random shuffle access vector
    for (size_t i = av.size(); i != 0; --i)
      std::swap(av[i - 1], av[m_rand.getRandInt(0, i - 1)]);

    for (size_t i = 0; i < tr_data_size; ++i, ++step)
    {
      // LVQ1
      UnitIndex winner_unit = winnerUnit(tr_data[av[i]]);
      lvq1ExtUpdate
          (
            tr_data[av[i]],
            tr_data_class[av[i]],
            winner_unit,
            alpha,
            sigma
          );
      updateUnitClass(winner_unit, tr_data_class[av[i]], av[i]);

      //// LVQ3
      //std::vector<UnitIndex> closest_units;
      //winnerUnits(tr_data[av[i]], 2, closest_units);
      //lvq3ExtUpdate
      //    (
      //      tr_data[av[i]],
      //      tr_data_class[av[i]],
      //      closest_units[0],
      //      closest_units[1],
      //      0.2, // window
      //      0.1, // epsilon
      //      alpha,
      //      sigma
      //    );
      //updateUnitClass(closest_units[0], tr_data_class[av[i]], av[i]);

      // Alpha and sigma decay
      alpha = alphaDecay(alpha_ini, alpha_decay, total_steps, step);
      sigma = sigmaDecay(sigma_ini, sigma_fin, total_steps, step);

    } // for i

  } // for ep

  // Clear data structures
  m_classes_map.clear();
  m_winner_unit_vector.clear();

  return;
} // method superTrainProcedure

/**
 * Method unitClass
 *
 * Looking in the data structure m_classes_map computes and returns the winner
 * class (most frequent class) for the passed unit. If the unit have no classes
 * the value DEFAULT_CLASS is returned.
 */
inline
SuperSOM::Real SuperSOM::unitClass
(
    const UnitIndex& unit,
    const Real& default_class
) const
{
  Real unit_class = 0.0;
  Int class_max_count = 0;

  std::map<Real, Int>::const_iterator cl_it =
      m_classes_map[unit.first][unit.second].begin();
  std::map<Real, Int>::const_iterator cl_it_end =
      m_classes_map[unit.first][unit.second].end();

  for (/* nop */; cl_it != cl_it_end; ++cl_it)
  {
    // a check to be sure there are no bugs
    if (cl_it->second < 0)
      Log::err << "SuperSOM::unitClass: Fatal error: negative class count "
               << "(there is a bug here!)." << Log::endl;

    if (cl_it->second > class_max_count)
    {
      unit_class = cl_it->first;
      class_max_count = cl_it->second;
    }

  } // for cl_it

  // There was no classes in the unit (unknown winner class)
  if (class_max_count == 0)
    unit_class = default_class;

  return unit_class;
} // method unitClass

/**
 * Method unsuperTrainProcedure
 *
 * Trains the object m_map using passed parameters.
 * Note that the object m_neighbors_map must be already builded (used in the
 * updating procedure).
 */
inline
void SuperSOM::unsuperTrainProcedure
(
    const DataContainer& tr_data,
    const Uint& epochs,
    const Real& alpha_ini,
    const DecayType& alpha_decay,
    const Real& sigma_ini,
    const Real& sigma_fin
)
{
  size_t tr_size = tr_data.size();
  Uint total_steps = epochs * tr_size;
  Real alpha = alpha_ini;
  Real sigma = sigma_ini;

  // Build access vector
  std::vector<size_t> av(tr_size);
  for (size_t i = 0; i < tr_size; ++i)
    av[i] = i;

  // Start training
  Uint step = 0;
  for (Uint ep = 0; ep < epochs; ++ep)
  {
    // Random shuffle access vector
    for (size_t i = av.size(); i != 0; --i)
      std::swap(av[i - 1], av[m_rand.getRandInt(0, i - 1)]);

    for (size_t i = 0; i < tr_size; ++i, ++step)
    {
      // Get the winner unit
      UnitIndex win_unit = winnerUnit(tr_data[av[i]]);

      // Update codebook
      updateCodebook(win_unit, tr_data[av[i]], alpha, sigma);

      // Alpha and sigma decay
      alpha = alphaDecay(alpha_ini, alpha_decay, total_steps, step);
      sigma = sigmaDecay(sigma_ini, sigma_fin, total_steps, step);

    } // for i

  } // for ep

  return;
} // method unsuperTrainProcedure

/**
 * Method updateCodebook
 *
 * Updates the codebooks (weights) in the map by the following learning rule:
 *
 *   m_{i}(t + 1) = m_{i}(t) + h_{c,i} * (x(t) - m_{i}(t))
 *
 * where
 *   - m_{i}(t + 1) is the new (updated) codebook
 *   - m_{i}(t) is the current codebook to be updated
 *   - h_{c,i}(t) is the neighborhood kernel (c is the winner unit)
 *   - x(t) is the current data
 *
 * the neighborhood kernel h_{c,i} is defined as follow:
 *
 *   h_{c,i} =
 *     alpha * g_{c,i}  if g_{c,i} >= 1e-6
 *     0                otherwise
 *
 * where
 *   - alpha is the learning rate
 *   - sigma is the radius of the neighborhood function
 *   - g_{c,i} = exp(-0.5 * (top(c,i) / sigma)^2) is the gauss function
 *   - top(c,i) is the distance between the current (updating) unit and the
 *       winner unit in the map topology (hexagonal)
 *
 * To speed up the updating this method use the structure m_neighbors_map (that
 * must be already builded before the invokation of this method).
 */
inline
void SuperSOM::updateCodebook
(
    const UnitIndex& winner_unit,
    const Data& data,
    const Real& alpha,
    const Real& sigma,
    bool positive_updating,
    const Real& epsilon
)
{
  size_t neighborhood_size =
      m_neighbors_map[winner_unit.first][winner_unit.second].size();

  for (size_t neigh_dist = 0; neigh_dist < neighborhood_size; ++neigh_dist)
  {
    Real gauss_fun = std::exp(-0.5 * std::pow(neigh_dist / sigma, 2));
    if (gauss_fun < 1e-6)
      break;

    std::list<UnitIndex>::const_iterator it =
        m_neighbors_map[winner_unit.first][winner_unit.second][neigh_dist]->
        begin();
    std::list<UnitIndex>::const_iterator it_end =
        m_neighbors_map[winner_unit.first][winner_unit.second][neigh_dist]->
        end();

    // neighbors updating
    for (/* nop */; it != it_end; ++it)
    {
      if (positive_updating)
        m_map[it->first][it->second] =
            m_map[it->first][it->second] +
            epsilon * alpha * gauss_fun *
            (data - m_map[it->first][it->second]);
      else
        m_map[it->first][it->second] =
            m_map[it->first][it->second] -
            epsilon * alpha * gauss_fun *
            (data - m_map[it->first][it->second]);
    } // for it

  } // for neigh_dist

  //  // Classic updating
  //  size_t rows = m_map.size();
  //  size_t cols = m_map[0].size();
  //
  //  for (size_t r = 0; r < rows; ++r)
  //    for (size_t c = 0; c < cols; ++c)
  //      m_map[r][c] =
  //          m_map[r][c] +
  //          alpha *
  //          std::exp
  //          (
  //            -0.5 *
  //            std::pow
  //            (
  //              hexTop(winner_unit.first, winner_unit.second, r, c) / sigma,
  //              2
  //            )
  //          ) *
  //          (data - m_map[r][c]);

  return;
} // method updateCodebook

/**
 * Method updateUnitClass
 *
 * If the input data has been mapped in a different unit then unit's classes
 * must be updated. This method update the unit's classes in the data
 * structure m_classes_map and also the winner unit in m_winner_unit_vector.
 */
inline
void SuperSOM::updateUnitClass
(
    const UnitIndex& data_win_unit,
    const Real& data_class,
    const size_t& data_index
)
{
  const UnitIndex& old_win_unit = m_winner_unit_vector[data_index];

  if ((data_win_unit.first != old_win_unit.first) ||
      (data_win_unit.second != old_win_unit.second))
  {
    std::map<Real, Int>& win_unit_classes =
        m_classes_map[data_win_unit.first][data_win_unit.second];
    std::map<Real, Int>& old_win_unit_classes =
        m_classes_map[old_win_unit.first][old_win_unit.second];

    old_win_unit_classes[data_class] -= 1;

    if (win_unit_classes.count(data_class) == 0)
      win_unit_classes[data_class] = 1;
    else
      win_unit_classes[data_class] += 1;

    m_winner_unit_vector[data_index] = data_win_unit;
  }

  return;
} // method updateUnitClass

} // namespace ml
} // namespace moka

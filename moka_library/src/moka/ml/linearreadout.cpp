#include "linearreadout.h"

#include <boost/algorithm/string.hpp>

namespace moka {
namespace ml {

namespace mut = ::moka::util;

/**
 * Method compute
 *
 * Computes the output y given a state vector x as follows:
 *   y = W_out * x
 *
 * The output y will be returned by this method and will be also futher
 * available using the method getLastOutput.
 *
 * The object must be initialized before you can use this method (see method
 * init).
 *
 * A moka::GenericException exception will be thrown if the state doesn't
 * match the setted state size, or if the object is not initialized.
 */
const LinearReadout::Vector& LinearReadout::compute(const Vector& state)
{
  if (!isInitialized())
    throw moka::GenericException(
        "LinearReadout::compute: the object is not initialized");

  if (state.n_rows != m_state_size)
    throw moka::GenericException(
        "LinearReadout::compute: wrong state vector size");

  try
  {
    m_output_vector = m_W_out * state;
  } // try
  catch (std::exception& ex)
  {
    throw moka::GenericException(
        std::string("LinearReadout::compute: error: ") + ex.what());
  } // try-catch

  return m_output_vector;
} // method compute

/**
 * Method init
 *
 * Initializes the readout matrix (with all elements to 0 since you do the
 * training).
 * The following parameters must be properly setted before to call this method
 * and you must re-call this method if any of this is changed:
 *   - output size
 *   - state vector size
 *
 * Note that the state vector must have an element fixed to +1 if you want to
 * add the bias element, then the state vector size must have one more element.
 */
void LinearReadout::init()
{
  m_W_out.zeros(m_output_size, m_state_size);
  setInitialized(true);
  return;
} // method init

/**
 * Method read
 *
 * Read this object previously written using the method write.
 *
 * In case of errors an exception of type moka::GenericException will be
 * thrown and this object is left "clear" as just created.
 */
void LinearReadout::read(std::istream& is)
{
  clearObject();

  try
  {
    std::string line;

    mut::Math::read(is, m_W_out);
    mut::Math::read(is, m_output_vector);

    Global::readLine(is, m_output_size);
    Global::readLine(is, m_state_size);

    Global::readLine(is, line);
    m_regularization_method = strToRegm(line);

    Global::readLine(is, m_rr_lambda);
    Global::readLine(is, m_lasso_lambda);
    Global::readLine(is, m_en_lambda1);
    Global::readLine(is, m_en_lambda2);

    Global::readLine(is, line);
    m_is_initialized = Global::toBool(line);

  } // try
  catch (std::exception& ex)
  {
    clearObject();
    throw moka::GenericException(
        std::string("LinearReadout::read: fail reading the object: ") +
        ex.what());
  } // catch

  return;
} // method read

/**
 * Method regmToStr
 *
 * Converts a RegularizationMethod value into a std::string. If the
 * RegularizationMethod value is not recognized throws an exception of type
 * moka::GenericException.
 */
std::string LinearReadout::regmToStr(const RegularizationMethod& regm)
{
  switch (regm)
  {
  case no_regularization:
    return "none";
    break;
  case ridge_regression:
    return "ridge-regression";
    break;
  case lasso:
    return "lasso";
    break;
  case elastic_net:
    return "elastic-net";
    break;
  default:
    throw moka::GenericException(
        "LinearReadout::regmToStr: invalid RegularizationMethod value");
  }
  return "";
} // method regmToStr

/**
 * Method strToRegm
 *
 * Converts a an std::string into a RegularizationMethod value. If the string
 * is not recognize throw an exception of type moka::GenericException.
 */
LinearReadout::RegularizationMethod LinearReadout::strToRegm
(
    const std::string& str
)
{
  if (str == "none")
    return no_regularization;
  else if (str == "ridge-regression")
    return ridge_regression;
  else if (str == "lasso")
    return lasso;
  else if (str == "elastic-net")
    return elastic_net;
  else
    throw moka::GenericException(
        "LinearReadout::strToRegm: Invalid string representation of a "
        "RegularizationMethod value");
  // return 0;
} // method strToRegm

/**
 * Method train
 *
 * Trains the readout matrix W_out by solving a linear regression problem:
 *
 *   Y = W_out * X + E
 *
 * To solve it use the regularization method setted. In case of errors (e.g.
 * the two passed matrix have wrong sizes) will be throw an exception.
 *
 * The object must be initialized before you can use this method (see method
 * init), otherwise an exception will be throw.
 */
void LinearReadout::train(const Matrix& X, const Matrix& Y)
{
  if (!isInitialized())
    throw moka::GenericException(
        "LinearReadout::train: the object is not initialized");

  if (X.n_cols != Y.n_cols)
    throw moka::GenericException(
        "LinearReadout::train: X and Y should have same number of columns");

  if (Y.n_rows != m_W_out.n_rows)
    throw moka::GenericException(
        "LinearReadout::train: Y and W_out should have same number of rows");

  if (X.n_rows != m_W_out.n_cols)
    throw moka::GenericException(
        "LinearReadout::train: X and W_out should have same number of columns");

  try
  {
    switch (m_regularization_method)
    {
    case ridge_regression:
      mut::Math::solveTpRidgeRegression(m_W_out, X, Y, m_rr_lambda);
      break;
    case lasso:
      mut::Math::solveTpLasso(m_W_out, X, Y, m_lasso_lambda);
      break;
    case elastic_net:
      mut::Math::solveTpElasticNet(m_W_out, X, Y, m_en_lambda1, m_en_lambda2);
      break;
    case no_regularization: default:
      mut::Math::solveTpLinearRegression(m_W_out, X, Y);
      break;
    } // switch
  }
  catch (std::exception& ex)
  {
    throw moka::GenericException(
        std::string("LinearReadout::train: error during the training ") +
        "procedure (" + ex.what() + ")");
  } // try-catch

  return;
} // method train

/**
 * Method write
 *
 * Writes this object on the output stream (probably a file stream). You
 * can then reload it using the method read.
 */
void LinearReadout::write(std::ostream& os) const
{
  mut::Math::write(os, m_W_out);
  mut::Math::write(os, m_output_vector);
  os << m_output_size << std::endl;
  os << m_state_size << std::endl;
  os << regmToStr(m_regularization_method) << std::endl;
  os << m_rr_lambda << std::endl;
  os << m_lasso_lambda << std::endl;
  os << m_en_lambda1 << std::endl;
  os << m_en_lambda2 << std::endl;
  os << Global::toString(m_is_initialized) << std::endl;
  return;
} // method write

// ===============
// PRIVATE METHODS
// ===============

/**
 * Method clearObject
 *
 * Clears this object as just created (setting members to their initial values).
 */
void LinearReadout::clearObject()
{
  m_W_out.clear();
  m_output_vector.clear();
  m_output_size = 0;
  m_state_size = 0;
  m_regularization_method = no_regularization;
  m_rr_lambda = 0.0;
  m_lasso_lambda = 0.0;
  m_en_lambda1 = 0.0;
  m_en_lambda2 = 0.0;
  m_is_initialized = false;
  return;
} // method clearObject

/**
 * Method setInitialized
 *
 * By setting to false this parameter you can invalidate this object that
 * must be re-initialized (using method init).
 */
void LinearReadout::setInitialized(bool initialized)
{
  m_is_initialized = initialized;
  return;
} // method setInitialized


} // namespace ml
} // namespace moka

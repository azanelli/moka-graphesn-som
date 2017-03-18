#ifndef MOKA_ML_GRAPHRESERVOIR_IMPL_H
#define MOKA_ML_GRAPHRESERVOIR_IMPL_H

#include "graphreservoir.h"

#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <moka/log.h>

namespace moka {
namespace ml {

namespace bll = ::boost::lambda;
namespace mut = ::moka::util;

/**
 * Constructor
 */
template <typename T>
GraphReservoir<T>::GraphReservoir()
{
  clearObject();
}

/**
 * Destructor
 */
template <typename T>
GraphReservoir<T>::~GraphReservoir()
{ }

// ==============
// PUBLIC METHODS
// ==============

/**
 * Method encoding
 *
 * Given an input graph that must be consistent with the parameters setted in
 * this reservoir (e.g. the dimension of the verteces elements must be equal
 * to the setted input dimension), this method computes the encoding process
 * computing the state graph. You can access the computed state graph using
 * the method getLastStateGraph. This method returns true if the fixed point
 * is reached (within the threshold "epsilon" and using at most "max
 * iterations"), returns false otherwise.
 *
 * The passed graph must be also a consistent graph (see the method
 * "checkConsistence" of the Graph class) otherwise during the computation
 * there will be errors due to invalid accesses to vertices.
 *
 * The object must be initialized before you can use this method (see method
 * init).
 *
 * A moka::GenericException exception will be thrown if the input graph doesn't
 * match parameters of this reservoir, or if the object is not initialized.
 */
template <typename Graph>
bool GraphReservoir<Graph>::encoding(const Graph& input_graph)
{
  // Checks the initialization
  if (!isInitialized())
    throw moka::GenericException(
        "GraphReservoir::encoding: the object is not initialized");

  // Checks the input graph
  if (input_graph.getElementsSize() != m_N_u)
    throw moka::GenericException(
        "GraphReservoir::encoding: wrong elements size in the input graph");

  // Builds two states graphs with the same structure of the input graph and
  // elements of size Nr initialized as null vectors (vectors of all 0).
  Graph graph1, graph2;
  graph1.buildFrom(input_graph, arma::zeros(m_N_r));
  graph2.buildFrom(input_graph, arma::zeros(m_N_r));

  // Makes two pointers that will be swapped
  Graph *current = &graph1;
  Graph *previous = &graph2;

  // Starts the iterative encoding process
  m_iterations = 0;
  while (m_iterations < m_max_iterations)
  {
    // The current state graph becomes the state graph at the previous step.
    std::swap(current, previous);

    // Computes the states of the current state graph according to the state
    // transition function for each vertex n:
    //    x_{t}(n) = tanh(W_in * u(n) + Sum_{n'}(What * x_{t-1}(n'))
    // where n' are the neighbors of the vertex n
    for (size_t n = 0; n < current->getSize(); ++n)
    {
      Vector vsum(m_N_r);
      vsum.zeros();

      // Note: also if the graph degree is greater than k (i.e. m_max_degree)
      // the state is computed
      for (Uint i = 0; i < previous->getNeighborsSize(n); ++i)
        vsum += m_What * previous->getNeighbor(n, i);

      Vector vin = input_graph.getVertexElement(n);

      // Adds the fixed input for the bias
      vin.insert_rows(0, 1);
      vin[0] = 1;

      // Computes the n-th state
      current->setVertexElement(n, arma::tanh(m_W_in * vin + vsum));

    } // for n

    // Computes max_{n} || x_{t}(n) - x_{t-1}(n) || in order to check the
    // fixed point reaching
    Real max_n_norm = 0;
    for (Uint n = 0; n < current->getSize(); ++n)
    {
      Real n_norm =
          arma::norm(
            current->getVertexElement(n) - previous->getVertexElement(n), 2);
      if (n_norm > max_n_norm)
        max_n_norm = n_norm;
    } // for n

    // Iterates until the fixed point is reached (approssimated by the
    // threshold epsilon)
    m_iterations++;
    if (max_n_norm <= m_epsilon)
      break;

  } // while m_iterations

  bool return_value = true;
  if (m_iterations == m_max_iterations)
  {
    Log::vrb << "GraphReservoir::encoding: reached the maximum number of "
             << "iterations (the fixed point has not been reached)."
             << Log::endl;
    return_value = false;
  }

  // Copy result graph in the state graph (note that the pointers must not be
  // deleted)
  m_state_graph = (*current);

  return return_value;
} // method encoding

/**
 * Method encoding
 *
 * See encoding(const GraphType&).
 *
 * This method return a const reference on the computed state graph and put
 * true into the fail argument if the fixed point is reached, false otherwise.
 */
template <typename Graph>
const Graph& GraphReservoir<Graph>::encoding
(
    const Graph& input_graph,
    bool& fail
)
{
  fail = encoding(input_graph);
  return getLastStateGraph();
} // method encoding

/**
 * Method encodingAnyway
 *
 * See encoding(const GraphType&).
 *
 * Computes the encoding function ignoring if the fixed point is reached or not.
 * Returns a const reference to the computed state graph.
 */
template <typename Graph>
const Graph& GraphReservoir<Graph>::encodingAnyway(const Graph& input_graph)
{
  encoding(input_graph);
  return getLastStateGraph();
} // method encodingAnyway

/**
 * Method init
 *
 * Initializes the two matrix W_in and What.
 * The following parameters must be properly setted before to call this method
 * and you must re-call this method if any of this is changed:
 *   - input size
 *   - reservoir size
 *   - random seed
 *   - input scaling
 *   - sigma
 *   - max degree
 *
 */
template <typename T>
void GraphReservoir<T>::init()
{
  // Inits matrices
  m_W_in.set_size(m_N_r, m_N_u + 1);
  m_What.set_size(m_N_r, m_N_r);

  // Fills W_in
  std::generate
      (
        m_W_in.begin(),
        m_W_in.end(),
        bll::bind
            (
              &Global::UniformRandomGenerator::getRandReal,
              bll::var(m_rand),
              -m_input_scaling,
              m_input_scaling
            )
      );

  // Fills What
  typename Matrix::iterator what_it = m_What.begin();
  for (/* nop */; what_it != m_What.end(); ++what_it)
    *what_it =
        m_rand.getRandReal(0, 1) <= m_connectivity ?
          m_rand.getRandReal(-1.0, 1.0) : 0.0;

  // Rescales What in order to hold the contraction coefficient
  double what_norm2 = arma::norm(m_What, 2);
  if (what_norm2 == 0 || m_sigma == 0)
    m_What.zeros();
  else
    m_What = (m_sigma / (m_max_degree * what_norm2)) * m_What;

  // Makes this object usable
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
template <typename T>
void GraphReservoir<T>::read(std::istream& is)
{
  clearObject();

  try
  {
    std::string line;

    Global::readLine(is, line);
    m_is_initialized = Global::toBool(line);

    mut::Math::read(is, m_W_in);
    mut::Math::read(is, m_What);

    Global::readLine(is, m_N_u);
    Global::readLine(is, m_N_r);
    Global::readLine(is, m_epsilon);
    Global::readLine(is, m_sigma);
    Global::readLine(is, m_input_scaling);
    Global::readLine(is, m_connectivity);
    Global::readLine(is, m_max_degree);
    Global::readLine(is, m_max_iterations);

    m_rand.setRandSeed(Global::readLine<Uint>(is));

  } // try
  catch (std::exception& ex)
  {
    clearObject();
    throw moka::GenericException(
        std::string("GraphReservoir::read: fail reading the object: ") +
        ex.what());
  } // catch

  return;
} // method read

/**
 * Method write
 *
 * Writes this object on the output stream (probably a file stream). You
 * can then reload it using the method read.
 *
 * Note that the last state graph computed and the random number generator are
 * not written. This implies that if you have read this object (by the method
 * read) the method getLastStateGraph returns an empty graph until you call
 * the encoding method, and (most important) by calling the init method you can
 * get a different initialization respect to call such method before the
 * write/read process.
 */
template <typename T>
void GraphReservoir<T>::write(std::ostream& os) const
{
  os << Global::toString(m_is_initialized) << std::endl;

  mut::Math::write(os, m_W_in);
  mut::Math::write(os, m_What);

  os << m_N_u << std::endl;
  os << m_N_r << std::endl;
  os << m_epsilon << std::endl;
  os << m_sigma << std::endl;
  os << m_input_scaling << std::endl;
  os << m_connectivity << std::endl;
  os << m_max_degree << std::endl;
  os << m_max_iterations << std::endl;

  os << m_rand.getRandSeed() << std::endl;

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
template <typename T>
void GraphReservoir<T>::clearObject()
{
  m_is_initialized = false;
  m_W_in.clear();
  m_What.clear();
  m_N_u = 0;
  m_N_r = 0;
  m_epsilon = 0.01;
  m_sigma = 0.9;
  m_input_scaling = 0.1;
  m_connectivity = 1.0;
  m_max_degree = 0;
  m_max_iterations = 10000;
  m_iterations = 0;
  m_rand.setRandSeed();
  m_state_graph.clear();
  return;
} // method clearObject

/**
 * Method setInitialized
 *
 * By setting to false this parameter you can invalidate this object that
 * must be re-initialized.
 */
template <typename T>
void GraphReservoir<T>::setInitialized(bool initialized)
{
  m_is_initialized = initialized;
  return;
} // method setInitialized

} // namespace ml
} // namespace moka

#endif // MOKA_ML_GRAPHRESERVOIR_IMPL_H

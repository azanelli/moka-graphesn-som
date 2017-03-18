#ifndef MOKA_ML_GRAPHRESERVOIR_H
#define MOKA_ML_GRAPHRESERVOIR_H

#include <istream>
#include <ostream>
#include <moka/exception.h>
#include <moka/global.h>
#include <moka/structure/graph.h>
#include <moka/util/math.h>

namespace moka {
namespace ml {

/**
 * Class GraphReservoir
 *
 * Represent a reservoir for Graph Echo State Networks, as described in [1].
 *
 * The template type must be a graph derived or specilized from the structure
 * moka::structure::Graph.
 *
 * References:
 *   [1] C. Gallicchio, A. Micheli. Supervised State Mapping of Clustered
 *       GraphESN States.
 */
template <typename GraphTemplate>
class GraphReservoir
{
  public:
    typedef GraphTemplate GraphType;
    typedef ::moka::Global::Uint Uint;
    typedef ::moka::Global::Real Real;
    typedef ::moka::util::Math::Matrix Matrix;
    typedef ::moka::util::Math::Vector Vector;

    //! Default constructor
    GraphReservoir();

    //! Destructor
    ~GraphReservoir();

    //! Clear this object as just created
    void clear()
    {
      clearObject();
    }

    //! Encoding proccess using this reservoir
    bool encoding(const GraphType& input_graph);

    //! Encoding proccess using this reservoir
    const GraphType& encoding(const GraphType& input_graph, bool& fail);

    //! Encoding proccess using this reservoir (ignoring fails)
    const GraphType& encodingAnyway(const GraphType& input_graph);

    //! Epsilon (threshold for the encoding process)
    const Real& getEpsilon() const
    {
      return m_epsilon;
    }

    //! Input matrix W_in
    const Matrix& getInputMatrix() const
    {
      return m_W_in;
    }

    //! Input scaling
    const Real& getInputScaling() const
    {
      return m_input_scaling;
    }

    //! Input size (N_u)
    const Uint& getInputSize() const
    {
      return m_N_u;
    }

    //! State graph resulting from the last call of the method encoding
    const GraphType& getLastStateGraph() const
    {
      return m_state_graph;
    }

    //! Iterations computed to reach the fixed point during the last encoding
    Uint getLastNumberOfIterations() const
    {
      return m_iterations;
    }

    //! Max vertex degree on future input graphs (k)
    const Uint& getMaxDegree() const
    {
      return m_max_degree;
    }

    //! Number of iterations to compute before to stop in the encoding process
    const Uint& getMaxIterations() const
    {
      return m_max_iterations;
    }

    //! Reservoir connectivity
    const Real& getReservoirConnectivity() const
    {
      return m_connectivity;
    }

    //! Reservoir matrix W_hat
    const Matrix& getReservoirMatrix() const
    {
      return m_What;
    }

    //! Reservoir size (N_r)
    const Uint& getReservoirSize() const
    {
      return m_N_r;
    }

    //! Random seed
    Uint getRandomSeed() const
    {
      return m_rand.getRandSeed();
    }

    //! Sigma (for reservoir scaling)
    const Real& getSigma() const
    {
      return m_sigma;
    }

    //! Reservoir initialization (to call after setted reservoir parameters)
    void init();

    //! Is this class initialized and then usable?
    bool isInitialized() const
    {
      return m_is_initialized;
    }

    //! Read this object from input stream
    virtual void read(std::istream& is);

    //! Epsilon (threshold for the encoding process)
    void setEpsilon(const Real& epsilon)
    {
      m_epsilon = epsilon;
    }

    //! Input scaling
    void setInputScaling(const Real& scaling)
    {
      m_input_scaling = scaling;
      setInitialized(false);
    }

    //! Input size (N_u)
    void setInputSize(const Uint& size)
    {
      m_N_u = size;
      setInitialized(false);
    }

    //! Max vertex degree on future input graphs (k)
    void setMaxDegree(const Uint& max_degree)
    {
      m_max_degree = max_degree;
      setInitialized(false);
    }

    //! Number of iterations to compute before to stop in the encoding process
    void setMaxIterations(const Uint& max_iterations)
    {
      m_max_iterations = max_iterations;
    }

    //! Reservoir connectivity
    void setReservoirConnectivity(const Real& connectivity)
    {
      if (connectivity < 0.0 || connectivity > 1.0)
        throw std::out_of_range(
            "GraphReservoir::setReservoirConnectivity: the connectivity "
            "parameter must be between 0 and 1.");
      m_connectivity = connectivity;
      setInitialized(false);
    }

    //! Reservoir size (N_r)
    void setReservoirSize(const Uint& size)
    {
      m_N_r = size;
      setInitialized(false);
    }

    //! Random seed
    void setRandomSeed(const Uint& seed)
    {
      m_rand.setRandSeed(seed);
      setInitialized(false);
    }

    //! Sigma (for reservoir scaling)
    void setSigma(const Real& sigma)
    {
      m_sigma = sigma;
      setInitialized(false);
    }

    //! Write this object on output stream
    virtual void write(std::ostream& os) const;

  private:
    bool m_is_initialized;

    // Reservoir parameters
    Matrix m_W_in, m_What;
    Uint m_N_u, m_N_r;
    Real m_epsilon, m_sigma;
    Real m_input_scaling;
    Real m_connectivity;
    Uint m_max_degree;
    Uint m_max_iterations;
    Uint m_iterations;
    GraphType m_state_graph;

    // Random number generator
    Global::UniformRandomGenerator m_rand;

    // Private methods
    void clearObject();
    void setInitialized(bool initialized = false);

}; // class GraphReservoir

} // namespace ml
} // namespace moka

#include "graphreservoir_impl.h"

#endif // MOKA_ML_GRAPHRESERVOIR_H

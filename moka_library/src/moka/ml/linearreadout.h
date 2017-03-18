#ifndef MOKA_ML_LINEARREADOUT_H
#define MOKA_ML_LINEARREADOUT_H

#include <istream>
#include <ostream>
#include <moka/global.h>
#include <moka/util/math.h>

namespace moka {
namespace ml {

/**
 * Class LinearReadout
 *
 * Represent a readout of linear units trainable through linear regression.
 * This readout is used for example in [1] in the Graph ESN model.
 *
 * References:
 *   [1] C. Gallicchio, A. Micheli. Supervised State Mapping of Clustered
 *       GraphESN States.
 */
class LinearReadout
{
  public:
    typedef ::moka::Global::Uint Uint;
    typedef ::moka::Global::Real Real;
    typedef ::moka::util::Math::Matrix Matrix;
    typedef ::moka::util::Math::Vector Vector;

    enum RegularizationMethod
    {
      no_regularization,
      ridge_regression,
      lasso,
      elastic_net
    };

    //! Default constructor
    LinearReadout()
    {
      clearObject();
    }

    //! Destructor
    ~LinearReadout()
    { }

    //! Clear this object as just created
    void clear()
    {
      clearObject();
    }

    //! Given a state vector computes the outputs using this readout
    const Vector& compute(const Vector& state);

    //! Lambda1 parameter for the Elastic Net method
    const Real& getElasticNetLambda1() const
    {
      return m_en_lambda1;
    }

    //! Lambda2 parameter for the Elastic Net method
    const Real& getElasticNetLambda2() const
    {
      return m_en_lambda2;
    }

    //! Lambda parameter for the LASSO method
    const Real& getLassoLambda() const
    {
      return m_lasso_lambda;
    }

    //! Last output computed by last call of method compute
    const Vector& getLastOutput() const
    {
      return m_output_vector;
    }

    //! Number of readout units (i.e. the number of outputs, N_y)
    Uint getOutputSize() const
    {
      return m_output_size;
    }

    //! Readout matrix W_out
    const Matrix& getReadoutMatrix() const
    {
      return m_W_out;
    }

    //! Regularization method used in the train
    RegularizationMethod getRegularizationMethod() const
    {
      return m_regularization_method;
    }

    //! Lambda parameter for the Ridge Regression method
    const Real& getRidgeRegressionLambda() const
    {
      return m_rr_lambda;
    }

    //! State vector size (add a +1 fixed element in the state for the bias)
    Uint getStateVectorSize() const
    {
      return m_state_size;
    }

    //! Readout initialization (to call after setted output and state sizes)
    void init();

    //! Is this class initialized (and then usable?)
    bool isInitialized() const
    {
      return m_is_initialized;
    }

    //! Read this object from input stream
    virtual void read(std::istream& is);

    //! RegularizationMethod to std::string conversion
    static std::string regmToStr(const RegularizationMethod& regm);

    //! Lambda1 parameter for the Elastic Net method
    void setElasticNetLambda1(const Real& lambda1)
    {
      m_en_lambda1 = lambda1;
    }

    //! Lambda2 parameter for the Elastic Net method
    void setElasticNetLambda2(const Real& lambda2)
    {
      m_en_lambda2 = lambda2;
    }

    //! Lambda parameter for the LASSO method
    void setLassoLambda(const Real& lambda)
    {
      m_lasso_lambda = lambda;
    }

    //! Number of readout units (i.e. the number of outputs, N_y)
    void setOutputSize(const Uint& size)
    {
      m_output_size = size;
      setInitialized(false);
    }

    //! Regularization method used in the train
    void setRegularizationMethod(RegularizationMethod method)
    {
      m_regularization_method = method;
    }

    //! Lambda parameter for the Ridge Regression method
    void setRidgeRegressionLambda(const Real& lambda)
    {
      m_rr_lambda = lambda;
    }

    //! State vector size (add a +1 fixed element in the state for the bias)
    void setStateVectorSize(const Uint& size)
    {
      m_state_size = size;
      setInitialized(false);
    }

    //! std::string to RegularizationMethod conversion
    static RegularizationMethod strToRegm(const std::string& str);

    //! Readout training procedure
    void train(const Matrix& X, const Matrix& Y);

    //! Write this object on output stream
    virtual void write(std::ostream& os) const;

  private:
    Matrix m_W_out;
    Vector m_output_vector;
    Uint m_output_size, m_state_size;
    RegularizationMethod m_regularization_method;
    Real m_rr_lambda, m_lasso_lambda, m_en_lambda1, m_en_lambda2;
    bool m_is_initialized;

    // Private methods
    void clearObject();
    void setInitialized(bool initialized = false);

}; // class LinearReadout

} // namespace ml
} // namespace moka

#endif // MOKA_ML_LINEARREADOUT_H

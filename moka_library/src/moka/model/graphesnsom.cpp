#include "graphesnsom.h"

#include <algorithm>
#include <map>
#include <sstream>
#include <moka/util/timer.h>

namespace moka {
namespace model {

namespace bll = boost::lambda;

using namespace ml;
using namespace model;
using namespace dataset;
using namespace structure;
using namespace util;

typedef GraphEsnSom::Int Int;
typedef GraphEsnSom::Real Real;
typedef GraphEsnSom::Uint Uint;

/**
 * Constructor
 *
 * Builds an empty GraphEsnSom. An empty GraphEsnSom has no sense and must be
 * used temporarily, for example to subsequently invoke the method load or the
 * method init.
 */
GraphEsnSom::GraphEsnSom() :
  m_trainingset(NULL),
  m_graphesnsom_bak(NULL)
{
  clearObject();
}

/**
 * Copy constructor
 *
 */
GraphEsnSom::GraphEsnSom(const GraphEsnSom& graphesnsom) :
  Model(graphesnsom),
  m_trainingset(NULL),
  m_graphesnsom_bak(NULL)
{
  copyObject(graphesnsom);
}

/**
 * Destructor
 *
 */
GraphEsnSom::~GraphEsnSom()
{
  // m_trainingset: is a bind (not to be deleted)
  delete m_graphesnsom_bak;
  return;
}

// ==============
// PUBLIC METHODS
// ==============

/**
 * Method bindTrainingSet
 *
 * See Model::bindTrainingSet.
 *
 * The dataset must be of type MultiLabeldGraphDataset (or derived) otherwise
 * an exception of type moka::GenericException will be throw.
 */
void GraphEsnSom::bindTrainingSet(const Dataset *trainingset)
{
  const MultiLabeledGraphDataset *mlg_dataset =
      dynamic_cast<const MultiLabeledGraphDataset*>(trainingset);
  if (!mlg_dataset)
    throw moka::GenericException(
        "GraphEsnSom::setTrainingSet: bad cast on the training set");

  m_trainingset = mlg_dataset;

  // The object must be initialized
  setInitialized(false);

  return;
} // method bindTrainingSet

/**
 * Method compareOutput
 *
 * Compares the last output (see method "getLastOutput") with the passed output.
 * Returns true if the two ouptuts are equivalent (see the method
 * "compareOutputs"), false otherwise.
 *
 * A moka::GenericException exception will be thrown if the the model is not
 * trained (see methods init and train).
 */
bool GraphEsnSom::compareOutput(const Vector& output) const
{
  if (!isTrained())
    throw moka::GenericException(
        "GraphEsnSom::compareOutput: the model is not trained");

  const Vector &last_output = getLastOutput();
  if (last_output.n_rows != output.n_rows)
    return false;

  return compareOutputs(last_output.begin(), last_output.end(), output.begin());
} // method compareOutput

/**
 * Method comparePerformance
 *
 * See Model::comparePerformance.
 *
 * In this case the performance are measured by the accuracy: p1 > p2.
 */
bool GraphEsnSom::comparePerformance(const Real& p1, const Real& p2) const
{
  return p1 > p2;
} // method comparePerformance

/**
 * Method compute
 *
 * Given an input graph this method computes the model output (for the whole
 * graph).
 *
 * The input graph must be consistent with the parameters setted in this
 * GraphEsnSom (e.g. the dimension of the verteces elements must be equal
 * to the setted input dimension) and must be also a consistent graph (see the
 * method "checkConsistence" of the Graph class) otherwise during the
 * computation there will be errors due to invalid accesses to vertices.
 *
 * The model must be at least inizialized (see method init) before to call this
 * method (better if it is also trained).
 *
 * A moka::GenericException exception will be thrown if the input graph doesn't
 * match parameters of this GraphEsnSom or if the model is not trained.
 *
 * The method returns a reference to the output just computed.
 */
const GraphEsnSom::Vector& GraphEsnSom::compute(const MultiLabeledGraph& input)
{
  if (!isTrained())
    throw moka::GenericException(
        "GraphEsnSom::compute: the model is not trained");

  return outputProcess(stateMappingFunctionProcess(encodingProcess(input)));
} // method compute

/**
 * Method computeOn
 *
 * See Model::computeOn.
 */
void GraphEsnSom::computeOn(const Dataset& ds, std::ostream& os, bool compare)
{
  if (!isTrained())
    throw moka::GenericException(
        "GraphEsnSom::computeOn: the model is not trained");

  const MultiLabeledGraphDataset *mlg_dataset = NULL;

  try
  {
    mlg_dataset = &(dynamic_cast<const MultiLabeledGraphDataset&>(ds));
  }
  catch (std::bad_exception& ex)
  {
    throw moka::GenericException(
        "GraphEsnSom::computeOn: invalid dataset type");
  } // try-catch

  if (!checkDatasetCompatibility(*mlg_dataset))
    throw moka::GenericException(
        "GraphEsnSom::computeOn: the dataset don't match with the current "
        "model");

  // print out name of fields
  os << "id, target, predicted";
  if (compare)
    os << ", compare result";
  os << std::endl;

  for (size_t i = 0; i < mlg_dataset->getSize(); ++i)
  {
    // id
    os << mlg_dataset->getId(i);

    // target
    for (size_t j = 0; j < mlg_dataset->getOutput(i).size(); ++j)
      os << ", " << mlg_dataset->getOutput(i)[j];

    // predicted
    compute(mlg_dataset->at(i).getInput());
    for (size_t j = 0; j < getLastOutput().n_elem; ++j)
      os << ", " << getLastOutput()[j];

    // compare result
    if (compare)
    {
      if (mlg_dataset->getOutput(i).size() != getLastOutput().n_elem)
        throw moka::GenericException(
            "GraphEsnSom::computeOn: wrong output size.");
      if (compareOutput(mlg_dataset->getOutput(i).begin()))
        os << ", true";
      else
        os << ", false";
    }

    os << "\n";
  } // for i

  return;
} // method computeOn

/**
 * Method computeOnTestFold
 *
 * See Model::computeOnTestFold.
 */
void GraphEsnSom::computeOnTestFold(std::ostream& os, bool compare)
{
  if (!m_trainingset)
    throw moka::GenericException(
        "GraphEsnSom::computeOnTestFold: there isn't any training set");

  if (!isTrained())
    throw moka::GenericException(
        "GraphEsnSom::computeOnTestFold: the model is not trained");

  // print out name of fields
  os << "id, target, predicted";
  if (compare)
    os << ", compare result";
  os << std::endl;

  for (size_t i = 0; i < m_trainingset->getTestFoldSize(); ++i)
  {
    // id
    os << m_trainingset->tsAt(i).getId();

    // target
    for (size_t j = 0; j < m_trainingset->tsAt(i).getOutput().size(); ++j)
      os << ", " << m_trainingset->tsAt(i).getOutput()[j];

    // predicted
    compute(m_trainingset->tsAt(i).getInput());
    for (size_t j = 0; j < getLastOutput().n_elem; ++j)
      os << ", " << getLastOutput()[j];

    // compare result
    if (compare)
    {
      if (m_trainingset->getOutput(i).size() != getLastOutput().n_elem)
        throw moka::GenericException(
            "GraphEsnSom::computeOn: wrong output size.");
      if (compareOutput(m_trainingset->tsAt(i).getOutput().begin()))
        os << ", true";
      else
        os << ", false";
    }

    os << "\n";
  } // for i

  return;
} // method computeOnTestFold

/**
 * Method computeOnTrainingSet
 *
 * See Model::computeOnTrainingSet.
 */
void GraphEsnSom::computeOnTrainingSet(std::ostream& os, bool compare)
{
  if (!m_trainingset)
    throw moka::GenericException(
        "GraphEsnSom::computeOnTrainingSet: there isn't any training set");

  if (!isTrained())
    throw moka::GenericException(
        "GraphEsnSom::computeOnTrainingSet: the model is not trained");

  // print out name of fields
  os << "id, target, predicted";
  if (compare)
    os << ", compare result";
  os << std::endl;

  for (size_t i = 0; i < m_trainingset->getTrSetSize(); ++i)
  {
    // id
    os << m_trainingset->trAt(i).getId();

    // target
    for (size_t j = 0; j < m_trainingset->trAt(i).getOutput().size(); ++j)
      os << ", " << m_trainingset->trAt(i).getOutput()[j];

    // predicted
    compute(m_trainingset->trAt(i).getInput());
    for (size_t j = 0; j < getLastOutput().n_elem; ++j)
      os << ", " << getLastOutput()[j];

    // compare result
    if (compare)
    {
      if (m_trainingset->getOutput(i).size() != getLastOutput().n_elem)
        throw moka::GenericException(
            "GraphEsnSom::computeOn: wrong output size.");
      if (compareOutput(m_trainingset->trAt(i).getOutput().begin()))
        os << ", true";
      else
        os << ", false";
    }

    os << "\n";
  } // for i

  return;
} // method computeOnTrainingSet

/**
 * Method evaluateOn
 *
 */
void GraphEsnSom::evaluateOn
(
    const dataset::Dataset& ds,
    const std::string& eq_filename,
    const std::string& ii_filename
)
{
  if (!isTrained())
    throw moka::GenericException(
        "GraphEsnSom::evaluateOn: the model is not trained");

  const MultiLabeledGraphDataset *mlg_dataset = NULL;

  try
  {
    mlg_dataset = &(dynamic_cast<const MultiLabeledGraphDataset&>(ds));
  }
  catch (std::bad_exception& ex)
  {
    throw moka::GenericException(
        "GraphEsnSom::evaluateOn: invalid dataset type");
  } // try-catch

  if (mlg_dataset->isEmpty())
    throw moka::GenericException("GraphEsnSom::evaluateOn: empty dataset");

  if (mlg_dataset->getInput(0).getElementsSize() != getInputSize())
    throw moka::GenericException(
        "GraphEsnSom::evaluateOn: the dataset don't match with the current "
        "model");

  // Open files
  std::ofstream eq_ofs(eq_filename.c_str(), std::fstream::out);
  std::ofstream ii_ofs(ii_filename.c_str(), std::fstream::out);

  if (eq_ofs.fail())
  {
    Log::err << "GraphEsnSom::evaluateOn: fail open file " << eq_filename
             << ". :(" << Log::endl;
    return;
  }

  if (ii_ofs.fail())
  {
    Log::err << "GraphEsnSom::evaluateOn: fail open file " << ii_filename
             << ". :(" << Log::endl;
    return;
  }

  bool compare = false;
  if (mlg_dataset->getOutput(0).size() != 0)
    compare = true;

  // Print out name of fields on equation file
  eq_ofs << "id, equation, predicted";
  if (compare)
    eq_ofs << ", target, compare result";
  eq_ofs << std::endl;

  for (size_t i = 0; i < mlg_dataset->getSize(); ++i)
  {
    // Compute on i-th input
    compute(mlg_dataset->at(i).getInput());

    // Write on equation file
    eq_ofs << mlg_dataset->getId(i) << ", "; // id
    writeInstanceEquation(eq_ofs, m_reservoir.getLastStateGraph());
    eq_ofs << ", " << getLastOutput()[0]; // predicted value
    if (compare)
    {
      eq_ofs << ", " << mlg_dataset->getOutput(i)[0]; // target
      if (compareOutput(mlg_dataset->getOutput(i).begin()))
        eq_ofs << ", true"; // compare result (true)
      else
        eq_ofs << ", false"; // compare result (false)
    }
    eq_ofs << "\n";

    // Write on instances info file
    ii_ofs << mlg_dataset->getId(i) << "\n"; // id
    if (compare)
      ii_ofs << mlg_dataset->getOutput(i)[0] << "\n"; // target output
    else
      ii_ofs << "*" << "\n"; // no target output
    ii_ofs << m_readout.getReadoutMatrix().at(0, 0) << "\n"; // model bias

    if (!writeVerticesInfo(ii_ofs, m_reservoir.getLastStateGraph()))
    {
      Log::err << "GraphEsnSom::evaluateOn: fail writing instance "
               << mlg_dataset->getId(i) << Log::endl;
    }
    ii_ofs << getLastOutput()[0] << "\n"; // predicted output

  } // for i

  return;
} // method evaluateOn

/**
 * Method getInputSize
 *
 * Returns the number of units in the input layer (Nu).
 * The model must be initialized otherwise a null value (0) will be returned.
 */
Uint GraphEsnSom::getInputSize() const
{
  if (!isInitialized())
    return 0;
  return m_reservoir.getInputSize();
} // method getInputSize

/**
 * Method getLastOutput
 *
 * Returns a reference to the output vector computed by the last call of the
 * method "compute".
 *
 * WARNING: the method compute can be used internally by other methods (e.g.
 * train or test methods). If you want to be sure that this method returns
 * what you want take care to don't call non-const method from the last call
 * of the method compute.
 */
const GraphEsnSom::Vector& GraphEsnSom::getLastOutput() const
{
  return m_readout.getLastOutput();
} // method getLastOutput

/**
 * Method getLastState
 *
 * Returns a reference to the state vector computed by the last call of the
 * method "compute".
 *
 * This state is the result of the state mapping function applied to the
 * whole state graph (given by the method "getLastStateGraph") plus an
 * element fixed to +1 at the beginning of the vector that is the fixed state
 * for the bias in the output units.
 *
 * WARNING: the method compute can be used internally by other methods (e.g.
 * train or test methods). If you want to be sure that this method returns
 * what you want take care to don't call non-const method from the last call
 * of the method compute.
 */
const GraphEsnSom::Vector& GraphEsnSom::getLastState() const
{
  return m_state_vect;
} // method getLastState

/**
 * Method getLastStateGraph
 *
 * Returns a reference to the state graph computed by the last call of the
 * method "compute". Each element of the state graph has size Nr, as the
 * reservoir size (different from the dimension of the last state: see
 * "getLastState" method).
 *
 * WARNING: the method compute can be used internally by other methods (e.g.
 * train or test methods). If you want to be sure that this method returns
 * what you want take care to don't call non-const method from the last call
 * of the method compute.
 */
const MultiLabeledGraph& GraphEsnSom::getLastStateGraph() const
{
  return m_reservoir.getLastStateGraph();
} // method getLastStateGraph

/**
 * Method getModelEquation
 *
 */
std::string GraphEsnSom::getModelEquation() const
{
  if (!m_readout.isInitialized())
  {
    Log::out << "GraphEsnSom::getModelEquation: readout not initialized"
             << Log::endl;
    return "";
  }

  if (m_readout.getOutputSize() != 1)
  {
    Log::err << "GraphEsnSom::getModelEquation: too many rows in readout matrix"
             << Log::endl;
    return "";
  }

  Real bias = m_readout.getReadoutMatrix().at(0,0);

  std::stringstream ss;
  ss << "sign(";
  if (bias >= 0)
    ss << "+";
  ss << bias;

  Uint i = 1;
  for (Uint row = 0; row < m_som.getNoRows(); ++row)
    for (Uint col = 0; col < m_som.getNoColumns(); ++col, ++i)
    {
      Real weight = m_readout.getReadoutMatrix().at(0, i);
      if (weight != 0)
      {
        if (weight > 0)
          ss << " +" << weight << "*Hit(" << row << "," << col <<")";
        else
          ss << " " << weight << "*Hit(" << row << "," << col <<")";
      }
    }

  ss << ")";

  return ss.str();
} // method getModelEquation

/**
 * Method getOutputSize
 *
 * Returns the number of units in the readout (Ny).
 * The model must be initialized otherwise a null value (0) will be returned.
 */
Uint GraphEsnSom::getOutputSize() const
{
  if (!isInitialized())
    return 0;
  return m_readout.getOutputSize();
} // method getOutputSize

/**
 * Method getReadout
 *
 * Returns a const reference to the readout component.
 */
const LinearReadout& GraphEsnSom::getReadout() const
{
  return m_readout;
} // method getReadout

/**
 * Method getReservoir
 *
 * Returns a const reference to the reservoir component.
 */
const GraphEsnSom::Reservoir& GraphEsnSom::getReservoir() const
{
  return m_reservoir;
} // method getReservoir

/**
 * Method getReservoirSize
 *
 * Returns a the number of units in the reservoir (Nr).
 * The model must be initialized otherwise a null value (0) will be returned.
 */
Uint GraphEsnSom::getReservoirSize() const
{
  if (!isInitialized())
    return 0;
  return m_reservoir.getReservoirSize();
} // method getReservoirSize

/**
 * Method getSOM
 *
 * Returns a const reference to the SOM component.
 */
const SuperSOM& GraphEsnSom::getSOM() const
{
  return m_som;
} // method getSOM

/**
 * Method info
 *
 * See Model::info.
 */
util::Info<std::string> GraphEsnSom::info() const
{
  util::Info<std::string> inf;
  inf.setTitle("GraphEsnSom");
  inf.pushBack(
      "input_size",
      "Input size",
      Global::toString(getInputSize()));
  inf.pushBack(
      "output_size",
      "Output size",
      Global::toString(getOutputSize()));
  inf.pushBack(
      "reservoir_rseed",
      "Reservoir random seed",
      Global::toString(m_reservoir.getRandomSeed()));
  inf.pushBack(
      "reservoir_size",
      "Reservoir size",
      Global::toString(getReservoirSize()));
  inf.pushBack(
      "reservoir_input_scaling",
      "Reservoir input scaling",
      Global::toString(m_reservoir.getInputScaling()));
  inf.pushBack(
      "reservoir_connectivity",
      "Reservoir connectivity",
      Global::toString(m_reservoir.getReservoirConnectivity() * 100),
      "%");
  inf.pushBack(
      "reservoir_sigma",
      "Reservoir sigma",
      Global::toString(m_reservoir.getSigma()));
  inf.pushBack(
      "reservoir_epsilon",
      "Reservoir epsilon",
      Global::toString(m_reservoir.getEpsilon()));
  inf.pushBack(
      "reservoir_max_degree",
      "Reservoir max degree (k)",
      Global::toString(m_reservoir.getMaxDegree()));
  inf.pushBack(
      "input_matrix_fnorm",
      "W_in frobenius norm",
      Global::toString(arma::norm(m_reservoir.getInputMatrix(), "fro")));
  inf.pushBack(
      "reservoir_matrix_fnorm",
      "W_hat frobenius norm",
      Global::toString(arma::norm(m_reservoir.getReservoirMatrix(), "fro")));

  if (!m_som_load_file.empty())
  {
    inf.pushBack(
        "som_load_file",
        "SOM loaded from file",
        m_som_load_file);
  } // if

  inf.pushBack(
      "som_rseed",
      "SOM random seed",
      Global::toString(m_som.getRandomSeed()));
  inf.pushBack(
      "som_no_rows",
      "SOM rows",
      Global::toString(m_som.getNoRows()));
  inf.pushBack(
      "som_no_cols",
      "SOM columns",
      Global::toString(m_som.getNoColumns()));
  inf.pushBack(
      "som_no_units",
      "SOM units",
      Global::toString(m_som.getNoUnits()));

  if (m_som_load_file.empty())
  {
    inf.pushBack(
        "som_training_type",
        "SOM training type",
        m_som_training_type);
  } // if

  inf.pushBack(
      "som_nepochs",
      "SOM epochs",
      (
        Global::toString(m_som.getNoEpochs1()) + " (1th), " +
        Global::toString(m_som.getNoEpochs2()) + " (2nd), " +
        Global::toString(m_som.getNoEpochs3()) + " (3rd) "
      ));
  inf.pushBack(
      "som_alpha",
      "SOM alpha",
      (
        Global::toString(m_som.getAlpha1()) + " (1th), " +
        Global::toString(m_som.getAlpha2()) + " (2nd), " +
        Global::toString(m_som.getAlpha3()) + " (3rd) "
      ));
  inf.pushBack(
      "som_sigma_ini",
      "SOM sigma ini",
      (
        Global::toString(m_som.getSigma1()) + " (1th), " +
        Global::toString(m_som.getSigma2()) + " (2nd), " +
        Global::toString(m_som.getSigma3()) + " (3rd) "
      ));
  inf.pushBack(
      "som_sigma_fin",
      "SOM sigma fin",
      (
        Global::toString(m_som.getSigmaFinal1()) + " (1th), " +
        Global::toString(m_som.getSigmaFinal2()) + " (2nd), " +
        Global::toString(m_som.getSigmaFinal3()) + " (3rd) "
      ));

  inf.pushBack(
      "state_vector_type",
      "State vector type",
      stvToStr(m_state_vect_type));

  inf.pushBack(
      "training_regularization",
      "Training regularization",
      ml::LinearReadout::regmToStr(m_readout.getRegularizationMethod()));
  switch (m_readout.getRegularizationMethod())
  {
    case LinearReadout::no_regularization:
      /* nop */
      break;
    case LinearReadout::ridge_regression:
      inf.pushBack(
          "rr_lambda",
          "Lambda",
          Global::toString(m_readout.getRidgeRegressionLambda()));
      break;
    case LinearReadout::lasso:
      inf.pushBack(
          "lasso_lambda",
          "Lambda",
          Global::toString(m_readout.getLassoLambda()));
      break;
    case LinearReadout::elastic_net:
      inf.pushBack(
          "en_lambda1",
          "Lambda 1",
          Global::toString(m_readout.getElasticNetLambda1()));
      inf.pushBack(
          "en_lambda2",
          "Lambda 2",
          Global::toString(m_readout.getElasticNetLambda2()));
      break;
  } // switch

  return inf;
} // method info

/**
 * Method init
 *
 * See Model::init.
 *
 * In case of error an exception of type moka::GenericException will be thrown.
 */
void GraphEsnSom::init()
{
  // Clears this object (also set isInitialized and isTrained to false)
  clearObjectForInit();

  // Initial checks
  if (!m_trainingset || m_trainingset->isEmpty())
    throw moka::GenericException(
        "GraphEsnSom::init: is not possible to initialize the model since the "
        "training set is empty");

  if (!checkParameters())
    throw moka::GenericException(
        "GraphEsnSom::init: is not possible to initialize the model since the "
        "setted parameters are not valid");

  // Automatically deducing and overwrite some parameters
  getParameters().at("input-size") =
      Global::toString(m_trainingset->getInput(0).getElementsSize());
  getParameters().at("output-size") =
      Global::toString(m_trainingset->getOutput(0).size());
  getParameters().at("reservoir-max-degree") =
      Global::toString(m_trainingset->maxDegree());

  // Parse and sets parameters values
  parseParameters();

  try
  {
    // Reservoir init
    m_reservoir.init();

    // If required try to load the SOM from file
    if (!m_som_load_file.empty())
      m_som.loadFromFile(m_som_load_file);

    // Readout init (+ 1 in the vector size is for the bias)
    if (m_state_vect_type == reservoir_state_vect)
      m_readout.setStateVectorSize(
            m_som.getNoUnits() * m_reservoir.getReservoirSize() + 1);
    else
      m_readout.setStateVectorSize(m_som.getNoUnits() + 1);
    m_readout.init();
  }
  catch (std::exception& ex)
  {
    clearObjectForInit();
    throw moka::GenericException(
        std::string("GraphEsnSom::init: initialization fail: ") + ex.what());
  }

  // Now this object is trainable
  setInitialized(true);

  // Makes a backup copy (used in the reset method)
  makeBackupCopy();

  return;
} // method init

/**
 * Method operator=
 *
 * Assignment operator.
 */
GraphEsnSom& GraphEsnSom::operator=(const GraphEsnSom& graphesnsom)
{
  Model::operator=(graphesnsom);
  copyObject(graphesnsom);
  return (*this);
} // method operator=

/**
 * Method read
 *
 * See Model::read.
 */
void GraphEsnSom::read(std::istream& is)
{
  clearObject();

  try
  {
    std::string line;

    // The first line is the version number: ignored
    Global::readLine(is, line);

    // Reservoir
    m_reservoir.read(is);

    // SOM
    m_som.read(is);
    Global::readLine(is, m_som_training_type);

    // State vector type
    Global::readLine(is, line);
    m_state_vect_type = strToStv(line);

    // Readout
    m_readout.read(is);

  } // try
  catch (std::exception& ex)
  {
    clearObject();
    throw moka::GenericException(
        std::string("GraphEsnSom::read: fail reading the model: ") + ex.what());
  } // catch

  // Since only trained model can be written (see Model::write) set initialized
  // and trained to true
  setInitialized(true);
  setTrained(true);

  return;
} // method read

/**
 * Method reset
 *
 * See Model::reset.
 */
void GraphEsnSom::reset()
{
  if (!isInitialized())
    throw moka::GenericException(
        "GraphEsnSom::reset: the model is not initialized");

  if (!m_graphesnsom_bak)
    throw moka::GenericException(
        "GraphEsnSom::reset: there isn't a backup copy");

  // Store temporarely the backup pointer
  GraphEsnSom *graphesnsom_bak_pt = m_graphesnsom_bak;

  // Copy the backup on this object
  this->operator=(*m_graphesnsom_bak);

  // A check to be sure there are no bugs
  if (m_graphesnsom_bak != NULL)
    Log::err << "GraphEsnSom::init: error: the backup copy had a not null "
             << "pointer (this is a bug!)" << Log::endl;

  // Restore the backup pointer
  m_graphesnsom_bak = graphesnsom_bak_pt;

  return;
} // method reset

/**
 * Method test
 *
 * See Model::test.
 */
const GraphEsnSom::NumericResults& GraphEsnSom::test()
{
  // Clears and inits m_test_res and test performance
  initTestResultsContainer();
  setTestPerf(0.0);

  if (!isTrained())
    throw moka::GenericException(
        "GraphEsnSom::test: the model is not trained");

  if (!m_trainingset)
    throw moka::GenericException(
        "GraphEsnSom::test: there isn't any training set");

  if (m_trainingset->getTestFoldSize() == 0)
    throw moka::GenericException("GraphEsnSom::test: the test fold is empty");

  // Timer start
  Timer test_timer;
  test_timer.start();

  Real mse = 0.0;
  Real acc = 0.0;
  Matrix Yhat_ts(getOutputSize(), m_trainingset->getTestFoldSize());
  Matrix Y_ts(getOutputSize(), m_trainingset->getTestFoldSize());

  // Collects outputs
  for (Uint i = 0; i < m_trainingset->getTestFoldSize(); ++i)
  {
    compute(m_trainingset->tsAt(i).getInput());
    std::copy(
        getLastOutput().begin(),
        getLastOutput().end(),
        Yhat_ts.begin_col(i));
    std::copy(
        m_trainingset->tsAt(i).getOutput().begin(),
        m_trainingset->tsAt(i).getOutput().end(),
        Y_ts.begin_col(i));
  } // for i

  // Computes mean squared error (MSE) using the frobenius norm
  mse = std::pow(arma::norm(Yhat_ts - Y_ts, "fro"), 2.0) / Yhat_ts.n_cols;

  // Computes validation accuracy
  Uint acc_count = 0;
  for (size_t i = 0; i < Yhat_ts.n_cols; ++i)
  {
    bool output_comp =
        GraphEsnSom::compareOutputs(
          Yhat_ts.begin_col(i), Yhat_ts.end_col(i), Y_ts.begin_col(i));
    if (output_comp)
      acc_count++;
  } // for
  acc = ((Real)acc_count) / Yhat_ts.n_cols;
  test_timer.stop();

  // Fills results
  setTestPerf(acc);
  m_test_res["mse"].value = mse;
  m_test_res["acc"].value = acc * 100;
  m_test_res["cpu_usage"].value = test_timer.getCpuUsage();

#ifdef MOKA_TMP_CODE
  // TODO: remove these call to temporary methods ____vvvvvvvvvvvvvvvvvvvvv____
  static int tmp_count = 1;
  if (!m_instances_info_save_file.empty())
  {
    std::list<structure::MultiLabeledGraph> ts_state_graphs;
    for (Uint i = 0; i < m_trainingset->getTestFoldSize(); ++i)
    {
      if (!m_reservoir.encoding(m_trainingset->tsAt(i).getInput()))
      {
        Log::err << "GraphEsnSom::test: the encoding process is failed."
                 << Log::endl;
        break;
      }
      ts_state_graphs.push_back(m_reservoir.getLastStateGraph());
    } // for i

    saveInstancesInfoTsf
      (
        m_instances_info_save_file + "." + Global::toString(tmp_count++) + ".tsf",
        ts_state_graphs,
        *m_trainingset,
        Yhat_ts
      );
  } // if (!m_unit_info_save_file.empty())

  // Up to here ----^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^----
#endif // MOKA_TMP_CODE

  return m_test_res;
} // method test

/**
 * Method testOn
 *
 * See ModelMaker::testOn.
 */
const GraphEsnSom::NumericResults& GraphEsnSom::testOn(const Dataset& testset)
{
  // Clears and inits m_test_res and test performance
  initTestResultsContainer();
  setTestPerf(0.0);

  if (!isTrained())
    throw moka::GenericException(
        "GraphEsnSom::testOn: the model is not trained");

  const MultiLabeledGraphDataset *mlg_dataset = NULL;

  try
  {
    mlg_dataset = &(dynamic_cast<const MultiLabeledGraphDataset&>(testset));
  }
  catch (std::bad_exception& ex)
  {
    throw moka::GenericException("GraphEsnSom::testOn: invalid dataset type");
  } // try-catch

  if (!checkDatasetCompatibility(*mlg_dataset))
    throw moka::GenericException(
        "GraphEsnSom::testOn: the dataset don't match with the current model");

  // Timer start
  Timer test_timer;
  test_timer.start();

  // Some initializations
  Real mse = 0.0;
  Real acc = 0.0;
  Math::Matrix Yhat_ts(getOutputSize(), mlg_dataset->getSize());
  Math::Matrix Y_ts(getOutputSize(), mlg_dataset->getSize());

  // Collects the reservoir states
  for (Uint i = 0; i < mlg_dataset->getSize(); ++i)
  {
    compute(mlg_dataset->at(i).getInput());
    std::copy(
        getLastOutput().begin(),
        getLastOutput().end(),
        Yhat_ts.begin_col(i));
    std::copy(
        mlg_dataset->at(i).getOutput().begin(),
        mlg_dataset->at(i).getOutput().end(),
        Y_ts.begin_col(i));
  } // for i

  // Computes mean squared error (MSE) using the frobenius norm
  mse = std::pow(arma::norm(Yhat_ts - Y_ts, "fro"), 2.0) / Yhat_ts.n_cols;

  // Computes test accuracy
  Uint acc_count = 0;
  for (Uint i = 0; i < Yhat_ts.n_cols; ++i)
  {
    bool output_comp =
        GraphEsnSom::compareOutputs(
          Yhat_ts.begin_col(i), Yhat_ts.end_col(i), Y_ts.begin_col(i));
    if (output_comp)
      acc_count++;
  } // for
  acc = ((Real)acc_count) / Yhat_ts.n_cols;
  test_timer.stop();

  // Fills results
  setTestPerf(acc);
  m_test_res["mse"].value = mse;
  m_test_res["acc"].value = acc * 100;
  m_test_res["cpu_usage"].value = test_timer.getCpuUsage();

#ifdef MOKA_TMP_CODE
  // TODO: remove these call to temporary methods ____vvvvvvvvvvvvvvvvvvvvv____
  static int tmp_count = 1;
  if (!m_instances_info_save_file.empty())
  {
    std::list<structure::MultiLabeledGraph> ts_state_graphs;
    for (Uint i = 0; i < mlg_dataset->getSize(); ++i)
    {
      if (!m_reservoir.encoding(mlg_dataset->at(i).getInput()))
      {
        Log::err << "GraphEsnSom::testOn: the encoding process is failed."
                 << Log::endl;
        break;
      }
      ts_state_graphs.push_back(m_reservoir.getLastStateGraph());
    } // for i

    saveInstancesInfoAll
      (
        m_instances_info_save_file + "." + Global::toString(tmp_count++) + ".ts",
        ts_state_graphs,
        *mlg_dataset,
        Yhat_ts
      );
  } // if (!m_unit_info_save_file.empty())

  // Up to here ----^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^----
#endif // MOKA_TMP_CODE

  return m_test_res;
} // method testOn

/**
 * Method train
 *
 * See ModelMaker::train.
 */
const GraphEsnSom::NumericResults& GraphEsnSom::train()
{
  if (!isInitialized())
    throw moka::GenericException(
        "GraphEsnSom::train: error: the model is not initialized");

  if (!m_trainingset)
    throw moka::GenericException(
        "GraphEsnSom::train: error: you must set a training set before to "
        "call the train method");

  setTrained(false);

  // Clears and inits m_training_res
  initTrainingResultsContainer();

  // Training start
  Timer training_timer;
  training_timer.start();

  Real avg_iterations = 0.0;
  SuperSOM::DataContainer *states_container = NULL;
  std::vector<Real> *states_class = NULL;
  std::list<MultiLabeledGraph> *state_graphs_list = NULL;
  Math::Matrix X, Y;

  try
  {
    // Collects all the states computed by the GraphEsnSom
    // Log::vrb << "Collects states" << Log::endl;
    states_container = new SuperSOM::DataContainer();
    states_class = new std::vector<Real>();
    state_graphs_list = new std::list<MultiLabeledGraph>();
    collectReservoirStates
        (
          *m_trainingset,
          *states_container,
          *states_class,
          *state_graphs_list,
          avg_iterations
        );

    // Fills avg. tr. iterations result
    m_training_res["res_avg_iters"].value = avg_iterations;

    if (!m_som_data_save_file.empty())
      saveSomDataOnFile
          (
            m_som_data_save_file,
            *state_graphs_list,
            *m_trainingset
          );

    // SOM training
    Timer som_training_timer;
    som_training_timer.start();

    // Log::vrb << "SOM training" << Log::endl;

    if (m_som_load_file.empty())
    {
      m_som.init(*states_container);

      if (m_som_training_type == "unsupervised")
        m_som.unsupervisedTraining(*states_container);

      else if (m_som_training_type == "supervised")
        m_som.supervisedTraining(*states_container, *states_class);

      else
        Log::err << "GraphEsnSom::train: error: unrecognized SOM training type "
                 << m_som_training_type << " (the map is left untrained)."
                 << Log::endl;
    } // if
    else
    { /* the SOM has been loaded from file in the init method */ }

    // Remove no longer useful states_container and states_class
    delete states_container;
    delete states_class;
    states_container = NULL;
    states_class = NULL;

    // Gets cpu usage for the SOM training
    som_training_timer.stop();
    m_training_res["som_cpu_usage"].value = som_training_timer.getCpuUsage();

    // Fills X with states resulting from the state mapping function applied to
    // state graphs previously collected.

    // Log::vrb << "Inits and fills matrix X" << Log::endl;

    // X \in IR^{N_s x N_tr} where N_s is the state dimension
    X.resize(m_readout.getStateVectorSize(), m_trainingset->getTrSetSize());

    Uint xcol = 0;
    std::list<MultiLabeledGraph>::const_iterator sg_it =
        state_graphs_list->begin();
    for (/* nop */; sg_it != state_graphs_list->end(); ++sg_it, ++xcol)
    {
      stateMappingFunctionProcess(*sg_it);
      std::copy
          (
            getLastState().begin(),
            getLastState().end(),
            X.begin_col(xcol)
          );
    } // for it

    // The state graph list can be used later to print out instances weights,
    // you can delete here then recompute it later if memory space is an issue.
    //
    // // Remove no longer useful state_graphs_list
    // delete state_graphs_list;
    // state_graphs_list = NULL;

    // Fills matrix Y coping training output column-wise: Y \in IR^{N_y x N_tr}
    // Log::vrb << "Inits and fills matrix Y" << Log::endl;
    Y.resize(getOutputSize(), m_trainingset->getTrSetSize());
    for (Uint i = 0; i < m_trainingset->getTrSetSize(); ++i)
      std::copy
          (
            m_trainingset->trAt(i).getOutput().begin(),
            m_trainingset->trAt(i).getOutput().end(),
            Y.begin_col(i)
          );

    // Readout training procedure
    // Log::vrb << "Readout training" << Log::endl;
    m_readout.train(X, Y);

  } // try
  catch (std::exception& ex)
  {
    delete states_container;
    delete states_class;
    delete state_graphs_list;

    throw moka::GenericException(
        std::string("GraphEsnSom::train: error: the model training is ") +
        "failed: " + ex.what());
  } // try-catch

  // Timer stop
  // Log::vrb << "Traininig end" << Log::endl;
  training_timer.stop();

  // Fills some first training results
  m_training_res["rout_fnorm"].value =
      arma::norm(m_readout.getReadoutMatrix(), "fro");
  m_training_res["rout_bias"].value =
      m_readout.getReadoutMatrix().at(0, 0);
  m_training_res["cpu_usage"].value = training_timer.getCpuUsage();

  // Counts null clusters
  if (m_state_vect_type == reservoir_state_vect)
  {
    Uint null_clusters_count = 0;
    Uint readout_cols = m_reservoir.getReservoirSize() * m_som.getNoUnits() + 1;
    const LinearReadout::Matrix &readout_mat = m_readout.getReadoutMatrix();

    // a check to be sure
    if (readout_mat.n_cols != readout_cols)
      Log::err << "GraphEsnSom::train: an error in the readout size during "
               << "null clusters count (a bug here!). :O" << Log::endl;
    else
      for (size_t i = 0; i < m_som.getNoUnits(); ++i)
      {
        size_t first_c = (i * m_reservoir.getReservoirSize()) + 1;
        size_t last_c = (i + 1) * m_reservoir.getReservoirSize();
        if (arma::norm(readout_mat.cols(first_c, last_c), "fro") == 0)
          ++null_clusters_count;
      }

    m_training_res["null_clusters"].value =
        (null_clusters_count / (Real)m_som.getNoUnits()) * 100.0;
  }
  else
  {
    const LinearReadout::Matrix &readout_mat = m_readout.getReadoutMatrix();
    Uint null_clusters_count = 0;
    for (size_t i = 1; i < readout_mat.n_cols; ++i)
      if (arma::norm(readout_mat.col(i), 1) == 0)
        ++null_clusters_count;

    m_training_res["null_clusters"].value =
        (null_clusters_count / (Real)(readout_mat.n_cols - 1)) * 100.0;

  } // if-else

  // Computes training performance
  // Log::vrb << "Compute training performance" << Log::endl;
  Real mse = 0.0;
  Real acc = 0.0;

  // Computes mean squared error (MSE) using frobenius norm
  Math::Matrix Yhat = m_readout.getReadoutMatrix() * X;
  mse = std::pow(arma::norm(Yhat - Y, "fro"), 2.0) / Yhat.n_cols;

  // Print out mean state value (skipping null values)
  if (Log::isVerbose())
  {
    Real sum_state = 0;
    Uint sum_state_count = 0;
    Matrix::const_iterator x_it = X.begin();
    for (x_it = X.begin(); x_it != X.end(); ++x_it)
      if (*x_it != 0)
      {
        sum_state += *x_it;
        ++sum_state_count;
      }
    Log::vrb << "Mean state value (skipping null values): "
             << sum_state / sum_state_count << "." << Log::endl;
  }

  // Computes training accuracy
  Uint acc_count = 0;
  for (size_t i = 0; i < Yhat.n_cols; ++i)
  {
    bool output_compare =
        GraphEsnSom::compareOutputs
            (
              Yhat.begin_col(i),
              Yhat.end_col(i),
              Y.begin_col(i)
            );
    if (output_compare)
      acc_count += 1;
  } // for i
  acc = ((Real)acc_count) / Yhat.n_cols;

  // Fills training performance
  m_training_res["mse"].value = mse;
  m_training_res["acc"].value = acc * 100;

  // If required saves the SOM on file
  if (!m_som_save_file.empty())
    if (m_som.saveOnFile(m_som_save_file))
      Log::vrb << "SOM succesfully saved on file " << m_som_save_file
               << Log::endl;

#ifndef MOKA_TMP_CODE
  // If required saves the intances info
  if (!m_instances_info_save_file.empty())
  {
    bool save_instances_info = saveInstancesInfo
      (
        m_instances_info_save_file,
        *state_graphs_list,
        *m_trainingset,
        Yhat
      );

    if (save_instances_info)
      Log::vrb << "Intances info succesfully saved on file "
               << m_instances_info_save_file << ". :)" << Log::endl;
  }
#endif // MOKA_TMP_CODE

  // If required saves unit info
  if (!m_unit_info_save_file.empty())
    if (saveUnitInfo(m_unit_info_save_file, *state_graphs_list))
      Log::vrb << "Unit info succesfully saved on file "
               << m_unit_info_save_file << ". :)" << Log::endl;

#ifdef MOKA_TMP_CODE
  // TODO: remove these call to temporary methods ____vvvvvvvvvvvvvvvvvvvvv____
  static int tmp_count = 1;

  if (!m_instances_info_save_file.empty())
  {
    bool save_instances_info = saveInstancesInfo
      (
        m_instances_info_save_file + "." + Global::toString(tmp_count) + ".tr",
        *state_graphs_list,
        *m_trainingset,
        Yhat
      );

    if (save_instances_info)
      Log::vrb << "Intances info succesfully saved on file "
               << m_instances_info_save_file << Log::endl;
  }

  if (m_readout.getRegularizationMethod() == LinearReadout::lasso ||
      m_readout.getRegularizationMethod() == LinearReadout::elastic_net)
  {
    writeSelectedFragments(
          "selected_fragments." + Global::toString(tmp_count++) + ".frags",
          *state_graphs_list);
  }
  // Up to here ----^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^----
#endif // MOKA_TMP_CODE

  // Now the state graph list is no longer usefull
  delete state_graphs_list;

  setTrained(true);

  return m_training_res;
} // method train

/**
 * Method write
 *
 * See Model::write.
 */
void GraphEsnSom::write(std::ostream& os) const
{
  if (!isTrained())
    throw moka::GenericException(
        "GraphEsnSom::write: the model is not trained");

  // In the first row put a version number that can be useful when reading
  os << Global::getLibraryVersion() << std::endl;

  // Writes other objects (skipping some of them as the training set bind or
  // the backup copy)
  m_reservoir.write(os);
  m_som.write(os);
  os << m_som_training_type << std::endl;
  os << stvToStr(m_state_vect_type) << std::endl;
  m_readout.write(os);

  return;
} // method write

// =================
// PROTECTED METHODS
// =================

/**
 * Method clearObject
 *
 * Clears this object as just created (setting members to their initial values).
 */
void GraphEsnSom::clearObject()
{
  Model::clearObject();

  m_trainingset = NULL; // not to be deleted (is a bind)

  clearObjectForInit(false);

  return;
} // method clearObject

/**
 * Method clearObjectForInit
 *
 * Clears members for a new initialization.
 */
void GraphEsnSom::clearObjectForInit(bool clear_base_model)
{
  if (clear_base_model)
    Model::clearObjectForInit();

  // Backup copy
  delete m_graphesnsom_bak;
  m_graphesnsom_bak = NULL;

  // Reservoir
  m_reservoir.clear();

  // SOM
  m_som.clear();
  m_som_training_type = "unsupervised";
  m_som_load_file.clear();
  m_som_save_file.clear();
  m_som_data_save_file.clear();

  // State vector
  m_state_vect_type = binary_state_vect;
  m_state_vect.clear();

  // Readout
  m_readout.clear();

  // Results containers
  m_training_res.clear();
  m_test_res.clear();

  // Other parameters
  m_instances_info_save_file.clear();
  m_unit_info_save_file.clear();

  return;
} // method clearObjectForInit

/**
 * Method copyObject
 *
 * Copy all the members values of the passed object in this object.
 */
void GraphEsnSom::copyObject(const GraphEsnSom& graphesnsom)
{
  // Training set (bind)
  m_trainingset = graphesnsom.m_trainingset;

  // Backup copy
  if (graphesnsom.m_graphesnsom_bak)
    m_graphesnsom_bak = new GraphEsnSom(*graphesnsom.m_graphesnsom_bak);
  else
    m_graphesnsom_bak = NULL;

  // Reservoir
  m_reservoir = graphesnsom.m_reservoir;

  // SOM
  m_som = graphesnsom.m_som;
  m_som_training_type = graphesnsom.m_som_training_type;
  m_som_load_file = graphesnsom.m_som_load_file;
  m_som_save_file = graphesnsom.m_som_save_file;
  m_som_data_save_file = graphesnsom.m_som_data_save_file;

  // State vector
  m_state_vect_type = graphesnsom.m_state_vect_type;
  m_state_vect = graphesnsom.m_state_vect;

  // Readout
  m_readout = graphesnsom.m_readout;

  // Results container
  m_training_res = graphesnsom.m_training_res;
  m_test_res = graphesnsom.m_test_res;

  // Other parameters
  m_instances_info_save_file = graphesnsom.m_instances_info_save_file;
  m_unit_info_save_file = graphesnsom.m_unit_info_save_file;

  return;
} // method copyObject

// ===============
// PRIVATE METHODS
// ===============

/**
 * Method checkDatasetCompatibility
 *
 * Returns true if the passed dataset is compatible with the current model,
 * false otherwise.
 */
bool GraphEsnSom::checkDatasetCompatibility(const MultiLabeledGraphDataset& ds)
{
  if (!ds.isEmpty())
  {
    //if (ds.maxDegree() != m_trainingset->maxDegree())
    //{
    //  Log::dbg << "GraphEsnSom::checkDatasetCompatibility: wrong max degree."
    //           << Log::endl;
    //  return false;
    //} // if

    if (ds.getInput(0).getElementsSize() != getInputSize())
    {
      Log::vrb << "GraphEsnSom::checkDatasetCompatibility: wrong input "
               << "element size." << Log::endl;
      return false;
    } // if

    if (ds.getOutput(0).size() != getOutputSize())
    {
      Log::vrb << "GraphEsnSom::checkDatasetCompatibility: wrong output size."
               << Log::endl;
      return false;
    } // if

  } // if !ds.isEmpty()

  return true;
} // method checkDatasetCompatibility

/**
 * Method checkParameters
 *
 * Returns "true" if the set of parameters setted in this object (see method
 * getParameters) is an admissible set of parameters, i.e. there are all the
 * required parameters and their values are within the ranges. Returns "false"
 * otherwise (if there are invalid parameters values).
 */
bool GraphEsnSom::checkParameters()
{
  using util::Prm;
  util::Parameters &parameters = getParameters();

  parameters.setMsgPrefix("GraphEsnSom: ");
  parameters.setVerboseMode();

  // Reservoir parameters
  bool reservoir_parameters_check =
      (
        parameters.check(
            "input-size", Prm::optional | Prm::uint | Prm::positive)
        &&
        parameters.check("reservoir-size", Prm::uint | Prm::positive)
        &&
        parameters.check(
            "reservoir-connectivity",
            Prm::optional | Prm::real | Prm::in_range, "0.0", "1.0")
        &&
        parameters.check(
            "reservoir-input-scaling",
            Prm::optional | Prm::real |  Prm::positive)
        &&
        parameters.check(
            "reservoir-max-degree", Prm::optional | Prm::uint | Prm::positive)
        &&
        parameters.check(
            "reservoir-sigma", Prm::optional | Prm::real | Prm::positive)
        &&
        parameters.check(
            "reservoir-epsilon", Prm::optional | Prm::real | Prm::non_negative)
        &&
        parameters.check(
            "reservoir-max-iters", Prm::optional | Prm::uint | Prm::positive)
        &&
        parameters.check("reservoir-rseed", Prm::optional | Prm::uint)
      ); // reservoir_parameters_check

  if (!reservoir_parameters_check)
    return false;

  // SOM parameters
  bool som_parameters_check =
      (
        parameters.check("som-no-rows", Prm::uint | Prm::positive)
        &&
        parameters.check("som-no-cols", Prm::uint | Prm::positive)
        &&
        parameters.check("som-rseed", Prm::optional | Prm::uint)
        &&
        parameters.check(
            "som-no-epochs-1", Prm::optional | Prm::uint | Prm::non_negative)
        &&
        parameters.check(
            "som-alpha-1", Prm::optional | Prm::real | Prm::non_negative)
        &&
        parameters.check(
            "som-sigma-1", Prm::optional | Prm::real | Prm::non_negative)
        &&
        parameters.check(
            "som-sigma-fin-1", Prm::optional | Prm::real | Prm::non_negative)
        &&
        parameters.check(
            "som-no-epochs-2", Prm::optional | Prm::uint | Prm::non_negative)
        &&
        parameters.check(
            "som-alpha-2", Prm::optional | Prm::real | Prm::non_negative)
        &&
        parameters.check(
            "som-sigma-2", Prm::optional | Prm::real | Prm::non_negative)
        &&
        parameters.check(
            "som-sigma-fin-2", Prm::optional | Prm::real | Prm::non_negative)
        &&
        parameters.check(
            "som-no-epochs-3", Prm::optional | Prm::uint | Prm::non_negative)
        &&
        parameters.check(
            "som-alpha-3", Prm::optional | Prm::real | Prm::non_negative)
        &&
        parameters.check(
            "som-sigma-3", Prm::optional | Prm::real | Prm::non_negative)
        &&
        parameters.check(
            "som-sigma-fin-3", Prm::optional | Prm::real | Prm::non_negative)
        &&
        parameters.check("som-save-file", Prm::optional | Prm::non_empty)
        &&
        parameters.check("som-load-file", Prm::optional | Prm::non_empty)
        &&
        parameters.check("som-data-save-file", Prm::optional | Prm::non_empty)
      );

  if (parameters.contains("som-alpha-decay")) try
  {
    SuperSOM::strToDect(parameters.get("som-alpha-decay"));
  }
  catch (std::exception& ex)
  {
    Log::out << "GraphEsnSom: invalid decay type value on <som-alpha-decay> "
             << "(" << parameters.get("som-alpha-decay") << ")" << Log::endl;
    som_parameters_check = false;
  }

  if (parameters.contains("som-training-type"))
  {
    std::string som_training_type = parameters.get("som-training-type");
    if (som_training_type != "supervised" &&
        som_training_type != "unsupervised")
    {
      Log::out << "GraphEsnSom: invalid value on <som-training-type> "
               << "(" << som_training_type << ")" << Log::endl;
      som_parameters_check = false;
    }
  } // if parameters.contains("som-alpha-decay")

  if (!som_parameters_check)
    return false;

  // State vector
  if (parameters.contains("state-vector-type")) try
  {
    strToStv(parameters.get("state-vector-type"));
  }
  catch (std::exception& ex)
  {
    Log::out << "GraphEsnSom: invalid values on parameter <state-vector-type> "
             << "(" << parameters.get("state-vector-type") << ")" << Log::endl;
    return false;
  }

  // Readout parameters
  parameters.check("output-size", Prm::optional | Prm::uint | Prm::positive);

  bool reg_parameters_check;
  if (parameters.contains("regularization")) try
  {
    switch (LinearReadout::strToRegm(parameters.get("regularization")))
    {
    case LinearReadout::ridge_regression:
      reg_parameters_check =
          parameters.check(
            "ridge-regression-lambda", Prm::real | Prm::positive);
      if (!reg_parameters_check)
        Log::out << "GraphEsnSom: invalid value on parameter "
                 <<"<ridge-regression-lambda>." << Log::endl;
      break;
    case LinearReadout::lasso:
      reg_parameters_check =
          parameters.check("lasso-lambda", Prm::real | Prm::positive);
      if (!reg_parameters_check)
        Log::out << "GraphEsnSom: invalid value on parameter <lasso-lambda>."
                 << Log::endl;
      break;
    case LinearReadout::elastic_net:
      reg_parameters_check =
          (
            parameters.check("elastic-net-lambda1", Prm::real | Prm::positive)
            &&
            parameters.check("elastic-net-lambda2", Prm::real | Prm::positive)
          );
      if (!reg_parameters_check)
        Log::out << "GraphEsnSom: invalid value on parameter "
                 << "<elastic-net-lambda1> or " << "<elastic-net-lambda2>."
                 << Log::endl;
      break;
    case LinearReadout::no_regularization: default:
      reg_parameters_check = true;
      break;
    } // switch

    if (!reg_parameters_check)
      return false;

  } // if
  catch (std::exception& ex)
  {
    Log::out << "GraphEsnSom: invalid value on parameter <regularization> "
             << "(" << parameters.get("regularization") << ")" << Log::endl;
    return false;
  }

  // Other parameters
  if (!parameters.check("unit-info-save-file", Prm::optional | Prm::non_empty))
    return false;

  return true;
} // method checkParameters

/**
 * Method collectReservoirStates
 *
 * Method used during the training procedure (method train). If the encoding
 * procedure fails an exception will be thrown.
 */
void GraphEsnSom::collectReservoirStates
(
    const MultiLabeledGraphDataset& training_set,
    SuperSOM::DataContainer& states_container,
    std::vector<Real>& states_class,
    std::list<MultiLabeledGraph>& state_graphs,
    Real& avg_iterations
)
{
  avg_iterations = 0.0;

  // Computes and collects all the state graphs from the training set
  for (Uint i = 0; i < training_set.getTrSetSize(); ++i)
  {
    if (!m_reservoir.encoding(training_set.trAt(i).getInput()))
      throw moka::GenericException("the encoding process is failed");
    state_graphs.push_back(m_reservoir.getLastStateGraph());
    avg_iterations += m_reservoir.getLastNumberOfIterations();

    // Collects all the states
    Uint n_states = state_graphs.back().getSize();
    for (Uint st = 0; st < n_states; ++st)
    {
      states_container.push_back(state_graphs.back().getVertexElement(st));
      states_class.push_back(training_set.trAt(i).getOutput()[0]);
    }

  } // for i

  avg_iterations = avg_iterations / (Real)(training_set.getTrSetSize());

  return;
} // method collectReservoirStates

/**
 * Method computeReservoirStateVector
 *
 * Support method for the state mapping function process.
 */
void GraphEsnSom::computeReservoirStateVector(
    const structure::MultiLabeledGraph& state_graph)
{
  if (m_state_vect_type != reservoir_state_vect)
    throw moka::GenericException(
        "GraphEsnSom::computeReservoirStateVector: invalid state vector type");

  std::vector< std::vector<Vector> > clusters(m_som.getNoUnits());

  Uint verteces = state_graph.getSize();
  for (Uint vertex = 0; vertex < verteces; ++vertex)
  {
    SuperSOM::UnitIndex wu =
        m_som.winnerUnit(state_graph.getVertexElement(vertex));

    size_t cluster_index = (wu.first * m_som.getNoColumns()) + wu.second;
    clusters[cluster_index].push_back(state_graph.getVertexElement(vertex));

  } // for vertex

  // Inits the state vector adding a fixed state for the bias
  m_state_vect.clear();
  m_state_vect.insert_rows(0, 1);
  m_state_vect[0] = 1.0;

  for (size_t cls = 0; cls < clusters.size(); ++cls)
  {
    Vector tmp_cls_mean;
    tmp_cls_mean.zeros(state_graph.getElementsSize());

    if (!clusters[cls].empty())
    {
      for (size_t n = 0; n < clusters[cls].size(); ++n)
        tmp_cls_mean += clusters[cls][n];
      tmp_cls_mean = tmp_cls_mean / clusters[cls].size();
    } // if

    m_state_vect.insert_rows(m_state_vect.size(), tmp_cls_mean);
  } // for cls

  return;
} // method computeReservoirStateVector

/**
 * Method computeStateVector
 *
 * Support method for the state mapping function process.
 */
void GraphEsnSom::computeStateVector(
    const structure::MultiLabeledGraph& state_graph)
{
  std::vector< std::vector<Real> > clusters(m_som.getNoUnits());

  // Each vertex of the graph is inserted in the cluster that best represents
  // it, as determined by the SOM mapping.
  Uint verteces = state_graph.getSize();
  for (Uint vertex = 0; vertex < verteces; ++vertex)
  {
    SuperSOM::UnitIndex wu =
        m_som.winnerUnit(state_graph.getVertexElement(vertex));

    // Puts the vector in its cluster. The cluster index is converted from map
    // to vector taking elements by rows, i.e. first all the first row (from
    // the first column to the last, or left to right in other words) then all
    // the second row and so on until the last row.
    size_t cls_index = (wu.first * m_som.getNoColumns()) + wu.second;
    Real cls_value;
    switch (m_state_vect_type)
    {
    case cauchy_state_vect:
      cls_value = m_som.activate(wu, state_graph.getVertexElement(vertex));
      break;
    case binary_state_vect:
      cls_value = 1.0;
      break;
    case sum_state_vect:
      cls_value = Math::sum(state_graph.getVertexElement(vertex));
      break;
    default:
      throw moka::GenericException(
          "GraphEsnSom::computeStateVector: invalid state vector type");
      break;
    }

    clusters[cls_index].push_back(cls_value);

    //if (clusters[cls_index].size() > 1)
    //  Log::vrb << "Collision in " << in_stategraph.getVertexItem(vertex)[0]
    //           << "( " << clusters[cls_index].size()  << ")" << Log::endl;

  } // for vertex

  // Inits the state vector adding a fixed state for the bias
  m_state_vect.zeros(clusters.size() + 1);
  m_state_vect[0] = 1.0;

  // Fills the final state vector
  for (size_t cls = 0; cls < clusters.size(); ++cls)
  {
    if (clusters[cls].empty())
    {
      m_state_vect[cls + 1] = 0.0;
    }
    else
    {
      if (m_state_vect_type == binary_state_vect)
        m_state_vect[cls + 1] = 1.0;
      else
        m_state_vect[cls + 1] = Math::avg(clusters[cls]);

      //if (fromMethodCompute && m_W_out(0, cls + 1) == 0)
      //  Log::vrb << "Atom falt into a null cluster" << Log::endl;

    } // if-else (clusters[cls].empty())

  } // for cls

  return;
} // method computeStateVector

/**
 * Method encodingProcess
 *
 * Given an input graph computes the encoding process by using the reservoir
 * and return the resulting state graph.
 */
const MultiLabeledGraph& GraphEsnSom::encodingProcess
(
    const MultiLabeledGraph& input
)
{
  m_reservoir.encoding(input);
  return m_reservoir.getLastStateGraph();
} // method encodingProcess

/**
 * Method initResultsContainers
 *
 * Clears and (re)builds the training and the test results containers.
 */
void GraphEsnSom::initResultsContainers()
{
  initTrainingResultsContainer();
  initTestResultsContainer();
  return;
} // method initResultsContainers

/**
 * Method initTestResultsContainer
 *
 * Clears and (re)builds the test results containers.
 */
void GraphEsnSom::initTestResultsContainer()
{
  m_test_res.clear();
  m_test_res["mse"].name = "MSE";
  m_test_res["mse"].value = 0.0;
  m_test_res["acc"].name = "Accuracy";
  m_test_res["acc"].value = 0.0;
  m_test_res["acc"].note = "%";
  m_test_res["cpu_usage"].name = "Cpu usage";
  m_test_res["cpu_usage"].value = 0.0;
  m_test_res["cpu_usage"].note = "sec.";
  return;
} // method initTestResultsContainer

/**
 * Method initTrainingResultsContainer
 *
 * Clears and (re)builds the training results containers returned by the
 * method train.
 */
void GraphEsnSom::initTrainingResultsContainer()
{
  m_training_res.clear();
  m_training_res["res_avg_iters"].name = "Reservoir avg iters";
  m_training_res["res_avg_iters"].value = 0.0;
  m_training_res["null_clusters"].name = "Null clusters";
  m_training_res["null_clusters"].value = 0.0;
  m_training_res["null_clusters"].note = "%";
  m_training_res["rout_fnorm"].name = "Readout frobenius norm";
  m_training_res["rout_fnorm"].value = 0.0;
  m_training_res["rout_bias"].name = "Readout bias";
  m_training_res["rout_bias"].value = 0.0;
  m_training_res["mse"].name = "MSE";
  m_training_res["mse"].value = 0.0;
  m_training_res["acc"].name = "Accuracy";
  m_training_res["acc"].value = 0.0;
  m_training_res["acc"].note = "%";
  m_training_res["cpu_usage"].name = "Cpu usage";
  m_training_res["cpu_usage"].value = 0.0;
  m_training_res["cpu_usage"].note = "sec.";
  m_training_res["som_cpu_usage"].name = "SOM cpu usage";
  m_training_res["som_cpu_usage"].value = 0.0;
  m_training_res["som_cpu_usage"].note = "sec.";
  return;
} // method initTrainingResultsContainer

/**
 * Method makeBackupCopy
 *
 * Method used (only) in the init method. Makes a backup copy of this object
 * and put it in the member m_graphesnsom_bak.
 * The backup will be used in the reset method to bring back this object in the
 * state as just after the initialization (end of the method init).
 */
void GraphEsnSom::makeBackupCopy()
{
  delete m_graphesnsom_bak;

  // very important that the backup copy has a null pointer here
  m_graphesnsom_bak = NULL;
  m_graphesnsom_bak = new GraphEsnSom(*this);

  return;
} // method makeBackupCopy

/**
 * Method outputProcess
 *
 * Given a state vector that must be consistent with the readout, this method
 * computes the final output passing the state vector to the readout.
 * You can further access the resulting output usign the method "getLastOutput".
 * A moka::GenericException will be thrown in case of errors.
 */
const GraphEsnSom::Vector& GraphEsnSom::outputProcess
(
    const Vector& state_vector
)
{
  return m_readout.compute(state_vector);
} // method outputProcess

/**
 * Method parseParameters
 *
 * Fills parameters by looking at getParameters() returned object.
 */
void GraphEsnSom::parseParameters()
{
  util::Parameters &parameters = getParameters();

  // Reservoir random seed
  if (parameters.contains("reservoir-rseed"))
    m_reservoir.setRandomSeed(parameters.getUint("reservoir-rseed"));
  else
  {
    timeval timev;
    gettimeofday(&timev, NULL);
    m_reservoir.setRandomSeed(timev.tv_usec % 100000);
  } // if-else

  // Reservoir elements
  m_reservoir.setInputSize(parameters.getUint("input-size"));
  m_reservoir.setReservoirSize(parameters.getUint("reservoir-size"));
  m_reservoir.setReservoirConnectivity(
      parameters.getReal("reservoir-connectivity", 1.0));
  m_reservoir.setMaxDegree(parameters.getUint("reservoir-max-degree"));
  m_reservoir.setInputScaling(
      parameters.getReal("reservoir-input-scaling", 0.1));
  m_reservoir.setSigma(parameters.getReal("reservoir-sigma", 0.9));
  m_reservoir.setEpsilon(parameters.getReal("reservoir-epsilon", 1e-2));
  m_reservoir.setMaxIterations(
      parameters.getUint("reservoir-max-iters", 10000));

  // SOM parameters
  m_som_load_file = parameters.get("som-load-file", "");

  if (m_som_load_file.empty())
  {
    // SOM random seed
    if (parameters.contains("som-rseed"))
      m_som.setRandomSeed(parameters.getUint("som-rseed"));
    else
    {
      timeval timev;
      gettimeofday(&timev, NULL);
      m_som.setRandomSeed(timev.tv_usec % 100000);
    } // if-else

    // SOM elements
    m_som.setNoRows(parameters.getUint("som-no-rows", 1));
    m_som.setNoColumns(parameters.getUint("som-no-cols", 1));

    // SOM default parameters
    Uint som_tr_set_len = 0;
    for (Uint i = 0; i < m_trainingset->getTrSetSize(); ++i)
      som_tr_set_len += m_trainingset->trAt(i).getInput().getSize();
    m_som.setDefaultParameters(som_tr_set_len);

    // SOM training parameters (phase 1)
    if (parameters.contains("som-no-epochs-1"))
      m_som.setNoEpochs1(parameters.getUint("som-no-epochs-1"));

    if (parameters.contains("som-alpha-1"))
      m_som.setAlpha1(parameters.getReal("som-alpha-1"));

    if (parameters.contains("som-sigma-1"))
      m_som.setSigma1(parameters.getReal("som-sigma-1"));

    if (parameters.contains("som-sigma-fin-1"))
      m_som.setSigmaFinal1(parameters.getReal("som-sigma-fin-1"));

    // SOM training parameters for phase 2
    if (parameters.contains("som-no-epochs-2"))
      m_som.setNoEpochs2(parameters.getUint("som-no-epochs-2"));

    if (parameters.contains("som-alpha-2"))
      m_som.setAlpha2(parameters.getReal("som-alpha-2"));

    if (parameters.contains("som-sigma-2"))
      m_som.setSigma2(parameters.getReal("som-sigma-2"));

    if (parameters.contains("som-sigma-fin-2"))
      m_som.setSigmaFinal2(parameters.getReal("som-sigma-fin-2"));

    // SOM training parameters for phase 3
    if (parameters.contains("som-no-epochs-3"))
      m_som.setNoEpochs3(parameters.getUint("som-no-epochs-3"));

    if (parameters.contains("som-alpha-3"))
      m_som.setAlpha3(parameters.getReal("som-alpha-3"));

    if (parameters.contains("som-sigma-3"))
      m_som.setSigma3(parameters.getReal("som-sigma-3"));

    if (parameters.contains("som-sigma-fin-3"))
      m_som.setSigmaFinal3(parameters.getReal("som-sigma-fin-3"));

    // SOM training type
    if (parameters.contains("som-training-type"))
      m_som_training_type = parameters.get("som-training-type");
    else
      m_som_training_type = "unsupervised"; // default value

  } // if (m_som_load_file.empty())

  // SOM save files
  m_som_save_file = parameters.get("som-save-file");
  m_som_data_save_file = parameters.get("som-data-save-file");

  // State vector type
  m_state_vect_type = binary_state_vect; // default value
  if (parameters.contains("state-vector-type")) try
  {
    m_state_vect_type = strToStv(parameters.get("state-vector-type"));
  }
  catch(std::exception& ex)
  { /* leave the default value */ }

  // Readout and regularization
  m_readout.setOutputSize(parameters.getUint("output-size"));

  m_readout.setRegularizationMethod(LinearReadout::no_regularization);
  if (parameters.contains("regularization")) try
  {
    m_readout.setRegularizationMethod(
        LinearReadout::strToRegm(parameters.get("regularization")));

    switch (m_readout.getRegularizationMethod())
    {
    case LinearReadout::ridge_regression:
      m_readout.setRidgeRegressionLambda(
          parameters.getReal("ridge-regression-lambda", 0.0));
      break;
    case LinearReadout::lasso:
      m_readout.setLassoLambda(
          parameters.getReal("lasso-lambda", 0.0));
      break;
    case LinearReadout::elastic_net:
      m_readout.setElasticNetLambda1(
          parameters.getReal("elastic-net-lambda1", 0.0));
      m_readout.setElasticNetLambda2(
          parameters.getReal("elastic-net-lambda2", 0.0));
      break;
    case LinearReadout::no_regularization: default:
      /* nop */
      break;
    }

  } // if-try
  catch(std::exception& ex)
  { /* leave the default value */ }

  // Other parameters
  m_instances_info_save_file = parameters.get("instances-info-save-file");
  m_unit_info_save_file = parameters.get("unit-info-save-file");

  return;
} // method parseParameters

/**
 * Method saveInstancesInfo
 *
 * Saves instances info (id, output, bias, vertices weights, winner unit) on
 * the passed file in the following format:
 *
 *   <instance ID>
 *   <instance output>
 *   <bias weight>
 *   <N>
 *   <winner unit 1> <weight 1> [<*>]
 *   <winner unit 2> <weight 2> [<*>]
 *   ...
 *   <winner unit N> <weight N> [<*>]
 *   <predicted output>
 *
 * for each instance (graph) into the state graph list.
 */
bool GraphEsnSom::saveInstancesInfo
(
    const std::string& filename,
    const std::list<structure::MultiLabeledGraph>& state_graphs,
    const MultiLabeledGraphDataset& training_dataset,
    const Matrix& outputs
) const
{
  // Some initial check
  if (state_graphs.size() != training_dataset.getTrSetSize())
  {
    Log::err << "GraphEsnSom::saveInstancesInfo: error: state graphs list "
             << "and the dataset have different size (a bug here!). :O"
             << Log::endl;
    return false;
  }

  if (outputs.n_cols != training_dataset.getTrSetSize())
  {
    Log::err << "GraphEsnSom::saveInstancesInfo: error: wrong outputs size "
             << "(a bug here!). :O" << Log::endl;
    return false;
  }

  if (m_readout.getOutputSize() != 1)
  {
    Log::err << "GraphEsnSom::saveInstancesInfo: error: more than one output."
             << Log::endl;
    return false;
  }

  // Open output stream (file)
  std::ofstream ofs(filename.c_str(), std::fstream::out);
  if (ofs.fail())
  {
    Log::err << "GraphEsnSom::saveInstancesInfo: fail open file " << filename
             << "." << Log::endl;
    return false;
  }

  // Write out data
  Uint d = 0;
  std::list<MultiLabeledGraph>::const_iterator g_it = state_graphs.begin();
  for ( /* nop */ ; g_it != state_graphs.end(); ++g_it, ++d)
  {
    // Instance ID
    ofs << training_dataset.trAt(d).getId() << "\n";

    // Output
    ofs << training_dataset.trAt(d).getOutput()[0] << "\n";

    // Bias
    ofs << m_readout.getReadoutMatrix().at(0, 0) << "\n";

    // Winner unit and weights
    if (!writeVerticesInfo(ofs, *g_it))
    {
      Log::err << "GraphEsnSom::saveInstancesInfo: fail writing instance "
               << training_dataset.trAt(d).getId() << Log::endl;
      return false;
    }

    // Predicted output
    ofs << outputs[d] << "\n";

  } // for g_it

  ofs.close();
  return true;
} // method saveInstancesInfo

/**
 * Method saveSomDataOnFile
 *
 * Saves passed data on passed file looking at dataset for outputs.
 */
bool GraphEsnSom::saveSomDataOnFile
(
    const std::string& filename,
    const std::list<MultiLabeledGraph>& state_graphs,
    const MultiLabeledGraphDataset& training_dataset
) const
{
  if (state_graphs.empty())
  {
    Log::err << "GraphEsnSom::saveSomDataOnFile: empty data." << Log::endl;
    return false;
  }

  if (state_graphs.size() != training_dataset.getTrSetSize())
  {
    Log::err << "GraphEsnSom::saveSomDataOnFile: error: state graphs list "
             << "and dataset have different size." << Log::endl;
    return false;
  }

  std::fstream fs(filename.c_str(), std::fstream::out);
  if (fs.fail())
  {
    Log::err << "GraphEsnSom::saveSomDataOnFile: fail open file " << filename
             << "." << Log::endl;
    return false;
  }

  // First row: data size
  fs << state_graphs.front().getElementsSize() << std::endl;

  // Comment with data labels
  fs << "#";
  for (Uint i = 0; i < state_graphs.front().getElementsSize(); ++i)
    fs << " d" << i;
  fs << " output_class";
  fs << std::endl;

  // Data
  std::list<MultiLabeledGraph>::const_iterator g_it;
  typename MultiLabeledGraph::Vertices::const_iterator v_it;
  typename MultiLabeledGraph::Vertex::Element::const_iterator e_it;
  Uint d = 0;
  for (g_it = state_graphs.begin(); g_it != state_graphs.end(); ++g_it, ++d)
  {
    for (v_it = g_it->getVertexBegin(); v_it != g_it->getVertexEnd(); ++v_it)
    {
      // Prints vertex elements
      for (e_it = v_it->getElement().begin();
           e_it != v_it->getElement().end(); ++e_it)
        fs << *e_it << " ";

      // Prints output_class
      fs << training_dataset.trAt(d).getOutput()[0] << " ";

      // Prints other labels
      for (size_t lbl = 0; lbl < v_it->getItem().size(); ++lbl)
        fs << v_it->getItem()[lbl] << " ";

      fs << std::endl;

    } // for v_it
  } // for g_it

  fs.close();
  return true;
} // method saveSomDataOnFile

/**
 * Method saveUnitInfo
 *
 * Saves on file a collections of informations about each unit of the SOM. The
 * file is written as follows:
 *
 *   <version>
 *   <n. units>
 *   <n. rows>
 *   <n. cols>
 *   [info unit 1]
 *   ...
 *   [info unit N]
 *
 * where <version> is the version number (that can be useful when reading this
 * file), <n. units> is the number of units saved on file (units that are in
 * the current SOM), on <n. rows> rows and <n. cols> columns, and [info unit i]
 * is the collection of informations about the i-th unit. Such informations
 * are written as follows:
 *
 *   <row>
 *   <col>
 *   <weight>
 *   <n. labels>
 *   [label value 1]
 *   ...
 *   [label value L]
 *
 * where <row> is the unit row number, <col> the unit column number, <weight>
 * is the readout weight associated with the unit, <n. labels> is the number
 * of labels and [label value j] is a block containing the value of the j-th
 * label. The label value block is written as follows:
 *
 *   <label name>
 *   <n. values>
 *   <value 1>
 *   <freq. value 1>
 *   ...
 *   <value M>
 *   <frew. value M>
 *
 * where <label name> is the name of the label, <n. values> is the number of
 * values collected in the unit for such label, <value k> is k-th most frequent
 * value for such label fallen in the unit with <freq. value k> its frequency.
 *
 * This file can be written only if there is one (no more) output in the
 * readout.
 *
 * The unit info blocks ([info unit i]) are written in row order, i.e. for
 * first the first row from left to right (from the first column to the last),
 * then the second row and so on.
 */
bool GraphEsnSom::saveUnitInfo
(
    const std::string& filename,
    const std::list<MultiLabeledGraph>& state_graphs
) const
{
  if (state_graphs.size() == 0)
    return false;

  // Some initial check
  if (getOutputSize() != 1)
  {
    Log::err << "GraphEsnSom::saveUnitInfo: the unit info file can be "
             << "written if there is only one output." << Log::endl;
    return false;
  }

  // Open output stream (file)
  std::ofstream ofs(filename.c_str(), std::fstream::out);
  if (ofs.fail())
  {
    Log::err << "GraphEsnSom::saveUnitInfo: fail open file " << filename << "."
             << Log::endl;
    return false;
  }

  // Some convenience values
  Uint som_rows = m_som.getNoRows();
  Uint som_cols = m_som.getNoColumns();

  // Number of labels: each vertex must have the same number of labels
  Uint n_labels = state_graphs.back().getLabelNameSize();

  // Labeling process: for each vertex label is built a label map.
  typedef std::vector< std::vector< std::map<std::string, Uint> > > LabelMap;
  std::vector<LabelMap> label_maps(n_labels);

  // Init each label map
  for (size_t l = 0; l < n_labels; ++l)
  {
    label_maps[l].resize(som_rows);
    for (size_t r = 0; r < som_rows; ++r)
      label_maps[l][r].resize(som_cols);
  }

  // Fill label maps
  std::list<MultiLabeledGraph>::const_iterator graph = state_graphs.begin();
  typename MultiLabeledGraph::Vertices::const_iterator vertex;

  for (/* nop */; graph != state_graphs.end(); ++graph)
  {
    // Check the number of labels in the graph
    if (graph->getLabelNameSize() != n_labels)
    {
      Log::err << "GraphEsnSom::saveUnitInfo: wrong number of labels in one "
               << "graph. The process is interrupted." << Log::endl;
      return false;
    }

    vertex = graph->getVertexBegin();
    for (/* nop */; vertex != graph->getVertexEnd(); ++vertex)
    {
      // Check the number of labels in the vertex
      if (vertex->getItem().size() != n_labels)
      {
        Log::err << "GraphEsnSom::saveUnitInfo: wrong number of labels in "
                 << "one vertex. The process is interrupted." << Log::endl;
        return false;
      }

      // Get the winner unit
      SuperSOM::UnitIndex wu = m_som.winnerUnit(vertex->getElement());

      // Update the map for each labels
      for (size_t m = 0; m < n_labels; ++m)
      {
        std::string label_value = vertex->getItem().at(m);

        if (!label_value.empty())
        {
          if (label_maps[m][wu.first][wu.second].count(label_value))
            label_maps[m][wu.first][wu.second][label_value] += 1;
          else
            label_maps[m][wu.first][wu.second][label_value] = 1;
        }
      } // for m

    } // for vertex
  } // for graph

  // Write unit info on file
  ofs << Global::getLibraryVersion() << std::endl;

  ofs << som_rows * som_cols << std::endl;
  ofs << som_rows << std::endl;
  ofs << som_cols << std::endl;

  for (Uint row = 0, i = 0; row < som_rows; ++row)
    for (Uint col = 0; col < som_cols; ++col, ++i)
    {
      ofs << row << std::endl;
      ofs << col << std::endl;

      // weight
      if (m_state_vect_type == reservoir_state_vect)
      {
        size_t first_c = (i * m_reservoir.getReservoirSize()) + 1;
        size_t last_c = (i + 1) * m_reservoir.getReservoirSize();
        if (last_c - first_c != m_som.getCodebookSize())
        {
          Log::err << "GraphEsnSom::saveUnitInfo: wrong state vector size. "
                   << "The process is interrupted and the file written is "
                   << "wrong. :(" << Log::endl;
          return false;
        }
        arma::Row<Real> unit_weights =
             m_readout.getReadoutMatrix().cols(first_c, last_c).row(0);
        ofs << unit_weights * m_som.getCodebook(row, col) << std::endl;
      }
      else
        ofs << m_readout.getReadoutMatrix().at(i + 1) << std::endl;

      ofs << n_labels << std::endl;

      for (size_t lbl = 0; lbl < n_labels; ++lbl)
      {
        ofs << state_graphs.back().getLabelName(lbl) << std::endl;

        std::list< std::pair<std::string, Uint> > label_values;
        Global::sortMapBySecond(label_maps[lbl][row][col], label_values);

        ofs << label_values.size() << std::endl;

        std::list< std::pair<std::string, Uint> >::reverse_iterator lbl_val;
        lbl_val = label_values.rbegin();
        for (/* nop */; lbl_val != label_values.rend(); ++lbl_val)
          ofs << lbl_val->first << std::endl
              << lbl_val->second << std::endl;

      } // for lbl

    } // for (r,c)

  ofs.close();
  return true;
} // method saveUnitInfo

/**
 * Method stateMappingFunctionProcess
 *
 * Given the state graph computes the state mapping function and returns the
 * resulting state vector (also available through the method getLastState).
 */
const GraphEsnSom::Vector& GraphEsnSom::stateMappingFunctionProcess
(
    const MultiLabeledGraph& state_graph
)
{
  // Initial check to rule out eventual bugs
  if (m_som.getNoUnits() == 0)
    throw moka::GenericException(
        "GraphEsnSom::stateMappingFunctionProcess: you must train the SOM "
        "before to use the state mapping function");

  if (!m_som.isInitialized())
    throw moka::GenericException(
        "GraphEsnSom::stateMappingFunctionProcess: seems you not trained the "
        "the som");

  if (state_graph.getElementsSize() != m_som.getCodebookSize())
    throw moka::GenericException("GraphEsnSom::stateMappingFunctionProcess: "
        "wrong vertex size in the state graph");

  // Case reservoir state vector
  if (m_state_vect_type == reservoir_state_vect)
    computeReservoirStateVector(state_graph);
  else
    computeStateVector(state_graph);

  //static size_t print_state_vect_count = 0;
  //if (print_state_vect_count % 100 == 0)
  //  Log::vrb << "State vect: " << m_statevec.t() << Log::end;
  //print_state_vect_count++;

  // A check just to be sure
  if (m_state_vect.size() != m_readout.getStateVectorSize())
    Log::err << "GraphEsnSom::stateMappingFunctionProcess: fatal error: the "
             << "state dimension does not match the readout dimension (you "
             << "must fix the code)." << Log::endl;

  return m_state_vect;
} // method stateMappingFunctionProcess

/**
 * Method strToDect
 *
 * Converts a an std::string into a StateVectorType value. If the string is not
 * recognized throw an exception of type moka::GenericException.
 */
GraphEsnSom::StateVectorType GraphEsnSom::strToStv(const std::string& str) const
{
  if (str == "binary")
    return binary_state_vect;
  else if (str == "cauchy")
    return cauchy_state_vect;
  else if (str == "sum")
    return sum_state_vect;
  else if (str == "reservoir")
    return reservoir_state_vect;
  else
    throw moka::GenericException(
        "GraphEsnSom::strToStv: Invalid string representation of a "
        "StateVectorType value");
  // return 0;
} // method strToStv

/**
 * Method stvToStr
 *
 * Converts a StateVectorType value into an std::string. If the DecayType value
 * is not recognized throws an exception of type moka::GenericException.
 */
std::string GraphEsnSom::stvToStr(const StateVectorType& stv) const
{
  switch (stv)
  {
  case binary_state_vect:
    return "binary";
    break;
  case cauchy_state_vect:
    return "cauchy";
    break;
  case sum_state_vect:
    return "sum";
    break;
  case reservoir_state_vect:
    return "reservoir";
    break;
  default:
    throw moka::GenericException(
          "GraphEsnSom::stvToStr: invalid StateVectorType value");
  }
  // return "";
} // method stvToStr

/**
 * Method writeInstanceEquation
 *
 */
bool GraphEsnSom::writeInstanceEquation(
    std::ostream& os,
    const structure::MultiLabeledGraph& state_graph) const
{
  // Some initial checks
  if (m_som.getNoUnits() == 0 || !m_som.isInitialized())
  {
    Log::err << "GraphEsnSom::writeInstanceEquation: seems the SOM is not "
             << "trained yet. :(" << Log::endl;
    os << "\"\"";
    return false;
  }

  if (state_graph.getElementsSize() != getReservoirSize())
  {
    Log::err << "GraphEsnSom::writeInstanceEquation: wrong vertex size in the "
             << "state graph. :(" << Log::endl;
    os << "\"\"";
    return false;
  }

  // Data structures
  Uint n_vertices = state_graph.getSize();
  std::vector<SuperSOM::UnitIndex> winner_unit(n_vertices);
  std::vector<size_t> weights_index(n_vertices, 0);
  std::map<size_t, Uint> collision_map;
  std::map<size_t, Uint>::iterator cm_it;

  // For each vertex is retrived its weight index, and stored the winner unit.
  for (Uint vertex = 0; vertex < n_vertices; ++vertex)
  {
    SuperSOM::UnitIndex wu =
        m_som.winnerUnit(state_graph.getVertexElement(vertex));
    weights_index[vertex] = (wu.first * m_som.getNoColumns()) + wu.second;
    winner_unit[vertex] = wu;
    cm_it = collision_map.find(weights_index[vertex]);
    if (cm_it == collision_map.end())
      collision_map[weights_index[vertex]] = 1;
    else
      cm_it->second++;
  } // for vertex (1)

  // Writes out the equation
  os << "\"";
  if (m_readout.getReadoutMatrix().at(0,0) >= 0)
    os << "+";
  os << m_readout.getReadoutMatrix().at(0,0);

  for (Uint vertex = 0; vertex < n_vertices; ++vertex)
  {
    cm_it = collision_map.find(weights_index[vertex]);
    if (cm_it == collision_map.end())
    {
      Log::err << "GraphEsnSom::writeVerticesInfo: error searching weight "
               << "collisions. :(" << Log::endl;
      return false;
    }
    else if (cm_it->second == 1)
    {
      Real weight =
          m_readout.getReadoutMatrix().at(0, weights_index[vertex] + 1);

      if (weight != 0)
      {
        os << " ";
        if (weight > 0)
          os << "+";
        os << weight;

        // "+ 1" in order to start indices at 1 and not at 0
        os << "*Hit(" << winner_unit[vertex].first << ","
           << winner_unit[vertex].second << ")";
      }
    } // else-if
    else
      cm_it->second -= 1;
  } // for vertex (2)
  os << ")\"";

  return true;
} // method writeInstanceEquation

/**
 * Method writeVerticesInfo
 *
 * Computes the mapping process on the passed state graph and writes winner
 * units and weights for each vertex on the output stream in the following
 * format:
 *
 *   <N>
 *   <winner unit 1> <weight 1> [<*>]
 *   <winner unit 2> <weight 2> [<*>]
 *   ...
 *   <winner unit 3> <weight N> [<*>]
 *
 * where N is the number of weights (vertices in the passed graph) and "*" is
 * the collision symbol (if presents means that such weight is shared with
 * another vertex). The field for winner units will be a couple of index comma
 * separated as "6,12". Return false in case of errors, true otherwise.
 */
bool GraphEsnSom::writeVerticesInfo
(
    std::ostream& os,
    const MultiLabeledGraph& state_graph
) const
{
  // Some initial checks
  if (m_som.getNoUnits() == 0 || !m_som.isInitialized())
  {
    Log::err << "GraphEsnSom::writeVerticesInfo: seems the SOM is not trained "
             << "yet. :(" << Log::endl;
    return false;
  }

  if (state_graph.getElementsSize() != getReservoirSize())
  {
    Log::err << "GraphEsnSom::writeVerticesInfo: wrong vertex size in the "
             << "state graph. :(" << Log::endl;
    return false;
  }

  // Data structures
  Uint n_vertices = state_graph.getSize();
  std::vector<SuperSOM::UnitIndex> winner_unit(n_vertices);
  std::vector<size_t> weights_index(n_vertices, 0);
  std::map<size_t, Uint> collision_map;
  std::map<size_t, Uint>::iterator cm_it;

  // For each vertex is retrived its weight index, and stored the winner unit.
  for (Uint vertex = 0; vertex < n_vertices; ++vertex)
  {
    SuperSOM::UnitIndex wu =
        m_som.winnerUnit(state_graph.getVertexElement(vertex));
    weights_index[vertex] = (wu.first * m_som.getNoColumns()) + wu.second;
    winner_unit[vertex] = wu;
    cm_it = collision_map.find(weights_index[vertex]);
    if (cm_it == collision_map.end())
      collision_map[weights_index[vertex]] = 1;
    else
      cm_it->second++;
  } // for vertex (1)

  // Writes out weights
  os << n_vertices << "\n";
  for (Uint vertex = 0; vertex < n_vertices; ++vertex)
  {
    // "+ 1" in order to start indices at 1 and not at 0
    os << winner_unit[vertex].first << ","
       << winner_unit[vertex].second << " ";

    // "+ 1" to take into account the bias component
    os << m_readout.getReadoutMatrix().at(0, weights_index[vertex] + 1);

    cm_it = collision_map.find(weights_index[vertex]);
    if (cm_it == collision_map.end())
    {
      Log::err << "GraphEsnSom::writeVerticesInfo: error searching weight "
               << "collisions. :(" << Log::endl;
      return false;
    }
    else if (cm_it->second > 1)
      os << " *"; // collision symbol

    os << std::endl;
  } // for vertex (2)

  return true;
} // method writeVerticesInfo

#ifdef MOKA_TMP_CODE

// =================
// TEMPORARY METHODS
// =================

// ___vvvvv TODO: remove these methods vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv___

/**
 * Method saveInstancesInfoAll
 *
 * See: saveInstancesInfo.
 */
bool GraphEsnSom::saveInstancesInfoAll
(
    const std::string& filename,
    const std::list<structure::MultiLabeledGraph>& state_graphs,
    const MultiLabeledGraphDataset& training_dataset,
    const Matrix& outputs
) const
{
  // Some initial check
  if (state_graphs.size() != training_dataset.getSize())
  {
    Log::err << "GraphEsnSom::saveInstancesInfoAll: error: state graphs list "
             << "and the dataset have different size (a bug here!). :O"
             << Log::endl;
    return false;
  }

  if (outputs.n_cols != training_dataset.getSize())
  {
    Log::err << "GraphEsnSom::saveInstancesInfoAll: error: wrong outputs size "
             << "(a bug here!). :O" << Log::endl;
    return false;
  }

  if (m_readout.getOutputSize() != 1)
  {
    Log::err << "GraphEsnSom::saveInstancesInfoAll: error: more than one output."
             << Log::endl;
    return false;
  }

  // Open output stream (file)
  std::ofstream ofs(filename.c_str(), std::fstream::out);
  if (ofs.fail())
  {
    Log::err << "GraphEsnSom::saveInstancesInfoAll: fail open file " << filename
             << ". :(" << Log::endl;
    return false;
  }

  // Write out data
  Uint d = 0;
  std::list<MultiLabeledGraph>::const_iterator g_it = state_graphs.begin();
  for ( /* nop */ ; g_it != state_graphs.end(); ++g_it, ++d)
  {
    // Instance ID
    ofs << training_dataset.at(d).getId() << "\n";

    // Output
    ofs << training_dataset.at(d).getOutput()[0] << "\n";

    // Bias
    ofs << m_readout.getReadoutMatrix().at(0, 0) << "\n";

    // Winner unit and weights
    if (!writeVerticesInfo(ofs, *g_it))
    {
      Log::err << "GraphEsnSom::saveInstancesInfoAll: fail writing instance "
               << training_dataset.at(d).getId() << Log::endl;
      return false;
    }

    // Predicted output
    ofs << outputs[d] << "\n";

  } // for g_it

  ofs.close();
  return true;
} // method saveInstancesInfoAll

/**
 * Method saveInstancesInfoTsf
 *
 * See: saveInstancesInfo.
 */
bool GraphEsnSom::saveInstancesInfoTsf
(
    const std::string& filename,
    const std::list<structure::MultiLabeledGraph>& state_graphs,
    const MultiLabeledGraphDataset& training_dataset,
    const Matrix& outputs
) const
{
  // Some initial check
  if (state_graphs.size() != training_dataset.getTestFoldSize())
  {
    Log::err << "GraphEsnSom::saveInstancesInfoTsf: error: state graphs list "
             << "and the dataset have different size (a bug here!). :O"
             << Log::endl;
    return false;
  }

  if (outputs.n_cols != training_dataset.getTestFoldSize())
  {
    Log::err << "GraphEsnSom::saveInstancesInfoTsf: error: wrong outputs size "
             << "(a bug here!). :O" << Log::endl;
    return false;
  }

  if (m_readout.getOutputSize() != 1)
  {
    Log::err << "GraphEsnSom::saveInstancesInfoTsf: error: more than one output."
             << Log::endl;
    return false;
  }

  // Open output stream (file)
  std::ofstream ofs(filename.c_str(), std::fstream::out);
  if (ofs.fail())
  {
    Log::err << "GraphEsnSom::saveInstancesInfoTsf: fail open file " << filename
             << ". :(" << Log::endl;
    return false;
  }

  // Write out data
  Uint d = 0;
  std::list<MultiLabeledGraph>::const_iterator g_it = state_graphs.begin();
  for ( /* nop */ ; g_it != state_graphs.end(); ++g_it, ++d)
  {
    // Instance ID
    ofs << training_dataset.tsAt(d).getId() << "\n";

    // Output
    ofs << training_dataset.tsAt(d).getOutput()[0] << "\n";

    // Bias
    ofs << m_readout.getReadoutMatrix().at(0, 0) << "\n";

    // Winner unit and weights
    if (!writeVerticesInfo(ofs, *g_it))
    {
      Log::err << "GraphEsnSom::saveInstancesInfoTsf: fail writing instance "
               << training_dataset.tsAt(d).getId() << Log::endl;
      return false;
    }

    // Predicted output
    ofs << outputs[d] << "\n";

  } // for g_it

  ofs.close();
  return true;
} // method saveInstancesInfoTsf

/**
 * Method writeSelectedFragments
 *
 */
void GraphEsnSom::writeSelectedFragments
(
    const std::string& filename,
    const std::list<MultiLabeledGraph>& state_graphs
) const
{
  if (state_graphs.size() == 0)
    return;

  // Some initial check
  if (getOutputSize() != 1)
  {
    Log::err << "GraphEsnSom::writeSelectedFragments: there are more than "
             << "one output in the model." << Log::endl;
    return;
  }

  // Open output stream (file)
  std::ofstream ofs(filename.c_str(), std::fstream::out);
  if (ofs.fail())
  {
    Log::err << "GraphEsnSom::writeSelectedFragments: fail open file "
             << filename << "." << Log::endl;
    return;
  }

  // Some convenience values
  Uint som_rows = m_som.getNoRows();
  Uint som_cols = m_som.getNoColumns();

  // Number of labels: each vertex must have the same number of labels
  Uint n_labels = state_graphs.back().getLabelNameSize();

  // Labeling process: for each vertex label is built a label map.
  typedef std::vector< std::vector< std::map<std::string, Uint> > > LabelMap;
  std::vector<LabelMap> label_maps(n_labels);

  // Init each label map
  for (size_t l = 0; l < n_labels; ++l)
  {
    label_maps[l].resize(som_rows);
    for (size_t r = 0; r < som_rows; ++r)
      label_maps[l][r].resize(som_cols);
  }

  // Fill label maps
  std::list<MultiLabeledGraph>::const_iterator graph = state_graphs.begin();
  typename MultiLabeledGraph::Vertices::const_iterator vertex;

  for (/* nop */; graph != state_graphs.end(); ++graph)
  {
    // Check the number of labels in the graph
    if (graph->getLabelNameSize() != n_labels)
    {
      Log::err << "GraphEsnSom::writeSelectedFragments: wrong number of labels "
               << "in a graph. The process is interrupted." << Log::endl;
      return;
    }

    vertex = graph->getVertexBegin();
    for (/* nop */; vertex != graph->getVertexEnd(); ++vertex)
    {
      // Check the number of labels in the vertex
      if (vertex->getItem().size() != n_labels)
      {
        Log::err << "GraphEsnSom::writeSelectedFragments: wrong number of "
                 << "labels in a vertex. The process is interrupted."
                 << Log::endl;
        return;
      }

      // Get the winner unit
      SuperSOM::UnitIndex wu = m_som.winnerUnit(vertex->getElement());

      // Update the map for each labels
      for (size_t m = 0; m < n_labels; ++m)
      {
        std::string label_value = vertex->getItem().at(m);

        if (!label_value.empty())
        {
          if (label_maps[m][wu.first][wu.second].count(label_value))
            label_maps[m][wu.first][wu.second][label_value] += 1;
          else
            label_maps[m][wu.first][wu.second][label_value] = 1;
        }
      } // for m

    } // for vertex
  } // for graph

  // Write fragments on file
  for (Uint row = 0, i = 0; row < som_rows; ++row)
    for (Uint col = 0; col < som_cols; ++col, ++i)
    {
      // Get unit weight
      Real weight = 0.0;
      if (m_state_vect_type == reservoir_state_vect)
      {
        size_t first_c = (i * m_reservoir.getReservoirSize()) + 1;
        size_t last_c = (i + 1) * m_reservoir.getReservoirSize();
        if (last_c - first_c != m_som.getCodebookSize())
        {
          Log::err << "GraphEsnSom::writeSelectedFragments: wrong state vector "
                   << "size. The process is interrupted and the file "
                   << "written is wrong." << Log::endl;
          return;
        }

        arma::Row<Real> unit_weights =
            m_readout.getReadoutMatrix().cols(first_c, last_c).row(0);
        weight = arma::Mat<Real>(
            unit_weights * m_som.getCodebook(row, col)).at(0,0);
      }
      else
        weight = m_readout.getReadoutMatrix().at(i + 1);

      // Write fragments and weights
      if (weight != 0)
      {
        std::map<std::string, Uint>::const_iterator lm_it;

        // Atom symbol
        lm_it = label_maps[1][row][col].begin();
        for (/* nop */; lm_it != label_maps[1][row][col].end(); ++lm_it)
          ofs << "A_" << lm_it->first << " " << weight << std::endl;

        // Atom round radius 1
        lm_it = label_maps[2][row][col].begin();
        for (/* nop */; lm_it != label_maps[2][row][col].end(); ++lm_it)
          ofs << "R1_" << lm_it->first << " " << weight << std::endl;

        // Atom round radius 2
        lm_it = label_maps[3][row][col].begin();
        for (/* nop */; lm_it != label_maps[3][row][col].end(); ++lm_it)
          ofs << "R2_" << lm_it->first << " " << weight << std::endl;

      } // if

    } // for (r,c)

  ofs.close();

  return;
} // method writeSelectedFragments
// ---^^^^^ TODO: remove up to here ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^---
#endif // MOKA_TMP_CODE

// ===================
// AUXILIARY FUNCTIONS
// ===================

/**
 * Function operator<<
 *
 * See GraphEsnSom::print() method.
 */
std::ostream& operator<<(std::ostream& os, const GraphEsnSom& graphesnsom)
{
  (void)graphesnsom;
  // TODO: print out the graphesnsom
  return os;
} // function operator<<

} // namespace model
} // namespace moka

#ifndef MOKA_MODEL_GRAPHESNSOM_H
#define MOKA_MODEL_GRAPHESNSOM_H

#include <vector>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <moka/dataset/multilabeledgraphdataset.h>
#include <moka/ml/graphreservoir.h>
#include <moka/ml/linearreadout.h>
#include <moka/ml/supersom.h>
#include <moka/model/model.h>
#include <moka/structure/multilabeledgraph.h>
#include <moka/util/math.h>

namespace moka {
namespace model {

/**
 * Class GraphEsnSom
 *
 * Represents a GraphESN-SOM model: composed by a graph reservoir, a SOM and a
 * linear readout layer, in other words is a GraphESN [2] plus a Self-Organizing
 * Map [3] as state mapping function.
 *
 * This class takes a set of parameters listed below:
 *
 *   - <input-size>: number of units in the input layer (Nu). This parameter is
 *       automatically setted looking at the training set instances.
 *   - <output-size>: number of units in the readout (Ny). This parameter is
 *       automatically setted looking at the training set instances.
 *
 * Reservoir parameters:
 *   - <reservoir-size>: number of hidden units in the reservoir (Nr).
 *   - <reservoir-connectivity>: degree of connectivity of reservoir
 *       connections. Value in [0,1] with 1 as default. For example: if a
 *       connectivity value of 0.4 is setted this mean that there are only
 *       the 40% of connections; in other words the reservoir matrix What is
 *       sparse to 40%.
 *   - <reservoir-input-scaling>: the (positive) real number w_in: the weights
 *       of the input-to-reservoir connections are initialized in the range
 *       [-w_in, +w_in]; by default is 0.1.
 *   - <reservoir-max-degree>: the maximum vertex degree over the graph input
 *       domain (also denoted by k). This value is used along with sigma to
 *       scale the reservoir weights. Futhermore this parameter is automatically
 *       setted looking at the training set instances.
 *   - <reservoir-sigma>: the contraction coefficient [2]; by default is 0.9.
 *       Note that the definition of contractivity (and then the value of
 *       sigma) is taken as in [2] and different from that in [1]. In this case
 *       is taken the local contractivity while in [1] is taken a "global"
 *       contractivity.
 *   - <reservoir-epsilon>: threshold of the fixed point in the encoding
 *       process [2]. By default is 1e-2.
 *   - <reservoir-max-iters>: sets the max number of iterations in the encoding
 *       process in order to avoid infinite loop if the computation does not
 *       converge. By default is 10000.
 *   - <reservoir-rseed>: the random number generator seed (optional). Random
 *       numbers are used in the initialization of the input weights and
 *       reservoir weights. By setting all the same parameters and the same
 *       seed will be built always the same reservoir and input matrix. By
 *       default is time(NULL).
 *
 * SOM Parameters:
 *   - <som-no-rows>: number of rows in the map.
 *   - <som-no-cols>: number of columns in the map.
 *   - <som-rseed>: the random number generator seed used to initialize the
 *       SOM (optional). By default is time(NULL).
 *   - <som-training-type>: the type of training:
 *       - "unsupervised": this is the default. If you don't set any training
 *           parameters will be done training in 3 phase: rough, fine, and
 *           final phase.
 *       - "supervised": the 3th phase is the supervised phase, using the LVQ3
 *           supervised method.
 *   - <som-no-epochs-1>: number of epochs in the rough train phase (optional).
 *   - <som-alpha-1>: learning rate on the rough train phase (optional).
 *   - <som-sigma-fin-1>: in the rough train phase sigma is decreased linearly
 *       up to this final radius (optional).
 *   - <som-no-epochs-2>: number of epochs in the fine tune phase (optional).
 *   - <som-alpha-2>: learning rate on the fine tune phase (optional).
 *   - <som-sigma-2>: radius for the fine tune phase (optional).
 *   - <som-sigma-fin-2>: final radius in the fine tune phase (optional).
 *   - <som-no-epochs-3>: number of epochs in last phase / supervised train
 *       (optional).
 *   - <som-alpha-3>: learning rate for the last phase / supervised train
 *       (optional).
 *   - <som-sigma-3>: radius for the last phase / supervised train (optional).
 *   - <som-sigma-fin-3>: final radius in for the last phase / supervised
 *       train (optional).
 *   - <som-act-fun-gamma>: parameter gamma for the unit activation function:
 *       f(x) = 1 / (1 + (x / gamma)^2). By default is 0.01.
 *   - <som-load-file>: you can load a previous saved SOM from file. In
 *       this case all the above parameters are ignored.
 *   - <som-save-file>: if you want save the map after the training, you
 *       can set in this parameter the file where you want to save the map.
 *   - <som-data-save-file>: if you give a file name all the SOM training data
 *       (i.e. all the vertices of each graph in the state space) will be
 *       saved, in this file with one or more labels. This file will may be
 *       used to calibrate (to label) the SOM.
 *
 * Regularization parameters used during the readout training:
 *   - <regularization> (optional): the regularization method to apply in the
 *       training of the readout weights. Can be one of the following:
 *         - "none": no regularization (equivalent to don't set the parameter).
 *         - "ridge-regression": regularization via ridge regression, this
 *             method require a further parameter:
 *               - <ridge-regression-lambda>: the parameter "lambda" of the
 *                   ridge regression method.
 *         - "lasso": regularization via the LASSO:
 *               - <lasso-lambda>: the parameter "lambda" of the LASSO method.
 *         - "elastic-net": regularization via the Elastic Net that finds
 *              the weights by solving:
 *                min_w 0.5 ||w X  - y||_2^2 + lambda1 ||w||_1 +
 *                          0.5 lambda2 ||w||_2^2
 *              for each row of W_out. This method requires two parameters:
 *               - <elastic-net-lambda1>: the parameter "lambda1" that control
 *                   the L1 penalization term.
 *               - <elastic-net-lambda2>: the parameter "lambda2" that control
 *                   the L2 penalization term.
 *
 * Other parameters:
 *   - <state-vector-type>: type of values to put in the state vector (out of
 *       the SOM clustering). Available values are:
 *         - "binary": only 1 or 0 values to indicate presence or absence of
 *             elements in the cluster. This is the default value.
 *         - "cauchy": given the distance D between the reservoir vector and
 *             the codebook, the value used in the final state vector is the
 *             result of the cauchy hat function applied to the distance D,
 *             that is the SOM unit activation function:
 *               f(D) = 1 / (1 + (D / <som-act-fun-gamma>)^2)
 *         - "sum": will be taken the sum of the reservoir vector elements.
 *         - "reservoir": the entire reservoir state is keept. The states are
 *              then concatenated to form the final vector.
 *       Note that if more than one state end up in the same cluster is taken
 *       the average of the final values.
 *   - <instances-info-save-file>: save on file info on input instances as
 *       weights assigned to each vertex. You can provide a file name here and
 *       a file will be created at end of training phase. Such file can then be
 *       used by moka-extra-create-svg to create SVGs files.
 *   - <unit-info-save-file>: a collection of informations associated with each
 *       unit of the SOM are saved on the specified file at the end of the
 *       training process. Such file can be used in MOKA-TOOL to show info
 *       about each unit. Note that the information collection process could be
 *       time consuming since the map labeling (on the SOM) is computed for
 *       each training instance.
 *
 * References:
 *   [1] C. Gallicchio, A. Micheli. Graph echo state networks.
 *   [2] C. Gallicchio, A. Micheli. Supervised State Mapping of Clustered
 *       GraphESN States.
 *   [3] T. Kohonen. The Self-Organizing Map. 1990.
 */
class GraphEsnSom : public Model
{
  public:
    typedef ::moka::Global::Int Int;
    typedef ::moka::ml::GraphReservoir< ::moka::structure::MultiLabeledGraph >
    Reservoir;
    typedef ::moka::util::Math::Vector Vector;
    typedef ::moka::util::Math::Matrix Matrix;

    GraphEsnSom();
    GraphEsnSom(const GraphEsnSom& graphesnsom);
    virtual ~GraphEsnSom();

    virtual void bindTrainingSet(const dataset::Dataset *trainingset);
    virtual bool compareOutput(const Vector& output) const;
    virtual bool comparePerformance(const Real& p1, const Real& p2) const;
    virtual const Vector& compute(const structure::MultiLabeledGraph& input);
    virtual void computeOn(
        const dataset::Dataset& ds, std::ostream& os, bool compare = false);
    virtual void computeOnTestFold(std::ostream& os, bool compare = false);
    virtual void computeOnTrainingSet(std::ostream& os, bool compare = false);
    virtual void evaluateOn(
        const dataset::Dataset& ds,
        const std::string& eq_filename,
        const std::string& ii_filename);
    virtual Uint getInputSize() const;
    virtual const Vector& getLastOutput() const;
    virtual const Vector& getLastState() const;
    virtual const structure::MultiLabeledGraph& getLastStateGraph() const;
    virtual std::string getModelEquation() const;
    virtual Uint getOutputSize() const;
    virtual const ml::LinearReadout& getReadout() const;
    virtual const Reservoir& getReservoir() const;
    virtual Uint getReservoirSize() const;
    virtual const ml::SuperSOM& getSOM() const;
    virtual util::Info<std::string> info() const;
    virtual void init();
    virtual GraphEsnSom& operator=(const GraphEsnSom& graphesnsom);
    virtual void read(std::istream& is);
    virtual void reset();
    virtual const NumericResults& test();
    virtual const NumericResults& testOn(const dataset::Dataset& testset);
    virtual const NumericResults& train();
    virtual void write(std::ostream& os) const;

    template <typename Iterator>
    bool compareOutput(Iterator begin) const;

    template <typename Iterator1, typename Iterator2>
    static bool compareOutputs(
        Iterator1 first1,
        Iterator1 last1,
        Iterator2 first2);

  protected:
    void clearObject();
    void clearObjectForInit(bool clear_base_model = true);
    void copyObject(const GraphEsnSom& graphesnsom);

  private:
    enum StateVectorType
    {
      cauchy_state_vect,
      binary_state_vect,
      sum_state_vect,
      reservoir_state_vect
    };

    // Training set (bind)
    const dataset::MultiLabeledGraphDataset *m_trainingset;

    // Backup copy
    GraphEsnSom *m_graphesnsom_bak;

    // Reservoir
    Reservoir m_reservoir;

    // SOM
    ml::SuperSOM m_som;
    std::string m_som_training_type;
    std::string m_som_load_file, m_som_save_file, m_som_data_save_file;

    // State vector
    StateVectorType m_state_vect_type;
    Vector m_state_vect;

    // Readout
    ml::LinearReadout m_readout;

    // Results containers
    NumericResults m_training_res, m_test_res;

    // Other parameters
    std::string m_instances_info_save_file;
    std::string m_unit_info_save_file;

    // Private methods
    bool checkDatasetCompatibility(
        const dataset::MultiLabeledGraphDataset& ds);

    bool checkParameters();

    void collectReservoirStates(
        const dataset::MultiLabeledGraphDataset& training_set,
        ml::SuperSOM::DataContainer& states_container,
        std::vector<Real>& states_class,
        std::list<structure::MultiLabeledGraph>& state_graphs,
        Real& avg_iterations);

    void computeReservoirStateVector(
        const structure::MultiLabeledGraph& state_graph);

    void computeStateVector(
        const structure::MultiLabeledGraph& state_graph);

    const structure::MultiLabeledGraph& encodingProcess(
        const structure::MultiLabeledGraph& input);

    void initResultsContainers();

    void initTestResultsContainer();

    void initTrainingResultsContainer();

    void makeBackupCopy();

    const Vector& outputProcess(const Vector& state_vector);

    void parseParameters();

    bool saveInstancesInfo(
        const std::string& filename,
        const std::list<structure::MultiLabeledGraph>& state_graphs,
        const dataset::MultiLabeledGraphDataset& training_dataset,
        const Matrix& outputs) const;

    bool saveSomDataOnFile(
        const std::string& filename,
        const std::list<structure::MultiLabeledGraph>& state_graphs,
        const dataset::MultiLabeledGraphDataset& training_dataset) const;

    bool saveUnitInfo(
        const std::string& filename,
        const std::list<structure::MultiLabeledGraph>& state_graphs) const;

    const Vector& stateMappingFunctionProcess(
        const structure::MultiLabeledGraph& state_graph);

    StateVectorType strToStv(const std::string& str) const;

    std::string stvToStr(const StateVectorType& stv) const;

    bool writeInstanceEquation(
        std::ostream& os,
        const structure::MultiLabeledGraph& state_graph) const;

    bool writeVerticesInfo(
        std::ostream& os,
        const structure::MultiLabeledGraph& state_graph) const;

#ifdef MOKA_TMP_CODE
    // Temporary methods (TODO: remove these as soon as possible)
    bool saveInstancesInfoAll(
        const std::string& filename,
        const std::list<structure::MultiLabeledGraph>& state_graphs,
        const dataset::MultiLabeledGraphDataset& training_dataset,
        const Matrix& outputs) const;

    bool saveInstancesInfoTsf(
        const std::string& filename,
        const std::list<structure::MultiLabeledGraph>& state_graphs,
        const dataset::MultiLabeledGraphDataset& training_dataset,
        const Matrix& outputs) const;

    void writeSelectedFragments(
        const std::string& filename,
        const std::list<structure::MultiLabeledGraph>& state_graphs) const;

#endif // MOKA_TMP_CODE

}; // class GraphEsnSom

// ===================
// AUXILIARY FUNCTIONS
// ===================

std::ostream& operator<<(std::ostream& os, const GraphEsnSom& gesm);

// ==================
// METHOD DEFINITIONS
// ==================

/**
 * Method compareOutput
 *
 * Compares the last output (see the method "getLastOutput") with the passed
 * output starts at "begin". Returns true if the two ouptuts are equivalent
 * (see the method "compareOutputs"), false otherwise.
 *
 * A moka::GenericException exception will be thrown if the the model is not
 * initialized (see method init).
 */
template <typename Iterator>
bool GraphEsnSom::compareOutput(Iterator begin) const
{
  if (!isInitialized())
    throw moka::GenericException(
        "GraphEsnSom::compareOutput: the model is not initialized");

  const Vector &last_output = getLastOutput();

  return compareOutputs(last_output.begin(), last_output.end(), begin);
} // method compareOutput

/**
 * Method compareOutputs
 *
 * Compares two outputs: the first starts at "first1" and ends at "last1",
 * the second output starts at "first2" and must have the same size (number of
 * elements) of the first one.
 * Returns true if the two ouptuts are equivalent, false otherwise.
 * Two ouptuts are considered equivalent if they have the same number of
 * elements and each element is equal in sign to the another one.
 */
template <typename Iterator1, typename Iterator2>
bool GraphEsnSom::compareOutputs
(
    Iterator1 first1,
    Iterator1 last1,
    Iterator2 first2
)
{
  return std::equal
      (
        first1,
        last1,
        first2,
        (boost::lambda::_1 < 0 && boost::lambda::_2 < 0) ||
        (boost::lambda::_1 >= 0 && boost::lambda::_2 >= 0)
      );
} // method compareOutputs

} // namespace model
} // namespace moka

#endif // MOKA_MODEL_GRAPHESNSOM_H

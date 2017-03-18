#include "crossvalidation.h"

#include <fstream>
#include <boost/filesystem.hpp>
#include <moka/exception.h>
#include <moka/log.h>
#include <moka/util/math.h>

namespace moka {
namespace procedure {

using namespace model;
using namespace dataset;
using namespace util;

namespace bfs  = boost::filesystem;

/**
 * Constructor
 *
 * Constructs an object of type CrossValidation.
 */
CrossValidation::CrossValidation() :
  m_model(NULL),
  m_training_set(NULL),
  m_test_set(NULL),
  m_params(NULL)
{
  clearObject();
} // constructor

/**
 * Destructor
 */
CrossValidation::~CrossValidation()
{
  // m_model: is a bind (not to be deleted)
  // m_training_set: is a bind (not to be deleted)
  // m_test_set: is a bind (not to be deleted)
  delete m_params;
} // destructor

// ==============
// PUBLIC METHODS
// ==============

/**
 * Method bindModel
 *
 * Binds your Model with this object. The model will be used in the method
 * start on the training set (see bindTrainingDataset) to train it and make
 * validation, and will be used on the test set (see bindTestDataset) to make
 * an additional test during the cross validation procedure.
 * You must configure your model setting on it suitable parameters before to
 * call the method start.
 * Note also that datasets loaded must be compatible with the model, otherwise
 * errors occur in the method start.
 */
void CrossValidation::bindModel(Model* model)
{
  m_model = model;
  return;
} // method bindModel

/**
 * Method bindTestSet
 *
 * Binds a test set on which the model will be tested. This dataset will be
 * passed in the binded model during the cross validation procedure in order to
 * test the trained model.
 *
 * IMPORTANT: the test set must be such that the loaded instances will be
 * equal to those loaded in the training set (e.g. with the same inputs
 * format and the same number of outputs). As good rule you should use the
 * same parameters used for load the training set, except for the file name.
 */
void CrossValidation::bindTestSet(Dataset *test_set)
{
  m_test_set = test_set;
  return;
} // method bindTestSet

/**
 * Method bindTrainingSet
 *
 * Binds the training set on which the model will be builded and trained.
 * Note that the dataset must be compatible with the binded model, otherwise
 * errors occur during the method start.
 */
void CrossValidation::bindTrainingSet(Dataset *training_set)
{
  m_training_set = training_set;
  return;
} // method bindTrainingSet

/**
 * Method getModelMaker
 *
 * Returns a const reference to the bound model, mainly to use when a signal is
 * handled in order to access at the model information without modify it.
 *
 * An exception of type moka::GenericException will be throw if there isn't
 * any binded model.
 */
const Model& CrossValidation::getModel() const
{
  if (!m_model)
  {
    Log::err << "CrossValidation::getModel: error: null model." << Log::endl;
    throw moka::GenericException("no model is setted");
  }
  return *m_model;
} // method getModel

/**
 * Method getAvgVaPerformance
 *
 * Returns the average performance given by the model on test folds of the
 * training set, averaged on the whole cross validation process, i.e. the
 * average over all the c.v. times of the test performance averaged over the
 * c.v. steps. If you know the specific type of Model you can use this measure
 * knowingly, otherwise you can use the method Model::comparePerformance
 * to compare two different measures.
 *
 * Note: it make sense to read these result after the execution of the method
 * start, not during the execution itself, e.g. while handling a signal,
 * since in such case can contains intermediate results.
 */
Global::Real CrossValidation::getAvgVaPerformance() const
{
  return m_avg_va_perf;
} // method getAvgVaPerformance

/**
 * Method getAvgTsPerformance
 *
 * If a test set is supplied, returns the average performance given by the
 * model on the test set averaged on the whole cross validation process, i.e.
 * the average over all the c.v. times of the test performance averaged over
 * the c.v. steps.
 * If you know the specific type of Model you can use this measure knowingly,
 * otherwise you can use the method Model::comparePerformance to compare two
 * different measures.
 *
 * It make sense to read these result after the execution of the method
 * start, not during the execution itself, e.g. while handling a signal,
 * since in such case can contains intermediate results.
 */
Global::Real CrossValidation::getAvgTsPerformance() const
{
  return m_avg_ts_perf;
} // method getAvgTsPerformance

/**
 * Method getCvFolds
 *
 * Returns the number of folds (setted by the parameter <cv-folds>) used during
 * the last call of method start.
 * You can read this value also during the execution of the method start
 * (after the signal sigCrossValidationStart), e.g. handling a signal.
 */
Global::Uint CrossValidation::getCvFolds() const
{
  return m_cv_folds;
} // method getCvFolds

/**
 * Method getCvSteps
 *
 * Returns the number of steps (setted by the parameter <cv-steps>) used during
 * the last call of method start.
 * You can read this value also during the execution of the method start
 * (after the signal sigCrossValidationStart), e.g. handling a signal.
 */
Global::Uint CrossValidation::getCvSteps() const
{
  return m_cv_steps;
} // method getCvSteps

/**
 * Method getCvTimes
 *
 * Returns the number of times (setted by the parameter <cv-times>) used during
 * the last call of the method start.
 * You can read this value also during the execution of the method start
 * (after the signal sigCrossValidationStart), e.g. handling a signal.
 */
Global::Uint CrossValidation::getCvTimes() const
{
  return m_cv_times;
} // method getCvTimes

/**
 * Method getTestSet
 *
 * Returns a const reference to the current test set (binded using the
 * "bindTestSet" method). You can use this method to display some
 * information about the binded test set (if you know the real type of dataset
 * contained you can do a dynamic cast in order to get more informations).
 * If no dataset was binded an exception of type moka::GenericException will
 * be thrown.
 */
const Dataset& CrossValidation::getTestSet() const
{
  if (!m_test_set)
  {
    Log::err << "CrossValidation::getTestSet: error: null dataset."
             << Log::endl;
    throw moka::GenericException("not test set is setted");
  }
  return *m_test_set;
} // method getTestSet

/**
 * Method getTrainingSet
 *
 * Returns a const reference to the current training set (binded using the
 * "bindTrainingSet" method). You can use this method to display some
 * information about the dataset loaded (if you know the real type of dataset
 * contained you can do a dynamic cast in order to get more informations).
 * If no dataset was loaded an exception of type moka::GenericException will
 * be thrown.
 */
const Dataset& CrossValidation::getTrainingSet() const
{
  if (!m_training_set)
  {
    Log::err << "CrossValidation::getTrainingSet: error: null dataset."
             << Log::endl;
    throw moka::GenericException("you must load a training set");
  }
  return *m_training_set;
} // method getTrainingSet

/**
 * Method getResults
 *
 * The model returns a set of numeric results at the end of the training and
 * test processes. This method returns such results obtained during the last
 * call of the method "start" collected in a object of type Info<std::string>.
 * Such results are reported in a different way depending by the number of
 * steps (see parameters <cv-folds> and <cv-steps>) and by the number of times
 * (see the parameter <cv-times>) of the cross validation process.
 * They are returned into an object of type Info that can quickly printed
 * by the operator << (see the class Info).
 *
 * If there were 1 step and 1 time the returned results are the same returned
 * by the model object.
 *
 * If there was more than 1 steps, this method returns the average and the
 * standard deviation of the numerical results over the computed steps, both
 * into the field "value" of the returned Info object in a string formatted as
 * follows:
 *   "<avg> (+/- <stdev>)"
 * and the field "note" contains the string:
 *   "on <cv-steps> steps"
 *
 * If the cross validation process was repeated more than one times, i.e. the
 * parameter <cv-times> was greater than 1, the returned results are the
 * average and the standard deviation over all the times of the average values
 * over all the steps of each time. In this case the fields "note" contains a
 * string in the form:
 *   "on <cv-times> times"
 *
 * It make sense to read these results after the execution of the method
 * start, not during the execution itself, e.g. while handling a signal,
 * since in such case can contains intermediate results.
 */
Info<std::string> CrossValidation::getResults() const
{
  return m_final_results;
} // method getResults

/**
 * Mehod removeTestSet
 *
 * If was binded a test dataset (see bindTestDataset) it is removed.
 */
void CrossValidation::removeTestSet()
{
  m_test_set = NULL;
  return;
} // method removeTestSet

/**
 * Method setParameters
 *
 * See setParameters(Parameters &)
 *
 * IMPORTANT: the passed Parameters object is not copied but become property
 * of this object, then the object pointed by <params> will be deleted by
 * this object (in its destructor or when no longer useful) and changes to
 * parameters after they are passed to this method are not legal and cause
 * errors.
 */
void CrossValidation::setParameters(Parameters *params)
{
  delete m_params;
  m_params = params;
  return;
} // method setParameters

/**
 * Method setParameters
 *
 * You can set parameters for the cross validation procedure passing them to
 * this method. Available parameters are listed below.
 *
 * Cross validation parameters:
 *   - <cv-folds>: number of folds to split the dataset in order to do the
 *       cross validation. If it is set to 1 the entire dataset will be used
 *       for training without a test process.
 *   - <cv-steps> (optional): an integer indicating the number of steps to do in
 *       the cross validation, in other words the number of folds to use in the
 *       cross validation process. Must be a number between 1 and <cv-folds>.
 *       The default is <cv-folds> and will be made the full k-fold cross
 *       validation. If it is set to 1 (and <cv-folds> is not 1) the simple
 *       validation is made using only one fold (the first) for the test
 *       process.
 *   - <cv-times> (optional): an integer determining the number of times to
 *       repeat the whole cross-validation. The default is 1. If you also set
 *       the parameter <dataset-rseed> (see below) at each time the dataset
 *       is random shuffled so you can test your model on dataset with
 *       different intances order. Futhermore, if the building of your model
 *       (by the method init) involves a random initialization, at each time a
 *       different random initialization is taken.
 *   - <cv-times-dataset-shuffle> (optional): a boolean value, true as default;
 *       if you set to true the parameter <dataset-rseed> at each time the
 *       dataset is random shuffled. By setting this parameter as false the
 *       dataset is shuffled only in the first time, then is keeped the same
 *       partitioning on each succeeding times.
 *   - <train-fail-behaviour> (optional): behavior in case of failure of the
 *       training. Valid values are:
 *         - "stop":   the procedure is stopped (default value).
 *         - "repeat": try to repeat the model training, restarting from the
 *                     last time by reinitializing the model.
 *   Other parameters:
 *   - <stratified-split> (optional): use the stratified splitting instead of
 *       the classic splitting. To use with care since works properly only in
 *       limited cases: only when instances have one single discrete output.
 *   - <dataset-rseed> (optional): the random number generator seed for the
 *       training set, it is used for random shuffle the dataset instances
 *       before start the training process. If <cv-times> is greater than 1,
 *       the seed is setted only the first time, so if you use always the same
 *       seed the instances are reordered always in the same way at each time.
 *       If it is 0 will be taken time(NULL) as value. If you set the parameter
 *       <cv-times-dataset-shuffle> to false and there are more than one times
 *       the dataset will be shuffled only at the start of the whole process.
 *       If it is not setted the dataset will not be shuffled.
 *   - <save-outputs-file> (optional): if you set a file name in this parameter
 *       after that a new model has been built, it is used on binded datasets
 *       to produce outputs for each instance and such outputs are saved into
 *       the setted file. A suffix ".tr" (training), .tsf (test fold) and ".ts"
 *       (test) will be appended respectively for outputs of the training set,
 *       test fold (if any) and of the test set (if it has been binded).
 *       Futhermore, before the training/test suffix, an other suffix ".t.s"
 *       will be appended where t is the time number (see <cv-times>) and m is
 *       the step number (see <cv-steps>).
 *       ATTENTION: if the file(s) already exits will be overwritten.
 *
 * NOTE: this method only set the Parameters object, without checking its
 * content. If there are errors in the passed parameters these will be found
 * during the execution of the method start.
 *
 * NOTE: the passed Parameters object is copied into this object, then it can
 * be memory and time consuming if the passed object is huge. In such case
 * consider passing by pointer (see setParameters(Parameters*)).
 */
void CrossValidation::setParameters(const Parameters& params)
{
  delete m_params;
  m_params = new Parameters(params);
  return;
} // method setParameters

/**
 * Method start
 *
 * Starts the cross validation procedure following the setted parameters (see
 * "setParameters"). Before to call this method you must set the parameters,
 * bind a model (see "bindModel") and load a training set that must be
 * compatible with the chosen model (see "bindTrainingSet"). You can bind once
 * the dataset and reuse more than once this method changing the model and/or
 * the cross validation parameters.
 * This method uses the binded model (that must be appropriately configured
 * with desired parameters) to train it on the training partitioning of the
 * training set, then test the model created on the test partition. This is
 * repeated for each setted fold resetting the model each time. You can change
 * the type of cross validation process by changing the object parameters
 * (method "setParameters") and you can change the behaviour of your model by
 * changing the model configuration.
 *
 * IMPORTANT: note that the model chosen must be compatible with the binded
 * training dataset and vicevera (e.g. if you choose a GraphEsnSom you must
 * bind a MultiLabelledGraphDataset) otherwise an exception will be throw.
 *
 * More precisley this method does follows steps:
 *   - Checks and parse setted parameters.
 *   - Binds datasets to the model object.
 *   - For each time (see the parameter <cv-times>):
 *       - If it is setted the parameter <dataset-random-seed> the training
 *         set is radomly shuffled (only the first time if <cv-times-dataset-
 *         shuffle> is false).
 *       - Initializes the model calling its init method (Model::init).
 *       - For each cross validation steps (e.g. for each fold in which is
 *         divided the training set):
 *           - The model is resetted to its initialization point (using the
 *             method Model::reset).
 *           - The model is trained on the training part of the training set,
 *             i.e. all the training set except the test fold (using the method
 *             Model::train).
 *           - The trained model is tested on the test fold (using the method
 *             Model::test).
 *           - If there is a test set, the trained model is tested on the test
 *             set (using the method Model::testOn).
 *           - All the results returned by the model are accumulated.
 *           - If the parameter <save-outputs-file> is setted the model outputs
 *             are saved on file.
 *       - The average and the standard deviation of the results over one cross
 *         validation process are computed and accumulated.
 *   - The final results are computed, that are the average of average and of
 *     standard deviation over all the repeated times.
 *
 * During the execution a set of signals (boost::signals2) will be emitted by
 * this method in order to report the start or the end of certain steps or also
 * to communicate intermediate results. Each signal passes a const reference
 * to this CrossValidation object that can be readed in order to get more
 * informations without modify it. The signals are emitted in the following
 * order:
 *   -> sigCrossValidationStart: emitted after that all the parameters are
 *        checked and the process can start.
 *   -> sigModelInitialized: emitted just after the model is initialized but
 *        is not still trained (after the call of the method ModelMaker::init),
 *        you can access the model to get initialization info. Since this is
 *        made at the start of each repeat time (see the parameter <cv-times>)
 *        in the second argument the signal reports the time number.
 *   -> sigStepStart: start one cross validation step by training the model.
 *        You can read the training dataset to get the number of cross
 *        validation folds and the fold used for test. On the second argument
 *        is passed the c.v. step number.
 *   -> sigTrainingFail: if the training is failed this signal is emitted. If
 *        the behaviour is to stop the training this is the last signal emitted
 *        (then the procedure end), if instead the setted behaviour is to repeat
 *        the training the procedure restart by reinitilize the model (then
 *        the signal sigModelInitialized is emitted). The signal report also an
 *        error description message.
 *   -> sigStepEnd: the cross validation step is finished: the model has been
 *        trained and tested. It is passed also the step number (just finished).
 *        The signal reports in the third argument the results obtained during
 *        this step.
 *   -> sigSaveOutputsEnd: if the options <save-outputs> is setted, this
 *        signals when it has finished to save the outputs. The signal report
 *        the filename where the outputs are saved and a boolean value, "true"
 *        if the writing has success, "false" otherwise.
 *   -> sigTimeEnd: emitted when a time (of <cv-times>) is finished, passing
 *        also the time number. The signal reports in the third argument the
 *        results obtained during this time (the results avareged over all the
 *        steps). If there is only one time (i.e. <cv-times> is 1) these results
 *        are also the final results, otherwise will be averaged on all the
 *        times to get the final results.
 *   -> sigCrossValidationEnd: emitted when the whole cross validation process
 *        has ended without errors (if errors occur this signal is not emitted).
 *        From now you can now access the method getResults to get the cross
 *        validation final results.
 *
 * If the set of parameters in not valid, if the model or the training set have
 * not been binded, an exception will be throw of type moka::GenericException
 * with the error description and the results are left empty.
 */
void CrossValidation::start()
{
  clearResults();

  // Checks the parameters
  std::string error_description;
  if (!checkParameters(error_description))
    throw moka::GenericException("invalid parameters: " + error_description);

  // Parse setted parameters
  parseParameters();

  // Checks the training set
  if (!m_training_set)
    throw moka::GenericException("you must load a (non empty) training set");

  if (m_training_set->isEmpty())
    throw moka::GenericException("the training set can not be empty");

  // Checks the model
  if (!m_model)
    throw moka::GenericException("you must set a valid model");

  // Try to bind the dataset to the model
  try
  {
    // throw an exception if the dataset type is not correct
    m_model->bindTrainingSet(m_training_set);
  } // try
  catch (std::exception& ex)
  {
    Log::err << "CrossValidation::start: error binding the dataset to the "
             << "model: " << ex.what() << Log::endl;

    throw moka::GenericException(
        "the model can not be created due to an error on the dataset");
  } // try-catch

  // Signal the starting of cross validation procedure
  sigCrossValidationStart(*this);

  // Repeat cv-times the procedure
  Uint t = 0;
  while (t < m_cv_times)
  {
    // Initializes the training set
    m_training_set->restore();
    if (m_params->contains("dataset-rseed"))
    {
      if (t == 0)
      {
        if (m_dataset_rseed == 0)
          m_dataset_rseed = Global::timeNull() % 100000;
        m_training_set->setRandomNumberGeneratorSeed(m_dataset_rseed);
      }
      else if (!m_cv_times_dataset_shuffle)
        m_training_set->setRandomNumberGeneratorSeed(m_dataset_rseed);

      m_training_set->randomShuffleDataset();
    } // if

    // Try to split the dataset
    try
    {
      if (m_stratified_split)
        m_training_set->splitInStratifiedFolds(m_cv_folds);
      else
        m_training_set->splitInFolds(m_cv_folds);
    } // try
    catch (std::exception& ex)
    {
      Log::err << "CrossValidation::start: error splitting the training set: "
               << ex.what() << "." << Log::endl;

      throw moka::GenericException("error splitting the training set");
    } // try-catch

    // Initialize the model
    try
    {
      m_model->init();
    }
    catch (std::exception& ex)
    {
      Log::err << "CrossValidation::start: error initializing the model: "
               << ex.what() << "." << Log::endl;

      throw moka::GenericException(
          "the model can not be created due to an initialization error");
    } // try-catch

    // Signals the correct model initialization (at time t+1)
    sigModelInitialized(*this, t + 1);

    // Start the cross validation
    clearTimeResults(); // also clear m_time_results

    bool train_ok = false;
    for (Uint s = 0; s < m_cv_steps; ++s)
    {
      // Set the test fold
      m_training_set->setTestFold(s);

      // Reset the model
      m_model->reset();

      // Start model training
      sigStepStart(*this, s + 1);

      util::Info<Real> train_res;
      try
      {
        train_res = m_model->train();
        train_ok = true;
      }
      catch (std::exception& ex)
      {
        train_ok = false;

        Log::err << "CrossValidation::start: error during the training "
                 << "procedure: " << ex.what() << "." << Log::endl;

        sigTrainingFail(*this, ex.what());

        break;
      } // try-catch

      if (train_ok)
      {
        m_step_tr_res = train_res;

        // Model test
        if (m_training_set->getTestFoldSize() != 0)
        {
          m_step_va_res = m_model->test();
          m_avg_time_va_perf += m_model->getTestPerformance();
        }

        // Model test on external test set
        if (thereIsTestSet()) try
        {
          // throw an exception if the dataset type is not correct
          m_step_ts_res = m_model->testOn(*m_test_set);
          m_avg_time_ts_perf += m_model->getTestPerformance();
        } // if-try
        catch (std::exception& ex)
        {
          Log::err << "CrossValidation::start: error setting the test set: "
                   << ex.what() << ". The test phase is skipped." << Log::endl;
          m_step_ts_res.clear();
        } // try-catch

        accumulateStepsResults(); // also fills m_step_results
        sigStepEnd(*this, m_step_results, s + 1);

        // saves outputs on file
        if (!m_save_output_file.empty())
          saveOutputs(m_save_output_file, t + 1, s + 1);

      } // if (train_ok)

    } // for m

    if (train_ok)
    {
      accumulateTimesResults(); // also fills m_time_results
      sigTimeEnd(*this, m_time_results, t + 1);
      ++t;
    }

    else if (m_train_fail_behaviour == stop_training)
    {
      clearResults();
      return;
    }

  } // while (t < m_cv_times)

  computeFinalResults();
  sigCrossValidationEnd(*this);

  return;
} // method start

/**
 * Method thereIsTestSet
 *
 * Return true if a test dataset is binded to this object, false otherwise.
 */
bool CrossValidation::thereIsTestSet() const
{
  return m_test_set;
} // method thereIsTestSet

/**
 * Method thereIsTrainingSet
 *
 * Return true if a training dataset is binded to this object, false otherwise.
 */
bool CrossValidation::thereIsTrainingSet() const
{
  return m_training_set;
} // method thereIsTrainingSet

// ===============
// PRIVATE METHODS
// ===============

/**
 * Method accumulateStepsResults
 *
 * Support method for the method "start".
 * Taking results from "m_step_tr_res", "m_step_va_res" and "m_step_ts_res",
 * fills the object "m_step_results" and put such results in "m_steps_res_accm"
 * in order to calculate average and standard deviation later, after the last
 * step.
 */
void CrossValidation::accumulateStepsResults()
{
  if (m_steps_res_accm.isEmpty())
  {
    // Adds training results entries
    for (size_t i = 0; i < m_step_tr_res.getSize(); ++i)
      m_steps_res_accm.pushBack(
          "tr_" + m_step_tr_res[i].key,
          "Tr. " + m_step_tr_res[i].name,
          std::list<Real>(),
          m_step_tr_res[i].note);

    // Adds validation results entries
    for (size_t i = 0; i < m_step_va_res.getSize(); ++i)
      m_steps_res_accm.pushBack(
          "va_" + m_step_va_res[i].key,
          "Va. " + m_step_va_res[i].name,
          std::list<Real>(),
          m_step_va_res[i].note);

    // Adds test results entries
    for (size_t i = 0; i < m_step_ts_res.getSize(); ++i)
      m_steps_res_accm.pushBack(
          "test_" + m_step_ts_res[i].key,
          "Test " + m_step_ts_res[i].name,
          std::list<Real>(),
          m_step_ts_res[i].note);

  } // if

  Uint result_size = m_step_tr_res.getSize() + m_step_va_res.getSize() +
      m_step_ts_res.getSize();

  // Some checks
  if (m_steps_res_accm.getSize() != result_size)
  {
    Log::err << "CrossValidation::accumulateStepResults: fatal error: results "
             << "are different in size (there is a bug in the code!)"
             << Log::endl;
    m_steps_res_accm.clear(); // try to fix the error
    return;
  }

  // Push back training values
  size_t shift = 0;
  for (size_t i = 0; i < m_step_tr_res.getSize(); ++i)
    m_steps_res_accm[shift + i].value.push_back(m_step_tr_res[i].value);

  // Push back validation values
  shift += m_step_tr_res.getSize();
  for (size_t i = 0; i < m_step_va_res.getSize(); ++i)
    m_steps_res_accm[shift + i].value.push_back(m_step_va_res[i].value);

  // Push back test values
  shift += m_step_va_res.getSize();
  for (size_t i = 0; i < m_step_ts_res.getSize(); ++i)
    m_steps_res_accm[shift + i].value.push_back(m_step_ts_res[i].value);

  // Fills m_step_results
  if (m_step_results.isEmpty())
    for (size_t i = 0; i < m_steps_res_accm.getSize(); ++i)
      m_step_results.pushBack(
          m_steps_res_accm[i].key,
          m_steps_res_accm[i].name,
          "",
          m_steps_res_accm[i].note);

  if (m_step_results.getSize() != result_size)
  {
    Log::err << "CrossValidation::accumulateStepResults: fatal error: step "
             << "results are different in size (there is a bug in the code!)"
             << Log::endl;
    m_step_results.clear(); // try to fix the error
    return;
  }

  for (size_t i = 0; i < m_step_results.getSize(); ++i)
    m_step_results[i].value =
        Global::toString(m_steps_res_accm[i].value.back());

  return;
} // method accumulateStepsResults

/**
 * Method accumulateTimesResults
 *
 * Support method for the method "start".
 * Fills the object "m_time_results" computing the average and the std dev on
 * values accumulated in "m_steps_res_acc". If there is only one accumulated
 * value (i.e. <cv-steps> is 1) the std dev is not reported into the results.
 * Furthermore, accumulates step results into "m_time_res_accm" and values for
 * "m_avg_va_perf" and "m_avg_ts_perf", in order to calculate the mean later,
 * when the whole cross validation process is finish.
 */
void CrossValidation::accumulateTimesResults()
{
  if (!m_time_results.isEmpty()) // to be sure (there not should be needed)
    m_time_results.clear();

  if (m_times_res_accm.isEmpty())
  {
    for (size_t i = 0; i < m_steps_res_accm.getSize(); ++i)
      m_times_res_accm.pushBack(
          m_steps_res_accm[i].key,
          m_steps_res_accm[i].name,
          std::make_pair(std::list<Real>(), std::list<Real>()),
          m_steps_res_accm[i].note);
  } // for

  for (size_t i = 0; i < m_steps_res_accm.getSize(); ++i)
  {
    std::string res_value = "", res_note = "";

    // computes the average
    Real avg = Math::avg(m_steps_res_accm[i].value);

    res_value =
        Global::toString(avg) +
        (
          m_steps_res_accm[i].note.empty() ?
             "" : " " + m_steps_res_accm[i].note
        );

    // computes the standard deviation
    Real stdev = 0.0;
    size_t n_elements = m_steps_res_accm[i].value.size(); // m_cv_steps
    if (n_elements > 1)
    {
      stdev = Math::stdev(m_steps_res_accm[i].value, avg);
      res_value += " (+/- " + Global::toString(stdev) + ")";
      res_note = "on " + Global::toString(n_elements) + " steps";
    } // if

    // fills the object "m_time_results"
    m_time_results.pushBack(
        m_steps_res_accm[i].key,
        m_steps_res_accm[i].name,
        res_value,  // value = "<mean> <note> (+/- <stdev>)"
        res_note);  // note = "on <cv-steps> steps"

    // accumulate results into "m_times_res_accm" (average and std. dev.)
    m_times_res_accm[i].value.first.push_back(avg);
    m_times_res_accm[i].value.second.push_back(stdev);

  } // for i

  // accumulate "m_avg_vaperf" and "m_avg_tsperf"
  m_avg_time_va_perf = m_avg_time_va_perf / (Real)m_cv_steps;
  m_avg_time_ts_perf = m_avg_time_ts_perf / (Real)m_cv_steps;
  m_avg_va_perf += m_avg_time_va_perf;
  m_avg_ts_perf += m_avg_time_ts_perf;

  return;
} // method accumulateTimesResults

/**
 * Method checkParameters
 *
 * Checks values for parameters in m_params. If all the parameters are correct
 * return true and error_description is empty, otherwise return false and put
 * into error_description a short description on the wrong parameter.
 */
bool CrossValidation::checkParameters(std::string& error_description)
{
  typedef util::Prm Prm; // short alias for Parameters

  error_description.clear();

  if (!m_params)
  {
    error_description = "empty set of parameters";
    return false;
  }

  m_params->setVerboseMode();
  m_params->setMsgPrefix("CrossValidation::checkParameters: ");

  if (!m_params->check("cv-folds", Prm::required | Prm::uint | Prm::non_zero))
  {
    error_description = "invalid cross validation folds value";
    return false;
  }

  bool cv_steps_check = m_params->check
      (
        "cv-steps",
        Prm::optional | Prm::uint | Prm::in_range,
        "1",
        m_params->get("cv-folds")
      );

  if (!cv_steps_check)
  {
    error_description = "invalid number of steps";
    return false;
  }

  if (!m_params->check("cv-times", Prm::optional | Prm::uint | Prm::non_zero))
  {
    error_description = "invalid cross validation times value";
    return false;
  }

  if (m_params->contains("train-fail-behaviour"))
  {
    std::string train_fail_behaviour = m_params->get("train-fail-behaviour");
    if (train_fail_behaviour != "stop" && train_fail_behaviour != "repeat")
    {
      error_description =
          "invalid value in <train-fail-behaviour>: " + train_fail_behaviour;
      return false;
    }
  }

  if (!m_params->check("save-outputs-file", Prm::optional | Prm::non_empty))
  {
    error_description = "the file name where save the outputs is empty";
    return false;
  }

  return true;
} // method checkParameters

/**
 * Method clearObject
 *
 * Clears all the members to their initial value.
 */
void CrossValidation::clearObject()
{
  delete m_params;
  // m_model: is a bind (not to be deleted)
  // m_training_set: is a bind (not to be deleted)
  // m_test_set: is a bind (not to be deleted)

  m_model = NULL;
  m_training_set = NULL;
  m_test_set = NULL;
  m_params = NULL;

  // Cross validation parameters
  m_cv_folds = 0;
  m_cv_steps = 0;
  m_cv_times = 0;
  m_cv_times_dataset_shuffle = true;
  m_stratified_split = false;
  m_dataset_rseed = 0;
  m_train_fail_behaviour = stop_training;

  // Other parameters
  m_save_output_file.clear();

  clearResults();

  return;
} // method clearMembers

/**
 * Method clearResults
 *
 * Support method for the method "start".
 * Clears all the results variables and structures.
 */
void CrossValidation::clearResults()
{
  clearTimeResults();

  m_times_res_accm.clear();
  m_avg_va_perf = 0.0;
  m_avg_ts_perf = 0.0;
  m_final_results.clear();

  return;
} // method clearResults

/**
 * Method clearTimeResults
 *
 * Support method for the method "start".
 * Clears variables and structures used to computes time results.
 */
void CrossValidation::clearTimeResults()
{
  m_avg_time_va_perf = 0.0;
  m_avg_time_ts_perf = 0.0;
  m_step_tr_res.clear();
  m_step_va_res.clear();
  m_step_ts_res.clear();
  m_step_results.clear();
  m_steps_res_accm.clear();
  m_time_results.clear();
  return;
} // method clearStepsResults

/**
 * Method computeFinalResults
 *
 * Support method for the method "start".
 * Fills the object "m_final_results". If there is only one accumulated value
 * into "m_time_res_accm" (i.e. the number of times is 1) the results are
 * simply those contained in the "m_time_results" object.
 * If there are more than 1 times then are computed average and standard
 * deviation of the average values accumulated into "m_time_res_accm" and they
 * are reported into the results.
 * Also computes "m_avg_vaperf" and "m_avg_tsperf" values.
 */
void CrossValidation::computeFinalResults()
{
  if (!m_final_results.isEmpty()) // to be sure (there not should be needed)
    m_final_results.clear();

  if (m_times_res_accm.getSize() != m_time_results.getSize())
  {
    Log::err << "CrossValidation::computeFinalResults: fatal error: results "
             << "are different in size (there is a bug in the code!)"
             << Log::endl;
    m_final_results = m_time_results; // try to fix the error
    return;
  }

  for (Uint i = 0; i < m_times_res_accm.getSize(); ++i)
  {
    std::string res_value = "", res_note = "";
    Uint n_elements = m_times_res_accm[i].value.first.size(); // m_cv_times

    if (m_times_res_accm[i].value.second.size() != n_elements)
    {
      Log::err << "CrossValidation::computeFinalResults: fatal error: "
               << "accumulated times results are different in size (there is "
               << "a bug in the code!)" << Log::endl;
      m_times_res_accm[i].value.first.clear(); // a sort of fix
      m_times_res_accm[i].value.second.clear();
      n_elements = 0;
    }

    if (n_elements <= 1)
    {
      res_value = m_time_results[i].value;
      res_note = m_time_results[i].note;
    } // if
    else
    {
      // computes average and standard deviation
      Real avg = Math::avg(m_times_res_accm[i].value.first);
      Real stdev = Math::stdev(m_times_res_accm[i].value.first, avg);

      // value = "<mean> <note> (+/- <stdev>)"
      res_value =
          Global::toString(avg) +
          (
            m_times_res_accm[i].note.empty() ?
                "" : " " + m_times_res_accm[i].note
          ) +
          " (+/- " + Global::toString(stdev) + ")";

      // note = "on <cv-times> times"
      res_note = "on " + Global::toString(n_elements) + " times";
    } // if-else

    // fills the object m_final_results
    m_final_results.pushBack(
          m_times_res_accm[i].key,
          m_times_res_accm[i].name,
          res_value,
          res_note);

  } // for i

  m_avg_va_perf = m_avg_va_perf / (Real)m_cv_times;
  m_avg_ts_perf = m_avg_ts_perf / (Real)m_cv_times;

  return;
} // method computeFinalResults

/**
 * Method parseParameters
 *
 * Fills parameters by looking at member m_params.
 */
void CrossValidation::parseParameters()
{
  // Sets default parameters
  m_params->setDefault("cv-folds", "1");
  m_params->setDefault("cv-steps", m_params->get("cv-folds"));
  m_params->setDefault("cv-times", "1");
  m_params->setDefault("cv-times-dataset-shuffle", "true");
  m_params->setDefault("stratified-split", "false");

  // Cross validation parameters
  m_cv_folds = m_params->getUint("cv-folds");
  m_cv_steps = m_params->getUint("cv-steps");
  m_cv_times = m_params->getUint("cv-times");
  m_cv_times_dataset_shuffle = m_params->getBool("cv-times-dataset-shuffle");
  m_stratified_split = m_params->getBool("stratified-split");
  m_dataset_rseed = m_params->getUint("dataset-rseed");

  m_train_fail_behaviour = stop_training;
  if (m_params->get("train-fail-behaviour") == "repeat")
    m_train_fail_behaviour = repeat_training;

  // Other parameters
  m_save_output_file = m_params->get("save-outputs-file");

  return;
} // method parseParameters

/**
 * Method saveOutputs
 *
 * Saves model outputs on the passed file.
 */
void CrossValidation::saveOutputs
(
    const std::string& filename,
    Uint time,
    Uint step
)
{
  std::string fileprefix =
      filename + "." + Global::toString(time) + "." + Global::toString(step);

  // computes and saves the training outputs
  std::string trfile = fileprefix + ".tr.out";
  std::fstream trfstream(trfile.c_str(), std::fstream::out);
  if (trfstream.fail())
    sigSaveOutputsEnd(*this, trfile, false);
  else
  {
    m_model->computeOnTrainingSet(trfstream, true);
    sigSaveOutputsEnd(*this, trfile, true);
  } // if-else
  trfstream.close();

  // computes and saves the test fold outputs
  if (m_training_set->getTestFoldSize() != 0)
  {
    std::string tsffile = fileprefix +  ".tsf.out";
    std::fstream tsffstream(tsffile.c_str(), std::fstream::out);
    if (tsffstream.fail())
      sigSaveOutputsEnd(*this, tsffile, false);
    else
    {
      m_model->computeOnTestFold(tsffstream, true);
      sigSaveOutputsEnd(*this, tsffile, true);
    } // if-else
    tsffstream.close();
  } // if

  // computes and saves test outputs
  if (thereIsTestSet() && !m_test_set->isEmpty())
  {
    std::string tsfile = fileprefix +  ".ts.out";
    std::fstream tsfstream(tsfile.c_str(), std::fstream::out);
    if (tsfstream.fail())
      sigSaveOutputsEnd(*this, tsfile, false);
    else
    {
      m_model->computeOn(*m_test_set, tsfstream, true);
      sigSaveOutputsEnd(*this, tsfile, true);
    } // if-else
    tsfstream.close();
  } // if

  return;
} // method saveOutputs

} // namespace procedure
} // namespace moka

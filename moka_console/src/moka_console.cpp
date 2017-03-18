#include <exception>
#include <iostream>
#include <string>
#include <sstream>
#include <boost/program_options.hpp>
#include <boost/signals2.hpp>
#include <moka/dataset/datasetdispenser.h>
#include <moka/log.h>
#include <moka/model/model.h>
#include <moka/model/modeldispenser.h>
#include <moka/procedure/crossvalidation.h>
#include <moka/util/parameters.h>

// Namespaces
namespace bpo = boost::program_options;
namespace mds = moka::dataset;
namespace mmo = moka::model;
namespace mpc = moka::procedure;
namespace mut = moka::util;

// Functions
bool parseProgramOptions(int argc, char *argv[]);
bool loadAndFillParameters();
int datasetPreprocessingProcedure();
int crossValidationProcedure();
void bindCrossValidationSignals(mpc::CrossValidation& cv);
int simpleTrainingProcedure();
int evaluateProcedure();
std::string programDescription();

// Cross validation slots
void sltCrossValidationStart(const mpc::CrossValidation& cv);
void sltModelInitialized(
    const mpc::CrossValidation& cv,
    mpc::CrossValidation::Uint time);
void sltStepStart(
    const mpc::CrossValidation& cv,
    mpc::CrossValidation::Uint step);
void sltTrainingFail(
    const mpc::CrossValidation& cv,
    const std::string& error);
void sltStepEnd(
    const mpc::CrossValidation& cv,
    const moka::util::Info<std::string>& results,
    mpc::CrossValidation::Uint step);
void sltSaveOutputsEnd(
    const mpc::CrossValidation& cv,
    const std::string& filename,
    bool success);
void sltTimeEnd(
    const mpc::CrossValidation& cv,
    const moka::util::Info<std::string>& results,
    mpc::CrossValidation::Uint time);
void sltCrossValidationEnd(const mpc::CrossValidation& cv);

// Modes
enum Mode
{
  mode_dataset_preprocess,
  mode_simple_training,
  mode_cross_validation,
  mode_invalid
};

// Parameters
Mode mode;
std::string program_name;
bool verbose;
std::string config_filename;
std::string input_filename;
std::string output_filename;
mut::Parameters cv_params;
mut::Parameters model_params;
mut::Parameters tr_set_params;
mut::Parameters ts_set_params;
bool there_is_test_set;
mut::Parameters dataset_params;

// Global objects: used for handle c.v. signals
bool training_fail;

/**
 * Function main
 *
 * Does the following things:
 *  - Parse program options.
 *  - Loads from a specified configuration file the set of parameters for
 *    the building and training of a machine learning model using the moka
 *    library.
 *  - Start a different procedure according on the read parameters:
 *      - If there are cross validation parameters starts a cross validation
 *        procedure.
 *      - Otherwise starts a simple training (and optionally test) procedure.
 *  - Ends returning the procedure returned value.
 */
int main(int argc, char *argv[])
{
  // Parse parameters
  if (!parseProgramOptions(argc, argv))
    return 1;

  // Set the verbose mode
  if (verbose)
    moka::Log::setVerbose();

  // Load parameters from file and fill moka parameters
  if (!loadAndFillParameters())
    return 1;

  // Start the selected procedure
  int return_value = 1;
  switch (mode)
  {
  case mode_dataset_preprocess:
    return_value = datasetPreprocessingProcedure();
    break;
  case mode_simple_training:
    return_value = simpleTrainingProcedure();
    break;
  case mode_cross_validation:
    return_value = crossValidationProcedure();
    break;
  case mode_invalid: default:
    break;
  }

  return return_value;
} // function main

// ====================
// FUNCTION DEFINITIONS
// ====================

/**
 * Function parseProgramOptions
 *
 * Parse program options filling the following global parameters:
 *   - program_name
 *   - verbose
 *   - config_filename
 *   - input_filename
 *   - output_filename
 *
 * If there aren't some required options or if an help message is requested,
 * it is printed out the program usage description and eventualy the error
 * description and the function returns false, otherwise (if all it's ok)
 * it returns true.
 */
bool parseProgramOptions(int argc, char *argv[])
{
  // Set the program name
  program_name = argv[0];

  // Default values into optional parametrs
  verbose = false;

  bpo::variables_map opt;
  bpo::options_description opt_desc(
      "Usage: " + std::string(argv[0]) + " OPTIONS");
  try
  {
    opt_desc.add_options()
        ("help,h",
         "Print help message.")
        ("config,c",
         bpo::value<std::string>(&config_filename)->required(),
         "Specify configuration file (.ini).")
        ("input,i",
         bpo::value<std::string>(&input_filename),
         "Input file.")
        ("output,o",
         bpo::value<std::string>(&output_filename),
         "Output file.")
        ("verbose,v",
         "Log out verbose messages.");

    bpo::store(bpo::parse_command_line(argc, argv, opt_desc), opt);

    // Check for help request
    if (opt.count("help"))
    {
      std::cout << opt_desc << std::endl
                << programDescription() << std::endl;
      return false;
    }

    bpo::notify(opt);

    // Inits some values
    verbose = opt.count("verbose");

  } // try
  catch (std::exception& ex)
  {
    std::cerr << "Error: " << ex.what() << "." << std::endl
              << std::endl
              << opt_desc << std::endl;
    return false;
  } // try-catch

  return true;
} // function parseProgramOptions

/**
 * Function loadAndFillParameters
 *
 * Loads from the file config_filename all the parameters, fills the
 * following global objects and set the mode based on configuration sections:
 *   - cv_params: parameters in the section [cross-validation].
 *   - model_params: parameters in the section [model].
 *   - tr_set_params: parameters in the section [training-set].
 *   - ts_set_params: parameters in the section [test-set].
 *   - there_is_test_set: true if there was the section [test-set].
 *   - dataset_params: parameters in the section [dataset].
 *
 * If an error occours reading the configuration file will be printed out
 * an error message and the function returns false, otherwise all the objects
 * are filled and the function returns true.
 */
bool loadAndFillParameters()
{
  try
  {
    mode = mode_invalid;

    if (dataset_params.readFromIni(config_filename, "dataset"))
      mode = mode_dataset_preprocess;

    if (model_params.readFromIni(config_filename, "model"))
      mode = mode_simple_training;

    if (cv_params.readFromIni(config_filename, "cross-validation"))
      mode = mode_cross_validation;

    tr_set_params.readFromIni(config_filename, "training-set");

    if (ts_set_params.readFromIni(config_filename, "test-set"))
      there_is_test_set = true;

  } // try
  catch (std::exception& ex)
  {
    std::cerr << "Error: " << ex.what() << std::endl;
    std::cerr << "Check the configuration file." << std::endl;
    return false;
  } // try-catch

  if (mode == mode_invalid)
  {
    std::cerr << "Invalid configuration file." << std::endl;
    return false;
  }

  return true;
} // function loadAndFillParameters

/**
 * Function datasetPreprocessingProcedure
 *
 * Load a dataset specified in the configuration file and write it on the
 * output file using the Dataset::write method. In the loading procedure the
 * Dataset object will do all the necessary preprocessing on data, thus the
 * method write will write on file the dataset preprocessed.
 */
int datasetPreprocessingProcedure()
{
  // Initial checks
  if (input_filename.empty())
  {
    std::cerr << "Error: you must specify an input file name (try with "
              << program_name << " --help)." << std::endl;
    return 1;
  }

  if (output_filename.empty())
  {
    std::cerr << "Error: you must specify an output file name (try with "
              << program_name << " --help)." << std::endl;
    return 1;
  }

  std::fstream ofs(output_filename.c_str(), std::fstream::out);
  int return_value = 0;

  try
  {
    // Set input file
    dataset_params["file-path"] = input_filename;

    // Load the dataset
    mds::Dataset *dataset;
    dataset = mds::DatasetDispenser::get(dataset_params["dataset-type"]);
    if (!dataset)
      throw std::runtime_error(
          std::string("invalid value in the parameter <dataset-type> in ") +
          "the [dataset] section (" + dataset_params["dataset-type"] + ")");
    dataset_params.remove("dataset-type");
    dataset->load(dataset_params);

    // Save on file the parsed dataset
    if (ofs.fail())
      throw std::runtime_error(
          std::string("fail opening the file ") + output_filename);

    dataset->write(ofs);

  } // try
  catch (std::exception& ex)
  {
    std::cerr << "Error: " << ex.what() << "." << std::endl;
    return_value = 1;
  } // try-catch

  ofs.close();

  return return_value;
} // function datasetPreprocessingProcedure

/**
 * Function crossValidationProcedure
 *
 * Using a CrossValidation object from moka library do a cross validation
 * procedure. Each results will be printed on standard output.
 */
int crossValidationProcedure()
{
  int return_value = 0;
  mmo::Model *model = NULL;
  mds::Dataset *training_set = NULL, *test_set = NULL;
  mpc::CrossValidation *cv = new mpc::CrossValidation();

  try
  {
    // Set parameters in the Cross validation object
    cv->setParameters(cv_params);

    // Load the training set
    training_set = mds::DatasetDispenser::get(tr_set_params["dataset-type"]);
    if (!training_set)
      throw std::runtime_error(
          std::string("invalid value in the parameter <dataset-type> in ") +
          "the [training-set] section (" + tr_set_params["dataset-type"] + ")");

    training_set->load(tr_set_params);

    // Load the test set
    if (there_is_test_set)
    {
      test_set = mds::DatasetDispenser::get(ts_set_params["dataset-type"]);
      if (!test_set)
        throw std::runtime_error(
            std::string("invalid value in the parameter <dataset-type> in ") +
            "the [test-set] section (" + ts_set_params["dataset-type"] + ")");
      test_set->load(ts_set_params);
    }

    // Set parameters in the Model object
    model = mmo::ModelDispenser::get(model_params["model-type"]);
    if (!model)
      throw std::runtime_error(
          std::string("invalid value in the parameter <model-type> (") +
          model_params["model-type"] + ")");

    model->setParameters(model_params);

    // Binds model and dataset(s) in the CrossValidation object
    cv->bindModel(model);
    cv->bindTrainingSet(training_set);

    if (there_is_test_set)
      cv->bindTestSet(test_set);

    // Connect boost signals
    bindCrossValidationSignals(*cv);

    // Start the cross validation procedure
    training_fail = false;
    cv->start();

    if (training_fail)
      return_value = 1;

  } // try
  catch (std::exception& ex)
  {
    std::cerr << "Error: " << ex.what() << "." << std::endl;
    return_value = 1;
  } // try-catch

  // Delete objects
  delete model;
  delete training_set;
  delete test_set;
  delete cv;

  return return_value;
} // function crossValidationProcedure

/**
 * Function bindCrossValidationSignals
 *
 * Connects the CrossValidation boost signals to slots defined here.
 */
void bindCrossValidationSignals(mpc::CrossValidation& cv)
{
  cv.sigCrossValidationStart.connect(sltCrossValidationStart);
  cv.sigModelInitialized.connect(sltModelInitialized);
  cv.sigStepStart.connect(sltStepStart);
  cv.sigTrainingFail.connect(sltTrainingFail);
  cv.sigStepEnd.connect(sltStepEnd);
  cv.sigSaveOutputsEnd.connect(sltSaveOutputsEnd);
  cv.sigTimeEnd.connect(sltTimeEnd);
  cv.sigCrossValidationEnd.connect(sltCrossValidationEnd);
  return;
} // function bindCrossValidationSignals

/**
 * Function simpleTrainingProcedure
 *
 * Using moka library classes builds and trains a machine learning model. Each
 * results will be printed on standard output.
 */
int simpleTrainingProcedure()
{
  int return_value = 0;
  mmo::Model *model = NULL;
  mds::Dataset *training_set = NULL, *test_set = NULL;

  try
  {
    // Load the training set
    training_set = mds::DatasetDispenser::get(tr_set_params["dataset-type"]);
    if (!training_set)
      throw std::runtime_error(
          std::string("invalid value in the parameter <dataset-type> in ") +
          "the [training-set] section" + tr_set_params["dataset-type"]);

    tr_set_params.remove("dataset-type");

    training_set->load(tr_set_params);

    std::cout << "Training set" << std::endl
              << training_set->info().list("> ") << std::endl
              << std::endl;

    // Load the test set
    if (there_is_test_set)
    {
      test_set = mds::DatasetDispenser::get(ts_set_params["dataset-type"]);
      if (!test_set)
        throw std::runtime_error(
            std::string("invalid value in the parameter <dataset-type> in ") +
            "the [test-set] section" + ts_set_params["dataset-type"]);

      ts_set_params.remove("dataset-type");

      test_set->load(ts_set_params);

      std::cout << "Test set" << std::endl
                << test_set->info().list("> ") << std::endl
                << std::endl;
    } // if (there_is_test_set)

    // Get the Model object
    model = mmo::ModelDispenser::get(model_params["model-type"]);
    if (!model)
      throw std::runtime_error(
          std::string("invalid value in the parameter <model-type>") +
          model_params["model-type"]);

    model_params.remove("model-type");

    // Check if the model must be loaded from file (in case skip the training)
    if (model_params.contains("model-load-file"))
    {
      // Load the model from file
      model->loadFromFile(model_params.get("model-load-file"));

      std::cout << "Model succesfully loaded from file: "
                << model_params.get("model-load-file") << std::endl
                << std::endl;

      std::cout << model->info().getTitle() << std::endl
                << model->info().list("> ") << std::endl
                << std::endl;
    }
    else
    {
      // Set model parameters
      model->setParameters(model_params);
      model->bindTrainingSet(training_set);

      // Initialize the model
      model->init();

      std::cout << model->info().getTitle() << std::endl
                << model->info().list("> ") << std::endl
                << std::endl;

      // Train the model
      mmo::Model::NumericResults tr_res;
      tr_res = model->train();

      std::cout << "Training results" << std::endl
                << tr_res.list("> ") << std::endl
                << std::endl;
      std::cout << "Training results (CSV)" << std::endl
                << tr_res.csvHeader() << std::endl;
      std::cout << tr_res.csvLine() << std::endl
                << std::endl;

    } // if-else

    // Test the model
    if (there_is_test_set)
    {
      mmo::Model::NumericResults ts_res;
      ts_res = model->testOn(*test_set);

      std::cout << "Test results" << std::endl
                << ts_res.list("> ") << std::endl
                << std::endl;
      std::cout << "Test results (CSV)" << std::endl
                << ts_res.csvHeader() << std::endl;
      std::cout << ts_res.csvLine() << std::endl
                << std::endl;
    } // if (there_is_test_set)

    // Check if the model must be saved on file
    if (model_params.contains("model-save-file"))
    {
      model->saveOnFile(model_params.get("model-save-file"));

      std::cout << "Model succesfully saved on file: "
                << model_params.get("model-save-file") << std::endl
                << std::endl;
    } // if

  } // try
  catch (std::exception& ex)
  {
    std::cerr << "Error: " << ex.what() << "." << std::endl;
    return_value = 1;
  } // try-catch

  // Delete objects
  delete model;
  delete training_set;
  delete test_set;

  return return_value;
} // function simpleTrainingProcedure

/**
 * Function programDescription
 *
 * Returns into a string object the program description (usage and
 * functionality).
 */
std::string programDescription()
{
  std::stringstream ss;
//  ss << "Here the program description. \n"
//     << "Bla bla bla bla bla bla bla bla bla bla bla bla bla bla bla bla \n"
//     << "bla bla bla bla bla bla bla bla. \n";
  return ss.str();
} // function programDescription

// ======================
// CROSS VALIDATION SLOTS
// ======================

/**
 * Function sltCrossValidationStart
 *
 * Connected to CrossValidation::sigCrossValidationStart.
 */
void sltCrossValidationStart(const mpc::CrossValidation& cv)
{
  std::cout
      << "## Cross Validation Start ##" << std::endl
      << std::endl
      << "Training set" << std::endl
      << cv.getTrainingSet().info().list("> ") << std::endl;

  if (cv.thereIsTestSet())
    std::cout
        << "Test set" << std::endl
        << cv.getTestSet().info().list("> ") << std::endl;

  std::cout << std::endl;

  return;
} // function sltCrossValidationStart

/**
 * Function sltModelInitialized
 *
 * Connected to CrossValidation::sigModelInitialized.
 */
void sltModelInitialized
(
    const mpc::CrossValidation& cv,
    mpc::CrossValidation::Uint time
)
{
  (void) cv;

  if (!training_fail)
  {
    std::cout << "## Cross Validation Time " << time << " ##" << std::endl
              << std::endl;
  }

  return;
} // function sltModelInitialized

/**
 * Function sltCrossValidationStepStart
 *
 * Connected to CrossValidation::sigStepStart.
 */
void sltStepStart
(
    const mpc::CrossValidation& cv,
    mpc::CrossValidation::Uint step
)
{
  (void)cv; // warning suppress
  (void)step; // warning suppress

  // signal ignored

  return;
} // function sltCrossValidationStepStart

/**
 * Function sltTrainingFail
 *
 * Connected to CrossValidation::sigTrainingFail.
 */
void sltTrainingFail
(
    const mpc::CrossValidation& cv,
    const std::string& error
)
{
  (void) cv; // warning suppress
  (void) error; // warning suppress

  std::cerr << "Error: the training procedure is failed." << std::endl
            << std::endl;

  training_fail = true;

  return;
} // function sltTrainingFail

/**
 * Function sltStepEnd
 *
 * Connected to CrossValidation::sigStepEnd.
 */
void sltStepEnd
(
    const mpc::CrossValidation& cv,
    const mut::Info<std::string>& results,
    mpc::CrossValidation::Uint step
)
{
  // Prints out the csv header
  if (step == 1)
  {
    std::cout
        << cv.getModel().info().getTitle() << std::endl
        << cv.getModel().info().list("> ") << std::endl
        << std::endl;

    std::cout
        << "Step n., " << "Tr. instances, ";

    if (cv.getCvFolds() > 1)
      std::cout << "Va. on fold, " << "Va. instances, ";

    if (cv.thereIsTestSet())
      std::cout << "Test instances, ";

    std::cout << results.getCsvHeader() << std::endl;
  } // if

  // Prints result line
  std::cout << step << ", " << cv.getTrainingSet().getTrSetSize() << ", ";

  if (cv.getCvFolds() > 1)
    std::cout
        << cv.getTrainingSet().getTestFold() + 1 << ", "
        << cv.getTrainingSet().getTestFoldSize() << ", ";

  if (cv.thereIsTestSet())
    std::cout
        << cv.getTestSet().getSize() << ", " << results.getCsvLine()
        << std::endl;

  std::cout << results.getCsvLine() << std::endl;

  return;
} // function sltStepEnd

/**
 * Function sltSaveOutputsEnd
 *
 * Connected to CrossValidation::sigSaveOutputsEnd.
 */
void sltSaveOutputsEnd
(
    const mpc::CrossValidation& cv,
    const std::string& filename,
    bool success
)
{
  (void)cv; // suppress unused variable warning
  if (!success)
    std::cerr
        << "Error: fail to saving outputs on file """ << filename
        << """ (check file or directory permissions)" << std::endl;
  return;
} // function sltSaveOutputsEnd

/**
 * Function sltTimeEnd
 *
 * Connected to CrossValidation::sigTimeEnd.
 */
void sltTimeEnd
(
    const mpc::CrossValidation& cv,
    const moka::util::Info<std::string>& results,
    mpc::CrossValidation::Uint time
)
{
  (void)time; // suppress warning

  std::cout
      << "TOT on " << cv.getCvSteps() << " steps, "
      << "/, "; // Tr. instances

  if (cv.getCvFolds() > 1)
    std::cout
        << "on " << cv.getTrainingSet().getNumberOfFolds() << " folds, "
        << "/, "; // Va. instances

  if (cv.thereIsTestSet())
    std::cout << "/, ";

  std::cout << results.getCsvLine() << std::endl << std::endl;

  training_fail = false;

  return;
} // function sltTimeEnd

/**
 * Function sltCrossValidationEnd
 *
 * Connected to CrossValidation::sigCrossValidationEnd.
 */
void sltCrossValidationEnd(const mpc::CrossValidation& cv)
{
  std::cout
      << "## Cross Validation End ##" << std::endl
      << std::endl
      << "Final results " << std::endl
      << cv.getResults().list("> ") << std::endl
      << std::endl
      << "Final results (CSV)" << std::endl
      << cv.getResults().csvHeader() << std::endl
      << cv.getResults().csvLine() << std::endl
      << std::endl;

  return;
} // function sltCrossValidationEnd

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
int evaluateProcedure();
std::string programDescription();

// Parameters
std::string program_name;
bool verbose;
std::string config_filename;
std::string model_filename;
std::string input_filename;
std::string output_filename;
mut::Parameters dataset_params;

/**
 * Function main
 *
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

  return evaluateProcedure();
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
         "Specify dataset configuration file (.ini).")
        ("model,m",
         bpo::value<std::string>(&model_filename)->required(),
         "Model file (.model).")
        ("input,i",
         bpo::value<std::string>(&input_filename)->required(),
         "Input file (.sdf).")
        ("output,o",
         bpo::value<std::string>(&output_filename)->required(),
         "Output file (prefix for .eq and .mol files).")
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
    if (!dataset_params.readFromIni(config_filename, "dataset"))
      throw std::runtime_error("there isn't a [dataset] section");

  } // try
  catch (std::exception& ex)
  {
    std::cerr << "Error: " << ex.what() << std::endl;
    std::cerr << "Check the configuration file." << std::endl;
    return false;
  } // try-catch

  return true;
} // function loadAndFillParameters

/**
 * Function evaluateProcedure
 *
 */
int evaluateProcedure()
{
  int return_value = 0;
  mmo::GraphEsnSom *model = new mmo::GraphEsnSom();
  mds::Dataset *dataset = NULL;

  try
  {
    // Set input file
    dataset_params["file-path"] = input_filename;

    // Load the dataset
    dataset = mds::DatasetDispenser::get(dataset_params["dataset-type"]);
    if (!dataset)
      throw std::runtime_error(
          std::string("invalid value in the parameter <dataset-type> in ") +
          "the [dataset] section: " + dataset_params["dataset-type"]);

    dataset_params.remove("dataset-type");

    dataset->load(dataset_params);

    std::cout << "Dataset loaded" << std::endl
              << dataset->info().list("> ") << std::endl
              << std::endl;

    if (dataset->isEmpty())
      throw std::runtime_error("the dataset is empty");

    // Load the model from file
    model->loadFromFile(model_filename);

    std::cout << "Model succesfully loaded from file: "
              << model_filename << std::endl
              << std::endl;

    std::cout << model->info().getTitle() << std::endl
              << model->info().list("> ") << std::endl
              << std::endl;

    // Performes the evaluation on the dataset saving results on file
    model->evaluateOn(
          *dataset, output_filename + ".eq.csv", output_filename + ".mol");

    std::cout << "Equation file created and saved on "
              << output_filename + ".eq.csv "
              << "(you can use it as csv file)" << std::endl;
    std::cout << "Info file created and saved on "
              << output_filename + ".mol "
              << "(you can use it on moka-svg-creator)" << std::endl;

    std::cout << std::endl;

  } // try
  catch (std::exception& ex)
  {
    std::cerr << "Error: " << ex.what() << "." << std::endl;
    return_value = 1;
  } // try-catch

  // Delete objects
  delete model;
  delete dataset;

  return return_value;
} // function evaluateProcedure

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

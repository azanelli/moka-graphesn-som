#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <armadillo>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <knnl/neural_net_headers.hpp>
#include "../common.h"

// ==========
// NAMESPACES
// ==========

namespace balg = boost::algorithm;
namespace bpo  = boost::program_options;

// ================
// TYPE DEFINITIONS
// ================

// typedef std::vector<double> DataType;
typedef arma::colvec DataType;

typedef std::vector<DataType> DataContainer;

typedef neural_net::Cauchy_function
<
    DataType::value_type, // activation function result type
    DataType::value_type, // type of sigma
    int                   // type of the exponent
> ActFun;

typedef distance::Euclidean_distance_function<DataType> DistFun;

typedef neural_net::Basic_neuron<ActFun, DistFun> KohonenNeuron;

typedef neural_net::Rectangular_container<KohonenNeuron> KohonenNetwork;

typedef neural_net::External_randomize RandPolicy;

// typedef neural_net::Hexagonal_topology<int> NetTop;
typedef neural_net::Euclidean_topology <int> NetTop;

typedef neural_net::Gauss_function
<
    double, // Value type (v)
    double, // Scalar type (sigma)
    int     // Exponent type (p)
> NetFun;

typedef neural_net::Constant_function
<
    DataType::value_type, // Value type (value)
    double                // Scalar type (c)
> SpaceFun;

typedef neural_net::Classic_training_weight
<
    DataType, // type of values
    int,      // type of interation counter
    NetFun,   // network topology update function type
    SpaceFun, // space topology update function type
    NetTop,   // network topology type
    DistFun,  // distance function type
    int       // type of index in the neural network container
> WeightFun;

typedef neural_net::Wtm_classical_training_functional
<
  DataType,   // type of values
  double,     // type of parameters of training functional
  int,        // type of step counter value
  int,        // type of index used in network
  WeightFun   // type of functor used to set up training weight
> TrainingFun;

//typedef neural_net::Wtm_training_algorithm
//<
//  KohonenNetwork, // network type
//  DataType,       // type of single data in mathematical meanning
//  DataContainer::iterator, // iterator for container with training data.
//  TrainingFun,    // type of training functional.
//  int             // type of index of neurons used in network.
//> TrainingAlg;
typedef neural_net::Wtm_decay_training_algorithm
<
  KohonenNetwork, // network type
  DataType,       // type of single data in mathematical meanning
  DataContainer::iterator, // iterator for container with training data.
  TrainingFun,    // type of training functional.
  int             // type of index of neurons used in network.
> TrainingAlg;

typedef std::vector< std::pair<DataType, std::string> > CalDataContainer;

typedef std::vector
<
  std::vector< std::map<std::string, size_t> >
> WinnerMatrix;

// =====================
// FUNCTION DECLARATIONS
// =====================

bool parseParameters(int argc, char *argv[]);

void printParameters();

void loadTrainingData(
    DataContainer& training_data,
    const std::string& filename);

void initNetwork(
    KohonenNetwork& knetwork,
    DataContainer& training_data,
    int rseed,
    int nrows,
    int ncols);

void train(
    KohonenNetwork& knetwork,
    DataContainer& training_data,
    double alpha,
    double sigma_ini,
    double sigma_fin,
    int iterations);

void calibrate(
    KohonenNetwork& knetwork,
    const std::string& calibration_file,
    WinnerMatrix& wm);

void printNetworkWeights(
    const KohonenNetwork& knetwork);

void printNetworkResults(
    KohonenNetwork& knetwork,
    const DataType& data_value);

void saveMap(
    const KohonenNetwork& knetwork,
    const std::string& filename,
    const WinnerMatrix& wm);

// =================
// GLOBAL PARAMETERS
// =================

int rseed;
std::string training_file;
std::string calibration_file;
std::string save_file;
int nrows;
int ncols;
TrainingAlg::decay_type alpha_decay;
double alpha_p1;
double alpha_p2;
double sigma_p1;
double sigma_p2;
int niters_p1;
int niters_p2;

// ====================
// FUNCTION DEFINITIONS
// ====================

/**
 * Function main
 */
int main(int argc, char *argv[])
{
  // Parse parameters
  if (!parseParameters(argc, argv))
    return 1;
  printParameters();

  try
  {
    // Loads data from file
    DataContainer training_data;
    loadTrainingData(training_data, training_file);

    // Inits network
    KohonenNetwork knetwork;
    initNetwork(knetwork, training_data, rseed, nrows, ncols);

    // Rough train
    std::cout << "Start rough train (on " << training_data.size() << " ";
    std::cout << "instances and " << niters_p1 << " iterations)" << std::endl;
    train
    (
          knetwork,
          training_data,
          alpha_p1,
          sigma_p1,
          std::max(1.0, std::ceil(sigma_p1 / 4)),
          niters_p1
    );

    // Fine tune
    std::cout << "Start fine tune (on " << training_data.size() << " ";
    std::cout << "instances and " << niters_p2 << " iterations)" << std::endl;
    train
    (
          knetwork,
          training_data,
          alpha_p2,
          sigma_p2,
          1.0,
          niters_p2
    );

    // Network calibration
    WinnerMatrix winner_matrix;
    if (!calibration_file.empty())
      calibrate(knetwork, calibration_file, winner_matrix);

    // Saves the map on file (with SOM_PAK format)
    saveMap(knetwork, save_file, winner_matrix);

  } // try
  catch(std::exception& ex)
  {
    std::cerr << std::endl;
    std::cerr << "Error: " << ex.what() << std::endl << std::endl;
    return 1;
  } // try-catch

  return 0;
} // function main

/**
 * Function parseParameters
 */
bool parseParameters(int argc, char *argv[])
{
  // Puts default values into optional parametrs
  char alpha_decay_c = 'i';
  calibration_file = "";

  bpo::variables_map opt;
  bpo::options_description optdesc(
      "Usage: " + std::string(argv[0]) + " OPTIONS");
  try {
    optdesc.add_options()
        ("help,h",
         "print help message")
        ("rseed",
         bpo::value<int>(&rseed)->required(),
         "random seed")
        ("nrows", bpo::value<int>(&nrows)->required(),
         "number of rows")
        ("ncols", bpo::value<int>(&ncols)->required(),
         "number of cols")
        ("alpha_decay", bpo::value<char>(&alpha_decay_c),
         "decay type of learning rate alpha: 'l' for linear or 'i' for "
         "inverse (default)")
        ("alpha1", bpo::value<double>(&alpha_p1)->required(),
         "alpha (learning rate) to use in the first phase")
        ("sigma1", bpo::value<double>(&sigma_p1)->required(),
         "sigma (in the gauss function) to use in the first phase")
        ("niters1", bpo::value<int>(&niters_p1)->required(),
         "iterations for phase 1")
        ("alpha2", bpo::value<double>(&alpha_p2)->required(),
         "alpha (learning rate) to use in the second phase")
        ("sigma2", bpo::value<double>(&sigma_p2)->required(),
         "sigma (in the gauss function) to use in the second phase")
        ("niters2", bpo::value<int>(&niters_p2)->required(),
         "iterations for phase 2")
        ("training-file,T",
         bpo::value<std::string>(&training_file)->required(),
         "file with input training data")
        ("calibration-file,C",
         bpo::value<std::string>(&calibration_file),
         "file with calibration data (optional, if no present no calibration "
         "will be made)")
        ("save-file,S", bpo::value<std::string>(&save_file)->required(),
         "file where save the final kohonen network");
    bpo::store(bpo::parse_command_line(argc, argv, optdesc), opt);

    if (opt.count("help"))
    {
      std::cout << optdesc << "\n";
      return false;
    }

    bpo::notify(opt);

  } // try
  catch (std::exception& ex)
  {
    std::cerr << "Error: " << ex.what() << std::endl << std::endl;
    std::cerr << optdesc << "\n";
    return false;
  } // try-catch

  // Fills other parameters
  switch (alpha_decay_c)
  {
  case 'l': alpha_decay = TrainingAlg::linear_decay; break;
  case 'i': default: alpha_decay = TrainingAlg::inverse_decay; break;
  }

  return true;
} // function parseParameters

/**
 * Function printParameters
 */
void printParameters()
{
  std::cout << "Parameters: ";
  std::cout << "rseed=" << rseed << ", ";
  std::cout << "nrows=" << nrows << ", ";
  std::cout << "ncols=" << ncols << ", ";
  std::cout << "alpha_decay=" << alpha_decay << ", ";
  std::cout << "alpha1=" << alpha_p1 << ", ";
  std::cout << "sigma1=" << sigma_p1 << ", ";
  std::cout << "niters1=" << niters_p1 << ", ";
  std::cout << "alpha2=" << alpha_p2 << ", ";
  std::cout << "sigma2=" << sigma_p2 << ", ";
  std::cout << "niters2=" << niters_p2 << ", ";
  std::cout << std::endl;
  return;
} // function printParameters

/**
 * Function loadTrainingData
 */
void loadTrainingData(
    DataContainer& training_data,
    const std::string& filename)
{
  training_data.clear();

  std::ifstream file(filename.c_str());
  if (!file.is_open())
    throw std::runtime_error(
        "loadTrainingData: error opening file " + filename);

  std::string line;
  std::getline(file, line);
  size_t dim = atoi(line.c_str());

  while (file.good())
  {
    std::getline(file, line);
    boost::trim(line);
    if (!line.empty() && line[0] != '#')
    {
      std::vector<std::string> splv;
      boost::split(splv, line, boost::is_any_of("\t "));

      if (splv.size() < dim)
        throw std::runtime_error("loadTrainingData: wrong line size: " + line);

      training_data.push_back(DataType(dim));
      for (size_t i = 0; i < dim; ++i)
        training_data.back().at(i) = atof(splv.at(i).c_str());
    } // if
  } // while

  return;
} // function loadTrainingData

/**
 * Function initNetwork
 */
void initNetwork(
    KohonenNetwork& knetwork,
    DataContainer& training_data,
    int rseed,
    int nrows,
    int ncols)
{
  // Construct the activation function. Given a value x, the Cauchy function
  // computes:
  //   y = 1 / (1 + (x / s)^p)
  // where s is the parameter sigma, and p is the exponent.
  // The network is a lattice of Kohonen neurons. Given a value x, the output
  // of a neuron is:
  //   y = f(g(w, x))
  // where f is the activation function, g is the distance operator and
  // w is the neuron weight.
  // During the training the winner is taken as the neuron with the max output
  // given a value x.
  //   act_fun(x) = 1 / (1 + (x / 2.0))
  ActFun act_fun(2.0, 1); // (s, p)

  // Define the distance function
  DistFun dist_fun;

  // Randomization policies (external)
  RandPolicy rand_policy;

  // Inits random number generator
  std::srand(rseed);

  // Generate the network, based on designed neuron.
  neural_net::generate_kohonen_network
  <
      DataContainer,
      KohonenNetwork,
      RandPolicy
  >
  (
      nrows,         // number of rows
      ncols ,        // number of columns
      act_fun,       // activation function
      dist_fun,      // distance function
      training_data, // data container
      knetwork,      // network
      rand_policy    // randomize policy
  );

  return;
} // function initNetwork

/**
 * Function train
 */
void train(
    KohonenNetwork& knetwork,
    DataContainer& training_data,
    double alpha,
    double sigma_ini,
    double sigma_fin,
    int iterations)
{
  // Define network topology
  NetTop net_top;

  // Define the distance function
  DistFun dist_fun;

  // Gauss function is y = e^{-0.5 * (v / sigma)^p}
  NetFun net_fun(sigma_ini, 2); // net_fun(x) = e^{-0.5 * (x / sigma)^2}

  // Constant function: y = c
  SpaceFun space_fun(1.0); // space_fun(x) = 1.0

  // The weight function (generalized weight) is used by the training functor
  // to omputes the function G:
  //     G(w, x, t, c_1, c_2, v_1, v_2) =
  //         n_f(n_t(c_1, c_2, v_1, v_2)) * s_f(s_t(x, w))
  //   where
  //     n_f is the network functor
  //     n_t is the network topology functor
  //     s_f is the space functor
  //     s_t is the space topology functor
  //
  // Since SpaceFun is the constant function, G is the follows:
  //   G(w, x, t, c_1, c_2, v_1, v_2) = net_fun(net_top(c_1, c_2, v_1, v_2))
  WeightFun weight_fun
  (
      net_fun,    // network functor
      space_fun,  // space functor
      net_top,    // network topology functor
      dist_fun    // space topology functor
  );

  // Construction of the training functional: the training algorithm applies
  // this functional to each neuron in the map after that the winner is
  // computed. This functional update the neuron weight as follow:
  //     w_{i,j}(t+1) =
  //         w_{i,j}(t) + a * G(w_{i,j}(t), x(t), s, c_i, c_j, i, j) *
  //        (x(t) - w_{i,j}(t))
  //   where
  //     w_{i,j}(t) is the weight of the actual neuron (to train)
  //     e is the scaling factor (alpha)
  //     x(t) is the input value
  //     s is the step number
  //     c_i is the row index of the winner neuron
  //     c_j is the column index of the winner neuron
  //     i is the row index of the actual neuron
  //     j is the column index of the actual neuron
  //   and the function G is computed by the functor setted in the constructor.
  TrainingFun training_fun
  (
      weight_fun, // reference to the functor
      alpha       // scaling parameter (a)
  );

  // Construction of training algorithm: given an input value (taken from
  // data during the training) do the follows:
  //   - Selects the winner as the neuron that returns the greatest value of
  //       y = f(g(x, w))
  //     where f is the activation function, g is the distance function, x is
  //     the input value and w is the neuron weight.
  //       f(v) = 1 / (1 + (v / 2))
  //       g(v, v') computes the euclidean distance between v and v'
  //   - Applies to each neuron in the map the training functional uptading its
  //     weight.
  // TrainingAlg training_alg(training_fun);
  TrainingAlg training_alg
  (
      training_fun,
      sigma_fin,
      training_data.size() * iterations
  );

  // Start the train
  for (int it = 0; it < iterations; ++it)
  {
    training_alg(training_data.begin(), training_data.end(), &knetwork);

//    // decrease sigma parameter
//    training_alg.training_functional.generalized_training_weight.
//        network_function.sigma = sigma * (1.0 - (it / (double)iterations));

//    // linear decrease the learning rate (alpha)
//    training_alg.training_functional.parameter =
//         * (1.0 - (it / (double)iterations));

    // shuffle data
    std::random_shuffle(training_data.begin(), training_data.end());
  } // for it

  return;
} // function train

/**
 * Function calibrate
 */
void calibrate(
    KohonenNetwork& knetwork,
    const std::string& calibration_file,
    WinnerMatrix& wm)
{
  // Loads data
  CalDataContainer calibration_data;

  std::ifstream file(calibration_file.c_str());
  if (!file.is_open())
    throw std::runtime_error(
        "loadCalibrationData: error opening file " + calibration_file);

  std::string line;
  std::getline(file, line);
  size_t dim = atoi(line.c_str());

  while (file.good())
  {
    std::getline(file, line);
    boost::trim(line);
    if (!line.empty() && line[0] != '#')
    {
      std::vector<std::string> splv;
      boost::split(splv, line, boost::is_any_of("\t "));
      if (splv.size() != dim + 1)
        throw std::runtime_error("calibrate: wrong line size: " + line);
      calibration_data.push_back(std::make_pair(DataType(dim), ""));
      for (size_t i = 0; i < dim; ++i)
        calibration_data.back().first.at(i) = atof(splv.at(i).c_str());
      calibration_data.back().second = splv.at(dim);
    } // if
  } // while

  // Builds the winner matrix
  wm.resize(knetwork.get_no_rows());
  for (size_t r = 0; r < wm.size(); ++r)
    wm.at(r).resize(knetwork.get_no_columns());

  // For each calibration data gets the winner and saves the label in the
  // winner matrix
  for (size_t cdata = 0; cdata < calibration_data.size(); ++cdata)
  {
    ActFun::result_type max_value = 0.0;
    size_t win_row = 0, win_col = 0;

    for (size_t col = 0; col < knetwork.get_no_columns(); ++col)
    {
      for (size_t row = 0; row < knetwork.get_no_rows(); ++row)
      {
        // Gets the result of the unit activation function
        ActFun::result_type res_value =
            knetwork(row, col)(calibration_data.at(cdata).first);

        if (res_value > max_value)
        {
          max_value = res_value;
          win_row = row;
          win_col = col;
        } // if

      } // for row
    } // for col

    std::map<std::string, size_t>::iterator wmit =
        wm[win_row][win_col].find(calibration_data.at(cdata).second);
    if (wmit == wm[win_row][win_col].end())
      wm[win_row][win_col][calibration_data.at(cdata).second] = 1;
    else
      wmit->second++;

  } // for cdata

  return;
} // function calibrate

/**
 * Function printNetworkWeights
 */
void printNetworkWeights(const KohonenNetwork& knetwork)
{
  const typename KohonenNetwork::row_size_t M = knetwork.objects.size();
  const typename KohonenNetwork::col_size_t N = knetwork.objects[0].size();
  for (typename KohonenNetwork::row_size_t i = 0; i < M; ++i)
  {
    for (typename KohonenNetwork::col_size_t j = 0; j < N; ++j)
    {
      std::cout << "\t" << "[";
      for (size_t w = 0; w < knetwork.objects[i][j].weights.size(); ++w)
      {
        if (w != 0) std::cout << ", ";
        std::cout << std::fixed << std::setw(7) << std::setprecision(2)
                  << knetwork.objects[i][j].weights[w];
      } // for w
      std::cout << "]";
    } // for j
    std::cout << std::endl;
  } // for i
  return;
} // function printNetworkWeights

/**
 * Function printNetworkResults
 */
void printNetworkResults(
    KohonenNetwork& knetwork,
    const DataType& data_value)
{
  const typename KohonenNetwork::row_size_t M = knetwork.objects.size();
  const typename KohonenNetwork::col_size_t N = knetwork.objects[0].size();
  for (typename KohonenNetwork::row_size_t i = 0; i < M; ++i)
  {
    for (typename KohonenNetwork::col_size_t j = 0; j < N; ++j)
    {
      std::cout << "\t" << "[";
      std::cout << std::fixed << std::setw(7) << std::setprecision(6)
                << knetwork(i, j)(data_value);
      std::cout << "]";
    } // for j
    std::cout << std::endl;
  } // for i
  return;
} // function printNetworkResults

/**
 * Function saveMap
 */
void saveMap(
    const KohonenNetwork& knetwork,
    const std::string& filename,
    const WinnerMatrix& wm)
{
  std::fstream save_file(filename.c_str(), std::fstream::out);
  if (save_file.fail())
    throw std::runtime_error("Error saving on file " + filename);
  save_file << knetwork.objects[0][0].weights.size() << " hexa ";
  save_file << knetwork.get_no_columns() << " ";
  save_file << knetwork.get_no_rows() << " ";
  save_file << "gaussian \n";

  for (size_t row = 0; row < knetwork.get_no_rows(); ++row)
  {
    for (size_t col = 0; col < knetwork.get_no_columns(); ++col)
    {
      for (size_t w = 0; w < knetwork.objects[row][col].weights.size(); ++w)
        save_file << knetwork.objects[row][col].weights[w] << " ";

      if (!wm.empty())
      {
        // search the most frequent label
        std::string label = "";
        size_t lfreq = 0;
        std::map<std::string, size_t>::const_iterator wmit;
        for (wmit = wm[row][col].begin(); wmit != wm[row][col].end(); ++wmit)
          if (wmit->second > lfreq)
            label = wmit->first;
        save_file << label;
      } // if

      save_file << std::endl;
    } // for col
  } // for row

  save_file.close();
  return;
} // function saveMap


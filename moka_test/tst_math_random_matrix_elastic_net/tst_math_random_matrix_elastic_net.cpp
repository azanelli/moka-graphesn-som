#include <mlpack/methods/lars/lars.hpp>
#include <mlpack/methods/linear_regression/linear_regression.hpp>
#include <moka/global.h>
#include <moka/log.h>
#include "common.h"
#include "common_math.h"

using namespace moka;

/**
 * Function main
 *
 * Computes the least mean square solution on a custom dimensions/random
 * initialized multiresponse linear regression problem with ridge regression,
 * by using different methods.
 */
int main(int argc, char *argv[])
{
  if (argc < 6 + 1)
  {
    Log::out << "Usage: " << Log::endl;
    Log::out << "  argv[1] : seed, 0 = time(NULL) \n";
    Log::out << "  argv[2] : n. examples (X.n_rows) \n";
    Log::out << "  argv[3] : n. predictors (X.n_cols) \n";
    Log::out << "  argv[4] : n. outputs (W.n_cols) \n";
    Log::out << "  argv[5] : lambda1 \n";
    Log::out << "  argv[6] : lambda2 \n";
    Log::out << "  argv[7] : set *not* full rank in the matrix X \n"
             << "if X.n_rows > X.n_cols (yes/no), default no. \n";
    Log::out << "  argv[8] : print matrices (yes/no), default no.\n";
    Log::out << Log::endl;
    return 1;
  } // if (argc < ...)

  // Get the arguments
  int rseed = Global::toInt(argv[1]);
  srand(rseed == 0 ? time(NULL) : rseed);
  Global::Uint n_examples   = Global::toUint(argv[2]);
  Global::Uint n_predictors = Global::toUint(argv[3]);
  Global::Uint n_outputs    = Global::toUint(argv[4]);
  Global::Real lambda1       = Global::toReal(argv[5]);
  Global::Real lambda2       = Global::toReal(argv[6]);
  bool force_not_full_rank = (argc >= 8 && std::string(argv[7]) == "yes");
  bool print_matrices = (argc >= 9 && std::string(argv[8]) == "yes");

  // Inits the matrices
  arma::mat X, Y, W;
  X.randu(n_examples, n_predictors);
  if (force_not_full_rank)
  {
    X.col(1) = X.col(0) * 3;
    X.col(2) = X.col(0);
    X.col(X.n_cols-1) = X.col(0) * 2;
  } // if
  Log::out << "Rank(X) = " << arma::rank(X) << Log::endl;
  W.randu(n_predictors, n_outputs);
  Y = X * W + (arma::randn<arma::mat>(X.n_rows, W.n_cols) / 1000);

  if (print_matrices)
  {
    Log::out << "X: \n" << X << Log::end;
    Log::out << "Y: \n" << Y << Log::endl;
    Log::out << "The solution should be: \n" << W << Log::endl;
  }

  // Ridge regression using LARS algorithm
  Log::out << "Start elastic net using LARS algorithm..." << Log::endl;
  startTimer();
  // Elastic Net with LARS from mlpack
  arma::mat W_mlpack(W.n_rows, W.n_cols);
  mlpack::regression::LARS lars(false, lambda1, lambda2);
  for (std::size_t i = 0; i < W_mlpack.n_cols; ++i)
  {
    arma::colvec w_solution;
    lars.DoLARS(X, Y.col(i));
    lars.Solution(w_solution);
    W_mlpack.col(i) = w_solution;
  } // for
  endTimer();

  if (print_matrices)
    Log::out << "Result: \n" << W_mlpack << Log::end;

  Log::out << "Error: " << pow(norm(W - W_mlpack, "fro"), 2) << Log::endl;
  Log::out << Log::endl;

  return 0;
} // function main


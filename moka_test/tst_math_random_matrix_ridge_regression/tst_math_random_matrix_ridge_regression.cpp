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
  if (argc < 5 + 1)
  {
    Log::out <<"Usage: " <<Log::endl;
    Log::out <<"  argv[1] : seed, 0 = time(NULL)" <<Log::endl;
    Log::out <<"  argv[2] : n. examples (X.n_rows)" <<Log::endl;
    Log::out <<"  argv[3] : n. predictors (X.n_cols)" <<Log::endl;
    Log::out <<"  argv[4] : n. outputs (W.n_cols)" <<Log::endl;
    Log::out <<"  argv[5] : lambda" <<Log::endl;
    Log::out <<"  argv[6] : set *not* full rank in the matrix X "
             <<"if X.n_rows > X.n_cols (yes/no), default no. \n";
    Log::out <<"  argv[7] : print matrices (yes/no), default no." <<Log::endl;
    return 1;
  } // if (argc < ...)

  // Get the arguments
  int rseed = Global::toInt(argv[1]);
  srand(rseed == 0 ? time(NULL) : rseed);
  Global::Uint n_examples   = Global::toUint(argv[2]);
  Global::Uint n_predictors = Global::toUint(argv[3]);
  Global::Uint n_outputs    = Global::toUint(argv[4]);
  Global::Real lambda       = Global::toReal(argv[5]);
  bool force_not_full_rank = (argc >= 7 && std::string(argv[6]) == "yes");
  bool print_matrices = (argc >= 8 && std::string(argv[7]) == "yes");

  // Inits the matrices
  arma::mat X, Y, W;
  X.randu(n_examples, n_predictors);
  if (force_not_full_rank)
  {
    X.col(1) = X.col(0) * 3;
    X.col(2) = X.col(0);
    X.col(X.n_cols-1) = X.col(0) * 2;
  } // if
  Log::out <<"Rank(X) = " <<arma::rank(X) <<Log::endl;
  W.randu(n_predictors, n_outputs);
  Y = X * W + (arma::randn<arma::mat>(X.n_rows, W.n_cols) / 1000);

  if (lambda == 0)
  {
    Log::err <<"Lambda cannot be 0: return." <<Log::endl;
    return 1;
  }
  if (print_matrices)
  {
    Log::out <<"X: \n" <<X <<Log::end;
    Log::out <<"Y: \n" <<Y <<Log::endl;
    Log::out <<"The solution should be: \n" <<W <<Log::endl;
  }

  // Ridge regression through classic formula
  Log::out <<"Start ridge regression through classic formula ..." <<Log::endl;
  startTimer();
  arma::mat *W_classic = new arma::mat(
        arma::inv(
        arma::trans(X) * X +
        arma::diagmat(lambda * arma::ones<arma::Col<Global::Real> >(X.n_cols))
        ) * arma::trans(X) * Y );
  endTimer();
  if (print_matrices)
    Log::out <<"Result: \n" <<(*W_classic) <<Log::end;
  Log::out <<"Error: " <<pow(norm(W - (*W_classic), "fro"), 2) <<Log::endl;
  Log::out <<Log::endl;
  delete W_classic;

  // Ridge regression through SVD decomposition
  Log::out <<"Start ridge regression through SVD decomposition ..." <<Log::endl;
  startTimer();
  arma::mat *U = new arma::mat();
  arma::mat *V = new arma::mat();
  arma::vec *s = new arma::vec();
  arma::svd(*U, *s, *V, X);
  // R = U * S
  // W = V * (R^{T} * R + lambda * I)^{-1} * R^{T} * Y
  arma::mat *R = new arma::mat(X.n_rows, X.n_cols);
  for (Global::Uint i = 0; i < (s->n_rows); ++i)
    R->col(i) = s->at(i) * U->col(i);
  if ((R->n_cols) > (s->n_rows))
    R->submat(arma::span::all, arma::span(s->n_rows, R->n_cols-1)).fill(0.0);
  // lambda != 0 ==> 1/(s^2 + lambda) always ok
  arma::mat *W_svd = new arma::mat(
        (*V) * arma::diagmat(1 / (arma::square(*s) + lambda)) *
        arma::trans(*R) * Y );
  endTimer();
  delete U;
  delete V;
  delete s;
  if (print_matrices)
    Log::out <<"Result: \n" <<(*W_svd) <<Log::end;
  Log::out <<"Error: " <<pow(norm(W - (*W_svd), "fro"), 2) <<Log::endl;
  Log::out <<Log::endl;
  delete W_svd;

  // Ridge regression through mlpack
  Log::out <<"Start ridge regression with mlpack ..." <<Log::endl;
  startTimer();
  mlpack::regression::LARS lars(false, 0.0, lambda);
  arma::mat *W_mlpack = new arma::mat(X.n_cols, Y.n_cols);
  for (Global::Uint i = 0; i < Y.n_cols; ++i) {
    arma::vec w_lars_solution;
    lars.DoLARS(X, Y.col(i));
    lars.Solution(w_lars_solution);
    W_mlpack->col(i) = w_lars_solution;
  }
  endTimer();
  if (print_matrices)
    Log::out << "Result: \n" << (*W_mlpack) << Log::end;
  Log::out << "Error: " << pow(norm(W - (*W_mlpack), "fro"), 2) << Log::endl;
  Log::out << Log::endl;
  delete W_mlpack;

  return 0;
} // function main


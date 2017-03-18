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
 * initialized multiresponse linear regression problem, by using two different
 * methods: the pseudo inversion and through the QR factorization.
 */
int main(int argc, char *argv[])
{
  if (argc < 4 + 1)
  {
    Log::out <<"Usage: " <<Log::endl;
    Log::out <<"  argv[1] : seed, 0 = time(NULL) \n";
    Log::out <<"  argv[2] : n. examples (X.n_rows) \n";
    Log::out <<"  argv[3] : n. predictors (X.n_cols) \n";
    Log::out <<"  argv[4] : n. outputs (W.n_cols) \n";
    Log::out <<"  argv[5] : set *not* full rank in the matrix X "
             <<"if X.n_rows > X.n_cols (yes/no), default no. \n";
    Log::out <<"  argv[6] : print matrices (yes/no), default no." <<Log::endl;
    return 1;
  } // if (argc < ...)

  // Get the arguments
  int rseed = Global::toInt(argv[1]);
  srand(rseed == 0 ? time(NULL) : rseed);
  Global::Uint n_examples   = Global::toUint(argv[2]);
  Global::Uint n_predictors = Global::toUint(argv[3]);
  Global::Uint n_outputs    = Global::toUint(argv[4]);
  bool force_not_full_rank = (argc >= 6 && std::string(argv[5]) == "yes");
  bool print_matrices = (argc >= 7 && std::string(argv[6]) == "yes");

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

  if (print_matrices)
  {
    Log::out <<"X: \n" <<X <<Log::end;
    Log::out <<"Y: \n" <<Y <<Log::endl;
    Log::out <<"The solution should be: \n" <<W <<Log::endl;
  }

  // Regression through pseudo inversion
  Log::out <<"Start regression through pseudo inversion ..." <<Log::endl;
  startTimer();
  arma::mat *W_pinv = new arma::mat(arma::pinv(X)*Y);
  endTimer();
  if (print_matrices)
    Log::out <<"Result: \n" <<(*W_pinv) <<Log::end;
  Log::out <<"Error: " <<pow(norm(W - (*W_pinv), "fro"), 2) <<Log::endl;
  Log::out <<Log::endl;
  delete W_pinv;

  // Regression through QR factorization
  Log::out <<"Start regression through QR factorization ..." <<Log::endl;
  startTimer();
  arma::mat *W_qr = new arma::mat();
  arma::mat *Q = new arma::mat();
  arma::mat *R = new arma::mat();
  arma::qr(*Q, *R, X);
  arma::solve(*W_qr, *R, arma::trans(*Q) * Y); // R * W = Q^T * X
  endTimer();
  delete Q;
  delete R;
  if (print_matrices)
    Log::out << "Result: \n" << (*W_qr) << Log::end;
  Log::out << "Error: " << pow(norm(W - (*W_qr), "fro"), 2) << Log::endl;
  Log::out << Log::endl;
  delete W_qr;

  return 0;
} // function main


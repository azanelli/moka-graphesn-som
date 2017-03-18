#include <mlpack/methods/lars/lars.hpp>
#include <mlpack/methods/linear_regression/linear_regression.hpp>
#include <moka/global.h>
#include <moka/log.h>
#include "common.h"
#include "math_common.h"

using namespace moka;

/**
 * Function main
 *
 * Computes the least mean square solution on a fixed multiresponse linear
 * regression example by using two different methods: the pseudo inversion and
 * through the QR factorization.
 */
int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  // Data and outputs
  arma::mat X, Y, W;
  initExample2(X, Y, W);
  Log::out <<"X: \n" <<X <<Log::end;
  Log::out <<"Y: \n" <<Y <<Log::endl;
  Log::out <<"The solution should be: \n" <<W <<Log::endl;

  // Regression through pseudo inversion
  Log::out <<"Start regression pseudo inversion ..." <<Log::endl;
  arma::mat W_pinv = arma::pinv(X)*Y;
  Log::out <<"Finish:" <<Log::endl;
  Log::out <<W_pinv <<Log::endl;

  // Regression through QR factorization
  Log::out <<"Start regression through QR factorization ..." <<Log::endl;
  arma::mat W_qr;
  arma::mat Q, R;
  arma::qr(Q, R, X);
  // R * W = Q^T * X
  arma::solve(W_qr, R, arma::trans(Q) * Y);
  Log::out <<"Finish:" <<Log::endl;
  Log::out <<W_qr <<Log::endl;
  return 0;
} // function main


#include "math.h"

#include <limits>
#include <vector>
#include <mlpack/methods/lars/lars.hpp>
#include <moka/log.h>

namespace moka {
namespace util {

/**
 * Method read
 *
 * Reads a Matrix from input stream, written using the method Math::write,
 * discarding last characters until a carriage return is readed.
 * If the reading fails an exception of type moka::GenericException will be
 * thrown.
 */
void Math::read(std::istream& is, Matrix& matrix)
{
  if (!matrix.load(is, arma::arma_ascii))
    throw moka::GenericException("Math::read: fail reading the matrix.");
  is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  return;
} // method read

/**
 * Method read
 *
 * Reads a Vector from input stream, written using the method Math::write,
 * discarding last characters until a carriage return is readed.
 * If the reading fails an exception of type moka::GenericException will be
 * thrown.
 */
void Math::read(std::istream& is, Vector& vector)
{
  if (!vector.load(is, arma::arma_ascii))
    throw moka::GenericException("Math::read: fail reading the vector");
  is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  return;
} // method read

/**
 * Method solveTpElasticNet
 *
 * Let be the following the problem of the (multivariate) linear regression
 * transposed against the classical one:
 *   B * X = Y
 * Given the matrix X that contains the input variables column-wise and
 * the matrix Y that contains the response variables column-wise, this method
 * solve the problem finding a value for the coefficients B with the Elastic
 * Net method.
 * Parameters lambda1 and lambda2 that controls the amount of regularization.
 * The Elastic Net solution is computed by the LARS algorithm and it solves:
 *   min_b 0.5 ||b X  - y||_2^2 + lambda1 ||b||_1 + 0.5 lambda2 ||b||_2^2
 * for each row b of the matrix B and respective row y of matrix Y.
 */
void Math::solveTpElasticNet
(
    Matrix& out_B,
    const Matrix& in_X,
    const Matrix& in_Y,
    Real lambda1,
    Real lambda2
)
{
  if (lambda1 == 0 && lambda2 == 0) // more efficent way through pseudo-inv
  {
    Log::err << "Math::solveTpElasticNet: Warning: solving classic linear "
             << "regression problem since both lambda1 and lambda2 are 0."
             << Log::endl;
    solveTpLinearRegression(out_B, in_X, in_Y);
  } // if

  else if (lambda1 == 0) // mlpack 1.0.3 don't work in this case
  {
    Log::err << "Math::solveTpElasticNet: Warning: solving ridge regression "
             << "problem since lambda1 is 0." << Log::endl;
    solveTpRidgeRegression(out_B, in_X, in_Y, lambda2);
  } // else if

  else
  {
    mlpack::regression::LARS lars
    (
        true,    // use cholesky decomposition (avoid some unsolvable probs)
        lambda1, // regularization parameter for l1-norm penalty
        lambda2  // regularization parameter for l2-norm penalty
    );

    for (Uint i = 0; i < out_B.n_rows; ++i)
    {
      Vector b_sol;
      lars.Regress(in_X, arma::trans(in_Y.row(i)), b_sol);
      out_B.row(i) = arma::trans(b_sol);
    } // for

  } // else

  return;
} // method solveTpElasticNet

/**
 * Method solveTpLasso
 *
 * Let be the following the problem of the (multivariate) linear regression
 * transposed against the classical one:
 *   B * X = Y
 * Given the matrix X that contains the input variables column-wise and
 * the matrix Y that contains the response variables column-wise, this method
 * solve the problem finding a value for the coefficients B with the LASSO
 * method. The parameter lambda controls the amount of regularization. LASSO is
 * computed through the LARS algorithm.
 */
void Math::solveTpLasso
(
    Matrix& out_B,
    const Matrix& in_X,
    const Matrix& in_Y,
    Real lambda
)
{
  solveTpElasticNet(out_B, in_X, in_Y, lambda, 0.0);
  return;
} // method solveTpLasso

/**
 * Method solveTpLinearRegression
 *
 * Let be the following the problem of the (multivariate) linear regression
 * transposed against the classical one:
 *   B * X = Y
 * Given the matrix X that contains the input variables column-wise and
 * the matrix Y that contains the response variables column-wise, this method
 * solve the problem finding a value for the coefficients B with the classic
 * linear regression method (computes the solution of the least mean squares
 * problem).
 */
void Math::solveTpLinearRegression
(
    Matrix& out_B,
    const Matrix& in_X,
    const Matrix& in_Y
)
{
  if (in_X.n_cols < in_X.n_rows)
  {
    // if N_R > N_T use Moore-Penrose pseudo inversion
    out_B = in_Y * arma::pinv(in_X);
  } // if
  if ((in_X.n_cols / (double)in_X.n_rows) >= 3.5)
  {
    // if N_T >> N_R use Moore-Penrose pseudo inversion
    out_B = in_Y * arma::pinv(in_X);
  } // if
  else
  {
    // if N_T > N_R use QR factorization
    Matrix Wt, Q, R;
    arma::qr(Q, R, arma::trans(in_X));
    // R * Wt = Q^T * Y^T
    arma::solve(Wt, R, arma::trans(Q) * arma::trans(in_Y));
    out_B = arma::trans(Wt);
  } // if-else
  return;
} // method solveTpLinearRegression

/**
 * Method solveTpRidgeRegression
 *
 * Let be the following the problem of the (multivariate) linear regression
 * transposed against the classical one:
 *   B * X = Y
 * Given the matrix X that contains the input variables column-wise and
 * the matrix Y that contains the response variables column-wise, this method
 * solve the problem finding a value for the coefficients B with the ridge
 * regression method. The parameter lambda controls the amount of
 * regularization.
 */
void Math::solveTpRidgeRegression
(
    Matrix& out_B,
    const Matrix& in_X,
    const Matrix& in_Y,
    Real lambda
)
{
  if (lambda == 0) // prevent singular matrix inversion
    solveTpLinearRegression(out_B, in_X, in_Y);
  else
  {
    Matrix *Xt = new Matrix(arma::trans(in_X));
    // uses Xt
    Matrix YXt = in_Y * (*Xt);
    Matrix XXt = in_X * (*Xt);
    // free memory
    delete Xt;
    // "classic" solution: B = Y * X^T * (X * X^T + lambda * I)
    out_B = YXt * arma::inv(XXt + arma::diagmat(lambda *
        arma::ones<Vector>(in_X.n_rows)));
  }
  return;
} // method solveTpRidgeRegression

/**
 * Method write
 *
 * Write a matrix on output stream in the arma::arma_ascii format.
 */
void Math::write(std::ostream& os, const Matrix& matrix)
{
  matrix.save(os, arma::arma_ascii);
  return;
} // method read

/**
 * Method write
 *
 * Write a vector on output stream in the arma::arma_ascii format.
 */
void Math::write(std::ostream& os, const Vector& vector)
{
  vector.save(os, arma::arma_ascii);
  return;
} // method read

} // namespace util
} // namespace moka

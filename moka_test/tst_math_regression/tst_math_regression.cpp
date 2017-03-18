#include <armadillo>
#include <mlpack/methods/lars/lars.hpp>
#include <mlpack/methods/linear_regression/linear_regression.hpp>

// Functions
void leastSquares();
void ridgeRegression();
void elasticNet();
void lasso();

// Global variables
double lambda1 = 0.0;
double lambda2 = 0.0;
arma::mat X;
arma::colvec y;
arma::colvec w_solution;

/**
 * Function main
 *
 * ...
 */
int main(int argc, char *argv[])
{
  if (argc < 3 + 1)
  {
    std::cout << "Usage: " << std::endl;
    std::cout << "  argv[1]: file containing matrix X." << std::endl;
    std::cout << "  argv[2]: file containing vector y." << std::endl;
    std::cout << "  argv[3]: method: ls, rr, en, lasso." << std::endl;
    std::cout << "  argv[4]: lambda1 (only for 'rr', 'en' and 'lasso')"
              << std::endl;
    std::cout << "  argv[5]: lambda2 (only for 'en')" << std::endl;
    return 1;
  } // if (argc < ...)

  // Parse command options
  std::string X_file = argv[1];
  std::string y_file = argv[2];
  std::string method = argv[3];

  if (method != "ls" && method != "rr" && method != "lasso" && method != "en")
  {
    std::cout << "Error: wrong method." << std::endl;
    return 1;
  }

  if (method == "rr" || method == "lasso" || method == "en")
    lambda1 = atof(argv[4]);
  if (method == "en")
    lambda2 = atof(argv[5]);

  // Reads data
  X.load(X_file, arma::arma_ascii);
  y.load(y_file, arma::arma_ascii);

  std::cout << "X:  \n" << X;
  std::cout << "y': \n" << y.t() << std::endl;
  // std::cout << "The solution should be: \n" << w.t() << std::endl;

  // Switch on method
  if (method == "ls")
    leastSquares();
  else if (method == "rr")
    ridgeRegression();
  else if (method == "en")
    elasticNet();
  else if (method == "lasso")
    lasso();
  else
    return 1;

  std::cout << "Finish.  " << std::endl << std::endl;
  std::cout << "Weights: \n" << w_solution.t();
  std::cout << "Result:  \n" << (X * w_solution).t();
  std::cout << "y':      \n" << y.t();

  return 0;
} // function main

/**
 * Function leastSquares
 *
 * ...
 */
void leastSquares()
{
  // Inserts the additional column for the bias computation
  // X.insert_cols(0, arma::ones(X.n_rows));

  // Least squares using mlpack
  // mlpack::regression::LinearRegression lr;
  // arma::mat Xt = arma::trans(X);
  // std::cout << "Start regression with mlpack ..." << std::endl;
  // lr = mlpack::regression::LinearRegression(Xt, y);
  // arma::colvec w_mlpack = lr.Parameters();

  // Least squares using armadillo pseudoinversion
  std::cout << "Least Squares using armadillo pinv... ";
  w_solution = arma::pinv(X) * y;

  return;
} // function leastSquares

/**
 * Function ridgeRegression
 *
 * ...
 */
void ridgeRegression()
{
  // Inserts the additional column for the bias computation
  // X.insert_cols(0, arma::ones(X.n_rows));

  //  // Ridge regression using mlpack
  //  std::cout << "Start ridge regression with mlpack ..." << std::endl;
  //  mlpack::regression::LARS lars
  //  (
  //      false,   // whether or not to use Cholesky decomposition
  //      0.0,    // regularization parameter for l1-norm penalty
  //      lambda  // regularization parameter for l2-norm penalty
  //  );
  //  arma::vec w_mlpack;
  //  lars.Regress(X, y, w_mlpack, true);

  // Ridge regression using classic formula
  std::cout << "Ridge Regression using classic formula...";
  w_solution = arma::inv(arma::trans(X) * X +
      arma::diagmat(lambda1 * arma::ones<arma::vec>(X.n_cols))) *
      arma::trans(X) * y;

  return;
} // function ridgeRegression

/**
 * Function elasticNet
 *
 * ...
 */
void elasticNet()
{
  // Inserts the additional column for the bias computation
  // X.insert_cols(0, arma::ones(X.n_rows));

  // Elastic Net using LARS from mlpack
  std::cout << "Elastic Net using lars from mlpack... ";
  mlpack::regression::LARS lars
  (
      true,    // whether or not to use Cholesky decomposition
      lambda1, // regularization parameter for l1-norm penalty
      lambda2  // regularization parameter for l2-norm penalty
  );
  lars.Regress(X, y.unsafe_col(0), w_solution, true);

  return;
} // function elasticNet

/**
 * Function lasso
 *
 * ...
 */
void lasso()
{
  // Inserts the additional column for the bias computation
  // X.insert_cols(0, arma::ones(X.n_rows));

  // LASSO using LARS from mlpack
  std::cout << "Lasso using lars from mlpack... ";
  mlpack::regression::LARS lars
  (
      true,    // whether or not to use Cholesky decomposition
      lambda1, // regularization parameter for l1-norm penalty
      0.0      // regularization parameter for l2-norm penalty
  );
  lars.Regress(X, y, w_solution, true);

  return;
} // function lasso

/**
 * Function initExample1
 *
 * Inits the matrix X and the vectors w and y with a fixed example of
 * linear regression, such that X * w = y.
 */
void initExample1(arma::mat& X, arma::colvec& y, arma::colvec& w)
{
  X.reset();
  X   <<  0.81 << -0.17 <<  0.08 << -0.70 << arma::endr
      <<  0.16 << -0.09 <<  0.84 << -0.40 << arma::endr
      << -0.93 << -0.01 <<  0.98 << -0.36 << arma::endr
      <<  0.65 << -0.90 <<  0.92 <<  0.39 << arma::endr
      << -0.04 <<  0.99 <<  0.69 <<  0.63 << arma::endr
      <<  0.70 <<  0.09 << -0.33 << -0.75 << arma::endr
      << -0.47 << -0.50 <<  0.45 <<  0.97 << arma::endr;
  y.reset();
  y <<  1.12 << arma::endr
    <<  0.13 << arma::endr
    <<  0.04 << arma::endr
    << -0.34 << arma::endr
    << -1.21 << arma::endr
    <<  1.44 << arma::endr
    << -0.51 << arma::endr;
  w.reset();
  w <<  0.54 << arma::endr
    << -0.11 << arma::endr
    << -0.50 << arma::endr
    << -0.97 << arma::endr
    << -0.94 << arma::endr;
  return;
} // function initExample1

/**
 * Function initExample2
 *
 * Inits the matrices X, Y and W with a fixed example of multiresponse linear
 * regression, such that X * W = Y.
 */
void initExample2(arma::mat& X, arma::mat& Y, arma::mat& W)
{
  X.reset();
  X << 1 <<  0.34 <<  0.63 <<  0.49 << -0.01 << arma::endr
    << 1 << -0.21 << -0.97 <<  0.98 <<  0.15 << arma::endr
    << 1 <<  0.00 << -0.59 << -0.80 << -0.85 << arma::endr
    << 1 <<  0.11 << -0.35 <<  0.10 <<  0.10 << arma::endr
    << 1 << -0.67 << -0.43 <<  0.53 << -0.89 << arma::endr
    << 1 <<  0.03 <<  0.72 <<  0.94 <<  0.82 << arma::endr
    << 1 << -0.20 <<  0.71 <<  0.61 <<  0.31 << arma::endr;
  Y.reset();
  Y << -0.8150 << -0.3595 << 0.8956 << arma::endr
    << -1.2212 << -1.4806 << 0.2585 << arma::endr
    << -0.8963 << -1.9961 << 1.0205 << arma::endr
    << -0.8348 << -1.0578 << 0.6492 << arma::endr
    << -1.5025 << -1.6920 << 0.7866 << arma::endr
    << -0.8291 <<  0.4945 << 0.5978 << arma::endr
    << -0.9584 <<  0.1499 << 0.7972 << arma::endr;
  W.reset();
  W << -0.85 << -0.79 <<  0.80 << arma::endr
    <<  0.37 << -0.33 <<  0.02 << arma::endr
    <<  0.07 <<  0.89 <<  0.30 << arma::endr
    << -0.27 << -0.02 << -0.21 << arma::endr
    <<  0.26 <<  0.82 << -0.27 << arma::endr;
  return;
} // function initExample2

/**
 * Function initExampleBoolean
 *
 * Inits the matrix X and the output vector y with a boolean function (with
 * only 1 or 0 elements).
 */
void initExampleBoolean(arma::mat& X, arma::colvec& y)
{
  X.reset();
  X   << 1 << 1 << 0 << 0 << 0 << arma::endr
      << 0 << 1 << 0 << 0 << 0 << arma::endr
      << 1 << 0 << 0 << 0 << 0 << arma::endr
      << 0 << 0 << 0 << 0 << 0 << arma::endr
      << 1 << 1 << 1 << 0 << 0 << arma::endr
      << 0 << 1 << 1 << 0 << 0 << arma::endr
      << 1 << 0 << 1 << 0 << 0 << arma::endr
      << 0 << 0 << 1 << 0 << 0 << arma::endr
      << 1 << 1 << 0 << 1 << 0 << arma::endr
      << 0 << 1 << 0 << 1 << 0 << arma::endr
      << 0 << 0 << 1 << 0 << 0 << arma::endr;

  y.reset();
  y
    <<  1 << arma::endr
    << -1 << arma::endr
    << -1 << arma::endr
    << -1 << arma::endr
    <<  1 << arma::endr
    << -1 << arma::endr
    << -1 << arma::endr
    << -1 << arma::endr
    <<  1 << arma::endr
    << -1 << arma::endr
    <<  1 << arma::endr;

  return;
} // function initExampleBoolean

/**
 * Function initRandomExample
 *
 * Fills the matrices X, Y and W
 */
void initRandomExample(arma::mat& X, arma::mat& Y, arma::mat& W)
{
  X.randu();
  W.randu();
  Y = X * W;
  return;
} // function initRandomExample

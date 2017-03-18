#ifndef MOKA_UTIL_MATH_H
#define MOKA_UTIL_MATH_H

#include <vector>
#include <armadillo>
#include <moka/global.h>

namespace moka {
namespace util {

/**
 * Class Math
 *
 * Contains various math utilities and functions.
 */
class Math
{
  public:

    typedef Global::Int Int;
    typedef Global::Real Real;
    typedef Global::Uint Uint;

    typedef arma::Col<Real> Vector;
    typedef arma::Mat<Real> Matrix;

    //! Max absolute value: Max_i( |x_i| )
    template <typename Container>
    static inline Real maxAbs(const Container& container);

    //! Average: Sum_i(x_i) / N
    template <typename Container>
    static inline Real avg(const Container& container);

    //! Median value for std::vector
    template <typename ValueType>
    static inline ValueType median(std::vector<ValueType>& vect);

    //! Vecctor norm 1: Sum_i( |x_i| )
    template <typename Container>
    static inline Real norm1(const Container& container);

    //! Reads a matrix from input stream.
    static void read(std::istream& is, Matrix& matrix);

    //! Reads a vector from input stream.
    static void read(std::istream& is, Vector& vector);

    //! Solve linear regression (trasposed) problem using elastic net.
    static void solveTpElasticNet
    (
        Matrix& out_B,
        const Matrix& in_X,
        const Matrix& in_Y,
        Real lambda1,
        Real lambda2
    );

    //! Solve linear regression (trasposed) problem using LASSO.
    static void solveTpLasso
    (
        Matrix& out_B,
        const Matrix& in_X,
        const Matrix& in_Y,
        Real lambda
    );

    //! Solve linear regression (trasposed) problem using least mean squares.
    static void solveTpLinearRegression
    (
        Matrix& out_B,
        const Matrix& in_X,
        const Matrix& in_Y
    );

    //! Solve linear regression (trasposed) problem using ridge regression.
    static void solveTpRidgeRegression
    (
        Matrix& out_B,
        const Matrix& in_X,
        const Matrix& in_Y,
        Real lambda
    );

    //! Standard deviation: sqrt(Sum_i((x_i - mean)^2) / N)
    template <typename Container>
    static inline Real stdev(const Container& container);

    //! Standard deviation: sqrt(Sum_i((x_i - mean)^2) / N)
    template <typename Container>
    static inline Real stdev(const Container& container, const Real& average);

    //! Sum of elements: Sum_i(x_i)
    template <typename Container>
    static inline Real sum(const Container& container);

    //! Writes a matrix on output stream.
    static void write(std::ostream& os, const Matrix& matrix);

    //! Writes a vector on output stream.
    static void write(std::ostream& os, const Vector& vector);

}; // class Math

} // namespace util
} // namespace moka

#include "math_impl.h"

#endif // MOKA_UTIL_MATH_H

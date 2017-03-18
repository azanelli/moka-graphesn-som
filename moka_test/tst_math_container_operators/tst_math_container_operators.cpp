#include <vector>
#include <list>
#include <moka/global.h>
#include <moka/util/math.h>
#include "../common.h"

using namespace moka;
using namespace moka::util;

/**
 * Function main
 */
int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " [list of real numbers]" << std::endl;
    return 1;
  } // if (argc < ...)

  std::vector<double> elements;
  std::cout << "Elements: ";
  for (int n = 1; n < argc; ++n)
  {
    std::cout << std::atof(argv[n]) << " ";
    elements.push_back(std::atof(argv[n]));
  }
  std::cout << std::endl;

  double mean = Math::mean(elements);
  std::cout << "max abs: " << Math::maxAbs(elements) << std::endl;
  std::cout << "mean:    " << Math::mean(elements) << std::endl;
  std::cout << "norm_1:  " << Math::norm1(elements) << std::endl;
  std::cout << "stdev:   " << Math::standardDeviation(elements, mean)
            << std::endl;

  return 0;
} // function main

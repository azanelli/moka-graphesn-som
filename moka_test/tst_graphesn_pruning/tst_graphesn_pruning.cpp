#include <string>
#include <moka/structure/sdfdataset.h>
#include <moka/model/graphesn.h>
#include <moka/modelmaker/graphesnmaker.h>
#include "common.h"

using namespace moka;
using namespace moka::model;
using namespace moka::modelmaker;
using namespace moka::structure;
using namespace moka::util;

/**
 * Function main
 */
int main(int argc, char *argv[]) 
{
  if (argc < 6 + 1) 
  {
    std::cout <<"Usage: " <<std::endl;
    std::cout <<"  argv[1] : random seed (0 for time(NULL))" <<std::endl;
    std::cout <<"  argv[2] : ptc file (sdf format)" <<std::endl;
    std::cout <<"  argv[3] : reservoir-size" <<std::endl;
    std::cout <<"  argv[4] : regularization (none, ridge-regression, lasso, "
              <<"elastic-net)" <<std::endl;
    std::cout <<"  argv[5] : sigma1 (rr, lasso, en)" <<std::endl;
    std::cout <<"  argv[6] : sigma2 (only used for en)" <<std::endl;
    return 1;
  } // if (argc < ...)

  // Parse parameters
  Uint rseed = Global::toInt(argv[1]);
  std::string dsfile = argv[2];
  Uint res_size = Global::toInt(argv[3]);
  std::string regularization = argv[4];
  Real lambda1 = Global::toReal(argv[5]);
  Real lambda2 = Global::toReal(argv[6]);

  // Loads the dataset
  SdfDataset ds;
  ds.loadSDFFile(dsfile, "RecNN.name", "PTC.CLASS.FM", 1);

  // Inits the GraphESNMaker
  GraphESNMaker graphesn_maker;
  util::Parameters model_prm, training_prm;
  model_prm["reservoir-size"] = Global::toString(res_size);
  model_prm["random-seed"] = Global::toString(rseed);
  training_prm["regularization"] = regularization;
  if (regularization == "ridge-regression")
    training_prm["ridge-regression-lambda"] = Global::toString(lambda1);
  if (regularization == "lasso")
    training_prm["lasso-lambda"] = Global::toString(lambda1);
  if (regularization == "elastic-net")
  {
    training_prm["elastic-net-lambda1"] = Global::toString(lambda1);
    training_prm["elastic-net-lambda2"] = Global::toString(lambda2);
  } // if
  graphesn_maker.bindTrainingSet(&ds);
  graphesn_maker.setParameters(model_prm, training_prm);

  // Builds and trains the GraphESN
  graphesn_maker.build();
  graphesn_maker.train();

  // Prints training results
  std::cout << "## GraphESN succesfully trained ##" << std::endl;
  std::cout << std::endl;
  std::cout << graphesn_maker.getModel().info().list();
  std::cout << std::endl << std::endl;
  std::cout << graphesn_maker.info().list();
  std::cout << std::endl << std::endl;
  std::cout << graphesn_maker.getNumericResults().list();
  std::cout << std::endl << std::endl;

  // Prints the GraphESN just trained
  std::cout << "## GraphESN ##" << std::endl;
  std::cout << std::endl;
  static_cast<const GraphESN&>(graphesn_maker.getModel()).
      print(std::cout, true);

  return 0;
} // function main


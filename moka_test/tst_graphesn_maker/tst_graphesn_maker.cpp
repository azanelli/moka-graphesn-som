#include <moka/structure/graphdataset.h>
#include <moka/structure/sdfdataset.h>
#include <moka/model/graphesn.h>
#include <moka/modelmaker/graphesnmaker.h>
#include "common.h"

using namespace moka::model;
using namespace moka::modelmaker;
using namespace moka::structure;
using namespace moka::util;

/**
 * Function graphesn_maker_test
 *
 * Does the following:
 *   - Builds and loads a dataset
 *   - Builds the GraphESNMaker
 *   - Trains a GraphESN through the GraphESNMaker
 */
int graphesn_maker_test(int argc, char *argv[]) 
{
  if (argc < 4 + 1) 
  {
    std::cout <<"Usage: " <<std::endl;
    std::cout <<"  argv[1] : random seed (0 == time(NULL))" <<std::endl;
    std::cout <<"Dataset arguments:" <<std::endl;
    std::cout <<"  argv[2] : moldirectory" <<std::endl;
    std::cout <<"  argv[3] : outputsfile" <<std::endl;
    std::cout <<"GraphESN arguments:" <<std::endl;
    std::cout <<"  argv[4] : reservoir-size" <<std::endl;
    std::cout <<"  argv[5] : input-weights-range (optional)" <<std::endl;
    std::cout <<"  argv[6] : sigma (optional)" <<std::endl;
    std::cout <<"  argv[7] : epsilon (optional)" <<std::endl;
    return 1;
  } // if (argc < ...)

  // Sets random generator seed
  Uint seed = Global::toUint(argv[1]);
  Global::setRandSeed( seed == 0 ? time(NULL) : seed );
  std::cout <<"Random seed setted as: " <<Global::getRandSeed() <<std::endl;

  // Builds and loads the dataset
  std::cout <<"Builds and loads the dataset" <<std::endl;
  Dataset ds;
  ds.loadMOLDir(argv[2],argv[3]);
  std::cout <<"  > Dataset size: " <<ds.getSize() <<std::endl;

  // Builds the GraphESNMaker
  std::cout <<"Builds the GraphESNMaker" <<std::endl;
  Global::Params *modelparams = new Global::Params();
  Global::Params *trainingparams = new Global::Params();
  (*modelparams)["reservoir-size"] = argv[4];
  if (argc >= 6) (*modelparams)["input-weights-range"] = argv[5];
  if (argc >= 7) (*modelparams)["sigma"] = argv[6];
  if (argc >= 8) (*modelparams)["epsilon"] = argv[7];
  GraphESNMaker gesnmaker(trainingparams, modelparams, ds);

  // Trains the GraphESN through the GraphESNMaker
  ds.randomShuffleDataset();
  ds.splitVaSetPercent(20);
  std::cout <<"Start the training" <<std::endl;
  gesnmaker.train();

  return 0;
} // function main



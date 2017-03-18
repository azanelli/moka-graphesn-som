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
 * Function main
 *
 * Does the following:
 *   - Sets random generator seed.
 *   - Builds and loads the dataset.
 *   - Builds the GraphESN.
 *   - For each instance in the dataset computes the output.
 */
int main(int argc, char *argv[]) 
{

  if (argc < 4 + 1) 
  {
    std::cout <<"Usage: " <<std::endl;
    std::cout <<"  argv[0] : <program name>" <<std::endl;
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

  // Builds the GraphESN
  std::cout <<"Builds the GraphESN" <<std::endl;
  Global::Params params;
  params["input-size"] =
        Global::toString(ds[0].getInputGraph().getElementsSize());
  Uint k = 0, k_tmp = 0;
  for (Uint i = 0; i < ds.getSize(); ++i)
    k = ( (k_tmp = ds[i].getInputGraph().maxDegree()) > k ) ? k_tmp : k;
  params["graph-max-degree"] = Global::toString(k);
  params["output-size"] = Global::toString(ds.at(0).getOutputs().size());
  params["reservoir-size"] = argv[4];
  if (argc >= 6) params["input-weights-range"] = argv[5];
  if (argc >= 7) params["sigma"] = argv[6];
  if (argc >= 8) params["epsilon"] = argv[7];
  GraphESN graphesn(params);
  std::cout <<graphesn <<std::endl;

  // For each instance in the dataset computes the output
  std::cout <<"For each instance in the dataset computes the output"
           <<std::endl;
  for (Uint i = 0; i < ds.getSize(); ++i) {
    graphesn.compute(ds[i].getInputGraph());
    std::cout <<ds[i].getId() <<": " <<std::endl;
    std::cout <<"  > state:  " <<graphesn.getLastState().t();
    std::cout <<"  > output: " <<graphesn.getLastOutput().t();
  } // for i
  std::cout <<std::endl;

  return 0;
} // function main


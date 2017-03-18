#include <moka/structure/graphdataset.h>
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
 *
 * Does the following:
 *   - Sets random generator seed.
 *   - Builds and prints a GraphESN with the passed parameters.
 *   - Builds the example graph
 *   - Tests the GraphESN on that graph
 */
int main(int argc, char *argv[]) {
  if (argc < 6) {
    std::cout <<"Usage: " <<std::endl;
    std::cout <<"  argv[0] : <program name>" <<std::endl;
    std::cout <<"  argv[1] : random seed (0 for time(NULL))" <<std::endl;
    std::cout <<"  argv[2] : input-size" <<std::endl;
    std::cout <<"  argv[3] : reservoir-size" <<std::endl;
    std::cout <<"  argv[4] : output-size" <<std::endl;
    std::cout <<"  argv[5] : graph-max-degree (k)" <<std::endl;
    std::cout <<"Optional arguments:" <<std::endl;
    std::cout <<"  argv[6] : input-weights-range" <<std::endl;
    std::cout <<"  argv[7] : sigma" <<std::endl;
    std::cout <<"  argv[8] : epsilon" <<std::endl;
    return 1;
  } // if (argc < ...)

  GraphESN graphesn1;

  // Sets random generator seed.
  Uint seed = Global::toUint(argv[1]);
  Global::setRandSeed( seed == 0 ? time(NULL) : seed );

  // Builds and prints the GraphESN
  util::Parameters params;
  params["input-size"] = argv[2];
  params["reservoir-size"] = argv[3];
  params["output-size"] = argv[4];
  params["graph-max-degree"] = argv[5];
  if (argc >= 7) params["input-weights-range"] = argv[6];
  if (argc >= 8) params["sigma"] = argv[7];
  if (argc >= 9) params["epsilon"] = argv[8];
  graphesn1.build(params);
  std::cout <<graphesn1 <<std::endl;

  // Builds the example graph (from AA2 slides)
  Graph aa2graph(1);
  aa2graph.pushBack(
      new arma::colvec("0.5"),
      new std::vector<Uint>(s2v<Uint>("1 2")) );
  aa2graph.pushBack(
      new arma::colvec("-1"),
      new std::vector<Uint>(s2v<Uint>("0 2")) );
  aa2graph.pushBack(
      new arma::colvec("0.5"),
      new std::vector<Uint>(s2v<Uint>("4")) );
  aa2graph.pushBack(
      new arma::colvec("1"),
      new std::vector<Uint>(s2v<Uint>("2")) );
  aa2graph.pushBack(
      new arma::colvec("-0.5"),
      new std::vector<Uint>(s2v<Uint>("0 3")) );
  std::cout <<"Input graph: " <<std::endl;
  aa2graph.print(std::cout);
  std::cout <<std::endl;

  // Tests the GraphESN on that graph
  graphesn1.compute(aa2graph);
  std::cout <<"Final state graph: " <<std::endl;
  graphesn1.getLastStateGraph().print(std::cout);
  std::cout <<std::endl;
  std::cout <<"Final state: " <<std::endl;
  std::cout <<graphesn1.getLastState().t();
  std::cout <<"Final output: " <<std::endl;
  std::cout <<graphesn1.getLastOutput().t();
  std::cout <<"Iterations: " <<graphesn1.getLastNumberOfIterations() <<std::endl;

  return 0;
} // function main


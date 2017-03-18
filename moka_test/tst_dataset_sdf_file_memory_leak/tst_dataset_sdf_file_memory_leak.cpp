#include <iostream>
#include <moka/global.h>
#include <moka/log.h>
#include <moka/structure/sdfdataset.h>
#include "common.h"

using namespace moka;
using namespace moka::structure;

/**
 * Function main
 *
 * Builds and destroys many datasets loaded from an SDF File in order to
 * test if there are memory leaks.
 */
int main(int argc, char *argv[])
{
  if (argc < 4+1)
  {
    std::cout <<"Usage: " <<std::endl;
    std::cout <<"  argv[1] : sdffile" <<std::endl;
    std::cout <<"  argv[2] : idname" <<std::endl;
    std::cout <<"  argv[3] : output-name" <<std::endl;
    std::cout <<"  argv[4] : noutputs" <<std::endl;
    return 1;
  } // if (argc < ...)

  std::vector<SdfDataset*> vds;
  std::string opt("");
  while (opt != "q")
  {
    std::cout <<"There are " <<vds.size() <<" (active) datasets in memory.";
    std::cout <<std::endl;
    if (!vds.empty())
    {
      std::cout <<"The last dataset contains " <<vds.back()->getSize();
      std::cout <<" instances." <<std::endl;
    }
    std::cout <<"Select an option: " <<std::endl;
    std::cout <<"  [q] Exit." <<std::endl;
    std::cout <<"  [a] Loads other <N> datasets." <<std::endl;
    std::cout <<"  [b] Removes all the datasets loaded." <<std::endl;
    std::cout <<"  [c] Loads and removes <N> datasets <T> times." <<std::endl;
    std::cout <<"  >";
    std::cin >>opt;
    std::cout <<std::endl;
    if (opt == "q")
    {
      /* nop */
    } // if opt == q
    else if (opt == "a")
    {
      std::string strN;
      std::cout <<"N: ";
      std::cin >>strN;
      Uint N = Global::toUint(strN);
      std::cout <<"N = " <<N <<std::endl;
      for (Uint i = 0; i < N; ++i)
      {
        vds.push_back(new SdfDataset());
        vds.back()->
            loadSDFFile(
                argv[1], // sdf file
                argv[2], // id name
                argv[3], // outputs name
                Global::toUint(argv[4])); // n. outputs
      } // for
      std::cout <<std::endl;
    } // if opt == a
    else if (opt == "b")
    {
      std::for_each(vds.begin(), vds.end(), bll::delete_ptr());
      vds.clear();
      vds.reserve(0);
    } // if opt == b
    else if (opt == "c")
    {
      std::string strN, strT;
      std::cout <<"N: ";
      std::cin >>strN;
      std::cout <<"T: ";
      std::cin >>strT;
      Uint N = Global::toUint(strN);
      Uint T = Global::toUint(strT);
      std::cout <<"N = " <<N <<", T = " <<T <<std::endl;
      for (Uint t = 0; t < T; ++t)
      {
        for (Uint n = 0; n < N; ++n)
        {
          vds.push_back(new SdfDataset());
          vds.back()->
              loadSDFFile(
                  argv[1], // sdf file
                  argv[2], // id name
                  argv[3], // outputs name
                  Global::toUint(argv[4])); // n. outputs
        } // for n
        std::for_each(vds.begin(), vds.end(), bll::delete_ptr());
        vds.clear();
        std::cout <<"... time: " <<t+1 <<std::endl;
      } // for t
      std::cout <<std::endl;
    } // if opt == b
    else
      std::cout <<"Invalid option..." <<std::endl <<std::endl;
  } // while opt

  return 0;
} // function main


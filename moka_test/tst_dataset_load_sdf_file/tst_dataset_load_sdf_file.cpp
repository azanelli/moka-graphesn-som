#include <exception>
#include <fstream>
#include <iostream>
#include <moka/dataset/datasetdispenser.h>
#include <moka/util/parameters.h>

using namespace moka;
using namespace moka::dataset;

/**
 * Function main
 *
 * Test the class SdfDataset (in particular way the load method).
 */
int main(int argc, char *argv[])
{
  if (argc < 4+1)
  {
    Log::out <<"Usage: \n";
    Log::out <<"  argv[1] : sdffile \n";
    Log::out <<"  argv[2] : idname \n";
    Log::out <<"  argv[3] : output-name \n";
    Log::out <<"  argv[4] : noutputs \n";
    Log::out <<"  argv[5] : atom list (default \"\") \n";
    Log::out <<"  argv[6] : try to split the dataset (yes/no, default no) \n";
    Log::out <<"  argv[7] : print graph for each instance (yes/no, "
        <<"default no) \n";
    Log::out <<"  argv[8] : sort by id (yes/no, default yes) \n";
    Log::out <<Log::endl;
    return 1;
  } // if (argc < ...)

  std::string atomlist = (argc >= 6 ? std::string(argv[5]) : "");
  bool split = (argc >= 7 && std::string(argv[6]) == "yes");
  bool print_graphs = (argc >= 8 && std::string(argv[7]) == "yes");
  bool sort_by_id = (argc < 9 || std::string(argv[8]) != "no");

  SdfDataset ds;
  ds.setRandomNumberGeneratorSeed(); // time(NULL)

  // should do nothing
  ds.randomShuffleDataset();
  ds.randomShuffleTrainingSet();

  ds.loadSDFFile(
      argv[1], // sdf file
      argv[2], // idname
      argv[3], // output name
      Global::toUint(argv[4]), // n outputs
      atomlist, // atom list (e.g.: "Cl,H,Na,O")
      sort_by_id);

  Log::out <<"Dataset succesfullty loaded (skipped ";
  Log::out <<ds.getSkippedMolecules() <<" molecules):";
  Log::out <<Log::endl;
  std::stringstream ss;
  ds.print(
        ss, // ostream
        false, // prints split
        print_graphs); // prints graphs
  Log::out <<ss.str() <<Log::endl;

  // some shuffle
  for (int i = 0; i < 100; ++i)
  {
    ds.randomShuffleDataset();
    ds.randomShuffleTrainingSet();
  }

  if (split) {
    ds.splitInFolds(5);
    ds.setValidationFold(3);
    ds.randomShuffleTrainingSet();
    ss.str("");
    Log::out <<"After split in 5 folds, set the fold 3 to validation and "
        <<"random shuffle the training set: " <<Log::endl;
    ds.print(
          ss, // ostream
          true, // prints split
          false); // prints graphs
    Log::out <<ss.str() <<Log::endl;
  } // if

  return 0;
} // function main


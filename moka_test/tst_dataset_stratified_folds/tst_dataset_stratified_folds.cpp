#include <string>
#include <moka/global.h>
#include <moka/structure/basedataset.h>
#include <moka/structure/csvdataset.h>

using namespace moka;
using namespace moka::structure;

void printDataset(const CsvDataset& ds);
void printTrSetVaSet(const CsvDataset& ds);

/**
 * Function main
 */
int main(int argc, char *argv[])
{
  if (argc < 3+1)
  {
    std::cout << "Usage: \n";
    std::cout << "  argv[1] : file.csv \n";
    std::cout << "  argv[2] : n inputs \n";
    std::cout << "  argv[3] : n outputs \n";
    std::cout << "  argv[4] : n folds \n";
    std::cout << std::endl;
    return 1;
  } // if (argc < ...)

  // parse parameters
  std::string csv_file = std::string(argv[1]);
  Uint ninputs = Global::toUint(argv[2]);
  Uint noutputs = Global::toUint(argv[3]);
  Uint nfolds = Global::toUint(argv[4]);

  CsvDataset ds;
  ds.setRandomNumberGeneratorSeed(); // time(NULL)

  // should do nothing
  ds.randomShuffleDataset();
  ds.randomShuffleTrainingSet();

  ds.loadCSVFile(csv_file, ninputs, noutputs);

  std::cout << "## Dataset succesfullty loaded ## " << std::endl;
  std::cout << "Instances: " << ds.getSize() << std::endl;
  std::cout << std::endl;
  printDataset(ds);
  std::cout << std::endl;

  std::cout << "## Stratified splitting ##";
  std::cout << std::endl << std::endl;
  ds.splitInStratifiedFolds(nfolds);
  printTrSetVaSet(ds);
  std::cout << std::endl;

  for (Uint fold = 0; fold < nfolds; ++fold)
  {
    ds.setValidationFold(fold);
    std::cout << "## Validation on fold " << ds.getValidationFold() << " ##";
    std::cout << std::endl << std::endl;
    printTrSetVaSet(ds);
    std::cout << std::endl;
  } // for

  std::cout << "## Merge ##";
  std::cout << std::endl << std::endl;
  ds.merge();
  std::cout << "Dataset size: " << ds.getSize() << std::endl;
  std::cout << "Training set size: " << ds.getTrSetSize() << std::endl;
  std::cout << "Validation set size: " << ds.getVaSetSize() << std::endl;
  std::cout << std::endl;
  printDataset(ds);
  std::cout << std::endl;

  std::cout << "## Shuffle the dataset and splitting (stratified) ##";
  std::cout << std::endl << std::endl;
  ds.randomShuffleDataset();
  ds.splitInStratifiedFolds(nfolds);
  printTrSetVaSet(ds);
  std::cout << std::endl;

  std::cout << "## Shuffle training set ##";
  std::cout << std::endl << std::endl;
  ds.randomShuffleTrainingSet();
  printTrSetVaSet(ds);
  std::cout << std::endl;

  std::cout << "## Merge ##";
  std::cout << std::endl << std::endl;
  ds.merge();
  printDataset(ds);
  std::cout << std::endl;

  std::cout << "## Restore ##";
  std::cout << std::endl << std::endl;
  ds.splitInStratifiedFolds(nfolds);
  std::cout << "Dataset size: " << ds.getSize() << std::endl;
  std::cout << "Training set size: " << ds.getTrSetSize() << std::endl;
  std::cout << "Validation set size: " << ds.getVaSetSize() << std::endl;
  std::cout << std::endl;
  ds.restore();
  printDataset(ds);
  std::cout << std::endl;

  return 0;
} // function main

/**
 * Function printDataset
 */
void printDataset(const CsvDataset& ds)
{
  for (Uint i = 0; i < ds.getSize(); ++i)
  {
    std::cout << ds.getId(i) << " " << Global::toString(ds.getInput(i));
    std::cout << " " << Global::toString(ds.getOutput(i)) << std::endl;
  } // for
  return;
} // function printDataset

/**
 * Function printTrSetVaSet
 */
void printTrSetVaSet(const CsvDataset& ds)
{
  std::cout << "Va. set" << std::endl;
  for (Uint i = 0; i < ds.getVaSetSize(); ++i)
  {
    std::cout << ds.vaAt(i).getId() << " ";
    std::cout << Global::toString(ds.vaAt(i).getInput());
    std::cout << " " << Global::toString(ds.vaAt(i).getOutput()) << std::endl;
  } // for
  std::cout << std::endl;
  std::cout << "Tr. set" << std::endl;
  for (Uint i = 0; i < ds.getTrSetSize(); ++i)
  {
    std::cout << ds.trAt(i).getId() << " ";
    std::cout << Global::toString(ds.trAt(i).getInput());
    std::cout << " " << Global::toString(ds.trAt(i).getOutput());
    std::cout << std::endl;
  } // for
  return;
} // function printTrSetVaSet


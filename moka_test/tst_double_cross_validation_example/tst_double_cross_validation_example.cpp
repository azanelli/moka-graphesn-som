#include <moka/modelmaker/modelmaker.h>
#include <moka/modelmaker/modelmakerdispenser.h>
#include <moka/procedure/doublecrossvalidation.h>
#include <moka/dataset/datasetdispenser.h>
#include <moka/util/parameters.h>

using namespace moka;
using namespace moka::dataset;
using namespace moka::modelmaker;
using namespace moka::procedure;
using namespace moka::util;

/**
 * Function main
 */
int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  // Create a new ModelSelection object with the set of parameters that you
  // want to test. At each step of the double cross validation the
  // ModelSelection object will be used to select the best parameters on the
  // training split of the dataset and such parameters are used to build a
  // model to test on the validation split.
  ModelSelection *ms = new ModelSelection();
  Parameters model_prm, training_prm;
  model_prm["reservoir-size"] = "10 & 20";
  model_prm["reservoir-connectivity"] = "0.4";
  model_prm["input-weights-range"] = "0.01";
  model_prm["sigma"] = "1.0";
  model_prm["epsilon"] = "1e-02";
  model_prm["som-no-rows"] = "10";
  model_prm["som-no-cols"] = "6";
  training_prm["regularization"] = "ridge-regression";
  training_prm["ridge-regression-lambda"] = "1e-2 & 1e-3 & 1e-5";
  ms->setModelMakerParameters(model_prm, training_prm);

  // Builds the CrossValidation object to use in the model selection (i.e. the
  // "sub cross validation" in the double cross validation procedure).
  CrossValidation *cv = new CrossValidation();
  Parameters cv_prm;
  cv_prm["cv-folds"] = "10";
  cv_prm["cv-steps"] = "5";
  cv->setParameters(cv_prm);
  ms->bindCrossValidation(cv);

  // Create a new DoubleCrossValidation object.
  DoubleCrossValidation *dcv = new DoubleCrossValidation();

  // Get a new ModelMaker object for a GraphESN model and bind it to the
  // DoubleCrossValidation.
  ModelMaker *mm = ModelMakerDispenser::get("gmm");
  dcv->bindModelMaker(mm);

  // Load the training set in the DoubleCrossValidation object.
  Parameters trset_prm;
  trset_prm["dataset-type"] = "multi-labeled-graph";
  trset_prm["load-from"] = "dataset-file";
  trset_prm["file-path"] = "test/data/ptc_pp3.MR.dataset";

  // Bind the model selection object in the double cross validation.
  dcv->bindModelSelection(ms);

  // Set your parameters for the double cross validation process
  Parameters dcv_prm;
  dcv_prm["dcv-folds"] = "5"; // splits the dataset into 5 folds
  dcv_prm["dcv-steps"] = "3";
  dcv->setParameters(dcv_prm);

  // Loads and bind training set
  Dataset *trset = DatasetDispenser::get(trset_prm["dataset-type"]);
  if (!trset)
    throw std::runtime_error("invalid <dataset-type> loading tr. set");
  trset_prm.remove("dataset-type");
  trset->load(trset_prm);
  dcv->bindDataset(trset);

  // Start the double cross validation procedue.
  dcv->start();

  // Print out the double cross validation results.
  std::cout << "Double cross validation results: " << std::endl;
  std::cout << dcv->getResults().getList("  > ") << std::endl;

  // Delete all the objects before exit.
  delete dcv;
  delete ms;
  delete cv;
  delete mm;

  return 0;
} // function main


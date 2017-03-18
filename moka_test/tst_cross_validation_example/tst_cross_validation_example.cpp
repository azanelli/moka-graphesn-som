#include <moka/modelmaker/modelmaker.h>
#include <moka/modelmaker/modelmakerdispenser.h>
#include <moka/procedure/crossvalidation.h>
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

  // Get a new ModelMaker object for a GMM model.
  ModelMaker *mm = ModelMakerDispenser::get("gmm");

  // Put into model_prm your parameters for the model and into training_prm
  // parameters for the training.
  Parameters model_prm, training_prm;
  model_prm["reservoir-size"] = "30";
  model_prm["reservoir-connectivity"] = "0.4";
  model_prm["input-weights-range"] = "0.1";
  model_prm["sigma"] = "1.5";
  model_prm["epsilon"] = "1e-02";
  model_prm["som-no-rows"] = "10";
  model_prm["som-no-cols"] = "6";
  training_prm["regularization"] = "ridge-regression";
  training_prm["ridge-regression-lambda"] = "1e-02";

  // Sets the model and the training parameters into the ModelMaker.
  mm->setParameters(model_prm, training_prm);

  // Set parameters to load the training set.
  Parameters trset_prm;
  trset_prm["dataset-type"] = "multi-labeled-graph";
  trset_prm["load-from"] = "dataset-file";
  trset_prm["file-path"] = "test/data/ptc_pp3.MR.dataset";

  // Fill cv_prm with your parameters for the cross validation procedure.
  Parameters cv_prm;
  cv_prm["cv-folds"] = "5"; // splits the dataset into 10 folds

  // Create a new CrossValidation object setting the parameters, loading the
  // training set and binding the model maker.
  CrossValidation *cv = new CrossValidation();
  cv->setParameters(cv_prm);
  cv->bindModelMaker(mm);

  // Loads and bind training set
  Dataset *trset = DatasetDispenser::get(trset_prm["dataset-type"]);
  if (!trset)
    throw std::runtime_error("invalid <dataset-type> loading tr. set");
  trset_prm.remove("dataset-type");
  trset->load(trset_prm);
  cv->bindTrainingSet(trset);

  // Start the cross validation using the model maker on the loaded dataset
  // (splitted in 10 folds).
  cv->start();

  // Print out the cross validation results obtained by the model.
  std::cout << "Cross validation results: " << std::endl;
  std::cout << cv->getResults().list("  > ") << std::endl;

  // Remember to delete also the model maker before exit
  delete cv;
  delete mm;

  return 0;
} // function main


#include <moka/modelmaker/modelmaker.h>
#include <moka/modelmaker/modelmakerdispenser.h>
#include <moka/procedure/crossvalidation.h>
#include <moka/procedure/modelselection.h>
#include <moka/util/parameters.h>

using namespace moka;
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

  // Get a new ModelMaker object for a GraphESN model.
  ModelMaker *mm = ModelMakerDispenser::get("graphesn");

  // Put into model_prm and into training_prm multiple parameters separated
  // by "&" respectively for the model and the training procedure.
  // The ModelSelection will yield a model for each combination of these
  // parameters and will test each model by cross validation.
  Parameters model_prm, training_prm;
  model_prm["reservoir-size"] = " 100 ";
  model_prm["reservoir-connectivity"] = "0.4&1";
  model_prm["input-weights-range"] = "&0.01&";
  model_prm["sigma"] = "2.0";
  model_prm["epsilon"] = "1e-02";
  training_prm["regularization"] = "ridge-regression";
  training_prm["ridge-regression-lambda"] = "1e-2&1e-3&1e-4&1e-5&1e-6";
  // TOTAL: 40 different sets of parameters

  // Set parameters to load the training set (to pass at the CrossValidation
  // object).
  Parameters trset_prm;
  trset_prm["dataset-type"] = "sdf-file";
  trset_prm["dataset-sdf-file"] = "test/data/ptc.sdf";
  trset_prm["dataset-sdf-id"] = "RecNN.name";
  trset_prm["dataset-sdf-output"] = "PTC.CLASS.FM";
  trset_prm["dataset-sdf-noutputs"] = "1";

  // Fill cv_prm with your parameters for the cross validation procedure.
  Parameters cv_prm;
  cv_prm["cv-folds"] = "10";
  cv_prm["cv-steps"] = "5";

  // Create a new CrossValidation object loading the training set (but
  // without binding the model maker).
  CrossValidation *cv = new CrossValidation();
  cv->setParameters(cv_prm);
  cv->loadTrainingDataset(trset_prm);

  // Create a ModelSelection object binding the model maker, binding the
  // cross validation (used to measure the model performance) and setting
  // the multiple parameters for the model maker.
  ModelSelection *ms = new ModelSelection();
  ms->bindModelMaker(mm);
  ms->bindCrossValidation(cv);
  ms->setModelMakerParameters(model_prm, training_prm);

  // Start the model selection.
  ms->start();

  // Print out the parameters that give the best results.
  std::cout << "Model selection result: \n";
  std::cout << ms->getBestModelParameters().info().list("  > ") << "\n";
  std::cout << ms->getBestTrainingParameters().info().list("  > ") << "\n";
  std::cout << std::endl;
  std::cout << ms->getBestResults().getList("  > ") << "\n";
  std::cout << std::endl;

  // Remember to delete also the model maker and the cross validation
  // objects before exit.
  delete ms;
  delete cv;
  delete mm;

  return 0;
} // function main


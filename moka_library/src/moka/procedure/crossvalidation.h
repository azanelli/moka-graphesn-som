#ifndef MOKA_PROCEDURE_CROSSVALIDATION_H
#define MOKA_PROCEDURE_CROSSVALIDATION_H

#include <list>
#include <string>
#include <utility>
#include <boost/signals2.hpp>
#include <moka/dataset/dataset.h>
#include <moka/global.h>
#include <moka/model/model.h>
#include <moka/util/parameters.h>
#include <moka/util/info.h>

namespace moka {
namespace procedure {

/**
 * Class CrossValidation
 *
 * Given a Model object and given a training set for such Model, a
 * CrossValidation object allow you to get performance measures (e.g.
 * training or test accuracy) of the model on the training set through a cross
 * validation procedure.
 *
 * For example with few lines of code you can test a model (in this example
 * a GraphEsnSom) on a loaded training set:
 *
 *   // Get a new Model object for a GraphEsnSom.
 *   Model *model = ModelDispenser::get("gmm");
 *
 *   // Put into model_prm your parameters for the model.
 *   Parameters model_prm;
 *   model_prm["reservoir-size"] = "100";
 *   // ...
 *   model_prm["regularization"] = "ridge-regression";
 *   // ...
 *
 *   // Sets parameters into the Model object.
 *   model->setParameters(model_prm);
 *
 *   // Get the right Dataset for the GraphEsnSom model.
 *   Dataset *dataset = DatasetDispenser::get("multi-labeled-graph");
 *
 *   // Put into trset_prm a set of parameters to load the training set.
 *   Parameters trset_prm;
 *   trset_prm["load-from"] = "sdf-file";
 *   trset_prm["file-path"] = "./training.sdf";
 *   // ...
 *
 *   // Load the training set.
 *   dataset->load(trset_prm);
 *
 *   // Create a new CrossValidation object and set its parameters.
 *   CrossValidation *cv = new CrossValidation();
 *
 *   // Fill cv_prm with your parameters for the cross validation procedure and
 *   // set it into the CrossValidation object.
 *   Parameters cv_prm;
 *   cv_prm["cv-folds"] = "10"; // splits dataset on 10 folds
 *   // ...
 *   cv->setParameters(cv_prm);
 *
 *   // Bind Model and Dataset to the CrossValidation object.
 *   cv->bindModel(model);
 *   cv->bindTrainingSet(dataset);
 *
 *   // Start the cross validation procedure
 *   cv-start();
 *
 *   // Print out the cross validation results obtained by the model.
 *   std::cout << "Cross validation results: " << std::endl;
 *   std::cout << cv->getResults().list("  > ") << std::endl;
 *
 *   // Remember to delete all the objects before to exit.
 *   delete cv;
 *   delete dataset;
 *   delete model;
 *
 * The training dataset can be setted by method "bindTrainingSet". The binded
 * dataset must be of a type compatible with the binded model. To set the model
 * you can use the method "bindModelMaker". To set parameters for the cross
 * validation procedure use the method "setParameters".
 * Once you have setted dataset, model and parameters, you can start the cross
 * validation procedure by the method "start". This method uses the init/train/
 * test methods of the model object to train one or more models on the training
 * set and after each train test it on the test/validation set.
 *
 * A set of signals (boost::signals2) will be emitted at every step of the
 * process: you can connect these signals with your own slots in order to show
 * process informations.
 *
 * At the end of the method "start" you can use "getResults" to get final
 * results of the cross validation procedure.
 */
class CrossValidation
{
  public:
    typedef Global::Uint Uint;
    typedef Global::Real Real;

    enum TrainFailBehaviour
    {
      stop_training,
      repeat_training
    };

    CrossValidation();
    ~CrossValidation();

    void bindModel(model::Model *model);
    void bindTestSet(dataset::Dataset *test_set);
    void bindTrainingSet(dataset::Dataset *training_set);
    const model::Model& getModel() const;
    Real getAvgVaPerformance() const;
    Real getAvgTsPerformance() const;
    Uint getCvFolds() const;
    Uint getCvSteps() const;
    Uint getCvTimes() const;
    const dataset::Dataset& getTestSet() const;
    const dataset::Dataset& getTrainingSet() const;
    util::Info<std::string> getResults() const;
    void removeTestSet();
    void setParameters(util::Parameters *params);
    void setParameters(const util::Parameters& params);
    void start();
    bool thereIsTestSet() const;
    bool thereIsTrainingSet() const;

    // signals
    boost::signals2::signal<void (const CrossValidation& cv)>
    sigCrossValidationStart;

    boost::signals2::signal<void (const CrossValidation& cv, Uint time)>
    sigModelInitialized;

    boost::signals2::signal<void (const CrossValidation& cv, Uint step)>
    sigStepStart;

    boost::signals2::signal
    < void
      (
        const CrossValidation& cv,
        const util::Info<std::string>& res,
        Uint step
      )
    > sigStepEnd;

    boost::signals2::signal
    < void
      (
        const CrossValidation& cv,
        const std::string& error
      )
    > sigTrainingFail;

    boost::signals2::signal
    < void
      (
        const CrossValidation& cv,
        const std::string& filename,
        bool success
      )
    > sigSaveOutputsEnd;

    boost::signals2::signal
    < void
      (
        const CrossValidation& cv,
        const util::Info<std::string>& res,
        Uint time
      )
    > sigTimeEnd;

    boost::signals2::signal<void (const CrossValidation& cv)>
    sigCrossValidationEnd;

  private:
    model::Model *m_model;
    dataset::Dataset *m_training_set;
    dataset::Dataset *m_test_set;
    util::Parameters *m_params;

    // Cross validation parameters
    Uint m_cv_folds, m_cv_steps, m_cv_times;
    bool m_cv_times_dataset_shuffle;
    bool m_stratified_split;
    Uint m_dataset_rseed;
    TrainFailBehaviour m_train_fail_behaviour;

    // Other parameters
    std::string m_save_output_file;

    // Cross validation results
    Real m_avg_time_va_perf, m_avg_time_ts_perf;
    util::Info<Real> m_step_tr_res, m_step_va_res, m_step_ts_res;
    util::Info<std::string> m_step_results;
    util::Info<std::list<Real> > m_steps_res_accm;
    util::Info<std::string> m_time_results;
    util::Info< std::pair< std::list<Real>, std::list<Real> > >
    m_times_res_accm;
    Real m_avg_va_perf, m_avg_ts_perf;
    util::Info<std::string> m_final_results;

    // Private methods
    void accumulateStepsResults();
    void accumulateTimesResults();
    bool checkParameters(std::string& error_description);
    void clearObject();
    void clearResults();
    void clearTimeResults();
    void computeFinalResults();
    void parseParameters();
    void saveOutputs(const std::string& filename, Uint time, Uint step);

    // Copy constructor and assignment operator are turned off.
    CrossValidation(const CrossValidation&);
    CrossValidation& operator=(const CrossValidation&);

}; // class CrossValidation

} // namespace procedure
} // namespace moka

#endif // MOKA_PROCEDURE_CROSSVALIDATION_H

#ifndef MOKA_MODEL_MODEL_H
#define MOKA_MODEL_MODEL_H

#include <fstream>
#include <istream>
#include <ostream>
#include <string>
#include <moka/dataset/dataset.h>
#include <moka/exception.h>
#include <moka/global.h>
#include <moka/log.h>
#include <moka/util/info.h>
#include <moka/util/parameters.h>

namespace moka {
namespace model {

/**
 * Class Model
 *
 * Represents a generic interface of a machine learning model.
 *
 * This class allow you to build a machine learning model, training it on a
 * given training set and test it on a validation fold of the training
 * set or test it on an external test set.
 *
 * Changes in the instances of the binded dataset (e.g. reload the instances in
 * the dataset) are not legals and cause the invalidation of this object, i.e.
 * use its methods can cause errors or exceptions. In this case you must create
 * a new object of this type after the changes in the dataset or re-invoke the
 * method "init" to rebuild this object on the new dataset.
 *
 * Changes in the partitioning of the dataset (training / test folds) instead
 * are absolutely legal and, for example, you can change the dataset
 * partitioning between a call of the method train and another one in order to
 * make the training procedure on different parts of the dataset and then use
 * the method test to test the trained model on the test fold.
 */
class Model
{
  public:
    typedef ::moka::Global::Uint Uint;
    typedef ::moka::Global::Real Real;
    typedef ::moka::util::Info<Real> NumericResults;

    /**
     * Constructor
     *
     */
    Model() :
      m_parameters(NULL)
    {
      clearObject();
    }

    /**
     * Copy constructor
     *
     */
    Model(const Model& cpy_model) :
      m_parameters(NULL)
    {
      copyObject(cpy_model);
    }

    /**
     * Destructor
     *
     */
    virtual ~Model()
    {
      delete m_parameters;
      return;
    }

    /**
     * Method bindTrainingSet
     *
     * Sets the training set on which the model will be builded and trained.
     * The dataset must be of a specific type (derived from Dataset) that
     * depends by the specific type of Model; if the type is not correct an
     * exception of type moka::GenericException will be throw.
     *
     * IMPORTANT: the training set can be needed to initialize the model, thus
     * you must set a training set before to call the method "init". For this
     * reason by binding a new training set this object is invalidated (see
     * isInitialized) and not trainable.
     */
    virtual void bindTrainingSet(const dataset::Dataset *trainingset) = 0;

    /**
     * Method comparePerformance
     *
     * Given two performance measures (see methods "test" and "validation") p1
     * and p2, this method returns "true" if the p1 is better than p2, return
     * "false" otherwise (note that if are equal returns "false").
     */
    virtual bool comparePerformance(const Real& p1, const Real& p2) const = 0;

    /**
     * Method computeOn
     *
     * Use the model created and trained (method train) to compute an output
     * for each instance in the passed dataset and prints on the output stream
     * <os> the outputs in the following form (csv-like):
     *
     *   instance-id, out_1, out_2, ..., out_m
     *
     * where "m" is the model output size.
     *
     * The dataset must be of a specific type (derived from Dataset) that
     * depends on the specific type of Model; if the type is not correct an
     * exception of type moka::GenericException will be throw.
     *
     * The datased must be also compatible with the model (e.g. the input and
     * the output sizes must be as those of the model) otherwise an exception
     * of type moka::GenericException is thrown.
     *
     * If the model is not trained (see isTrained) an exception will be thrown.
     */
    virtual void computeOn(
        const dataset::Dataset& ds, std::ostream& os, bool compare = false) = 0;

    /**
     * Method computeOnTestFold
     *
     * As computeOnTrainingSet but on the current test fold (if there is any).
     */
    virtual void computeOnTestFold(
        std::ostream& os, bool compare = false) = 0;

    /**
     * Method computeOnTrainingSet
     *
     * Use the model created and trained (using the method train) to compute an
     * output for each instance in the current training folds of the training
     * set and print on the output stream OS the outputs formatted as follows
     * (csv-like):
     *
     *   instance-id, out_1, out_2, ..., out_m
     *
     * where "m" is the model output size.
     *
     * If the model is not trained (see isTrained) an exception will be thrown.
     */
    virtual void computeOnTrainingSet(
        std::ostream& os, bool compare = false) = 0;

    ///**
    // * Method getTrainingInfoResults
    // *
    // * Returns a reference to a InfoResults container, i.e set of training
    // * results in the form:
    // *   (key, name, value)
    // * where "name" is the description of the information content, "value" is
    // * the value of the information and both are std::string.
    // * This set of results is filled at the end of the method "train" with the
    // * last training results and is emptied when the model is re-builded or
    // * resetted.
    // * The content of these results is of descriptive or categorical type
    // * (different from that returned by the method "getTrainingResultsNumeric").
    // */
    //virtual const InfoResults& getTrainingInfoResults() const
    //{
    //  return m_infores;
    //} // getTrainingInfoResults

    /**
     * Method getTestPerformance
     *
     * Returns a measure of the performance on the test set obtained by the
     * last call of the method "test" or of the method "testOn".
     * Usually the performance are measured by the accuracy in case of
     * classification or by the MSE in case of regression, thus the means of
     * the returned value can change in each implementation of Model and can
     * also change by passing some parameter.
     * You can compare two performance usgin the method "comparePerformance"
     * that tells you if a performance is better than another one.
     *
     * If there isn't a test set this method return a "null" measure (probably
     * 0).
     */
    virtual Real getTestPerformance() const
    {
      return m_test_perf;
    }

    /**
     * Method info
     *
     * Returns an Info element (possibly built at the moment) that describes
     * this object (e.g. all the setted parameters).
     */
    virtual util::Info<std::string> info() const = 0;

    /**
     * Method init
     *
     * Builds a new, untrained, initial model by using the setted parameters
     * (see setParameters) and (possibly) looking at the current instances in
     * the setted training set (see method "bindTrainingSet"). After this
     * method you can call the method "train" if you want to train the model.
     *
     * Sometimes, some initial values are taken by a random generator, thus at
     * each call of this method a different model can be built also if you
     * provide the same parameters.
     *
     * You can split the training set before to call this method; after that
     * you can change the test fold on the dataset without recall this method
     * each time; if you change the partitioning or the instances into the
     * dataset you must after re-invoke this method in order to rebuild an
     * appropriate model on the new dataset.
     *
     * If there are errors in the building, e.g. is missing a parameter or a
     * parameter has a wrong value or the training set is empty, an exception
     * of type moka::GenericException will be throw.
     */
    virtual void init() = 0;

    /**
     * Method isInitialized
     *
     * Returns true if this object is correctly initialized (the method init
     * has been called) and then is trainable (you can call the method train
     * to train this object).
     * Note that some changes to the object (e.g. setting new parameters or
     * binding a new training set) can invalidate the object itself (this
     * method returns false) and must be recalled the method init.
     */
    virtual bool isInitialized() const
    {
      return m_is_initialized;
    }

    /**
     * Method isTrained
     *
     * Returns true if this object has been trained (the method train has been
     * successfully executed).
     * Note that some changes to the object (e.g. setting new parameters or
     * binding a new training set) can invalidate the object itself (this
     * method returns false) and must be recalled the method init.
     */
    virtual bool isTrained() const
    {
      return m_is_trained;
    }

    /**
     * Method loadFromFile
     *
     * Loads a Model from file using the method read.
     *
     * In case of errors an exception of type moka::GenericException will be
     * thrown.
     *
     * Note: if the error is during the file opening the object is not modified,
     * otherwise the method read is called and the object will be cleared.
     */
    void loadFromFile(const std::string& filename)
    {
      std::fstream fs(filename.c_str(), std::fstream::in);
      if (fs.fail())
        throw moka::GenericException(
            "Model::loadFromFile: fail opening the file " + filename);
      read(fs);
      fs.close();
      return;
    } // method loadFromFile

    /**
     * Methos operator=
     *
     * Assigment operator.
     */
    virtual Model& operator=(const Model& cpy_model)
    {
      copyObject(cpy_model);
      return (*this);
    } // method operator=

    /**
     * Method read
     *
     * Reads a model from input stream, previously written using the method
     * write.
     *
     * In case of error an exception of type moka::GenericException will be
     * thrown and the object will be left empty as just created.
     *
     * NOTE: in any case (errors or not) by calling this method all data in the
     * current object will be lost.
     */
    virtual void read(std::istream& is) = 0;

    /**
     * Method reset
     *
     * Reset the model as just after the initialization (before the training
     * process), i.e. in the state that was at the end of the last call of the
     * method init. If appears that the object must be initialized (the method
     * isInitialized returns false) an exception of type moka::GenericException
     * will be thrown.
     *
     * If the model is loaded using the method read it could be that is not
     * possible to reset the model (although it is trained). In this case an
     * exception of type moka::GenericException will be thrown.
     */
    virtual void reset() = 0;

    /**
     * Method saveOnFile
     *
     * Saves the Model on file using the method write.
     *
     * In case of error returns false, a message will be logged on Log::err and
     * the model is not saved. Otherwise returns true.
     */
    bool saveOnFile(const std::string& filename) const
    {
      std::fstream fs(filename.c_str(), std::fstream::out);
      if (fs.fail())
      {
        Log::err << "Model::saveOnFile: fail openining the file " << filename
                 << "." << Log::endl;
        return false;
      }
      write(fs);
      fs.close();
      return true;
    } // method saveOnFile

    /**
     * Method setParameters
     *
     * Set the parameters that will be used to build a model (see method init).
     * If you set new parameters this object is invalidated and must be
     * recalled the method init to make it usable.
     *
     * NOTE: this method only set the Parameters object, without checking its
     * contents. If there are errors in the passed parameters these will be
     * found during the execution of the method init.
     *
     * IMPORTANT: the passed Parameters objects are not copied but become
     * property of this object, then the object pointed by <parameters> and
     * will be deleted by this object (in its destructor or when no longer
     * useful) and changes to parameters after they are passed to this method
     * are not legal and cause errors.
     */
    virtual void setParameters(moka::util::Parameters *parameters)
    {
      delete m_parameters;
      m_parameters = parameters;

      setInitialized(false);

      return;
    } // method setParameters

    /**
     * Method setParameters
     *
     * See setParameters(util::Parameters *).
     *
     * NOTE: the difference between this and the previous method is that in
     * this case the passed Parameters objects are copied into this object,
     * then it can be memory and time consuming if the passed object are huge.
     * In that case consider passing by pointers.
     */
    virtual void setParameters(const moka::util::Parameters& parameters)
    {
      delete m_parameters;
      m_parameters = new util::Parameters(parameters);

      setInitialized(false);

      return;
    } // method setParameters

    /**
     * Method test
     *
     * Tests the trained model on the test fold setted in the training set. You
     * can get the test results through the method getTestPerformance.
     *
     * If no training set is present or if the model is not trained (see
     * isTrained) an exception of type moka::GenericException will be thrown.
     *
     * This method returns a reference to a NumericResults container, i.e set
     * of training results into an util::Info object with values of type Real.
     * The content of these results is of numeric type and can be used to
     * compute statistical results (like average and/or standard deviation).
     */
    virtual const NumericResults& test() = 0;

    /**
     * Method testOn
     *
     * Test the trained model on the passed test set. You can get the test
     * results through the method getTestPerformance.
     *
     * The dataset must be of a specific type (derived from Dataset) that
     * depends on the specific type of Model; if the type is not correct an
     * exception of type moka::GenericException will be throw.
     *
     * The datased must be also compatible with the model (e.g. the input and
     * the output sizes must be as those of the model) otherwise an exception
     * of type moka::GenericException is thrown.
     *
     * If the model is not trained (see isTrained) an exception will be thrown.
     *
     * This method returns a reference to a NumericResults container, i.e set
     * of training results into an util::Info object with values of type Real.
     * The content of these results is of numeric type and can be used to
     * compute statistical results (like a mean and standard deviation).
     */
    virtual const NumericResults& testOn(const dataset::Dataset& testset) = 0;

    /**
     * Method train
     *
     * Starts the training process using the current training set. To create a
     * new initial model to train see the method init.
     *
     * If the training set is splitted into training and test folds the
     * training is done using only the instances on the training split.
     *
     * In case of errors (e.g. model not initialized but also errors during the
     * training procedure) an exception of type moka::GenericException will be
     * thrown.
     *
     * This method returns a reference to a NumericResults container, i.e set
     * of training results into an util::Info object with values of type Real.
     * The content of these results is of numeric type and can be used to
     * compute statistical results (like average and/or standard deviation) on
     * more training results.
     */
    virtual const NumericResults& train() = 0;

    /**
     * Method write
     *
     * Writes this object on the output stream (probably a file stream). You
     * can then reload it using the method read.
     *
     * It is possible to write the model only if it is trained (see isTrained),
     * otherwise an exception will be thrown.
     *
     * Note that not necessarily is written to the entire object (e.g.
     * parameters setted using setParameters may not be written), but only the
     * parts useful for the output computation.
     * Once you have read a model written trough this method you can re-use it
     * to test and compute tasks, but not to reset/init/train procedures (you
     * can do it by setting new parameters).
     */
    virtual void write(std::ostream& os) const = 0;

  protected:

    /**
     * Method clearObject
     *
     * Clears this object setting all the members to their initial value.
     */
    void clearObject()
    {
      delete m_parameters;
      m_parameters = new util::Parameters();

      clearObjectForInit();

      return;
    } // method clearObject

    /**
     * Method clearObjectForInit
     *
     * Clears members for a new initialization (doesn't clear parameters).
     */
    void clearObjectForInit()
    {
      m_test_perf = 0.0;
      m_is_initialized = false;
      m_is_trained = false;
      return;
    }

    /**
     * Method copyObject
     *
     * Copy all the members values of the passed object in this object.
     */
    void copyObject(const Model& cpy_model)
    {
      // Always make a copy also if parameters are passed as pointer
      delete m_parameters;
      m_parameters = new moka::util::Parameters(*cpy_model.m_parameters);

      // Other members
      m_test_perf = cpy_model.m_test_perf;
      m_is_initialized = cpy_model.m_is_initialized;
      m_is_trained = cpy_model.m_is_trained;

      return;
    } // method copyObject

    /**
     * Method getParameters
     *
     * Returns a reference to the parameters setted by the method setParameters.
     */
    inline
    util::Parameters& getParameters()
    {
      return (*m_parameters);
    } // method getParameters

    /**
     * Method setInitialized
     *
     * Set if the model is initialized (then trainable) or not.
     * If it is setted to false is automatically called the method
     * setTrained(false) to set to false also the isTrained property.
     */
    inline
    void setInitialized(bool is_initialized = true)
    {
      m_is_initialized = is_initialized;
      if (!m_is_initialized)
        setTrained(false);
      return;
    } // method setInitialized

    /**
     * Method setTrained
     *
     * Set if the model is trained (then usable) or not.
     */
    inline
    void setTrained(bool is_trained = true)
    {
      m_is_trained = is_trained;
    } // method setTrained

    /**
     * Method setTestPerf
     *
     * Set value of test performance returned by the public method
     * getTestPerformance.
     */
    inline
    void setTestPerf(const Real& test_perf)
    {
      m_test_perf = test_perf;
    } // method setTestPerf

  private:
    util::Parameters *m_parameters;
    Real m_test_perf;
    bool m_is_initialized;
    bool m_is_trained;

}; // class Model

} // namespace model
} // namespace moka

#endif // MOKA_MODEL_MODEL_H

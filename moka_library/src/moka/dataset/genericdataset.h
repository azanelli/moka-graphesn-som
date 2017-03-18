#ifndef MOKA_DATASET_GENERICDATASET_H
#define MOKA_DATASET_GENERICDATASET_H

#include <ctime>
#include <string>
#include <vector>
#include <moka/global.h>
#include <moka/dataset/dataset.h>

#ifdef MOKA_TMP_CODE
  #include <fstream> // TODO: remove temporary include
#endif // MOKA_TMP_CODE

namespace moka {
namespace dataset {

/**
 * Class GenericDataset
 *
 * Represents a dataset of intances of the form <id,input,output>. The id is an
 * unique identifier of the instance, input and output can be single elements,
 * vector of elements, or also datasetd data as graphs.
 * It is possible to access at the dataset instances (to their id, inputs and
 * outputs). Through method randomShuffleDataset you can create a random
 * permutation of dataset instances; through method restore can be restored the
 * original instance order.
 * Through the method splitInFolds or splitTsFoldPercent it is possible to
 * divide the dataset into folds and setting one of those fold as test fold.
 * Through the methods trAt and tsAt you can access the intances within
 * the training set or whithin the test fold.
 */
template <typename InputTemplate>
class GenericDataset : public Dataset
{
  public:
    typedef Global::Real Real;
    typedef InputTemplate InputType;

    class Instance;
    class ConstRandomShuffle;

    GenericDataset();
    virtual ~GenericDataset();

    virtual const Instance& at(Uint i) const;
    virtual void clear();
    virtual Dataset* cloneTrSet() = 0;
    virtual Uint getNumberOfFolds() const;
    virtual const std::string& getId(Uint i) const;
    virtual const InputTemplate& getInput(Uint i) const;
    virtual Uint getTestFold() const;
    virtual const std::vector<Real>& getOutput(Uint i) const;
    virtual Uint getSize() const;
    virtual Uint getTrSetSize() const;
    virtual Uint getTestFoldSize() const;
    virtual util::Info<std::string> info() const;
    virtual bool isEmpty() const;
    virtual void load(const util::Parameters& params) = 0;
    virtual void merge();
    virtual const Instance& operator[](Uint i) const;
    virtual void randomShuffleTrainingSet();
    virtual void randomShuffleDataset();
    virtual void read(std::istream& is) = 0;
    virtual void restore();
    virtual void splitInFolds(Uint n);
    virtual void splitInStratifiedFolds(Uint n);
    virtual void splitTestFoldPercent(Uint p);
    virtual void setTestFold(Uint k);
    virtual void setRandomNumberGeneratorSeed(Uint s = std::time(NULL));
    virtual const Instance& trAt(Uint i) const;
    virtual const Instance& tsAt(Uint i) const;

#ifndef MOKA_TMP_CODE

    virtual void write(std::ostream& os) const = 0;

#else
    // TEMPORARY CODE HERE (REMOVE ASAP) _____vvvvvvvvvvvvvvvvvvvvvvvvvvvv____
    void write(std::string& filename) const
    {
      std::string tr_file = filename + ".training";
      std::string ts_file = filename + ".test";

      std::fstream tr_fs(tr_file.c_str(), std::fstream::out);
      std::fstream ts_fs(ts_file.c_str(), std::fstream::out);

      for (Uint j = 0; j < getTrSetSize(); ++j)
        tr_fs << trAt(j).getId() << std::endl;

      for (Uint j = 0; j < getTestFoldSize(); ++j)
        ts_fs << tsAt(j).getId() << std::endl;

      return;
    } // method write

    // TEMPORARY CODE UP TO HERE --------^^^^^^^^^^^^^^^^^^^^^^^^^^^^^-------
#endif // MOKA_TMP_CODE

  protected:
    void clearDataset();
    void endLoadInstances(bool sortbyid = false);
    void fillWithTrInstances(GenericDataset& ds);
    void pushBackInstance(Instance* inst);

  private:
    std::vector<Instance*> m_instcontainer;

    // Partitioning members
    enum SplitMode {none_split, folds_split, percent_split} m_splitmode;
    std::vector<std::vector<Uint> > m_folds_vect;
    Uint m_folds_n, m_folds_test;
    Uint m_percent_test, m_percent_test_size;
    std::vector<Uint> m_av; // access vector
    std::vector<Uint> m_trav; // training set access vector
    std::vector<Uint> m_tsav; // test fold access vector

    // Random number generator
    Global::UniformRandomGenerator m_rand;

    void buildAccessVectors();
    void buildPartitioningAccessVectors();
    void buildTestFoldAccessVector();
    void buildTrainingSetAccessVector();
    void deleteDatasetElements();
    Uint endIndexFold(Uint k) const;
    Uint foldDimension(Uint k) const;
    void initMembers();
    void initPartitioningMembers();
    Uint startFoldIndex(Uint k, Uint nfolds, Uint nelem) const;

    // Copy constructor and the assignment operator are turned off.
    GenericDataset(const GenericDataset&);
    GenericDataset& operator=(const GenericDataset&);

}; // class GenericDataset

/**
 * Class GenericDataset::Instance
 *
 * Represents a dataset instance of the form <id,input,output> where the "id"
 * is a string that identifies the instance, "ouptut" is a vector of Real
 * number that represent the outputs of a model and "input" is of a custom
 * type, e.g a vector of Real as the outputs, but also a structured data like
 * a graph.
 */
template <typename InputTemplate>
class GenericDataset<InputTemplate>::Instance
{
  public:
    //! Constructor
    Instance() :
      m_id(new std::string()),
      m_input(new InputTemplate()),
      m_output(new std::vector<Real>())
    { }

    //! Distructor
    ~Instance()
    {
      delete m_id;
      delete m_input;
      delete m_output;
    } // destructor ~Instance

    //! Copy constructor
    Instance(const Instance& instance) :
      m_id(new std::string()),
      m_input(new InputTemplate()),
      m_output(new std::vector<Real>())
    {
      this->operator=(instance);
    } // copy constructor

    //! Get the instance ID
    std::string& getId()
    {
      return *m_id;
    }

    //! Get the instance ID
    const std::string& getId() const
    {
      return *m_id;
    }

    //! Get the instance input
    InputTemplate& getInput()
    {
      return *m_input;
    }

    //! Get the instance input
    const InputTemplate& getInput() const
    {
      return *m_input;
    }

    //! Get the instance output
    std::vector<Real>& getOutput()
    {
      return *m_output;
    }

    //! Get the instance output
    const std::vector<Real>& getOutput() const
    {
      return *m_output;
    }

    //! Assignment operator
    Instance& operator=(const Instance& instance)
    {
      *(this->m_id) = *(instance.m_id);
      *(this->m_input) = *(instance.m_input);
      *(this->m_output) = *(instance.m_output);
      return *this;
    }

    //! Set the instance ID
    void setId(std::string *id)
    {
      delete m_id;
      m_id = id;
    } // method setId

    //! Set the instance input
    void setInput(InputTemplate *input)
    {
      delete m_input;
      m_input = input;
    } // method setInputGraph

    //! Set the instance output
    void setOutput(std::vector<Real> *output)
    {
      delete m_output;
      m_output = output;
    } // method setOutputs

    //! Instance compare operator
    bool operator<(const Instance& instance) const
    {
      return this->getId() < instance.getId();
    } // operator <

  private:
    std::string       *m_id;
    InputTemplate     *m_input;
    std::vector<Real> *m_output;

}; // class GenericDataset::Instance

/**
 * Class GenericDataset::ConstRandomShuffle
 *
 * Use this class in order to access at the instances of a given GenericDataset,
 * even by random shuffle its training set, all leaving the GenericDataset
 * object constant. So you can do, for example, as follows:
 *
 *   void foo(const GenericDataset& c_ds)
 *   {
 *     GenericDataset::ConstRandomShuffle ds(&c_ds);
 *     std::cout << ds[0]; // == c_ds[0]
 *     ds.randomShuffleTrainingSet();
 *     std::cout << ds.trAt(0);
 *     std::cout << ds.tsAt(0);
 *     // ...
 *   }
 *
 * Therefore the methods of this class are particularly useful when you deal
 * with a "const GenericDataset&" (or something like that) and you want to
 * shuffle the training set. If you don't have particular needs to use this
 * class probably you must directly use the analogs methods of the class
 * GenericDataset.
 */
template <typename InputTemplate>
class GenericDataset<InputTemplate>::ConstRandomShuffle
{
  public:
    ConstRandomShuffle(const GenericDataset<InputTemplate>* dataset);
    ~ConstRandomShuffle() { }

    const Instance& at(Uint i) const;
    Uint getSize() const;
    Uint getTrSetSize() const;
    Uint getTestFoldSize() const;
    const Instance& operator[](Uint i) const;
    void randomShuffleTrainingSet();
    void rebuild();
    void restore();
    void setRandomNumberGeneratorSeed(Uint s);
    const Instance& trAt(Uint i) const;
    const Instance& tsAt(Uint i) const;

  private:
    const GenericDataset<InputTemplate> *m_dataset;
    std::vector<Uint> m_trav;
    Global::UniformRandomGenerator m_rand;

}; // class GenericDataset::ConstRandomShuffle

} // namespace dataset
} // namespace moka

#include "genericdataset_impl.h"

#endif // MOKA_DATASET_GENERICDATASET_H

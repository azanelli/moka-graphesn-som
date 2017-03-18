#ifndef MOKA_DATASET_GENERICDATASET_IMPL_H
#define MOKA_DATASET_GENERICDATASET_IMPL_H

#include "genericdataset.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/construct.hpp>
#include <moka/exception.h>
#include <moka/log.h>

namespace moka {
namespace dataset {

namespace bll  = boost::lambda;

typedef Dataset::Uint Uint;
typedef Global::Real Real;

/**
 * Default constructor
 *
 * Builds a void dataset.
 */
template <typename T>
GenericDataset<T>::GenericDataset()
{
  initMembers();
  return;
} // default constructor

/**
 * Destructor
 *
 * Deletes the dataset.
 */
template <typename T>
GenericDataset<T>::~GenericDataset()
{
  deleteDatasetElements();
  return;
} // destructor

// ==============
// PUBLIC METHODS
// ==============

/**
 * Method at
 *
 * Returns a constant reference to the i-th dataset element (instance).
 */
template <typename T>
const typename GenericDataset<T>::Instance& GenericDataset<T>::at(Uint i) const
{
  if (i >= m_av.size())
    throw std::out_of_range("GenericDataset::at: argument is out of range");
  return *(m_instcontainer[m_av[i]]);
} // method at

/**
 * Method clear
 *
 * See Dataset::clear.
 */
template <typename T>
void GenericDataset<T>::clear()
{
  clearDataset();
  return;
} // method clear

/**
 * Method getNumberOfFolds
 *
 * See Dataset::getNumberOfFolds.
 */
template <typename T>
Uint GenericDataset<T>::getNumberOfFolds() const
{
  return m_folds_n;
} // method getNumberOfFolds

/**
 * Method getTestFold
 *
 * See Dataset::getTestFold.
 */
template <typename T>
Uint GenericDataset<T>::getTestFold() const
{
  return m_folds_test;
} // method getTestFold

/**
 * Method getSize
 *
 * See Dataset::getSize.
 */
template <typename T>
Uint GenericDataset<T>::getSize() const
{
  return m_instcontainer.size();
} // method getSize

/**
 * Method getTrSetSize
 *
 * See Dataset::getTrSetSize.
 */
template <typename T>
Uint GenericDataset<T>::getTrSetSize() const
{
  return m_trav.size();
} // method getTrSetSize

/**
 * Method getTestFoldSize
 *
 * See Dataset::getTestFoldSize.
 */
template <typename T>
Uint GenericDataset<T>::getTestFoldSize() const
{
  return m_tsav.size();
} // method getTestFoldSize

/**
 * Method getId
 *
 * Returns a const reference to the i-th instance id.
 */
template <typename T>
const std::string& GenericDataset<T>::getId(Uint i) const
{
  if (i >= m_instcontainer.size())
    throw std::out_of_range("GenericDataset::getId: argument is out of range");
  return at(i).getId();
} // method getId

/**
 * Method getInput
 *
 * Returns a (constant) reference to the i-th instance input.
 */
template <typename T>
const T& GenericDataset<T>::getInput(Uint i) const
{
  if (i >= m_instcontainer.size())
    throw std::out_of_range("GenericDataset::getInputGraph: argument is out "
        "of range");
  return at(i).getInput();
} // method getInputGraph

/**
 * Method getOutput
 *
 * Returns a (constant) reference to the i-th instance output.
 */
template <typename T>
const std::vector<Real>& GenericDataset<T>::getOutput(Uint i) const
{
  if (i >= m_instcontainer.size())
    throw std::out_of_range("GenericDataset::getOutputs: argument is out "
        "of range");
  return at(i).getOutput();
} // method getOutputs

/**
 * Method info
 *
 * See Dataset::info.
 */
template <typename T>
util::Info<std::string> GenericDataset<T>::info() const
{
  util::Info<std::string> inf;
  inf.pushBack("size", "Instances", Global::toString(getSize()));
  return inf;
} // method info

/**
 * Method isEmpty
 *
 * See Dataset::isEmpty.
 */
template <typename T>
bool GenericDataset<T>::isEmpty() const
{
  return m_instcontainer.empty();
} // method isEmpty

/**
 * Method merge
 *
 * See Dataset::merge.
 */
template <typename T>
void GenericDataset<T>::merge()
{
  initPartitioningMembers();
  buildPartitioningAccessVectors();
  return;
} // method merge

/**
 * Method operator[]
 *
 * Returns a constant reference to the i-th dataset element (instance).
 */
template <typename T>
const typename GenericDataset<T>::Instance& GenericDataset<T>::operator[](
    Uint i) const
{
  if (i >= m_av.size())
    throw std::out_of_range("GenericDataset::operator[]: argument is out "
        "of range");
  return at(i);
} // method operator[]

/**
 * randomShuffleTrainingSet
 *
 * See Dataset::randomShuffleTrainingSet.
 */
template <typename T>
void GenericDataset<T>::randomShuffleTrainingSet()
{
  for (Uint i = m_trav.size(); i != 0; --i)
    std::swap(m_trav[i - 1], m_trav[m_rand.getRandInt(0, i - 1)]);
  return;
} // method randomShuffleTrainingSet

/**
 * randomShuffleDataset
 *
 * See Dataset::randomShuffleDataset.
 */
template <typename T>
void GenericDataset<T>::randomShuffleDataset()
{
  for (Uint i = m_av.size(); i != 0; --i)
    std::swap(m_av[i - 1], m_av[m_rand.getRandInt(0, i - 1)]);
  return;
} // method randomShuffleDataset

/**
 * Method restore
 *
 * See Dataset::restore.
 */
template <typename T>
void GenericDataset<T>::restore()
{
  // merges partitioning
  merge();
  // restores initial order
  for (uint i = 0; i < m_av.size(); ++i) m_av[i] = i;
  return;
} // method restore

/**
 * Method splitInFolds
 *
 * See Dataset::splitInFolds.
 */
template <typename T>
void GenericDataset<T>::splitInFolds(Uint n)
{
  if (n > m_instcontainer.size())
    throw moka::GenericException("GenericDataset::splitInFolds: argument out "
        "of range");
  if (n == 0 || n == 1)
  {
    merge();
    return;
  } // if
  initPartitioningMembers(); // also clear m_folds_vect
  m_splitmode = folds_split;
  m_folds_n = n;
  m_folds_test = 0;
  // builds the folds vector
  m_folds_vect.resize(m_folds_n);
  for (Uint fold = 0; fold < m_folds_n; ++fold)
  {
    Uint startf = startFoldIndex(fold, m_folds_n, m_instcontainer.size());
    Uint endf = startFoldIndex(fold + 1, m_folds_n, m_instcontainer.size());
    for (Uint i = startf; i < endf; ++i)
      m_folds_vect[fold].push_back(i);
  } // for fold
  // builds the access vectors
  buildPartitioningAccessVectors();
  return;
} // method splitInFolds

/**
 * Method splitInStratifiedFolds
 *
 * See Dataset::splitInStratifiedFolds.
 */
template <typename T>
void GenericDataset<T>::splitInStratifiedFolds(Uint n)
{
  if (n > m_instcontainer.size())
    throw moka::GenericException("GenericDataset::splitInStratifiedFolds: "
        "argument out of range");

  if (n == 0 || n == 1)
  {
    merge();
    return;
  } // if

  initPartitioningMembers(); // also clear m_folds_vect
  m_splitmode = folds_split;
  m_folds_n = n;
  m_folds_test = 0;

  // collects all the different values in a map
  std::map<Real, std::vector<Uint> > val_map;
  for (Uint i = 0; i < getSize(); ++i)
  {
    if (at(i).getOutput().empty())
    {
      merge();
      throw moka::GenericException("GenericDataset::splitInStratifiedFolds: "
          "error: there is one (or more) instance without outputs; the "
          "dataset is merged");
    } // if
    // looks only the first output element to do the stratification
    val_map[at(i).getOutput()[0]].push_back(i);
  } // for i

  // builds the folds vector
  m_folds_vect.resize(m_folds_n);
  std::map<Real, std::vector<Uint> >::iterator vm_it;

  for (vm_it = val_map.begin(); vm_it != val_map.end(); ++vm_it)
    for (Uint fold = 0; fold < m_folds_n; ++fold)
    {
      Uint startf = startFoldIndex(fold, m_folds_n, vm_it->second.size());
      Uint endf = startFoldIndex(fold + 1, m_folds_n, vm_it->second.size());

      if ((endf - startf) == 0) // a fold not contains instances of this value
      {
        Log::err << "GenericDataset::splitInStratifiedFolds: warning: the ";
        Log::err << "fold n. " << fold + 1 << " will not have instances ";
        Log::err << "with value " << vm_it->first << "." << Log::endl;
      } // if

      for (Uint i = startf; i < endf; ++i)
        m_folds_vect[fold].push_back(vm_it->second[i]);

    } // for fold

  // checks if all the folds are not empty
  for (Uint fold = 0; fold < m_folds_n; ++fold)
    if (m_folds_vect[fold].empty())
    {
      merge();
      throw moka::GenericException("GenericDataset::splitInStratifiedFolds: "
          "error: there is one (or more) fold empty; the dataset is merged");
    } // if

  // builds the access vectors
  buildPartitioningAccessVectors();

  return;
} // method splitInStratifiedFolds

/**
 * Method splitTestFoldPercent
 *
 * See Dataset::splitTestFoldPercent.
 */
template <typename T>
void GenericDataset<T>::splitTestFoldPercent(Uint p)
{
  if (p > 100)
    throw moka::GenericException("GenericDataset::splitVaSetPercent: argument "
        "out of range");
  if (p == 0)
  {
    merge();
    return;
  } // if
  initPartitioningMembers();
  m_splitmode = percent_split;
  m_percent_test = p;
  m_percent_test_size = Uint(ceil(double(m_instcontainer.size()*p) / 100.0));
  buildPartitioningAccessVectors();
  return;
} // method splitTestFoldPercent

/**
 * Method setTestFold
 *
 * See Dataset::setTestFold.
 */
template <typename T>
void GenericDataset<T>::setTestFold(Uint k)
{
  if (m_splitmode != folds_split)
    return;
  if (k >= m_folds_n)
    throw moka::GenericException("GenericDataset::setTestFold: argument out "
        "of range");
  if (m_folds_n == 1)
    return;
  m_folds_test = k;
  buildPartitioningAccessVectors();
  return;
} // method setTestFold

/**
 * Method setRandomNumberGeneratorSeed
 *
 * Dataset::setRandomNumberGeneratorSeed.
 */
template <typename T>
void GenericDataset<T>::setRandomNumberGeneratorSeed(Uint s)
{
  m_rand.setRandSeed(s);
  return;
} // method setRandomNumberGeneratorSeed

/**
 * Method trAt
 *
 * Returns the i-th element (instance) of the training set.
 */
template <typename T>
const typename GenericDataset<T>::Instance& GenericDataset<T>::trAt(
    uint i) const
{
  if (i >= m_trav.size())
    throw moka::GenericException(
        "GenericDataset::trAt: argument is out of range");
  return at(m_trav[i]);
} // method trAt

/**
 * Method tsAt
 *
 * Returns the i-th element (instance) of the validation set.
 */
template <typename T>
const typename GenericDataset<T>::Instance& GenericDataset<T>::tsAt(
    Uint i) const
{
  if (i >= m_tsav.size())
    throw moka::GenericException(
        "GenericDataset::tsAt: argument is out of range");
  return at(m_tsav[i]);
} // method tsAt

// =================
// PROTECTED METHODS
// =================

/**
 * Method clearDataset
 *
 * The dataset is restored to its initial state (no elements, no splitting).
 */
template <typename T>
void GenericDataset<T>::clearDataset()
{
  deleteDatasetElements();
  m_instcontainer.clear();
  m_av.clear();
  m_trav.clear();
  m_tsav.clear();
  initMembers();
  return;
} // method clearDataset

/**
 * Method endLoadInstances
 *
 * Runs procedures necessary to maintain the dataset consistency after that
 * new instances have been loaded. Must be called after that new instances
 * have been loaded into the dataset.
 * If the parametes <sortbyid> is setted to "true" the instances inside the
 * dataset are sorted by their id value.
 */
template <typename T>
void GenericDataset<T>::endLoadInstances(bool sortbyid)
{
  if (sortbyid) // the intances are reordered according to their id
    std::sort
        (
          m_instcontainer.begin(),
          m_instcontainer.end(),
          bll::bind(&Instance::operator<, *bll::_1, *bll::_2)
        );
  buildAccessVectors();
  return;
} // method endLoadInstances

/**
 * Method fillWithTrInstances
 *
 * First empties the passed dataset, then fills it with the training instances
 * of this dataset.
 * To use in the method cloneTrSet that must be implemented different for
 * each derived class.
 */
template <typename T>
void GenericDataset<T>::fillWithTrInstances(GenericDataset& ds)
{
  ds.clear();
  for (Uint i = 0; i < getTrSetSize(); ++i)
    ds.pushBackInstance(new Instance(trAt(i)));
  ds.endLoadInstances();
  return;
} // method fillWithTrInstances

/**
 * Method pushBackInstance
 *
 * Adds the passed instance at the end of the dataset.
 * IMPORTANT: this method only adds the instance, without update other data
 * datasets used by the class GenericDataset, thus remember to call the method
 * "endLoadInstances" when you finish to add instances, in order to mantain the
 * dataset consistent.
 */
template <typename T>
void GenericDataset<T>::pushBackInstance(Instance* inst)
{
  m_instcontainer.push_back(inst);
  return;
} // method pushBackInstance

// ===============
// PRIVATE METHODS
// ===============

/**
 * Method buildAccessVectors
 *
 * Builds all the access vectors: m_av, m_trav, m_vaav, respectively the
 * dataset a.v., the training set a.v. and the validation set a.v..
 * To be called when the entire dataset is changed.
 * Note that all the ordering are re-setted into the initial order.
 */
template <typename T>
inline
void GenericDataset<T>::buildAccessVectors()
{
  m_av.clear();
  m_av.resize(m_instcontainer.size());
  for (uint i = 0; i < m_av.size(); ++i) m_av[i] = i;
  buildPartitioningAccessVectors();
  return;
} // method buildAccessVectors

/**
 * Method buildPartitioningAccessVectors
 *
 * Reset and rebuilds the training set access vectors (m_trav) and the
 * validation set access vector (m_vaav) according to the dataset current
 * partitioning (looking at the specific class varibles).
 */
template <typename T>
inline
void GenericDataset<T>::buildPartitioningAccessVectors()
{
  buildTrainingSetAccessVector();
  buildTestFoldAccessVector();
  return;
} // method buildPartitioningAccessVectors

/**
 * Method buildTestFoldAccessVector
 *
 * Reset and rebuilds the test fold access vector m_tsav according to the
 * dataset current partitioning. The validation set a.v. is a vector that
 * contains all the validation set indexes (thus excluded those of the training
 * set) on this dataset.
 */
template <typename T>
inline
void GenericDataset<T>::buildTestFoldAccessVector()
{
  m_tsav.clear();
  switch (m_splitmode)
  {
  case none_split:
    break;
  case folds_split:
    m_tsav.reserve(m_folds_vect[m_folds_test].size());
    for (Uint i = 0; i < m_folds_vect[m_folds_test].size(); ++i)
      m_tsav.push_back(m_folds_vect[m_folds_test][i]);
    break;
  case percent_split:
    m_tsav.reserve(m_percent_test_size);
    for (Uint i = 0; i < m_tsav.size(); ++i)
      m_tsav.push_back(i);
    break;
  } // switch (m_splitmode)
  return;
} // method buildTestFoldAccessVector

/**
 * Method buildTrainingSetAccessVector
 *
 * Reset and rebuilds the training set access vector m_trav according to the
 * dataset current partitioning. The training set a.v. is a vector that
 * contains all the training set indexes (thus excluded those of the validation
 * set) on this dataset.
 */
template <typename T>
inline
void GenericDataset<T>::buildTrainingSetAccessVector()
{
  m_trav.clear();
  switch (m_splitmode)
  {
  case none_split:
    m_trav.reserve(m_instcontainer.size());
    for (Uint i = 0; i < m_instcontainer.size(); ++i) m_trav.push_back(i);
    break;
  case folds_split:
    m_trav.reserve(m_instcontainer.size());
    for (Uint fold = 0; fold < m_folds_vect.size(); ++fold)
      if (fold != m_folds_test)
        for (Uint i = 0; i < m_folds_vect[fold].size(); ++i)
          m_trav.push_back(m_folds_vect[fold][i]);
    break;
  case percent_split:
    m_trav.reserve(m_instcontainer.size());
    for (Uint i = m_percent_test_size; i < m_instcontainer.size(); ++i)
      m_trav.push_back(i);
    break;
  } // switch (m_splitmode)
  return;
} // method buildTrainingSetAccessVector

/**
 * Method deleteDatasetElements
 *
 * Deletes all the Instance objects into the vector this->dataset.
 * Keep attention that after a call to this method the dataset contains invalid
 * pointers.
 */
template <typename T>
inline
void GenericDataset<T>::deleteDatasetElements()
{
  std::for_each
      (
        m_instcontainer.begin(),
        m_instcontainer.end(),
        bll::delete_ptr()
      );
  return;
} // method deleteDatasetElements

/**
 * Method initMembers
 *
 * Set the private variables to their initial value, except for the vectors and
 * for the random generator that are left as are.
 */
template <typename T>
inline
void GenericDataset<T>::initMembers()
{
  initPartitioningMembers();
  return;
} // method initMembers

/**
 * Method initPartitioningMembers
 *
 * Set the partitioning variables members to their initial value.
 */
template <typename T>
inline
void GenericDataset<T>::initPartitioningMembers()
{
  m_splitmode = none_split;
  m_folds_vect.clear();
  m_folds_n = 0;
  m_folds_test = 0;
  m_percent_test = 0;
  m_percent_test_size = 0;
  return;
} // method initPartitioningMembers

/**
 * Method startFoldIndex
 *
 * Returns the index of the first element of the "k"-th fold of "nfolds" on a
 * container of "nelem" elements. If "nfolds" is greater than "nelem" will
 * have "nfolds" folds of size 1 and the rest of folds without elements, thus
 * the start index of folds without elements will be the last index of the
 * container, i.e. its size "nelem".
 * The fold index "k" must be between 0 and "nfolds". If "k" is equal to
 * "nfolds" will be returned the last index of the container, i.e. it size
 * "nelem". If "k" is greater than "nfolds" an exception will be thrown.
 */
template <typename T>
inline
Uint GenericDataset<T>::startFoldIndex(
    Uint k,
    Uint nfolds,
    Uint nelem) const
{
  if (k > nfolds)
    throw moka::GenericException("GenericDataset::startFoldIndex: the argument "
        "k is out of range");
  Uint rest = nelem % nfolds;
  if (k <= rest)
    return k * (floor(nelem / double(nfolds)) + 1);
  return (k * floor(nelem / double(nfolds))) + rest;
} // method startFoldIndex

// ============================
// CONST RANDOM SHUFFLE METHODS
// ============================

/**
 * Constructor
 *
 * This constructor takes a pointer to the dataset on which you have to deal.
 * If in the passed dataset change something must be called the method rebuild
 * in order to update the access methods. If you don't call the rebuild method
 * may will be errors or unexpected beheavior during the access.
 */
template <typename T>
GenericDataset<T>::ConstRandomShuffle::ConstRandomShuffle(
    const GenericDataset<T>* dataset) :
  m_dataset(dataset)
{
  this->rebuild();
  return;
} // constructor

/**
 * Method randomShuffleTrainingSet
 *
 * Same thing as GenericDataset::randomShuffleTrainingSet().
 */
template <typename T>
void GenericDataset<T>::ConstRandomShuffle::randomShuffleTrainingSet()
{
  for (Uint i = m_trav.size(); i != 0; --i)
    std::swap( m_trav[i-1], m_trav[m_rand.getRandInt(0, i - 1)] );
  return;
} // method randomShuffleTrainingSet

/**
 * Method restore
 *
 * Restores the order of the elements as in the given dataset.
 */
template <typename T>
void GenericDataset<T>::ConstRandomShuffle::restore()
{
  for (uint i = 0; i < m_trav.size(); ++i) m_trav[i] = i;
  return;
} // method restore

/**
 * Method getSize
 *
 * Same thing as GenericDataset::getSize()
 */
template <typename T>
Uint GenericDataset<T>::ConstRandomShuffle::getSize() const
{
  return m_dataset->getSize();
} // method getSize

/**
 * Method getTrSetSize
 *
 * Same thing as GenericDataset::getTrSetSize()
 */
template <typename T>
Uint GenericDataset<T>::ConstRandomShuffle::getTrSetSize() const
{
  return m_dataset->getTrSetSize();
} // method getTrSetSize

/**
 * Method getTestFoldSize
 *
 * Same thing as GenericDataset::getTestFoldSize()
 */
template <typename T>
Uint GenericDataset<T>::ConstRandomShuffle::getTestFoldSize() const
{
  return m_dataset->getTestFoldSize();
} // method getTestFoldSize

/**
 * Method trAt
 *
 * Same thing as GenericDataset::trAt()
 */
template <typename T>
const typename GenericDataset<T>::Instance&
GenericDataset<T>::ConstRandomShuffle::trAt(Uint i) const
{
  if (i >= m_trav.size())
    throw moka::GenericException("GenericDataset::ConstRandomShuffle::trAt: "
          "argument is out of range");
  return m_dataset->trAt(m_trav[i]);
} // method trAt

/**
 * Method tsAt
 *
 * Same thing as GenericDataset::tsAt(Uint i)
 */
template <typename T>
const typename GenericDataset<T>::Instance&
GenericDataset<T>::ConstRandomShuffle::tsAt(Uint i) const
{
  return m_dataset->tsAt(i);
} // method vaAt

/**
 * Method at
 *
 * Same thing as GenericDataset::at(Uint i)
 */
template <typename T>
const typename GenericDataset<T>::Instance&
GenericDataset<T>::ConstRandomShuffle::at(Uint i) const
{
  return m_dataset->at(i);
} // method at

/**
 * Method operator[]
 *
 * Same thing of GenericDataset::operator[](Uint i)
 */
template <typename T>
const typename GenericDataset<T>::Instance&
GenericDataset<T>::ConstRandomShuffle::operator[](Uint i) const
{
  return m_dataset->operator[](i);
} // method operator[]

/**
 * Method setRandomNumberGeneratorSeed
 *
 * Sets the seed of the internal random number generator.
 */
template <typename T>
void
GenericDataset<T>::ConstRandomShuffle::setRandomNumberGeneratorSeed(Uint s)
{
  m_rand.setRandSeed(s);
  return;
} // method setRandomNumberGeneratorSeed

/**
 * Method rebuild
 *
 * Updates the access methods for a correct access to the given dataset.
 * To call if the dataset passed is modified.
 */
template <typename T>
void GenericDataset<T>::ConstRandomShuffle::rebuild()
{
  m_trav.clear();
  m_trav.resize(m_dataset->getTrSetSize());
  for (uint i = 0; i < m_trav.size(); ++i)
    m_trav[i] = i;
  return;
} // method rebuild

} // namespace dataset
} // namespace moka

#endif // MOKA_DATASET_GENERICDATASET_IMPL_H

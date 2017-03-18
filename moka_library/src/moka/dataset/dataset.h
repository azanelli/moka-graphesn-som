#ifndef MOKA_DATASET_DATASET_H
#define MOKA_DATASET_DATASET_H

#include <istream>
#include <ostream>
#include <moka/global.h>
#include <moka/util/info.h>
#include <moka/util/parameters.h>

namespace moka {
namespace dataset {

/**
 * Class Dataset
 *
 * Basic interface for a dataset.
 */
class Dataset
{
  public:
    typedef Global::Uint Uint;

    Dataset() { }
    virtual ~Dataset() { }

    /**
     * Method clear
     *
     * The dataset is emptied, all its istances are dropped, and it returns at
     * the initial condition as just created.
     */
    virtual void clear() = 0;

    /**
     * Method cloneTrSet
     *
     * Creates and returns a new dataset taking instances from the current
     * training split.
     * The created dataset will be of the concret type of this class, and
     * putted on a pointer of the abstract class Dataset. If there are
     * errors a null pointer is returned.
     *
     * IMPORTANT: in the returned object (if not null) a new object is created
     * by the operator new that must be deleted by the caller when no longer
     * useful.
     *
     * IMPORTANT: each sub class must implement this method, also if it simply
     * call the parent implementation, since must return the same real type of
     * the object.
     */
    virtual Dataset* cloneTrSet() = 0;

    /**
     * Method getNumberOfFolds
     *
     * Returns the setted numbers of folds. If there aren't folds returns 0.
     */
    virtual Uint getNumberOfFolds() const = 0;

    /**
     * Method getTestFold
     *
     * Returns the setted test fold (see setTestFold).
     */
    virtual Uint getTestFold() const = 0;

    /**
     * Method getSize
     *
     * Returns the size of the dataset (number of instances into it).
     */
    virtual Uint getSize() const = 0;

    /**
     * Method getTrSetSize
     *
     * Returns the size (number of instances) of the training set (i.e. the
     * number of instances on the dataset minus the number of instances on the
     * test fold).
     */
    virtual Uint getTrSetSize() const = 0;

    /**
     * Method getTestFoldSize
     *
     * Returns the size (number of instances) of the setted test fold.
     */
    virtual Uint getTestFoldSize() const = 0;

    /**
     * Method getInfo
     *
     * Returns an Info element (possibly built at the moment) that describes
     * the dataset (e.g. the number of instances).
     */
    virtual util::Info<std::string> info() const = 0;

    /**
     * Method isEmpty
     *
     * Returns true if the dataset is empty (it don't contains any instance),
     * false otherwise.
     */
    virtual bool isEmpty() const = 0;

    /**
     * Method load
     *
     * Loads a dataset looking at passed parameters.
     * An exception of type moka::GenericException will be throw in case of
     * errors.
     *
     * NOTE: load the dataset from a file implies the loosing of any other
     * previous dataset stored in this object, and also splits, folds and other
     * things will be cleared in order to create a new dataset.
     */
    virtual void load(const util::Parameters& params) = 0;

    /**
     * Method merge
     *
     * Deletes the dataset partitioning.
     */
    virtual void merge() = 0;

    /**
     * randomShuffleTrainingSet
     *
     * Creates a random permutation of the training set instances.
     */
    virtual void randomShuffleTrainingSet() = 0;

    /**
     * Method randomShuffleDataset
     *
     * Makes a random permutation of the instances of the entire dataset. Note
     * that if there is any partitioning (training set and test fold) the
     * elements will be mixed between the partitions.
     */
    virtual void randomShuffleDataset() = 0;

    /**
     * Method read
     *
     * Reads a dataset from input stream, previously written using the method
     * write.
     *
     * NOTE: as for the method load, read the dataset implies the loosing of
     * any other previous dataset stored in this object, and also splits, folds
     * and other things will be cleared in order to create a new dataset.
     */
    virtual void read(std::istream& is) = 0;

    /**
     * Method restore
     *
     * Restores the dataset loaded, by deleting the partitions and by bringing
     * back the istances to their initial order.
     */
    virtual void restore() = 0;

    /**
     * Method splitInFolds
     *
     * Divides the dataset into <n> equal partitions. One of this partition will
     * be the test fold, the union of all the others partitions will form
     * the training set. If <n> is 0 or 1 is made ​​the merge (see merge method)
     * and the training set is the entire dataset (there's no test fold).
     * The test fold is by default the first (number 0), to change such fold
     * can be used the method setTestFold. The number of folds must be less
     * than the size of the dataset, otherwise an exception of type
     * moka::GenericException will be thrown. The splitting deletes any previous
     * dataset splitting (by folds or by percent). After this method you can
     * access to the training set and the test fold through the methods trAt
     * and tsAt.
     */
    virtual void splitInFolds(Uint n) = 0;

    /**
     * Method splitInStratifiedFolds
     *
     * Divides the dataset into <n> stratified partitions. One of this partition
     * will be the test fold, the union of all the others partitions will
     * form the training set. If <n> is 0 or 1 is made ​​the merge (see merge
     * method) and the training set is the entire dataset (there's no test
     * fold).
     * The test fold is by default the first (number 0), to change it you can
     * use the method setTestFold.
     * This method deletes any previous dataset splitting (by folds or by
     * percent).
     * After this method you can access to the training set and the test fold
     * through methods trAt and tsAt.
     *
     * IMPORTANT: this function is limited to certain cases (see below). Use
     * with care.
     *
     * LIMITATIONS: to make the stratification is used only the first output (no
     * all the output vector), thus it make sense use this splitting when there
     * is only one output. Moreover the stratification is made counting
     * and grouping different output values thus make attention to use this
     * function only when the output is discrete (otherwise the risk is to put
     * all the instances on the first fold leaving empty the others).
     *
     * In details this method does the following:
     *   1. For each value of the first output element is created a set
     *      containing all the instances with such value.
     *   2. Each set is then divided on <n> folds. If one set has less than
     *      <n> elements the first <n> folds will have one element while the
     *      others will be empty.
     *   3. Final folds are created combining various folds: all the
     *      instances within the first fold form the first final fold, all
     *      the second form the second final folds, and so on.
     *
     * NOTE: due to the point 2 the last folds may contains fewer instances
     * because they may not contain instances with certain output values: in
     * this case a warning will be printed out). If a fold remain empty (may
     * happen if there are not too many instances of each values respect to
     * the number of folds) an exception will be thrown.
     *
     * In case of error (when an exception is thrown) the dataset is merged and
     * left without splits.
     */
    virtual void splitInStratifiedFolds(Uint n) = 0;

    /**
     * Method splitTestFoldPercent
     *
     * Splits the dataset into two partitions: training set and test fold.
     * The dimensions of two partitions is given by the percentage of instances
     * on the test fold respect to the entire dataset, passed in the parameter
     * <p>. This value is a percentage given as an integer between 0 and 100.
     * If the percentage is 0 is made ​​the merge (see merge method) and the
     * training set is the entire dataset (there's no test fold). The instances
     * of the test fold will be the first in the dataset. The split deletes any
     * previous splitting of the dataset (folds or percent).
     * After this method you can access to the training set and the test fold
     * through methods trAt and tsAt.
     */
    virtual void splitTestFoldPercent(Uint p) = 0;

    /**
     * Method setTestFold
     *
     * Sets the test fold. The index <k> must be between 0 and n-1, where
     * n is the number of folds. If the number of folds is 1 the test fold
     * can't be done (the test fold is empty).
     * The invocation of this method also leads to remove any training random
     * shuffles, so to random order on the training set you must re-invoke the
     * method randomShuffleTrainingSet.
     * If the splitting has been made by percentage of test fold (through
     * the method splitTsFoldPercent) or has not been made any splitting, this
     * method does nothing (because there isn't folds).
     */
    virtual void setTestFold(Uint k) = 0;

    /**
     * Method setRandomNumberGeneratorSeed
     *
     * Sets the seed of the internal random number generator. By default is
     * setted as time(NULL). Random numbers are used for random shuffles the
     * dataset. By setting a seed and then shuffle the dataset the result will
     * be always the same if the seed is the same.
     */
    virtual void setRandomNumberGeneratorSeed(Uint s) = 0;

#ifndef MOKA_TMP_CODE

    /**
     * Method write
     *
     * Writes dataset entries on the output stream (probably a file stream).
     * You can then reload the dataset using the method read or the method
     * load with some suitable parameters.
     */
    virtual void write(std::ostream& os) const = 0;

#else

    virtual void write(std::string& filename) const = 0;

#endif // MOKA_TMP_CODE

}; // class Dataset

} // namespace dataset
} // namespace moka

#endif // MOKA_DATASET_DATASET_H

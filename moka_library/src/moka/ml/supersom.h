#ifndef MOKA_ML_SUPERSOM_H
#define MOKA_ML_SUPERSOM_H

#include <istream>
#include <list>
#include <map>
#include <ostream>
#include <string>
#include <utility>
#include <vector>
#include <moka/exception.h>
#include <moka/global.h>
#include <moka/util/math.h>

namespace moka {
namespace ml {

/**
 * Class SuperSOM
 *
 * Implementation of a Supervised Self-Organizing Map [1].
 *
 * References
 *   [1] T. Kohonen. The Self-Organizing Map. 1990.
 *   [2] T. Kohonen et al. SOM_PAK: The Self Organized Map Program Package.
 *       1996.
 */
class SuperSOM
{
  public:
    typedef Global::Int Int;
    typedef Global::Real Real;
    typedef Global::Uint Uint;
    typedef typename util::Math::Vector Data;
    typedef Data Codebook;
    typedef std::vector<Data> DataContainer;
    typedef std::pair<size_t, size_t> UnitIndex;
    typedef std::vector< std::vector<float> > UMatrix;

    enum TrainingType { unsupervised_training, supervised_training };
    enum DecayType { linear_decay, inverse_decay };

    //! Default constructor
    SuperSOM();

    //! Copy constructor
    SuperSOM(const SuperSOM& ssom);

    //! Destructor
    ~SuperSOM();

    //! Returns the unit activation function result on the passed data
    Real activate(const UnitIndex& unit, const Data& data) const;

    //! Clear this object as just created
    void clear()
    {
      clearObject();
    }

    //! DecayType to std::string conversion
    static std::string dectToStr(const DecayType& decay_type);

    //! Parameter gamma in the activation function
    const Real& getActFunGamma() const
    {
      return m_act_fun_gamma;
    }

    //! Learning rate for rough (unsupervised) train phase (1st phase)
    const Real& getAlpha1() const
    {
      return m_alpha_1;
    }

    //! Learning rate for fine (unsupervised) tune phase (2nd phase)
    const Real& getAlpha2() const
    {
      return m_alpha_2;
    }

    //! Learning rate for final phase / supervised train (3rd phase)
    const Real& getAlpha3() const
    {
      return m_alpha_3;
    }

    //! Alpha decay type (for all the phases)
    DecayType getAlphaDecayType() const
    {
      return m_alpha_decay_type;
    }

    //! Returns the codebook of the unit with index (row,col)
    const Data& getCodebook(size_t row, size_t col) const
    {
      return m_map[row][col];
    }

    //! Returns the codebook of the unit with passed index
    const Data& getCodebook(const UnitIndex& unit) const
    {
      return m_map[unit.first][unit.second];
    }

    //! Returns size of units in the map (i.e. the codebook size)
    size_t getCodebookSize() const
    {
      if (m_map.size() == 0)
        return 0;
      return m_map[0][0].size();
    }

    //! Number of columns in the map
    const Uint& getNoColumns() const
    {
      return m_ncols;
    }

    //! Number of epochs for rough (unsupervised) train phase (1st phase)
    const Uint& getNoEpochs1() const
    {
      return m_nepochs_1;
    }

    //! Number of epochs for fine (unsupervised) tune phase (2nd phase)
    const Uint& getNoEpochs2() const
    {
      return m_nepochs_2;
    }

    //! Number of epochs for final phase / supervised train (3rd phase)
    const Uint& getNoEpochs3() const
    {
      return m_nepochs_3;
    }

    //! Number of rows in the map
    const Uint& getNoRows() const
    {
      return m_nrows;
    }

    //! Number of units in the map
    Uint getNoUnits() const
    {
      return getNoColumns() * getNoRows();
    }

    //! Random seed for map generation
    Uint getRandomSeed() const
    {
      return m_rand.getRandSeed();
    }

    //! Initial radius for rough (unsupervised) train phase (1st phase)
    const Real& getSigma1() const
    {
      return m_sigma_1;
    }

    //! Initial radius for fine (unsupervised) tune phase (2nd phase)
    const Real& getSigma2() const
    {
      return m_sigma_2;
    }

    //! Initial radius for final phase / supervised train (3rd phase)
    const Real& getSigma3() const
    {
      return m_sigma_3;
    }

    //! Final radius for rough (unsupervised) train phase (1st phase)
    const Real& getSigmaFinal1() const
    {
      return m_sigma_fin_1;
    }

    //! Final radius for fine (unsupervised) tune phase (2nd phase)
    const Real& getSigmaFinal2() const
    {
      return m_sigma_fin_2;
    }

    //! Final radius for final phase / supervised train (3rd phase)
    const Real& getSigmaFinal3() const
    {
      return m_sigma_fin_3;
    }

    //! Inits map initial codebooks randomly in the range of training data
    void init(const DataContainer& training_data);

    //! Return true if the map has been initialized, false otherwise.
    bool isInitialized() const
    {
      return m_is_initialized;
    }

    //! Loads a SOM from the specified file
    void loadFromFile(const std::string& filename);

    //! Assignment operator
    SuperSOM& operator=(const SuperSOM& ssom);

    //! Reads the SOM from the passed input stream
    virtual void read(std::istream& is);

    //! Saves the SOM on file
    bool saveOnFile(const std::string& filename) const;

    //! Parameter gamma in the activation function
    void setActFunGamma(const Real& gamma)
    {
      m_act_fun_gamma = gamma;
    }

    //! Learning rate for rough (unsupervised) train phase (1st phase)
    void setAlpha1(const Real& alpha_1)
    {
      m_alpha_1 = alpha_1;
    }

    //! Learning rate for fine (unsupervised) tune phase (2nd phase)
    void setAlpha2(const Real& alpha_2)
    {
      m_alpha_2 = alpha_2;
    }

    //! Learning rate for final phase / supervised train (3rd phase)
    void setAlpha3(const Real& alpha_3)
    {
      m_alpha_3 = alpha_3;
    }

    //! Alpha decay type (set the same decay to all the phases)
    void setAlphaDecay(DecayType decay_type)
    {
      m_alpha_decay_type = decay_type;
    }

    //! Given the training set size automatically deduces good parameters
    void setDefaultParameters(Uint training_set_size);

    //! Number of columns in the map
    void setNoColumns(const Uint& ncols)
    {
      m_ncols = ncols;
      setInitialized(false);
    }

    //! Number of epochs for rough (unsupervised) train phase (1st phase)
    void setNoEpochs1(const Uint& nepochs_1)
    {
      m_nepochs_1 = nepochs_1;
    }

    //! Number of epochs for fine (unsupervised) tune phase (2nd phase)
    void setNoEpochs2(const Uint& nepochs_2)
    {
      m_nepochs_2 = nepochs_2;
    }

    //! Number of epochs for final phase / supervised train (3rd phase)
    void setNoEpochs3(const Uint& nepochs_3)
    {
      m_nepochs_3 = nepochs_3;
    }

    //! Number of rows in the map
    void setNoRows(const Uint& nrows)
    {
      m_nrows = nrows;
      setInitialized(false);
    }

    //! Random seed for map initialization
    void setRandomSeed(const Uint& rseed)
    {
      m_rand.setRandSeed(rseed);
      setInitialized(false);
    }

    //! Initial radius for rough (unsupervised) train phase (1st phase)
    void setSigma1(const Real& sigma_1)
    {
      m_sigma_1 = sigma_1;
    }

    //! Initial radius for fine (unsupervised) tune phase (2nd phase)
    void setSigma2(const Real& sigma_2)
    {
      m_sigma_2 = sigma_2;
    }

    //! Initial radius for final phase / supervised train (3rd phase)
    void setSigma3(const Real& sigma_3)
    {
      m_sigma_3 = sigma_3;
    }

    //! Final radius for rough (unsupervised) train phase (1st phase)
    void setSigmaFinal1(const Real& sigma_fin_1)
    {
      m_sigma_fin_1 = sigma_fin_1;
    }

    //! Final radius for fine (unsupervised) tune phase (2nd phase)
    void setSigmaFinal2(const Real& sigma_fin_2)
    {
      m_sigma_fin_2 = sigma_fin_2;
    }

    //! Final radius for final phase / supervised train (3rd phase)
    void setSigmaFinal3(const Real& sigma_fin_3)
    {
      m_sigma_fin_3 = sigma_fin_3;
    }

    //! std::string to DecayType conversion
    static DecayType strToDect(const std::string& str);

    //! Trains the SOM using the supervised training algorithm
    void supervisedTraining(
        const DataContainer& training_data,
        const std::vector<Real>& training_data_classes);

    //! Builds the U-matrix for the current SOM
    void umatrix(UMatrix& umat) const;

    //! Trains the SOM using training data
    void unsupervisedTraining(const DataContainer& training_data);

    //! Returns indeces of winner unit on the passed data
    UnitIndex winnerUnit(const Data& data) const;

    //! Fill the vector <winners> with indeces of the <n> closest units
    void winnerUnits(
        const Data& data,
        Uint n,
        std::vector<UnitIndex>& winners) const;

    //! Writes the SOM on the passed output stream
    void write(std::ostream& os) const;

  private:
    // Parameters
    Uint m_nrows;
    Uint m_ncols;
    Real m_act_fun_gamma;
    bool m_is_initialized;

    // Random number generator
    Global::UniformRandomGenerator m_rand;

    // SOM elements
    std::vector< std::vector<Codebook> > m_map;

    // Training parameters
    Uint m_nepochs_1, m_nepochs_2, m_nepochs_3;
    Real m_alpha_1, m_alpha_2, m_alpha_3;
    Real m_sigma_1, m_sigma_2, m_sigma_3;
    Real m_sigma_fin_1, m_sigma_fin_2, m_sigma_fin_3;
    DecayType m_alpha_decay_type;

    // Training support structures
    std::vector< std::vector< std::vector< std::list<UnitIndex>* > > >
    m_neighbors_map;
    std::vector< std::vector< std::map<Real, Int> > > m_classes_map;
    std::vector<UnitIndex> m_winner_unit_vector;

    // Private methods
    Real alphaDecay(
        const Real& alpha_ini,
        const DecayType& decay_type,
        const Uint& total_steps,
        const Uint& step) const;

    void clearNeighborsMap();

    void clearObject();

    Real distance(const Data& data, const Codebook& codebook) const;

    Uint hexTop(
        size_t index_1_1,
        size_t index_1_2,
        size_t index_2_1,
        size_t index_2_2) const;

    void initNeighborsMap();

    void lvq1ExtUpdate(
        const Data& data,
        const Real& data_class,
        const UnitIndex& win_unit,
        const Real& alpha,
        const Real& sigma);

    void lvq3ExtUpdate(
        const Data& data,
        const Real& data_class,
        const UnitIndex& closest_unit_1,
        const UnitIndex& closest_unit_2,
        const Real& window,
        const Real& epsilon,
        const Real& alpha,
        const Real& sigma);

    void setInitialized(bool initialized = false);

    Real sigmaDecay(
        const Real& sigma_ini,
        const Real& sigma_fin,
        const Uint& total_steps,
        const Uint& step) const;

    void superTrainProcedure(
        const DataContainer& tr_data,
        const std::vector<Real>& tr_data_class,
        const Uint& epochs,
        const Real& alpha,
        const DecayType& alpha_decay,
        const Real& sigma_ini,
        const Real& sigma_fin);

    Real unitClass(const UnitIndex& unit, const Real& default_class) const;

    void unsuperTrainProcedure(
        const DataContainer& tr_data,
        const Uint& epochs,
        const Real& alpha,
        const DecayType& alpha_decay,
        const Real& sigma_ini,
        const Real& sigma_fin);

    void updateCodebook(
        const UnitIndex& winner_unit,
        const Data& data,
        const Real& alpha,
        const Real& sigma,
        bool positive_updating = true,
        const Real& epsilon = 1.0);

    void updateUnitClass(
        const UnitIndex& data_win_unit,
        const Real& data_class,
        const size_t& data_index);

}; // class SuperSOM

} // namespace ml
} // namespace moka

#endif // MOKA_ML_SUPERSOM_H

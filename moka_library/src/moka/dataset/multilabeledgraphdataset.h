#ifndef MOKA_DATASET_MULTILABELEDGRAPHDATASET_H
#define MOKA_DATASET_MULTILABELEDGRAPHDATASET_H

#include <deque>
#include <list>
#include <set>
#include <string>
#include <utility>
#include <openbabel/mol.h>
#include <openbabel/shared_ptr.h>
#include <moka/dataset/genericdataset.h>
#include <moka/structure/multilabeledgraph.h>

namespace moka {
namespace dataset {

/**
 * Class MultiLabeledGraphDataset
 *
 * Represents a dataset of intances <id, graph, output> where graph are of
 * type MultiLabeledGraph.
 *
 * See the class Dataset and the class MultiLabeledGraph for more info.
 *
 * You can load a dataset from an SDF file by passing suitable parameters on
 * the method load.
 */
class MultiLabeledGraphDataset :
    public GenericDataset< ::moka::structure::MultiLabeledGraph >
{
  public:
    typedef GenericDataset<InputType> BaseClass;

    MultiLabeledGraphDataset();
    virtual ~MultiLabeledGraphDataset();

    virtual Dataset* cloneTrSet();
    virtual Uint getSkippedInstances() const;
    virtual util::Info<std::string> info() const;
    virtual void load(const util::Parameters& params);
    virtual Uint maxDegree() const;
    virtual void print(
        std::ostream& os,
        bool split = true,
        bool input = true) const;
    virtual void read(std::istream& is);

#ifndef MOKA_TMP_CODE
    virtual void write(std::ostream& os) const;
#endif // MOKA_TMP_CODE

  protected:
    virtual void clearDataset();

  private:
    typedef std::vector< std::vector< std::string > > SmartsQueriesList;
    typedef std::vector< std::vector< std::vector<int> > > SmartsQueriesResults;

    Uint m_skipped_instances;

    void buildAtomRound(
        OpenBabel::OBMol& mol,
        OpenBabel::OBAtom *mol_atom,
        Uint round_radius,
        OpenBabel::OBMol& out_submol) const;

    void buildAtomRoundRec(
        OpenBabel::OBMol& mol,
        OpenBabel::OBAtom *mol_atom,
        Uint round_radius,
        OpenBabel::OBMol& submol,
        OpenBabel::OBAtom *submol_atom,
        std::set<unsigned long>& added_atoms,
        std::set<unsigned long>& added_bonds,
        std::deque<unsigned long>& visited_atoms) const;

    void fillDataset(
        std::list< std::string* >& ids,
        std::list< shared_ptr<OpenBabel::OBMol> >& mols,
        std::list< std::vector<Real>* >& outputs,
        const std::string& atomlist,
        bool atom_aromaticity = false,
        bool atom_charge = false,
        bool lbl_output = false,
        bool lbl_atom_symbol = false,
        Uint lbl_round_k = 0,
        const std::string& lbl_smarts_queries_file = "",
        bool lbl_smarts_auto_hydrogens = true);

    void fillMultilabel(
        std::vector<std::string>& out_multilabel,
        OpenBabel::OBMol *molecule,
        OpenBabel::OBAtom *atom,
        bool add_output,
        const std::vector<Real>& instance_outputs,
        bool add_atom_symbol,
        Uint round,
        const SmartsQueriesList& smarts_queries_list,
        const SmartsQueriesResults& smarts_queries_results,
        bool smarts_autoh) const;

    void loadFromDatasetFile(const util::Parameters& params);

    void loadFromSdf(const util::Parameters& params);

    void printInstance(
        const Instance& inst,
        std::ostream& os,
        bool input = true) const;

}; // class MultiLabeledGraphDataset

} // namespace dataset
} // namespace moka

#endif // MOKA_DATASET_MULTILABELEDGRAPHDATASET_H

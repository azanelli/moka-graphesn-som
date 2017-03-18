#include "multilabeledgraphdataset.h"

#include <fstream>
#include <map>
#include <sstream>
#include <openbabel/mol.h>
#include <openbabel/obconversion.h>
#include <openbabel/obiter.h>
#include <openbabel/shared_ptr.h>
#include <openbabel/stereo/tetrahedral.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/tokenizer.hpp>
#include <moka/exception.h>
#include <moka/global.h>
#include <moka/log.h>
#include <moka/util/math.h>

namespace moka {
namespace dataset {

namespace ob   = ::OpenBabel;
namespace bt   = ::boost;
namespace balg = ::boost::algorithm;
namespace bfs  = ::boost::filesystem;
namespace bll  = ::boost::lambda;

using namespace ::moka::util;
using namespace ::moka::structure;

typedef MultiLabeledGraphDataset::Uint Uint;

/**
 * Default constructor
 *
 * Builds a void dataset.
 */
MultiLabeledGraphDataset::MultiLabeledGraphDataset() :
  BaseClass(),
  m_skipped_instances(0)
{ }

/**
 * Destructor
 *
 * Deletes the dataset.
 */
MultiLabeledGraphDataset::~MultiLabeledGraphDataset() { }

// ==============
// PUBLIC METHODS
// ==============

/**
 * Method cloneTrSet
 *
 * See Dataset::cloneTrSet.
 */
Dataset* MultiLabeledGraphDataset::cloneTrSet()
{
  MultiLabeledGraphDataset *lblg_ds = new MultiLabeledGraphDataset();
  fillWithTrInstances(*lblg_ds);
  return lblg_ds;
} // method cloneTrSet

/**
 * Method getSkippedInstances
 *
 * Returns the number of instances skipped during the execution of the last
 * load method (whereby the current instances have been loaded).
 */
Uint MultiLabeledGraphDataset::getSkippedInstances() const
{
  return m_skipped_instances;
} // method getSkippedInstances

/**
 * Method info
 *
 * See Dataset::info.
 */
util::Info<std::string> MultiLabeledGraphDataset::info() const
{
  util::Info<std::string> inf = BaseClass::info();
  inf.pushBack(
      "skipped_instances",
      "Skipped instances",
      Global::toString(getSkippedInstances()));
  return inf;
} // method info

/**
 * Method load
 *
 * See Dataset::load.
 *
 * Depending on the parameter <load-from> you can load the dataset from
 * different sources:
 *   "sdf-file": loads the dataset from a standard SDF (see loadFromSdf).
 *   "dataset-file": loads the dataset previous saved using the method write.
 */
void MultiLabeledGraphDataset::load(const util::Parameters& params)
{
  std::string load_from = params.get("load-from");
  if (load_from == "sdf-file")
    loadFromSdf(params);
  else if (load_from == "dataset-file")
    loadFromDatasetFile(params);
  else
    throw moka::GenericException(
        "MultiLabeledGraphDataset::load: invalid source (within parameter "
        "<load-from>): " + load_from);
  return;
} // method load

/**
 * Method maxDegree
 *
 * Computes and returns the max degree between all the graphs contained
 * in the dataset.
 */
Uint MultiLabeledGraphDataset::maxDegree() const
{
  Uint max_degree = 0;
  for (Uint i = 0, k = 0; i < this->getSize(); ++i)
    if ((k = this->getInput(i).maxDegree()) > max_degree)
      max_degree = k;
  return max_degree;
} // method maxDegree

/**
 * Method print
 *
 * Prints on the stream OS a description of this object. To use mainly for
 * debug purpose.
 *   - SPLIT: if true (default) the dataset instances will be divided in
 *       training set and validation set.
 *   - GRAPH: if true (default) will be represented the whole graph for each
 *       instance in the dataset.
 */
void MultiLabeledGraphDataset::print
(
    std::ostream& os,
    bool split,
    bool input
) const
{
  os << "> size:...... " << this->getSize() << "\n";
  os << "> n. folds:.. " << this->getNumberOfFolds() << "\n";
  os << "> tr. size:.. " << this->getTrSetSize() << "\n";
  os << "> va. size:.. " << this->getTestFoldSize() << "\n";
  os << "\n";

  if (split)
  {
    for (Uint i = 0; i < this->getTrSetSize(); ++i)
    {
      os << "> tr. instance at " << i << "\n";
      printInstance(this->trAt(i), os, input);
      os << "\n";
    } // for i

    for (Uint i = 0; i < this->getTestFoldSize(); ++i)
    {
      os << "> va. instance at " << i << "\n";
      printInstance(this->tsAt(i), os, input);
      os << "\n";
    } // for i
  }
  else
  {
    for (Uint i = 0; i < this->getSize(); ++i)
    {
      os << "> instance at " << i << "\n";
      printInstance(this->at(i), os, input);
      os << "\n";
    } // for i
  } // if-else (split)

  return;
} // method print

/**
 * Method read
 *
 * See Dataset::read
 *
 * Read from the input stream a dataset previously written using the method
 * write.
 */
void MultiLabeledGraphDataset::read(std::istream& is)
{
  // Clears the dataset and loads the entries.
  this->clearDataset();

  std::string *id = NULL;
  MultiLabeledGraph *input = NULL;
  std::vector<Real> *output = NULL;

  try
  {
    Uint size = Global::readLine<Uint>(is);

    for (Uint i = 0; i < size; ++i)
    {
      id = new std::string();
      input = new MultiLabeledGraph();
      output = new std::vector<Real>();

      Global::readLine(is, *id);
      input->read(is);
      Global::readLines(is, *output);

      // adds the new instance in the dataset
      Instance *instance = new Instance();
      instance->setId(id);
      instance->setInput(input);
      instance->setOutput(output);
      this->pushBackInstance(instance);
    } // for i

  } // try
  catch (std::exception& ex)
  {
    Log::err << "MultiLabeledGraphDataset::read: error: "
        << "dataset is left empty (" << ex.what() << ")" << Log::endl;
    delete id;
    delete input;
    delete output;
    this->clear();
    return;
  } // try-catch

  // The dataset is successfully loaded.
  this->endLoadInstances();

  return;
} // method read

#ifndef MOKA_TMP_CODE

/**
 * Method write
 *
 * See Dataset::write
 *
 * Writes dataset entries on the output stream (probably a file stream). You
 * can reload the dataset through the method load and the parameter from
 * "dataset-file".
 */
void MultiLabeledGraphDataset::write(std::ostream& os) const
{
  Uint size = this->getSize();
  os << size << std::endl;

  // Put entries on output stream
  for (Uint i = 0; i < size; ++i)
  {
    os << this->at(i).getId() << std::endl;
    this->at(i).getInput().write(os);
    Global::write(os, this->at(i).getOutput());
  }

  return;
} // method write

#endif // MOKA_TMP_CODE

// =================
// PROTECTED METHODS
// =================

/**
 * Method clearDataset
 *
 * The dataset is restored to its initial state.
 */
void MultiLabeledGraphDataset::clearDataset()
{
  BaseClass::clearDataset();
  m_skipped_instances = 0;
  return;
} // method clearDataset

// ===============
// PRIVATE METHODS
// ===============

/**
 * Method buildAtomRound
 *
 * Adds to a submolecule atom its round (substructure) of a given radius
 * taking it from a starting origin molecule.
 * The function takes the following arguments:
 *   - MOL: the starting molecule
 *   - MOL_ATOM: a pointer to an atom in mol from which take the round
 *   - ROUND_RADIUS: the round radius
 *   - OUT_SYMBOL: the resulting submol will be returned here (you must
 *       initialy put here an empty molecule.
 */
inline
void MultiLabeledGraphDataset::buildAtomRound
(
    ob::OBMol& mol,
    ob::OBAtom *mol_atom,
    Uint round_radius,
    ob::OBMol& out_submol
) const
{
  if (!out_submol.Empty())
    out_submol.Clear();

  // // Faster but doesn't handle right atom index
  // if (round_radius >= mol.NumAtoms())
  // {
  //   out_submol = mol;
  //   return;
  // }

  std::set<unsigned long> added_atoms;
  std::set<unsigned long> added_bonds;
  std::deque<unsigned long> visited_atoms;

  out_submol.BeginModify();

  ob::OBAtom *new_atom = out_submol.NewAtom(mol_atom->GetId());
  new_atom->SetIdx(1);
  new_atom->SetAtomicNum(mol_atom->GetAtomicNum());
  added_atoms.insert(mol_atom->GetId());

  buildAtomRoundRec
      (
        mol,
        mol_atom,
        round_radius,
        out_submol,
        new_atom,
        added_atoms,
        added_bonds,
        visited_atoms
      );

  out_submol.EndModify();

  return;
} // method buildAtomRound

/**
 * Method buildAtomRoundRec
 *
 * Supporting method for buildAtomRound.
 */
void MultiLabeledGraphDataset::buildAtomRoundRec
(
    ob::OBMol& mol,
    ob::OBAtom *mol_atom,
    Uint round_radius,
    ob::OBMol& submol,
    ob::OBAtom *submol_atom,
    std::set<unsigned long>& added_atoms,
    std::set<unsigned long>& added_bonds,
    std::deque<unsigned long>& visited_atoms
) const
{
  if (round_radius == 0)
    return;

  if (mol_atom->GetId() != submol_atom->GetId())
  {
    Log::err << "MultiLabeledGraphDataset::buildAtomRoundRec: Fatal error: "
             << "submolecule atom differ from molecule atom (there is a bug "
             << "here, the code need to be fixed)" << Log::endl;
    return;
  }

  // Adds neighbors (atoms and bonds) at radius 1 from current atom
  for(ob::OBAtomAtomIter neigh(*mol_atom); neigh; ++neigh)
  {
    // Check if the atoms was already added in the submol
    if (added_atoms.count(neigh->GetId()))
    {
      // Check if the bond already exists in the submol (otherwise adds it)
      ob::OBBond *mol_bond = mol.GetBond(mol_atom, &(*neigh));
      if (!mol_bond)
        Log::err << "MultiLabeledGraphDataset::buildAtomRoundRec: Fatal error: "
                 << "unexpected null bond (1)." << Log::endl;
      else if (!added_bonds.count(mol_bond->GetId()))
      {
        ob::OBBond *new_bond = submol.NewBond(mol_bond->GetId());
        new_bond->SetBegin(submol_atom);
        new_bond->SetEnd(submol.GetAtomById(neigh->GetId()));
        new_bond->SetBondOrder(mol_bond->GetBondOrder());
        added_bonds.insert(mol_bond->GetId());
      }
    } // if
    else
    {
      // Adds new atom and new bond to submolecule
      ob::OBAtom *new_atom = submol.NewAtom(neigh->GetId());
      new_atom->SetAtomicNum(neigh->GetAtomicNum());
      added_atoms.insert(new_atom->GetId());

      ob::OBBond *mol_bond = mol.GetBond(mol_atom, &(*neigh));
      if (!mol_bond)
      {
        Log::err << "MultiLabeledGraphDataset::buildAtomRoundRec: Fatal error: "
                 << "unexpected null bond (2)." << Log::endl;
        return;
      }
      ob::OBBond *new_bond = submol.NewBond(mol_bond->GetId());
      new_bond->SetBegin(submol_atom);
      new_bond->SetEnd(new_atom);
      new_bond->SetBondOrder(mol_bond->GetBondOrder());
      added_bonds.insert(mol_bond->GetId());
    } // else
  } // for neigh

  if (round_radius == 1)
    return;

  // Go on neighbors with decremented radius
  visited_atoms.push_front(submol_atom->GetId());
  for(ob::OBAtomAtomIter neigh(*mol_atom); neigh; ++neigh)
  {
    // Check if current path come from this neighbor
    std::deque<unsigned long>::const_iterator it =
        std::find(visited_atoms.begin(), visited_atoms.end(), neigh->GetId());

    if (it == visited_atoms.end())
    {
      buildAtomRoundRec
          (
            mol,
            &(*neigh),
            round_radius - 1,
            submol,
            submol.GetAtomById(neigh->GetId()),
            added_atoms,
            added_bonds,
            visited_atoms
          );
    } // if

  } // for neigh
  visited_atoms.pop_front();

  return;
} // method buildAtomRoundRec

/**
 * Method fillDataset
 *
 * Given a list of id, molecules and outputs, builds the instances and put
 * them into the dataset. The three lists must be of the same size, and a
 * new instance is builded by taking elements at the same positions from the
 * three lists. Each molecule is tranformed into a graph of type
 * MultiLabeledGraph with verteces composed by:
 *   - the 1-of-m (0/1) atom representation (looking also at parameter
 *       ATOMLIST for the atoms coding).
 *   - 1 vertex element for atom aromaticity with values 0 or 1
 *   - 1 vertex element for the charge coding with values in -1/1
 *
 * The method arguments are the following:
 *   - IDS: list containing the identifiers of the instances
 *   - MOLS: list containing the molecules
 *   - OUTPUTS: list containing the outputs of each instance
 *   - ATOM_LIST: string contains a comma separated list of atom names used
 *       to fix their 1-of-m coding.
 *   - ATOM_AROMATICITY: if true the aromaticity info is added as vertex
 *       element.
 *   - ATOM_CHARGE: if true the atom charge info is added as vertex
 *       element.
 *   - LBL_ROUND_K: unsigned integer; if 0 (default) no additional labels
 *       will added, if 1 or more will added labels representing the vertex
 *       round substructure (in canonical smiles format).
 *   - LBL_SMARTS_QUERIES_FILE: see the method loadFromDatasetFile.
 *   - LBL_SMARTS_AUTO_HYDROGENS: see the method loadFromDatasetFile.
 * If the three lists have different size an exception of type
 * moka::GenericException is thrown.
 *
 * At the end of this method the three list will be empty, with all the
 * elements transferred into the dataset.
 */
void MultiLabeledGraphDataset::fillDataset
(
    std::list<std::string*>& ids,
    std::list<shared_ptr<ob::OBMol> >& mols,
    std::list<std::vector<Real>*>& outputs,
    const std::string& atom_list,
    bool atom_aromaticity,
    bool atom_charge,
    bool lbl_output,
    bool lbl_atom_symbol,
    Uint lbl_round_k,
    const std::string& lbl_smarts_queries_file,
    bool lbl_smarts_auto_hydrogens
)
{
  // Check lists sizes
  if (ids.size() != mols.size() || mols.size() != outputs.size())
    throw moka::GenericException(
        "MultiLabeledGraphDataset::fillDataset: the three lists passed at "
        "this method are of different sizes");

  // If non-empty filename loads labels and smarts queries
  SmartsQueriesList smarts_queries_list;
  if (!lbl_smarts_queries_file.empty())
  {
    std::ifstream ifs(lbl_smarts_queries_file.c_str(), std::ifstream::in);
    if (ifs.fail())
      throw moka::GenericException("MultiLabeledGraphDataset::fillDataset: "
          "fail opening file " + lbl_smarts_queries_file);

    // Gets labels and queries from file
    while (ifs.good())
    {
      std::string line;
      std::getline(ifs, line);

      // skip empty lines
      balg::trim(line);
      if (line.empty())
      { /* nop */ }
      else
      {
        std::vector<std::string> sline;
        balg::split
            (
              sline,
              line,
              balg::is_any_of("\t "),
              balg::token_compress_on
            );

        if (sline.size() != 3)
          Log::err << "MultiLabeledGraphDataset::fillDataset: error: wrong "
                   << "line (skipped): " << line << Log::endl;
        else
        {
          for (size_t i = 0; i < sline.size(); ++i)
            balg::trim(sline[i]);
          smarts_queries_list.push_back(sline);
        }
      } // if-else

    } // while
    ifs.close();
  } // if

  // Parse the atom list
  std::vector<std::string> atom_list_vect;
  if (!atom_list.empty())
    balg::split
        (
          atom_list_vect,
          atom_list,
          balg::is_any_of(","),
          balg::token_compress_on
        );

  // Collects all the atoms (identified by their atomic number) in a map.
  std::map<Uint, int> atom_map;
  std::list< shared_ptr<ob::OBMol> >::iterator mols_it;

  for (mols_it = mols.begin(); mols_it != mols.end(); ++mols_it)
    for (ob::OBMolAtomIter atom_it(*(mols_it->get())); atom_it; ++atom_it)
      atom_map[atom_it->GetAtomicNum()] = -1; // for now inited at -1

  // Assign the 1-of-k coding position to each atom in the map
  Uint coding = 0;

  // If there is not an atom list assigns automatically an encoding to each
  // atom in the map.
  if (atom_list_vect.empty())
  {
    std::map<Uint, int>::iterator atom_map_it = atom_map.begin();
    for (/* nop */; atom_map_it != atom_map.end(); ++atom_map_it)
      atom_map_it->second = (coding++);
  }
  // Assign an encoding looking into the passed atom_list.
  else
  {
    // Note: the coding is incremented also if atomicnum is 0 (i.e. the atom
    // name in the list is not recognized by OpenBabel) and in the 1-of-k
    // representation there will be such index always null.
    for (size_t i = 0; i < atom_list_vect.size(); ++i, ++coding)
    {
      balg::trim(atom_list_vect[i]);
      int atomic_num = ob::etab.GetAtomicNum(atom_list_vect[i].c_str());
      if (atomic_num == 0)
        Log::err << "MultiLabeledGraphDataset::fillDataset: warning: invalid "
                 << "atom name in the atom list: " << atom_list_vect[i] << "."
                 << Log::endl;
      else if (atom_map.count(atomic_num) > 0)
        atom_map[atomic_num] = coding;
    } // for i

    // Remove atoms that were not in the atom list
    std::map<Uint, int>::iterator atom_map_it = atom_map.begin();
    while (atom_map_it != atom_map.end())
      if (atom_map_it->second < 0)
      {
        Log::vrb << "MultiLabeledGraphDataset::fillDataset: the following atom "
                 << "will have a null code since is not in the atom list: "
                 << ob::etab.GetSymbol(atom_map_it->first) << "." << Log::endl;
        atom_map.erase(atom_map_it++);
      }
      else
      {
        ++atom_map_it;
      }

  } // else

  // Builds the instances and puts them into the dataset. Each molecule is
  // trasformed in a MultiLabeledGraph object (by converting atoms in the
  // 1-of-k representation).
  while (!ids.empty())
  {
    Instance *newinstance = new Instance();

    // Adds the id
    newinstance->setId(ids.front());

    // Adds the outputs
    newinstance->setOutput(outputs.front());

    // Get the molecule
    ob::OBMol &molecule = (*mols.front());

    // Gets smart query list results
    SmartsQueriesResults smarts_queries_results;
    if (!smarts_queries_list.empty())
    {
      SmartsQueriesList::iterator sql_it = smarts_queries_list.begin();
      for (/* nop */; sql_it != smarts_queries_list.end(); ++sql_it)
      {
        ob::OBSmartsPattern sp;
        sp.Init(sql_it->at(2));
        sp.Match(molecule);
        smarts_queries_results.push_back(sp.GetUMapList());
      } // for

    } // if

    // Builds and adds the input graph (ATTENTION: in next versions of the
    // OpenBabel library the atom indexing may will be changed)
    for (size_t atom_n = 0; atom_n < mols.front()->NumAtoms(); ++atom_n)
    {
      // The atom indexing in a OBMol is "plus one" (atom_n + 1)
      ob::OBAtom *atom = molecule.GetAtom(atom_n + 1);

      size_t vertex_size = coding;
      size_t aromaticity_index = 0;
      size_t charge_index = 0;
      if (atom_aromaticity)
        aromaticity_index = vertex_size++;
      if (atom_charge)
        charge_index = vertex_size++;

      // Build the atom/vertex with atom number and other fields
      Math::Vector *vertex = new Math::Vector(vertex_size);
      vertex->fill(0.0);

      // 1-of-k atom representation
      vertex->at(atom_map[atom->GetAtomicNum()]) = 1.0;

      // Atom aromaticity: 0 or 1.
      if (atom_aromaticity)
        vertex->at(aromaticity_index) = (atom->IsAromatic() ? 1.0 : 0.0);

      // Charge value in [-1,+1]
      if (atom_charge)
        vertex->at(charge_index) = atom->GetFormalCharge();

      // // Radical value in [0,+1]
      // vertex->at(vertex->n_rows - 1) = atom->GetSpinMultiplicity() / 2.0;
      // // atom stereo parity in [0,+1]
      // ob::OBStereoFacade stfcd( &(*mols.front()) );
      // ob::OBTetrahedralStereo *thst =
      //     stfcd.GetTetrahedralStereo(atom->GetId());
      // if (!thst)
      //   vertex->at(vertex->n_rows - 1) = 0;
      // else
      //   vertex->at(vertex->n_rows - 1) =
      //     (
      //       ob::OBStereo::NumInversions(
      //           thst->GetConfig(
      //               ob::OBStereo::Clockwise,
      //               ob::OBStereo::ViewTowards
      //           ).refs) % 2
      //     ) ? 0.5 : 1;

      // Fill neighbors list
      std::vector<Uint> *neighbors = new std::vector<Uint>();
      for (ob::OBAtomAtomIter neigh_it(atom); neigh_it; ++neigh_it)
        neighbors->push_back(neigh_it->GetIdx() - 1);

      // Create the vertex multilabel
      std::vector<std::string> multilabel;
      fillMultilabel
          (
            multilabel,
            &molecule,
            atom,
            lbl_output,
            newinstance->getOutput(),
            lbl_atom_symbol,
            lbl_round_k,
            smarts_queries_list,
            smarts_queries_results,
            lbl_smarts_auto_hydrogens
          );

      // Adds the vertex to the last dataset instance
      newinstance->getInput().pushBack(multilabel, vertex, neighbors);

    } // for atom_it

    // Add label names in the multilabeled graph (see the method loadFromSdf).
    if (lbl_output)
      for (size_t i = 1; i <= newinstance->getOutput().size(); ++i)
        newinstance->getInput().addLabelName("Output " + Global::toString(i));

    if (lbl_atom_symbol)
      newinstance->getInput().addLabelName("Atom symbol");

    if (lbl_round_k != 0)
      for (size_t i = 1; i <= lbl_round_k; ++i)
        newinstance->getInput().addLabelName(
              "Atom round (radius " + Global::toString(i) + ")");

    if (!smarts_queries_list.empty())
      for (size_t i = 0; i < smarts_queries_list.size(); ++i)
        newinstance->getInput().addLabelName(smarts_queries_list[i][0]);

    // Check if the just builded graph is consistent, if it is not the instance
    // is removed
    if (!newinstance->getInput().checkConsistence())
    {
      Log::err << "MultiLabeledGraphDataset::fillDataset: the dataset instance "
               << "with id " << newinstance->getId() << " is removed because "
               << "the graph is inconsistent." << Log::endl;
      m_skipped_instances++;
      delete newinstance;
    }
    else
      this->pushBackInstance(newinstance);

    ids.pop_front();
    mols.pop_front();
    outputs.pop_front();

  } // while (!ids.empty())

  return;
} // method fillDataset

/**
 * Method fillMultilabel
 *
 * Support method for fillDataset. Push back labels in OUT_MULTILABEL vector
 * by taking them from ATOM and its own molecule MOLECULE.
 */
void MultiLabeledGraphDataset::fillMultilabel
(
    std::vector<std::string>& out_multilabel,
    ob::OBMol *molecule,
    ob::OBAtom *atom,
    bool add_output,
    const std::vector<Real>& instance_outputs,
    bool add_atom_symbol,
    Uint round,
    const SmartsQueriesList& smarts_queries_list,
    const SmartsQueriesResults& smarts_queries_results,
    bool smarts_autoh
) const
{
  // Some initial checks
  if (smarts_queries_list.size() != smarts_queries_results.size())
  {
    Log::err << "MultiLabeledGraphDataset::fillMultilabel: the smarts queries "
             << "list and the queries results list are of different size "
             << "(there is a bug in the code!)." << Log::endl;
    return;
  }

  // Output labels
  if (add_output)
    for (size_t i = 0; i < instance_outputs.size(); ++i)
      out_multilabel.push_back(Global::toString(instance_outputs[i]));

  // Atom symbol plus aromaticity and formal charge
  if (add_atom_symbol)
  {
    std::string atom_info_label = ob::etab.GetSymbol(atom->GetAtomicNum());

    if (atom->IsAromatic())
      balg::to_lower(atom_info_label);

    if (atom->GetFormalCharge() > 0)
      atom_info_label += "+";

    else if (atom->GetFormalCharge() < 0)
      atom_info_label += "-";

    out_multilabel.push_back(atom_info_label);
  }

  // Atom round substructures
  for (Uint k = 1; k <= round; ++k)
  {
    ob::OBMol submol;
    buildAtomRound(*molecule, atom, k, submol);

    ob::OBConversion conv;
    if (conv.SetOutFormat("can"))
    {
      // specify the first atom (but seem doesn't working with canonical)
      // conv.AddOption("f", ob::OBConversion::OUTOPTIONS, "1");

      // output explicit hydrogens as such
      conv.AddOption("h");

      // do not include isotopic or chiral markings
      conv.AddOption("i");

      // no molecule name
      conv.AddOption("n");

      // Write canonical smile
      std::string submol_can = conv.WriteString(&submol);
      balg::trim(submol_can);
      out_multilabel.push_back(submol_can);

    } // if
    else
    {
      Log::err << "MultiLabeledGraphDataset::fillMultilabel: error: fail in "
               << "SMILES conversion." << Log::endl;
      out_multilabel.push_back("");
    } // if-else
  } // for k

  // Atom belong to a SMARTS query result
  SmartsQueriesList::const_iterator list_it = smarts_queries_list.begin();
  SmartsQueriesResults::const_iterator res_it = smarts_queries_results.begin();
  for (/* nop */; res_it != smarts_queries_results.end(); ++res_it, ++list_it)
  {
    bool label = false;

    std::vector< std::vector<int> >::const_iterator it;
    std::vector<int>::const_iterator jt;

    // Hydrogens may be labeled also if its neighbor belong to a query result
    if (atom->GetAtomicNum() == 1)
    {
      for (it = res_it->begin(); (it != res_it->end()) && (!label); ++it)
        for (jt = it->begin(); (jt != it->end()) && (!label); ++jt)
        {
          if (atom->GetIdx() == (*jt))
            label = true;
          else if (smarts_autoh)
            for (ob::OBAtomAtomIter ng(atom); ng; ++ng)
              if (ng->GetIdx() == (*jt))
              {
                label = true;
                break;
              }
        } // for (it, jt)

    } // if (atom->GetAtomicNum() == 1)
    else
    {
      // Non hydrogen atoms
      for (it = res_it->begin(); (it != res_it->end()) && (!label); ++it)
        for (jt = it->begin(); (jt != it->end()) && (!label); ++jt)
          if (atom->GetIdx() == (*jt))
            label = true;
    } // if-else

    if (label)
      out_multilabel.push_back(list_it->at(1));
    else
      out_multilabel.push_back("");

  } // for res_it

  return;
} // method fillMultilabel

/**
 * Method loadFromDatasetFile
 *
 * Loads the dataset entries from a file written using the method write (see
 * also the method read).
 *
 * The method takes the following parameters:
 *   - <file-path>: path of the file containing dataset entries.
 */
void MultiLabeledGraphDataset::loadFromDatasetFile
(
    const util::Parameters& params
)
{
  std::string filename = params.get("file-path");

  std::ifstream ifs(filename.c_str(), std::ifstream::in);
  if (ifs.fail())
    throw moka::GenericException(
        "MultiLabeledGraphDataset::loadFromDatasetFile: fail opening the "
        "file " + filename);

  if (!ifs.good())
    throw moka::GenericException(
        "MultiLabeledGraphDataset::loadFromDatasetFile: error reading file " +
        filename);

  read(ifs);

  ifs.close();

  return;
} // method loadFromDatasetFile


/**
 * Method loadFromSdf
 *
 * Loads the dataset from a standard SDF file with the following format:
 *   [MOL_1]
 *   > <id>
 *   ID1
 *
 *   > <output.1>
 *   ID1_OUTPUT1
 *
 *   > <output.2>
 *   ID1_OUTPUT2
 *
 *   ...
 *
 *   $$$$
 *   [MOL_2]
 *   > <id>
 *   ID2
 *
 *   > <output.1>
 *   ID2_OUTPUT1
 *
 *   > <output.2>
 *   ID2_OUTPUT2
 *
 *   ...
 *
 *   $$$$
 *   ...
 *   ...
 * Where [MOL] is the molecule representation followed by two or more data
 * fields, the first has name "id" and must contain as value the instance id,
 * the others are the instance outputs. If there is only one output then the
 * name of the data field can be "output" or "output.1", if there are more
 * than one output then the names must be "output.n" where n is the output
 * number. All the molecules must have the same number of outputs. The molecules
 * with a wrong number of outputs will be discarded. If there are errors during
 * the reading of a molecule, such molecule will be discarded from the dataset.
 *
 * Each molecule is converted to a MultiLabeledGraph with verteces composed by
 * 1-of-k (0/1) atom representation and optionally additional info.
 *
 * The method takes the following parameters:
 *   - <file-path>: path of SDF file in the previous form.
 *   - <id-name>: the name (string) of the data field that contains the
 *       instance id.
 *   - <output-name>: the name (string) of the data fields that contains the
 *       instance outputs. If there are more than one output this string is the
 *       prefix of the name that in the sdf file must be followed by ".n".
 *   - <no-outptus>: number of outputs for each molecule. If 0 the output is
 *       ignored.
 *   - <atom-list>: a string containing an ordered (by any order) list of atom
 *       names separated by a comma (e.g.: "H,C,O,Zn,Ca,Na,Cl"). In the
 *       1-of-k representation conversion of atoms (for the graph verteces),
 *       each atom has its specific coding: the list is used to fix the
 *       atom coding, in the sense that the first atom of the list will have
 *       the first 1-of-k coding, the second atom will have the second
 *       coding and so on. If you provide always the same list, the atoms will
 *       have always the same coding also on different datasets. If in the
 *       dataset there are atoms not in the list they are ignored and a null
 *       1-of-k coding will be assigned to the vertex (all 0 values): this is
 *       done so that the atom coding (and the vertex size) will be the same
 *       also on different datasets (e.g. a training set and a test set) if you
 *       provide the same atom list. If in the list there are atoms that are
 *       not in the dataset (and this is also the case of names errors) then
 *       the corrispondent coding is always left empty. The default value is an
 *       empty string ("") by which the coding of each atom in the dataset is
 *       chosen automatically. Should be noted, however, that in this last case
 *       the coding may change on different datasets.
 *   - <atom-aromaticity>: boolean value, false as default. If true one vertex
 *       element is added for the atom aromaticity with values 0 or 1.
 *   - <atom-charge>: boolean value, false as default. If true one vertex
 *       element is added for the atom charge coding with values in -1/1.
 *   - <sort-by-id>: if true the dataset instances are reordered according to
 *       their instance id; if false are left in the building order (that can
 *       be whatever); the default is true; set it to false for computational
 *       cost optimization if the dataset is very large.
 *
 * To each graph vertex can be assigned a set of labels using one or more of
 * the below parameters. The labels are added in the vertex item in the order
 * that are listed below:
 *   - <add-lbl-output>: a boolean value, if true the value of each output (of
 *       the whole molecule) is added as label on each vertex. The label name
 *       is "Output <n>" (where n is the output number).
 *   - <add-lbl-atom-symbol>: a boolean value, if true the atom symbol
 *       corresponding to the vertex (in lower case if it is an aromatic atom,
 *       plus the +/- suffix if has not null charge value) is added as label.
 *       The label name is "Atom symbol".
 *   - <add-lbl-smile-round-k>: if this parameter is setted (you must set an
 *       integer value for k in this parameter), are added at each vertex
 *       labels containing the substructure of the vertex round of radius from
 *       1 to k, represented in canonical SMILE. The label name will be "Atom
 *       round (radius <k>)".
 *   - <add-lbl-smarts-queries-from>: adds a label to each vertex belonging to
 *       substructures found by a SMARTS query. Queries and respective labels
 *       are readed from specified file. The file must have one entry per line
 *       (at most empty lines are skipped) and each entry must be in the
 *       following form:
 *         <Name>, <Label>, <Smarts query>
 *       Each field must not contains space characters that are used as field
 *       separators:
 *         - Name: a (free) name used as label name.
 *         - Label: the label value (if a vertex is not part of the SMARTS
 *             query result it is assigned an empty label).
 *         - Smarts Query: will be labeled by <Label> each vertex contained in
 *             the SMARTS query results.
 *   - <add-lbl-smarts-auto-hydrogens>: a boolean value. If true hydrogens
 *       attached to atoms resulting from smarts query will be automatically
 *       added as query result. Default is true.
 *
 */
void MultiLabeledGraphDataset::loadFromSdf(const util::Parameters& params)
{
  // Parse parameters
  std::string filename = params.get("file-path");
  std::string id = params.get("id-name");
  std::string output = params.get("output-name");
  Uint noutputs = params.getUint("no-outputs");
  std::string atom_list = params.get("atom-list");
  bool atom_aromaticity = params.getBool("atom-aromaticity", false);
  bool atom_charge = params.getBool("atom-charge", false);
  bool lbl_output = params.getBool("add-lbl-output", false);
  bool lbl_atom_symbol = params.getBool("add-lbl-atom-symbol", false);
  Uint lbl_round_k = params.getUint("add-lbl-smile-round-k", 0);
  std::string lbl_smarts_queries_file =
      params.get("add-lbl-smarts-queries-from");
  bool lbl_smarts_auto_hydrogens =
      params.getBool("add-lbl-smarts-auto-hydrogens", true);
  bool sortbyid = params.getBool("sort-by-id", true);

  // Check parameters
  if (id == output)
    throw moka::GenericException(
        "MultiLabeledGraphDataset::load: the id name (" + id + ") can not " +
        "be equal to the output name (" + output + ").");

  bfs::path filepath(filename);
  try
  {
    if (!bfs::exists(filepath))
      throw moka::GenericException(
          "MultiLabeledGraphDataset::load: the file (" + filepath.string() +
          ") doesn't exists");
    if (!bfs::is_regular_file(filepath))
      throw moka::GenericException(
          "MultiLabeledGraphDataset::load: invalid file path (" +
          filepath.string() + ")");
  }
  catch (const bfs::filesystem_error& ex)
  {
    throw moka::GenericException(
        "MultiLabeledGraphDataset::load: one or more arguments are wrong.");
  } // try-catch

  // All the parameters are valid. Clears the dataset and loads the SDF file.
  this->clearDataset();
  ob::OBConversion obconversion;
  obconversion.SetInFormat("sdf"); // sets the file format (SDF)
  bfs::ifstream sdffstream(filepath);
  std::list<std::string*> ids;
  std::list< shared_ptr<ob::OBMol> > mols;
  std::list<std::vector<Real>*> outputs;
  while (sdffstream.good())
  {
    shared_ptr<ob::OBMol> molptr(new ob::OBMol); // openbabel/shared_ptr.h
    if (obconversion.Read(molptr.get(), &sdffstream))
    {
      // try to puts id, molecule and outputs into the lists
      if (molptr->Empty())
      {
        m_skipped_instances++;
        Log::err << "MultiLabeledGraphDataset::loadSDFFile: it was readed an "
                 << "empty molecule: skipped. " << Log::end;
        if (!std::string(molptr->GetTitle()).empty())
          Log::err << "The empty molecule have a name: " << molptr->GetTitle()
                   << ". " << Log::end;
        else
          Log::err << "The empty molecule is after the one with ID "
                   << *ids.back() << ". " << Log::end;
        Log::err << Log::endl;
      }
      else if (!molptr->GetData(id))
      {
        m_skipped_instances++;
        Log::err << "MultiLabeledGraphDataset::loadSDFFile: molecule without "
                 << "id data field (" << id << "): skipped." << Log::endl;
      }
      else
      {
        bool skip = false;

        // gets the id
        ids.push_back(new std::string(molptr->GetData(id)->GetValue()));

        // gets the molecule
        mols.push_back(molptr);

        // gets the outputs
        outputs.push_back(new std::vector<Real>(noutputs));
        if (noutputs == 0)
        { /* leave an empty output vector */ }
        else if (noutputs > 1)
        {
          // gets data fields <output.n>
          for (Uint i = 0; i < noutputs; ++i)
          {
            std::string outputname = output + "." + Global::toString(i+1);
            if (molptr->GetData(outputname) == NULL)
            {
              skip = true;
              break;
            }
            else
              outputs.back()->at(i) =
                  Global::toReal(molptr->GetData(outputname)->GetValue());
          } // for i
        } // if (noutputs > 1)

        else if (molptr->GetData(output) != NULL) // noutputs == 1
        {
          // gets data field <output>
          outputs.back()->at(0) =
              Global::toReal(molptr->GetData(output)->GetValue());
        }

        else if (molptr->GetData(output + ".1") != NULL) // noutputs == 1
        {
          // gets data field <output.1>
          outputs.back()->at(0) =
              Global::toReal(molptr->GetData(output + ".1")->GetValue());
        }

        else
          skip = true;

        if (skip)
        {
          // remove last molecule, id and outputs
          m_skipped_instances++;
          Log::err << "MultiLabeledGraphDataset::load: error reading outputs "
                   << "in molecule with id " << *ids.back() << ": molecule "
                   << "skipped." << Log::endl;
          delete ids.back();
          delete outputs.back();
          ids.pop_back();
          mols.pop_back();
          outputs.pop_back();
        } // if (skip)

      } // else-if

    } // if

    else if (sdffstream.tellg() != -1)
    {
      Log::err << "MultiLabeledGraphDataset::load: error: something has been "
               << "skipped during file reading (could be a whole molecule)."
               << Log::endl;
      if (ids.empty())
        Log::err << "The error occurred trying to read the first molecule."
                 << Log::endl;
      else
        Log::err << "The error occurred trying to read the molecule after the "
                 << "one with ID " << *ids.back() << " and name "
                 << mols.back()->GetTitle() << "." << Log::endl;
    } // else if
  } // while (sdffstream.good())

  sdffstream.close();

  // Fills the dataset with the data just loaded
  try
  {
    fillDataset
        (
          ids,
          mols,
          outputs,
          atom_list,
          atom_aromaticity,
          atom_charge,
          lbl_output,
          lbl_atom_symbol,
          lbl_round_k,
          lbl_smarts_queries_file,
          lbl_smarts_auto_hydrogens
        );
  } // try
  catch (std::exception& ex)
  {
    Log::err << "MultiLabeledGraphDataset::load: error: the dataset is left "
             << "empty (" << ex.what() << ")." << Log::endl;

    std::for_each(ids.begin(), ids.end(), bll::delete_ptr());
    std::for_each(outputs.begin(), outputs.end(), bll::delete_ptr());

    this->clear();
    return;
  } // try-catch

  // The dataset is successfully loaded.
  this->endLoadInstances(sortbyid);
  return;
} // method loadFromSdf

/**
 * Method printInstance
 *
 * Prints on the stream OS the passed Instance.
 */
void MultiLabeledGraphDataset::printInstance
(
    const Instance& inst,
    std::ostream& os,
    bool input
) const
{
  os << "id: " << inst.getId() << "\n";
  if (input)
  {
    os << "input graph: \n";
    inst.getInput().print(os);
    os << "\n";
  }
  else
    os << "input graph size: "  << inst.getInput().getSize() << "\n";
  os << "outputs: " << Global::toString(inst.getOutput()) << "\n";
  return;
} // method printInstance

} // namespace dataset
} // namespace moka

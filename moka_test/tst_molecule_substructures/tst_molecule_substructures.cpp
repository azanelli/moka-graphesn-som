#include <fstream>
#include <iostream>
#include <set>
#include <deque>
#include <string>
#include <sstream>
#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <openbabel/mol.h>
#include <openbabel/obconversion.h>
#include <openbabel/obiter.h>
#include <openbabel/stereo/tetrahedral.h>

namespace ob = OpenBabel;

// Functions
void readMolecule(const char* filename, ob::OBMol& mol);
void printMolecule(ob::OBMol& mol);
void printAtomsRound(ob::OBMol& mol, size_t round_radius);
void buildAtomRound
(
    ob::OBMol& mol,
    ob::OBAtom *mol_atom,
    size_t round_radius,
    ob::OBMol& out_submol
);
void buildAtomRoundRec
(
    ob::OBMol& mol,
    ob::OBAtom *mol_atom,
    size_t round_radius,
    ob::OBMol& submol,
    ob::OBAtom *submol_atom,
    std::set<unsigned long>& added_atoms,
    std::set<unsigned long>& added_bonds,
    std::deque<unsigned long>& visited_atoms
);

/**
 * Function main
 */
int main(int argc, char *argv[])
{
  // Check parameters
  if (argc < 2 + 1)
  {
    std::cout << "Usage: \n";
    std::cout << "  argv[1]: filename (mol/sdf) \n";
    std::cout << "  argv[2]: substructure round radius (k) \n";
    return 1;
  } // if (argc < ...)

  size_t rr = atoi(argv[2]);

  ob::OBMol molecule;
  readMolecule(argv[1], molecule);

  printMolecule(molecule);
  printAtomsRound(molecule, rr);

  std::cout << std::endl;

  return 0;
} // function main

/**
 * Function readMolecule
 */
void readMolecule(const char* filename, ob::OBMol& mol)
{
  std::ifstream file;
  file.open(filename, std::ifstream::in);
  ob::OBConversion obconversion;
  obconversion.SetInFormat("sdf");
  obconversion.Read(&mol, &file);
  file.close();
  return;
} // end function readMolecule

/**
 * Function printMolecule
 */
void printMolecule(ob::OBMol& mol)
{
  std::cout << "Molecule title:  " << mol.GetTitle()
            << std::endl;
  std::cout << "Formula:         " << mol.GetFormula()
            << std::endl;
  std::cout << "Weight:          " << mol.GetMolWt()
            << std::endl;
  std::cout << "Number of atoms: " << mol.NumAtoms()
            << std::endl;
  std::cout << "Number of bonds: " << mol.NumBonds()
            << std::endl;
  std::cout << std::endl;
  return;
} // end function printMolecule

/**
 * Function printAtomsRound
 */
void printAtomsRound(ob::OBMol& mol, size_t round_radius)
{
  for(ob::OBMolAtomIter atom(mol); atom; ++atom )
  {
    std::cout << "Atom name:  " << ob::etab.GetSymbol(atom->GetAtomicNum())
              << std::endl;

    std::cout << "Atom index: " << atom->GetIdx()
              << std::endl;

    ob::OBMol submol;
    buildAtomRound(mol, &(*atom), round_radius, submol);

    std::string submol_name;
    for(ob::OBMolAtomIter submol_atom(submol); submol_atom; ++submol_atom )
      submol_name += ob::etab.GetSymbol(submol_atom->GetAtomicNum());

    std::cout << std::endl;

    std::cout << "Sub-Molecule: " << submol_name << std::endl;
    printMolecule(submol);

    ob::OBConversion conv;
    if (!conv.SetOutFormat("can"))
      std::cerr << "Error setting canonical smile format" << std::endl;
    else
    {
      conv.AddOption("n"); // no molecule name
      std::cout << "Canonical Smile: " << conv.WriteString(&submol)
                << std::endl;
    }

    std::cout << std::endl;
  } // for atom

  return;
} // end function printAtomsRound

/**
 * Function buildAtomRound
 *
 * Adds to a submolecule atom its round (substructure) of a given radius
 * taking it from a starting origin molecule.
 * The function takes the following arguments:
 *   - <mol>: the starting molecule
 *   - <mol_atom>: a pointer to an atom in mol from which take the round
 *   - <round_radius>: the round radius
 *   - <out_submol>: the resulting submol will be returned here (you must
 *       initialy put here an empty molecule.
 */
void buildAtomRound
(
    ob::OBMol& mol,
    ob::OBAtom *mol_atom,
    size_t round_radius,
    ob::OBMol& out_submol
)
{
  if (!out_submol.Empty())
    out_submol.Clear();

  if (round_radius >= mol.NumAtoms())
  {
    out_submol = mol;
    return;
  }

  std::set<unsigned long> added_atoms;
  std::set<unsigned long> added_bonds;
  std::deque<unsigned long> visited_atoms;

  out_submol.BeginModify();

  ob::OBAtom *new_atom = out_submol.NewAtom(mol_atom->GetId());
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
} // function buildAtomRound

/**
 * Function buildAtomRoundRec
 *
 * Supporting function for buildAtomRound.
 */
void buildAtomRoundRec
(
    ob::OBMol& mol,
    ob::OBAtom *mol_atom,
    size_t round_radius,
    ob::OBMol& submol,
    ob::OBAtom *submol_atom,
    std::set<unsigned long>& added_atoms,
    std::set<unsigned long>& added_bonds,
    std::deque<unsigned long>& visited_atoms
)
{
  if (round_radius == 0)
    return;

  if (mol_atom->GetId() != submol_atom->GetId())
  {
    std::cerr << "Fatal error: submolecule atom differ from molecule atom "
              << "(you must fix the code)" << std::endl;
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
        std::cerr << "Fatal error: unexpected null bond." << std::endl;
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
        std::cerr << "Fatal error: unexpected null bond (2)." << std::endl;
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
} // function buildAtomRound

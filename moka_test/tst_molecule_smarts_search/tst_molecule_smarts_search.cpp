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
#include <openbabel/parsmart.h>
#include <openbabel/stereo/tetrahedral.h>

namespace ob = OpenBabel;

// Functions
void readMolecule(const char* filename, ob::OBMol& mol);
void printMolecule(ob::OBMol& mol);
void execSmartsQuery(ob::OBMol& mol, const std::string& smartsq);

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
    std::cout << "  argv[2]: SMARTS query \n";
    return 1;
  } // if (argc < ...)

  std::string smartsq = argv[2];

  ob::OBMol molecule;
  readMolecule(argv[1], molecule);
  printMolecule(molecule);
  std::cout << std::endl;

  execSmartsQuery(molecule, smartsq);

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
  return;
} // end function printMolecule

/**
 * Functions execSmartsQuery
 */
void execSmartsQuery(ob::OBMol& mol, const std::string& smartsq)
{
  ob::OBSmartsPattern sp;
  sp.Init(smartsq);
  sp.Match(mol);

  std::vector< std::vector<int> > maplist = sp.GetUMapList();

  //print out the results
  std::vector< std::vector<int> >::iterator map_it;
  std::vector<int>::iterator match_it;
  for (map_it = maplist.begin(); map_it != maplist.end(); ++map_it)
  {
    for (match_it = map_it->begin(); match_it != map_it->end(); ++match_it)
      std::cout << *match_it << " ";
    std::cout << std::endl;
  } // map_it

  return;
} // function execSmartsQuery

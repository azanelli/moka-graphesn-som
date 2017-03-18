#include <fstream>
#include <iostream>
#include <string>
#include <openbabel/mol.h>
#include <openbabel/obconversion.h>
#include <openbabel/obiter.h>
#include <openbabel/stereo/tetrahedral.h>

namespace ob = OpenBabel;

// Functions
void readSDFile(const char* filename);
void readGlobalMol(const char* filename);
void readMols(const char* filename);
void printGlobalMol();
void printGlobalMolAtoms();
void printGlobalMolBonds();

// Global variables
ob::OBMol globalMol;

/**
 * Function main
 */
int main(int argc, char *argv[])
{
  // Check parameters
  if (argc < 1 + 1)
  {
    std::cout << "Usage: \n";
    std::cout << "  argv[1]: filename (mol/sdf) \n";
    return 1;
  } // if (argc < ...)

  readGlobalMol(argv[1]);
  printGlobalMol();
  printGlobalMolAtoms();
  // printBonds();

} // function main

/**
 * Function readSDFile
 */
void readSDFile(const char* filename)
{
  std::ifstream molfile;
  molfile.open(filename, std::ifstream::in);

  ob::OBMol moltmp;
  ob::OBConversion obconversion;
  obconversion.SetInFormat("sdf");

  while (obconversion.Read(&moltmp, &molfile))
  {
    std::cout << "Molecule Title:  " << moltmp.GetTitle();
    std::cout << std::endl;
    std::cout << "Formula:         " << moltmp.GetFormula();
    std::cout << std::endl;
    std::cout << "Weight:          " << moltmp.GetMolWt();
    std::cout << std::endl;
    std::cout << "Mass:            " << moltmp.GetExactMass();
    std::cout << std::endl;
    std::cout << "Number Of Atoms: " << moltmp.NumAtoms();
    std::cout << std::endl;
    std::cout << "Number Of Bonds: " << moltmp.NumBonds();
    std::cout << std::endl << std::endl;
    moltmp.Clear();
  } // end while

  molfile.close();

  return;
} // end function readSDFile

/**
 * Function readGlobalMol
 */
void readGlobalMol(const char* filename)
{
  std::ifstream file;
  file.open(filename, std::ifstream::in);
  ob::OBConversion obconversion;
  obconversion.SetInFormat("sdf");
  obconversion.Read(&globalMol, &file);
  file.close();
  return;
} // end function readGlobalMol

/**
 * Function readMols
 */
void readMols(const char* filename)
{
  std::ifstream file;
  file.open(filename, std::ifstream::in);
  /*
  ob::OBConversion obconversion;
  obconversion.SetInFormat("sdf");
  obconversion.Read(&globalMol, &file);
  */
  file.close();
  return;
} // end function readMols

/**
 * Function printGlobalMol
 */
void printGlobalMol()
{
  std::cout << "Molecule title:  " << globalMol.GetTitle()
            << std::endl;
  std::cout << "Formula:         " << globalMol.GetFormula()
            << std::endl;
  std::cout << "Weight:          " << globalMol.GetMolWt()
            << std::endl;
  std::cout << "Mass:            " << globalMol.GetExactMass()
            << std::endl;
  std::cout << "Number of atoms: " << globalMol.NumAtoms()
            << std::endl;
  std::cout << "Number of bonds: " << globalMol.NumBonds()
            << std::endl;
  std::cout << "Has ch. perc.:   " << globalMol.HasChiralityPerceived()
            << std::endl;
  std::cout << std::endl;
  return;
} // end function printGlobalMol

/**
 * Function printGlobalMolAtoms
 */
void printGlobalMolAtoms()
{
  ob::OBStereoFacade str_fcd(&globalMol);
  for (ob::OBMolAtomIter aiter(globalMol); aiter; ++aiter)
  {
    // std::cout << "Atom Title:        " << aiter->GetTitle()
    //          << std::endl;

    // std::cout << "Atomic number:     " << aiter->GetAtomicNum()
    //           << std::endl;

    std::cout << "Atom name:         "
              << ob::etab.GetSymbol(aiter->GetAtomicNum())
              << std::endl;

    std::cout << "Formal charge:     " << aiter->GetFormalCharge()
              << std::endl;

    std::cout << "Formal isotope:    " << aiter->GetIsotope();
    std::cout << std::endl;

    std::cout << "Spin multiplicity: " << aiter->GetSpinMultiplicity()
              << std::endl;

    std::cout << "Atomic mass:       " << aiter->GetAtomicMass()
              << std::endl;

    std::cout << "Atom type:         " << aiter->GetType()
              << std::endl;

    std::cout << "Is chiral:         " << (aiter->IsChiral() ? "yes" : "no")
              << std::endl;

    ob::OBTetrahedralStereo *th_str =
        str_fcd.GetTetrahedralStereo(aiter->GetId());
    if (th_str)
    {
      std::cout << "Th. Stereo:       "
                << *th_str
                << std::endl;
      std::cout << "Num. Inversion:    "
                << (ob::OBStereo::NumInversions(
                       th_str->GetConfig(
                           ob::OBStereo::Clockwise,
                           ob::OBStereo::ViewTowards
                       ).refs) % 2
                    ? 0.5 : 1)
                << std::endl;
    } // if

    std::cout << std::endl;

  } // for

  return;
} // end function printGlobalMolAtoms

/**
 * Function printGlobalMolBonds
 */
void printGlobalMolBonds()
{
  for (ob::OBMolBondIter biter(globalMol); biter; ++biter)
  {
    std::cout << "Bond Title: " << biter->GetTitle()
              << std::endl;
    std::cout << "Bond order: " << biter->GetBondOrder()
              << std::endl;
    std::cout << "Begin atom: " << biter->GetBeginAtom()->GetAtomicNum()
              << std::endl;
    std::cout << "End atom    " << biter->GetEndAtom()->GetAtomicNum()
              << std::endl;
    std::cout << "Length:     " << biter->GetLength()
              << std::endl;
    std::cout << "Single      " << (biter->IsSingle() ? "yes" : "no")
              << std::endl;
    std::cout << "Double:     " << (biter->IsDouble() ? "yes" : "no")
              << std::endl;
    std::cout << "Triple:     " << (biter->IsTriple() ? "yes" : "no")
              << std::endl;
    std::cout << std::endl;
  }
  return;
} // end function printGlobalMolBonds

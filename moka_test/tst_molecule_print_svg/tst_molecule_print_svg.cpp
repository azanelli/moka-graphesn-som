#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <openbabel/generic.h>
#include <openbabel/mol.h>
#include <openbabel/obconversion.h>
#include <openbabel/obiter.h>
#include <openbabel_ext/formats/svgformatext.h>

namespace ob = OpenBabel;

/**
 * Function main
 */
int main(int argc, char *argv[])
{
  // Check parameters
  if (argc < 1 + 2)
  {
    std::cout << "Usage: \n";
    std::cout << "  argv[1]: input filename (mol/sdf) \n";
    std::cout << "  argv[2]: out filename (svg) \n";
    return 1;
  } // if (argc < ...)

  std::string infile = argv[1];
  std::string outfile = argv[2];

  // Reads molecule
  ob::OBMol *molecule = new ob::OBMol();
  std::ifstream ifs;
  ifs.open(infile.c_str(), std::ifstream::in);
  ob::OBConversion iconv;
  iconv.SetInFormat("sdf");
  iconv.Read(molecule, &ifs);
  ifs.close();

  // Adds molecule info
  ob::OBPairData *info = new ob::OBPairData();
  info->SetOrigin(ob::userInput); // set by user, not by Open Babel
  info->SetAttribute("Info");
  info->SetValue("copyright (c) moka project");
  molecule->SetData(info);

  // Adds labels
  std::srand(time(NULL));
  for (ob::OBMolAtomIter atom(*molecule); atom; ++atom)
  {
    double rlbl = ((rand() % 10001) - 5000) / 5000.0;
    std::stringstream sslbl;
    // std::scientific(sslbl);
    sslbl << std::setprecision(2) << rlbl;

    // Label
    ob::OBPairData *label = new ob::OBPairData();
    label->SetOrigin(ob::userInput); // set by user, not by Open Babel
    label->SetAttribute("CustomLabelText");
    label->SetValue(sslbl.str());
    atom->SetData(label);

    // Color
    ob::OBPairData *color = new ob::OBPairData();
    color->SetOrigin(ob::userInput);
    color->SetAttribute("CustomLabelColor");
    if (rlbl >= 0)
      color->SetValue("0.88, 0.47, 0.14"); // orange: rgb(225, 120, 35)
    else
      color->SetValue("0.0, 0.71, 0.73"); // cyan: rgb(0, 180, 185)
    atom->SetData(color);

  } // for atom

  // Save molecule as svg file
  std::ofstream ofs(outfile.c_str(), std::fstream::out);
  ob::OBConversion oconv;
  oconv.SetOutStream(&ofs);

  ob::SVGFormatExt svg;
  if (!oconv.SetOutFormat(&svg))
  {
    std::cerr << "Fail setting SVG out format." << std::endl;
    return 1;
  }

  // Adds output options

  // number of rows in table
  oconv.AddOption("r", ob::OBConversion::OUTOPTIONS, "1");

  // number of columns in table
  oconv.AddOption("c", ob::OBConversion::OUTOPTIONS, "1");

  // show all carbon atoms
  oconv.AddOption("a");

  // show atom custom labels
  oconv.AddOption("L");

  oconv.AddChemObject(molecule);
  svg.WriteChemObject(&oconv);

  /*
  if (oconv.Write(molecule))
    std::cout << "Success write on " << outfile << "." << std::endl;
  delete molecule;
  */

  ofs.close();

  return 0;
} // function main


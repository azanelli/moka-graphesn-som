#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <openbabel/atom.h>
#include <openbabel/mol.h>
#include <openbabel/obconversion.h>
#include <openbabel/obiter.h>
#include <openbabel_ext/formats/svgformatext.h>

// Namespaces
namespace balg = boost::algorithm;
namespace bfs  = boost::filesystem;
namespace bpo  = boost::program_options;
namespace ob   = OpenBabel;

// Functions
bool parseParameters(int argc, char *argv[]);
bool readIntancesInfoFile();
void applyInstancesFilter();
bool addMoleculeLabelsAndInfo(ob::OBMol *molecule);
bool writeMultipleSvgFiles();
bool writeSingleSvgFile();

// Structures
struct InstanceInfo
{
    std::string instance_output;
    double predicted_output;
    double bias;
    std::vector<std::string> winner_units;
    std::vector<double> weights;
    std::vector<std::string> collision_markers;
    bool filtered;
};

// Objects
std::string instances_info_file;
std::string sdf_input_file;
std::string sdf_id_name;
std::string svg_output_file;
std::string filter_winner_unit;
bool write_multiple_files;
size_t max_molecules;
bool verbose;
std::map<std::string, InstanceInfo> instances_info_map;
std::list<ob::OBMol*> molecules_list;
unsigned int molecules_skipped;
unsigned int molecules_filtered;

/**
 * Function main
 *
 * Loads the info file and a set of molecule from the SDF file, then try to
 * create an svg file.
 */
int main(int argc, char *argv[])
{
  // Parse parameters
  if (!parseParameters(argc, argv))
    return 1;

  // Loads weights file
  if (!readIntancesInfoFile())
    return 1;
  std::cout << "Instances info loaded." << std::endl;

  // Filters instances
  applyInstancesFilter();

  // Loads SDF file
  std::ifstream ifs(sdf_input_file.c_str(), std::ifstream::in);

  if (ifs.fail())
  {
    std::cerr << "Fail opening file " << sdf_input_file << std::endl;
    return 1;
  }

  ob::OBConversion iconv;
  iconv.SetInFormat("sdf");
  molecules_skipped = 0;
  molecules_filtered = 0;
  while (ifs.good())
  {
    // note: molecule will be shadow deleted later (during file writing)
    ob::OBMol *molecule = new ob::OBMol();
    if (iconv.Read(molecule, &ifs))
    {
      if (addMoleculeLabelsAndInfo(molecule))
        molecules_list.push_back(molecule);
    }
    else if (ifs.tellg() != -1)
    {
      std::cout << "Warning: fail reading a molecule from " << sdf_input_file
                << std::endl;
      delete molecule;
    } // else-if
  } // while
  std::cout << "SDF file loaded. ";
  std::cout << "Molecules skipped: " << molecules_skipped << ". ";
  std::cout << "Excluded by filter: " << molecules_filtered;
  std::cout << std::endl;
  ifs.close();

  // If a max number is setted remove excess molecules
  if (max_molecules > 0 && max_molecules < molecules_list.size())
  {
    std::list<ob::OBMol*>::iterator it = molecules_list.begin();
    for (size_t i = 0; i < max_molecules; ++i)
      ++it;
    while (it != molecules_list.end())
      molecules_list.erase(it++);
  }

  // Writes SVG file(s)
  if (write_multiple_files || molecules_list.size() == 1)
    writeMultipleSvgFiles();
  else
    writeSingleSvgFile();
  std::cout << "SVG file(s) written: " << molecules_list.size() << " molecules."
            << std::endl;

  return 0;
} // function main

/**
 * Function parseParameters
 *
 * Parse command line parameters.
 */
bool parseParameters(int argc, char *argv[])
{
  // Default values
  svg_output_file = "";
  max_molecules = 0;

  // Parse parameters
  bpo::variables_map opt;
  bpo::options_description optdesc(
      "Usage: " + std::string(argv[0]) + " OPTIONS");
  try {
    optdesc.add_options()
        ("help,h",
         "Print help message")
        ("sdf-file,s",
         bpo::value<std::string>(&sdf_input_file)->required(),
         "SDF file from which load molecules.")
        ("instances-info,i",
         bpo::value<std::string>(&instances_info_file)->required(),
         "File containing instances info.")
        ("id-name,n",
         bpo::value<std::string>(&sdf_id_name)->required(),
         "Name of the ID field on SDF file.")
        ("output,o",
         bpo::value<std::string>(&svg_output_file)->required(),
         "Output SVG file(s). In case of multiple file a prefix (with the "
         "molecule ID or name) will be added before the extension.")
        ("filter,f",
         bpo::value<std::string>(&filter_winner_unit),
         "You can specify here a cluster index, by a couple of integer comma "
         "separated (e.g: 1,2), if you want filter output molecule to only "
         "those containing such cluster.")
        ("multiple,m",
         "Write a separate SVG file for each molecule (default no).")
        ("max,M",
         bpo::value<size_t>(&max_molecules),
         "The max number of molecules to print out.")
        ("verbose,v",
         "Enable verbose mode.");

    bpo::store(bpo::parse_command_line(argc, argv, optdesc), opt);

    // Check for help request
    if (opt.count("help"))
    {
      std::cout << optdesc << "\n";
      return false;
    }

    bpo::notify(opt);

    // Check and set other values
    write_multiple_files = opt.count("multiple");
    verbose = opt.count("verbose");

    if (!filter_winner_unit.empty())
    {
      balg::trim(filter_winner_unit);
      std::vector<std::string> filter_wu_split;
      balg::split(filter_wu_split, filter_winner_unit, balg::is_any_of(","));
      if ((filter_wu_split.size() != 2) ||
          (atoi(filter_wu_split[0].c_str()) < 0) ||
          (atoi(filter_wu_split[1].c_str()) < 0) )
      {
        std::cout << "Error: invalid filter " << filter_winner_unit << "\n\n";
        std::cout << optdesc << std::endl;
        return false;
      }
    } // if filter_cluster

  } // try
  catch (std::exception& ex)
  {
    std::cerr << "Error: " << ex.what() << std::endl << std::endl
              << optdesc << "\n";
    return false;
  } // try-catch

  return true;
} // function parseParameters

/**
 * Function readIntancesInfoFile
 *
 * Loads info from <instances_info_file> in the following format:
 *
 *   <instance ID>
 *   <instance output>
 *   <bias weight>
 *   <N>
 *   <winner unit 1> <weight 1> [<*>]
 *   <winner unit 2> <weight 2> [<*>]
 *   ...
 *   <winner unit 3> <weight N> [<*>]
 *   <predicted output>
 *
 * and put them into data structure <instances_info_map>
 */
bool readIntancesInfoFile()
{
  std::ifstream ifs(instances_info_file.c_str(), std::ifstream::in);

  if (ifs.fail())
  {
    std::cerr << "Fail opening file " << instances_info_file << std::endl;
    return false;
  }

  std::string id;
  std::pair<std::map<std::string, InstanceInfo>::iterator, bool> ret;
  while (ifs.good()) try
  {
    std::getline(ifs, id);
    balg::trim(id);
    if (id.empty())
      continue;

    ret = instances_info_map.insert(std::make_pair(id, InstanceInfo()));

    if (!ret.second)
      std::cout << "Warning: duplicate element with ID " << id << ": will be"
                << "overwritten." << std::endl;

    ifs >> ret.first->second.instance_output;
    balg::trim(ret.first->second.instance_output);
    ifs >> ret.first->second.bias;

    size_t N = 0;
    ifs >> N;
    for (size_t i = 0; i < N; ++i)
    {
      std::string vertex_info;
      while (vertex_info.empty())
        std::getline(ifs, vertex_info);

      std::vector<std::string> vertex_info_split;
      balg::split(vertex_info_split, vertex_info, balg::is_any_of("\t "),
                  balg::token_compress_on);

      if (vertex_info_split.size() < 2 || vertex_info_split.size() > 3)
        throw std::logic_error("error reading instance info");

      ret.first->second.winner_units.push_back(vertex_info_split[0]);
      ret.first->second.weights.push_back(atof(vertex_info_split[1].c_str()));
      if (vertex_info_split.size() == 3)
        ret.first->second.collision_markers.push_back(vertex_info_split[2]);
      else
        ret.first->second.collision_markers.push_back("");
    } // for i

    ifs >> ret.first->second.predicted_output;
    ret.first->second.filtered = false;

  } // while-try

  catch (std::exception& ex)
  {
    std::cerr << "Error: an error occured parsing file " << instances_info_file
              << " on entry with id " << id << ": " << ex.what() << std::endl;
    return false;
  } // try-catch

  return true;
} // function readIntancesInfoFile

/**
 * Function applyInstancesFilter
 *
 */
void applyInstancesFilter()
{
  if (filter_winner_unit.empty())
    return;

  std::map<std::string, InstanceInfo>::iterator it;
  for (it = instances_info_map.begin(); it != instances_info_map.end(); ++it)
  {
    std::vector<std::string>::iterator jt = std::find
        (
          it->second.winner_units.begin(),
          it->second.winner_units.end(),
          filter_winner_unit
        );

    if (jt == it->second.winner_units.end())
      it->second.filtered = true;

  } // for

  return;
} // function applyInstancesFilter

/**
 * Function addMoleculeLabelsAndInfo
 *
 */
bool addMoleculeLabelsAndInfo(ob::OBMol *molecule)
{
  // Gets molecule ID
  if (!molecule->GetData(sdf_id_name))
  {
    std::cerr << "Error: fail trying to get molecule ID (" << sdf_id_name
              << ")." << std::endl;
    molecules_skipped++;
    return false;
  }
  std::string mol_id = molecule->GetData(sdf_id_name)->GetValue();

  // Search for molecule
  std::map<std::string, InstanceInfo>::iterator it;
  it = instances_info_map.find(mol_id);
  if (it == instances_info_map.end())
  {
    std::cout << "Warning: infos not found for molecule with ID "
              << mol_id << " (skipped)." << std::endl;
    molecules_skipped++;
    return false;
  }
  if (it->second.filtered)
  {
    if (verbose)
      std::cout << "Filtered molecule " << mol_id << "." << std::endl;
    molecules_filtered++;
    return false;
  }

  // Add Infos
  std::stringstream info_ss;

  // Molecule ID
  ob::OBPairData *mol_id_data = new ob::OBPairData();
  mol_id_data->SetOrigin(ob::userInput); // set by user, not by Open Babel
  mol_id_data->SetAttribute("InfoID");
  mol_id_data->SetValue(mol_id);
  molecule->SetData(mol_id_data);

  // Prediction
  info_ss.str("");
  if (it->second.weights.size() == 0)
    info_ss << "Prediction (WARNING NO WEIGHTS):";
  else
    info_ss << "Prediction: ";
  if (it->second.predicted_output >= 0)
    info_ss << "Toxic";
  else
    info_ss << "Non Toxic";
  ob::OBPairData *tox_data = new ob::OBPairData();
  tox_data->SetOrigin(ob::userInput);
  tox_data->SetAttribute("InfoTox");
  tox_data->SetValue(info_ss.str());
  molecule->SetData(tox_data);

  // Bias
  info_ss.str("");
  info_ss << "Bias = " << std::setprecision(2) << it->second.bias;
  ob::OBPairData *bias_data = new ob::OBPairData();
  bias_data->SetOrigin(ob::userInput);
  bias_data->SetAttribute("InfoBias");
  bias_data->SetValue(info_ss.str());
  molecule->SetData(bias_data);

  // Predicted Value
  info_ss.str("");
  info_ss << "Predicted value = sign(" << std::setprecision(2)
          << it->second.predicted_output << ") = "
          << (it->second.predicted_output >= 0 ? "+1" : "-1");
  ob::OBPairData *pvalue_data = new ob::OBPairData();
  pvalue_data->SetOrigin(ob::userInput);
  pvalue_data->SetAttribute("InfoPValue");
  pvalue_data->SetValue(info_ss.str());
  molecule->SetData(pvalue_data);

  // Target output
  info_ss.str("");
  if (it->second.instance_output != "*") // no target output
  {
    info_ss << "Target value = " << it->second.instance_output;
    ob::OBPairData *tvalue_data = new ob::OBPairData();
    tvalue_data->SetOrigin(ob::userInput);
    tvalue_data->SetAttribute("InfoTValue");
    tvalue_data->SetValue(info_ss.str());
    molecule->SetData(tvalue_data);
  }

  // Add ID on title
  //  if (!molecule->GetTitle() || std::string(molecule->GetTitle()).empty())
  //    molecule->SetTitle(mol_id);
  //  else
  //  {
  //    std::stringstream title_ss;
  //    title_ss << molecule->GetTitle() << " (" << mol_id << ")";
  //    molecule->SetTitle(title_ss.str().c_str());
  //  }
  //  molecule->SetTitle(mol_id);

  // Add atom labels
  if (molecule->NumAtoms() != it->second.weights.size())
  {
    std::cerr << "Error: atoms and weights are different in size on molecule "
              << mol_id << std::endl;
    molecules_skipped++;
    return false;
  }
  if (it->second.weights.size() != it->second.winner_units.size())
  {
    std::cerr << "Error: wrong winner units vector size." << std::endl;
    molecules_skipped++;
    return false;
  }
  if (it->second.weights.size() != it->second.collision_markers.size())
  {
    std::cerr << "Error: wrong collision markers vector size." << std::endl;
    molecules_skipped++;
    return false;
  }
  ob::OBMolAtomIter atom(*molecule);
  for (size_t i = 0; i < it->second.weights.size(); ++atom, ++i)
  {
    // Weight label
    double weight = it->second.weights[i];
    bool shared_weight = !(it->second.collision_markers[i].empty());
    if (weight != 0)
    {
      std::stringstream weight_label;
      // std::scientific(sslbl);
      weight_label << std::setprecision(2) << weight;
      weight_label << (shared_weight ? "*" : "");

      // Label
      ob::OBPairData *weight_data = new ob::OBPairData();
      weight_data->SetOrigin(ob::userInput); // set by user, not by Open Babel
      weight_data->SetAttribute("CustomLabelText");
      weight_data->SetValue(weight_label.str());
      atom->SetData(weight_data);

      // Color
      ob::OBPairData *color_data = new ob::OBPairData();
      color_data->SetOrigin(ob::userInput);
      color_data->SetAttribute("CustomLabelColor");
      if (it->second.bias < 0)
      {
        if (weight >= fabs(it->second.bias))
          color_data->SetValue("0.92, 0.29, 0.09"); // red: rgb(234, 74, 23)
        else if (weight >= 0 && weight < fabs(it->second.bias))
          color_data->SetValue("0.92, 0.61, 0.09"); // orange: rgb(235, 157, 23)
        else // weight < 0
          color_data->SetValue("0.0, 0.71, 0.73"); // cyan: rgb(0, 180, 185)
      }
      else // it->second.bias >= 0
      {
        if (weight <= (-1.0 * it->second.bias))
          color_data->SetValue("0.43, 0.56, 0.76"); // blue rgb(110, 144, 193)
        else if (weight <= 0 && weight > (-1.0 * it->second.bias))
          color_data->SetValue("0.0, 0.71, 0.73"); // cyan: rgb(0, 180, 185)
        else // weight > 0
          color_data->SetValue("0.92, 0.61, 0.09"); // orange: rgb(235, 157, 23)
      }
      atom->SetData(color_data);
    } // if (weight != 0)

    // Winner unit label
    ob::OBPairData *wu_data = new ob::OBPairData();
    wu_data->SetOrigin(ob::userInput);
    wu_data->SetAttribute("CustomSubLabelText");
    wu_data->SetValue(it->second.winner_units[i]);
    atom->SetData(wu_data);

    // Highlight atom data
    if ((!filter_winner_unit.empty()) &&
        (it->second.winner_units[i] == filter_winner_unit) )
    {
      ob::OBPairData *highlight_data = new ob::OBPairData();
      highlight_data->SetOrigin(ob::userInput);
      highlight_data->SetAttribute("HighlightAtom");
      atom->SetData(highlight_data);
    }

  } // for i (atom)

  return true;
} // function addMoleculeLabelsAndInfo

/**
 * Function writeMultipleSvgFiles
 *
 */
bool writeMultipleSvgFiles()
{
  // Set output file name
  bfs::path svg_output_path(svg_output_file);

  if (!svg_output_path.has_extension())
    svg_output_path.replace_extension(".svg");

  // Writes one by one each molecule
  size_t list_size = molecules_list.size();
  std::list<ob::OBMol*>::iterator mol = molecules_list.begin();

  if (verbose)
    std::cout << "Start write " << list_size << " molecules." << std::endl;

  for (size_t i = 0; i < list_size; ++mol, ++i)
  {
    ob::OBConversion oconv;
    ob::SVGFormatExt svg;
    if (!oconv.SetOutFormat(&svg))
    {
      std::cerr << "Fail setting SVG out format." << std::endl;
      return false;
    }

    // number of rows in table
    oconv.AddOption("r", ob::OBConversion::OUTOPTIONS, "1");

    // number of columns in table
    oconv.AddOption("c", ob::OBConversion::OUTOPTIONS, "1");

    // show all carbon atoms
    oconv.AddOption("a");

    // show atom custom labels
    oconv.AddOption("L");

    // build file name
    std::string svg_file_name =
        (svg_output_path.parent_path() /=
            (svg_output_path.stem().string() + "_" +
             (*mol)->GetData("InfoID")->GetValue() +
             svg_output_path.extension().string())
        ).string();

    std::ofstream ofs(svg_file_name.c_str(), std::fstream::out);
    oconv.SetOutStream(&ofs);
    oconv.AddChemObject(*mol);
    svg.WriteChemObject(&oconv);

    ofs.close();

    if (verbose)
      std::cout << "Written molecule " << i + 1 << " of " << list_size << "."
                << std::endl;
  } // for mol

  return true;
} // function writeMultipleSvgFiles

/**
 * Function writeSingleSvgFile
 *
 */
bool writeSingleSvgFile()
{
  ob::OBConversion oconv;
  ob::SVGFormatExt svg;
  if (!oconv.SetOutFormat(&svg))
  {
    std::cerr << "Fail setting SVG out format." << std::endl;
    return false;
  }

  // show all carbon atoms
  oconv.AddOption("a");

  // show atom custom labels
  oconv.AddOption("L");

  std::ofstream ofs(svg_output_file.c_str(), std::fstream::out);
  oconv.SetOutStream(&ofs);

  size_t list_size = molecules_list.size();
  std::list<ob::OBMol*>::iterator mol = molecules_list.begin();
  for (size_t i = 0; i < molecules_list.size() - 1; ++mol, ++i)
  {
    oconv.AddChemObject(*mol);
    oconv.SetLast(false);
    svg.WriteChemObject(&oconv);
  } // for mol

  if (verbose)
    std::cout << "Start write " << list_size << " molecules." << std::endl;

  oconv.AddChemObject(*mol);
  oconv.SetLast(true);
  svg.WriteChemObject(&oconv);

  ofs.close();

  return true;
} // function writeSingleSvgFile

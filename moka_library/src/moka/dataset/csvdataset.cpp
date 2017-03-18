#include "csvdataset.h"

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/tokenizer.hpp>
#include <moka/exception.h>
#include <moka/global.h>
#include <moka/log.h>

namespace moka {
namespace dataset {

namespace bt   = ::boost;
namespace balg = ::boost::algorithm;
namespace bfs  = ::boost::filesystem;
namespace bll  = ::boost::lambda;

/**
 * Constructor
 */
CsvDataset::CsvDataset() { }

/**
 * Destructor
 */
CsvDataset::~CsvDataset() { }

// ==============
// PUBLIC METHODS
// ==============

/**
 * Method cloneTrSet
 */
Dataset* CsvDataset::cloneTrSet()
{
  CsvDataset *csvds = new CsvDataset();
  fillWithTrInstances(*csvds);
  return csvds;
} // method cloneTrSet

/**
 * Method read
 *
 * See Dataset::read.
 *
 * Loads from a csv file (written through the method write) the dataset
 * instances. In the first line there must be input and output sizes.
 */
void CsvDataset::read(std::istream& is)
{
  std::string line;

  std::getline(is, line);
  balg::trim(line);

  std::vector<std::string> sline;
  balg::split(sline, line, balg::is_any_of(" "), balg::token_compress_on);

  if (line[0] == '#' && sline.size() >= 3)
  {
    Uint ninputs = moka::Global::toUint(sline[1]);
    Uint noutputs = moka::Global::toUint(sline[2]);

    readFromCsv(is, ninputs, noutputs);
  }
  else
    throw moka::GenericException(
        "CsvDataset::read: error: unable to get input and output sizes.");

  return;
} // method read

/**
 * Method load
 *
 * See Dataset::load.
 *
 * Loads from a csv file the dataset instances. The instances in the file
 * must be one per-line and in the following form:
 *    id, x1, ..., xn, y1, ..., ym
 * where id is a unique instance identifier, x are the n inputs and y are the
 * m outputs.
 *
 * Lines starting with a '#' character will be skipped (considered as comments).
 *
 * The method takes the following parameters:
 *   - <csv-file>: file name (.csv) on which to load the dataset.
 *   - <no-inputs>: number of inputs for each instance (n).
 *   - <no-outputs>: number of outputs for each instance (m).
 */
void CsvDataset::load(const util::Parameters& params)
{
  // Parse parameters
  std::string csvfile = params.get("csv-file");
  Uint ninputs = params.getUint("no-inputs");
  Uint noutputs = params.getUint("no-outputs");

  // Open the csv file
  std::ifstream file(csvfile.c_str());
  if (!file.is_open())
    throw moka::GenericException(
        "CsvDataset::loadCSVFile: can't open the file " + csvfile);

  readFromCsv(file, ninputs, noutputs);

  return;
} // method loadCSVFile

/**
 * Method write
 *
 * Writes dataset entries on the output stream (probably a file stream). You
 * can reload the dataset using the method read or the method load.
 */
void CsvDataset::write(std::ostream& os) const
{
  if (this->isEmpty())
    return;

  Uint input_size = this->at(0).getInput().size();
  Uint output_size = this->at(0).getOutput().size();

  // Writes the first line with input and output dimensions as a comment
  os << "# " << input_size << " " << output_size << std::endl;

  // Put entries on output stream
  for (Uint i = 0; i < this->getSize(); ++i)
  {
    os << this->at(i).getId();

    for (size_t j = 0; j < input_size; ++j)
      os << ", " << this->at(i).getInput().at(j);

    for (size_t k = 0; k < output_size; ++k)
      os << ", " << this->at(i).getOutput().at(k);

    os << std::endl;

  } // for i

  return;
} // method write

// ===============
// PRIVATE METHODS
// ===============

/**
 * Method readFromCsv
 *
 * Reads the instances (and fill the dataset) from a CSV formatted input
 * stream.
 *
 * The instances in the file must be one per-line and in the following form:
 *    id, x1, ..., xn, y1, ..., ym
 * where id is a unique instance identifier, x are the n inputs and y are the
 * m outputs.
 *
 * Lines starting with a '#' character will be skipped (considered as comments).
 *
 * If a line has wrong dimension (not ninputs + noutputs + 1) such line will
 * be skipped and an error message will be printed on Log::err.
 */
void CsvDataset::readFromCsv(std::istream& is, Uint ninputs, Uint noutputs)
{
  // Start to read the file and fill the dataset
  this->clearDataset();
  std::string line;
  while (is.good())
  {
    std::getline(is, line);
    balg::trim(line);
    if (!line.empty() && line[0] != '#')
    {
      std::vector<std::string> csvline;
      bt::tokenizer<bt::escaped_list_separator<char> > tok(line);
      bt::tokenizer<bt::escaped_list_separator<char> >::iterator tokit;

      for (tokit = tok.begin(); tokit != tok.end(); ++tokit)
        csvline.push_back(*tokit);

      if (ninputs + noutputs + 1 == csvline.size())
      {
        Instance *newinstance = new Instance();
        balg::trim(csvline[0]);
        newinstance->getId() = csvline[0];
        for (uint i = 0; i < ninputs; ++i)
          newinstance->getInput().push_back(Global::toReal(csvline[i+1]));
        for (uint i = 0; i < noutputs; ++i)
          newinstance->getOutput().push_back(
              Global::toReal(csvline[i+1+ninputs]));
        this->pushBackInstance(newinstance);
      } // if
      else
        Log::err << "CsvDataset::readFromCsv: wrong line dimension: skipped "
                 << "line (instance)" << Log::endl;

    } // if (!line.empty())

  } // while (file.good())

  this->endLoadInstances();

  return;
} // method readFromCsv

} // namespace dataset
} // namespace moka

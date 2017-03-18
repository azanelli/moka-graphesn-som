#include "multilabeledgraph.h"

#include <moka/exception.h>
#include <moka/global.h>
#include <moka/log.h>
#include <moka/util/math.h>

namespace moka {
namespace structure {

namespace mut = ::moka::util;

// ==============
// PUBLIC METHODS
// ==============

/**
 * Method addLabelName
 *
 * Add the passed label name in the end of the current label names list.
 */
void MultiLabeledGraph::addLabelName(const std::string& name)
{
  m_label_names.push_back(name);
  return;
} // method addLabelName

/**
 * Method buildFrom
 *
 * See Graph::buildFrom.
 */
MultiLabeledGraph& MultiLabeledGraph::buildFrom
(
    const MultiLabeledGraph& g,
    const Vector& elem
)
{
  BaseClass::buildFrom(g, elem);

  // Also copy label names from g
  m_label_names = g.m_label_names;

  return (*this);
} // method buildFrom

/**
 * Method getLabelName
 *
 * Return a const reference to the i-th label name.
 */
const std::string& MultiLabeledGraph::getLabelName(size_t i) const
{
  if (i >= m_label_names.size())
    throw moka::GenericException(
        "MultiLabeledGraph::getLabelName: out of range");
  return m_label_names[i];
} // method getLabelName

/**
 * Method getLabelName
 *
 * Return a reference to the i-th label name.
 */
std::string& MultiLabeledGraph::getLabelName(size_t i)
{
  if (i >= m_label_names.size())
    throw moka::GenericException(
        "MultiLabeledGraph::getLabelName: out of range");
  return m_label_names[i];
} // method getLabelName

/**
 * Method getLabelNameSize
 *
 * Return the current number of label names.
 */
Uint MultiLabeledGraph::getLabelNameSize() const
{
  return m_label_names.size();
} // method getLabelNameSize

/**
 * Method read
 *
 * Reads graph from input stream.
 */
bool MultiLabeledGraph::read(std::istream& is)
{
  this->clear();

  std::vector<std::string> *item = NULL;
  Vector *element = NULL;
  std::vector<Uint> *neighs = NULL;

  try
  {
    Uint size = Global::readLine<Uint>(is);

    for (Uint i = 0; i < size; ++i)
    {
      item = new std::vector<std::string>();
      element = new Vector();
      neighs = new std::vector<Uint>();

      Global::readLines(is, *item);
      mut::Math::read(is, *element);
      Global::readLines(is, *neighs);

      this->pushBack(*item, element, neighs);
      delete item;

    } // for

    Global::readLines(is, m_label_names);

  } // try
  catch (std::exception& ex)
  {
    Log::err << "MultiLabeledGraph::read: Error reading graph: " << ex.what()
             << " (the graph will be left empty)." << Log::endl;
    delete item;
    delete element;
    delete neighs;
    this->clear();
    return false;
  } // try-catch

  if (!this->checkConsistence())
  {
    Log::err << "MultiLabeledGraph::read: Error the readed graph is "
             << "inconsistence (the graph will be left empty)." << Log::endl;
    this->clear();
    return false;
  } // if

  return true;
} // method read

/**
 * Method setLabelName
 *
 * Set the name for the i-th label.
 */
void MultiLabeledGraph::setLabelName(size_t i, const std::string& name)
{
  if (i >= m_label_names.size())
    m_label_names.resize(i + 1, "");
  m_label_names[i] = name;
  return;
} // method setLabelName

/**
 * Method write
 *
 * Writes graph on output stream.
 */
void MultiLabeledGraph::write(std::ostream& os) const
{
  os << this->getSize() << std::endl;

  Vertices::const_iterator vit;
  for (vit = this->getVertexBegin(); vit != this->getVertexEnd(); ++vit)
  {
    Global::write(os, vit->getItem());
    mut::Math::write(os, vit->getElement());
    Global::write(os, vit->getNeighbors());
  } // for

  // Label names
  Global::write(os, m_label_names);

  return;
} // method write

} // namespace structure
} // namespace moka

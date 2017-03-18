#ifndef MOKA_STRUCTURE_GRAPH_IMPL_H
#define MOKA_STRUCTURE_GRAPH_IMPL_H

#include "graph.h"

#include <moka/exception.h>

namespace moka {
namespace structure {

typedef Global::Uint Uint;
typedef Global::Real Real;
typedef util::Math::Vector Vector;

/**
 * Constructor
 */
template <typename T>
Graph<T>::Graph() :
  m_elements_size(0)
{ }

/**
 * Constructor
 *
 * Sets the size of the elements associated with the verteces of the graph.
 * The size must be greater than 0. If you set the size to 0 the behavior is the
 * same as the default constructor, i.e. the size will be deduced in the first
 * vertex adding.
 */
template <typename T>
Graph<T>::Graph(Uint elements_size) :
  m_elements_size(elements_size)
{ }

/**
 * Copy constructor
 */
template <typename T>
Graph<T>::Graph(const Graph<T>& g) :
  m_elements_size(0)
{
  this->operator=(g);
} // copy constructor

/**
 * Destructor
 */
template <typename T>
Graph<T>::~Graph()
{ }

// ==============
// PUBLIC METHODS
// ==============

/**
 * Method buildFrom
 *
 * Clears all the verteces of this graph and builds a new graph by copying the
 * structure from the graph g and inits the elements with the passed element.
 * The results is a graph with the same structure of the passed graph g (and
 * same labels) and with verteces elements all equals to the argument elem.
 */
template <typename T>
Graph<T>& Graph<T>::buildFrom(const Graph<T>& g, const Vector& elem)
{
  m_vertices.clear();
  m_vertices.reserve(g.m_vertices.size());
  typename Vertices::const_iterator g_it = g.m_vertices.begin();
  for (/* nop */; g_it != g.m_vertices.end(); ++g_it)
  {
    m_vertices.push_back(Vertex());
    m_vertices.back().setItem(g_it->getItem());
    m_vertices.back().setElement(elem);
    m_vertices.back().setNeighbors(g_it->getNeighbors());
  }
  m_elements_size = elem.n_rows;
  return (*this);
} // method buildFrom

/**
 * Method checkConsistence
 *
 * Checks the consistence of this graph. A graph is considered consistence if
 * the list of neighbors of each vertex contains indexes of verteces that are
 * present into the graph. Returns true if the graph is consistence, false
 * otherwise.
 */
template <typename T>
bool Graph<T>::checkConsistence() const
{
  Uint graph_size = m_vertices.size();
  typename Vertices::const_iterator v_it = m_vertices.begin();
  for (/* nop */; v_it != m_vertices.end(); ++v_it)
  {
    typename Vertex::Neighbors::const_iterator neigh_it =
        v_it->getNeighbors().begin();
    for (/* nop */; neigh_it != v_it->getNeighbors().end(); ++neigh_it)
      if ((*neigh_it) >= graph_size)
        return false;
  } // for g_it

  return true;
} // method checkConsistence

/**
 * Method clear
 */
template <typename T>
inline void Graph<T>::clear()
{
  m_elements_size = 0;
  m_vertices.clear();
  return;
} // method clear

/**
 * Method getElementSize
 *
 * Returns the size of the verteces elements. If the size is not already setted,
 * e.g. because the graph is empty, 0 is returned.
 */
template <typename T>
inline Uint Graph<T>::getElementsSize() const
{
  return m_elements_size;
} // method getElementSize

/**
 * Method getNeighbor
 *
 * Returns a const reference to the neigh-th neighbor of the vertex i.
 *   - i     : vertex index (see getVertex).
 *   - neigh : neighbor index (see getNeighborsSize).
 * If the indexes are out of range, an exception of type moka::GenericException
 * will be thrown.
 */
template <typename T>
inline const Vector& Graph<T>::getNeighbor(Uint i, Uint neigh) const
{
  if (i >= this->getSize() || neigh >= this->getNeighborsSize(i))
    throw moka::GenericException("Graph::getNeighbor: indexes out of range.");
  return getVertexElement(m_vertices[i].getNeighbors().at(neigh));
} // method getNeighbor const

/**
 * Method getNeighbors
 *
 * Returns a const reference to a vector with the neighbors indexes of the
 * i-th vertex. The index i must be less than the number of verteces of the
 * graph (see getSize), otherwise an exception of type moka::GenericException
 * will be thrown.
 */
template <typename T>
inline const std::vector<Uint>& Graph<T>::getNeighbors(Uint i) const
{
  if (i >= getSize())
    throw moka::GenericException("Graph::getNeighbors: index out of range.");
  return m_vertices[i].getNeighbors();
} // method getNeighbors

/**
 * Method getNeighborsSize
 *
 * Returns the number of neighbors of the vertex i. The index i must be less
 * than the number of verteces of the graph (see getSize), otherwise an
 * exception will be thrown.
 */
template <typename T>
inline Uint Graph<T>::getNeighborsSize(Uint i) const
{
  if (i >= getSize())
    throw std::out_of_range("Graph::getNeighborsSize: index out of range.");
  return m_vertices[i].getNeighbors().size();
} // method getNeighborsSize

/**
 * Method getSize
 *
 * Returns the number of verteces into the graph.
 */
template <typename T>
inline Uint Graph<T>::getSize() const
{
  return m_vertices.size();
} // method getSize

/**
 * Method getVertexBegin
 *
 * Returns an iterator to first vertex.
 */
template <typename T>
inline typename Graph<T>::Vertices::iterator Graph<T>::getVertexBegin()
{
  return m_vertices.begin();
} // method getVertexBegin

/**
 * Method getVertexBegin
 *
 * Returns a const iterator to first vertex.
 */
template <typename T>
inline typename Graph<T>::Vertices::const_iterator
Graph<T>::getVertexBegin() const
{
  return m_vertices.begin();
} // method getVertexBegin

/**
 * Method getVertexElement
 *
 * Returns a const reference to the element of the vertex v.
 * The index i must be less than the number of verteces of the graph (see
 * getSize), otherwise an exception of type moka::GenericException will be
 * thrown.
 */
template <typename T>
inline const Vector& Graph<T>::getVertexElement(Uint v) const
{
  if (v >= getSize())
    throw moka::GenericException("Graph::getVertexElement: index out of "
        "range.");
  return m_vertices[v].getElement();
} // method getVertexElement

/**
 * Method getVertexElementAt
 *
 * Returns a reference (writable) to the i-th field of the element of the
 * vertex specified by the argument v.
 * The index v must be less than the number of verteces of the graph (see
 * getSize) and the index i must be less the size of the elements (see
 * getElementSize), otherwise an exception of type moka::GenericException will
 * be thrown.
 */
template <typename T>
Real& Graph<T>::getVertexElementAt(Uint v, Uint i)
{
  if (v >= getSize())
    throw moka::GenericException("Graph::getVertex: vertex index out of "
        "range.");
  if (i >= getElementsSize())
    throw moka::GenericException("Graph::getVertex: element index out of "
        "range.");
  return m_vertices[v].getElement().at(i);
} // method getVertexElementAt

/**
 * Method getVertexEnd
 *
 * Returns an iterator referring to the past-the-end vertex.
 */
template <typename T>
inline typename Graph<T>::Vertices::iterator Graph<T>::getVertexEnd()
{
  return m_vertices.end();
} // method getVertexEnd

/**
 * Method getVertexEnd
 *
 * Returns a const iterator referring to the past-the-end vertex.
 */
template <typename T>
inline
typename Graph<T>::Vertices::const_iterator Graph<T>::getVertexEnd() const
{
  return m_vertices.end();
} // method getVertexEnd

/**
 * Method getVertexItem
 *
 * Returns a const reference to the item of the vertex v.
 * The index v must be less than the number of verteces of the graph (see
 * getSize), otherwise an exception of type moka::GenericException will be
 * thrown.
 */
template <typename ItemTemplate>
inline
const ItemTemplate& Graph<ItemTemplate>::getVertexItem(Uint v) const
{
  if (v >= getSize())
    throw moka::GenericException("Graph::getVertexItem: index out of range.");
  return m_vertices[v].getItem();
} // method getVertexItem

/**
 * Method maxDegree
 *
 * Computes and returns the maximum degree (maximum number of neighbors) over
 * the verteces of this graph. The cost of this operation is linear in the
 * number of verteces.
 */
template <typename T>
Uint Graph<T>::maxDegree() const
{
  Uint maxd = 0;
  typename Vertices::const_iterator v_it = m_vertices.begin();
  for (/* nop */; v_it != m_vertices.end(); ++v_it)
    if (v_it->getNeighbors().size() > maxd)
      maxd = v_it->getNeighbors().size();
  return maxd;
} // method maxDegree

/**
 * operator =
 */
template <typename T>
Graph<T>& Graph<T>::operator=(const Graph<T>& g)
{
  m_vertices.clear();
  m_vertices.reserve(g.m_vertices.size());
  typename Vertices::const_iterator v_it = g.m_vertices.begin();
  for (/* nop */; v_it != g.m_vertices.end(); ++v_it)
    m_vertices.push_back(Vertex(*v_it));
  m_elements_size = g.m_elements_size;
  return (*this);
} // operator=

/**
 * Method print
 *
 * Prints a description of the object to the std::ostream <os>. To use mainly
 * for debug purpose.
 */
template <typename T>
void Graph<T>::print(std::ostream& os) const
{
  for (Uint i = 0; i < getSize(); ++i)
  {
    if (i != 0)
      os << std::endl;
    os << "\t[" << i << "]\t";
    // os << "\t[" << getVertexItem(i) << "]\t";
    os << "(" << getVertexElement(i).t() << ") -> ";
    os << Global::toString(getNeighbors(i));
  } // for
  return;
} // method toString

/**
 * Method pushBack
 *
 * Adds a vertex along with its list of neighbors at the end of the list of
 * verteces. As element of the vertex is taken the parameter element and as
 * list of neighbors is taken the parameter neighs or is left empty (the
 * default) if neighs is NULL. If the parameter element is NULL an exception of
 * type moka::GenericException is thrown. If the vertex is the first into the
 * graph and the element size is not already setted the size will set as the
 * size of the argument vector element. If the element size is already setted
 * then the size of the passed element must be of such size otherwise an
 * exception of type moka::GenericException will be thrown.
 * Note that is possible to add a list of neighbors that contains indexes of
 * verteces which are not yet presents in the graph, this leads to the
 * possibility of building, with this method, an inconsistent graph. To check
 * the consistence of the graph you can use the method checkConsistence, or
 * you can fix all the inconsistences through the method consolidateGraph.
 */
template <typename ItemTemplate>
void Graph<ItemTemplate>::pushBack
(
    const ItemTemplate& item,
    Vector *element,
    std::vector<Uint> *neighs
)
{
  if (!element)
    throw moka::GenericException("Graph::pushBack: element is a NULL pointer.");

  if (m_elements_size == 0 && m_vertices.empty())
    m_elements_size = element->n_rows;

  if (element->n_rows != m_elements_size)
    throw moka::GenericException("Graph::pushBack: wrong element size.");

  m_vertices.push_back(Vertex());
  m_vertices.back().setItem(item);
  m_vertices.back().setElement(element);
  m_vertices.back().setNeighbors(neighs);

  return;
} // method pushBack

/**
 * Method setVertex
 *
 * Sets the vertex i. The element of the vertex i is replaced by the parameter
 * element and the list of its neighbors is replaced by the parameter neighs or
 * left empty (the default) if neighs is NULL. If the vertex i doesn't exists
 * or if the element size is wrong (different from the one set) an exception
 * of type moka::GenericException will be thrown.
 * Note that is possible to add a list of neighbors that contains indexes of
 * verteces which are not yet presents in the graph, this leads to the
 * possibility of building, with this method, an inconsistent graph. To check
 * the consistence of the graph you can use the method checkConsistence, or
 * you can fix all the inconsistences through the method consolidateGraph.
 */
template <typename ItemTemplate>
void Graph<ItemTemplate>::setVertex
(
    Uint i,
    const ItemTemplate& item,
    Vector *element,
    std::vector<Uint> *neighs
)
{
  if (i >= this->getSize())
    throw moka::GenericException("Graph::setVertex: index out of range.");

  if (element == NULL)
    throw moka::GenericException("Graph::setVertex: element is a NULL "
        "pointer.");

  if (element->n_rows != m_elements_size)
    throw moka::GenericException("Graph::setVertex: wrong element size.");

  m_vertices[i].setItem(item);
  m_vertices[i].setElement(element);
  m_vertices[i].setNeighbors((!neighs ? new std::vector<Uint>() : neighs));

  return;
} // method setVertex

/**
 * Method setVertexElement
 *
 * Sets the element associated to the vertex v with the passed argument value.
 * The index v must be less than the number of verteces of the graph (see
 * getSize) and the size of the element (of the argument value) must be equal to
 * that of the previous element, otherwise an exception of type
 * moka::GenericException will be thrown.
 */
template <typename T>
void Graph<T>::setVertexElement(Uint v, const Vector& value)
{
  if (v >= getSize())
    throw moka::GenericException("Graph::setVertexElement: index out of "
         "range.");

  if (value.n_rows != m_elements_size)
    throw moka::GenericException("Graph::setVertexElement: wrong element "
         "size.");

  m_vertices[v].setElement(value);
  return;
} // method setVertexElement

/**
 * Method setVertexItem
 *
 * Sets the item associated to the vertex v with the passed argument value.
 * The index v must be less than the number of verteces of the graph (see
 * getSize), otherwise an exception of type moka::GenericException will be
 * thrown.
 */
template <typename ItemTemplate>
void Graph<ItemTemplate>::setVertexItem(Uint v, const ItemTemplate& item)
{
  if (v >= getSize())
    throw moka::GenericException("Graph::setVertexItem: index out of range.");

  m_vertices[v].setItem(item);
  return;
} // method setVertexItem

} // namespace structure
} // namespace moka

#endif // MOKA_STRUCTURE_GRAPH_IMPL_H

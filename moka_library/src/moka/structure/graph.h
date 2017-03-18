#ifndef MOKA_STRUCTURE_GRAPH_H
#define MOKA_STRUCTURE_GRAPH_H

#include <ostream>
#include <utility>
#include <string>
#include <vector>
#include <moka/global.h>
#include <moka/util/math.h>

namespace moka {
namespace structure {

/**
 * Class Graph
 *
 * Represent a graph formed by a vector of verteces and, for each vertex, a
 * list of indeces of its neighbors that represent its outgoing edges. In this
 * way the directed graphs are directly represented while the undirected graph
 * are represented through a couple of directed edge for each undirected edge
 * of the graph. This type of implementation of a graph is particularly useful
 * when you must deal with the verteces and with its neighbors in an simple and
 * efficient way. Each vertex has associated an element that is a real vector
 * and a label that can be any string. Each vertex element in the graph must
 * have the same dimension that can be setted in the graph constructor or
 * automatically deduced when the first vertex is added. In addition, at each
 * vertex is associated with an additional item of type ItemTemplate (e.g. a label
 * of type string).
 */
template <typename ItemTemplate>
class Graph
{
  public:
    class Vertex;
    typedef Global::Uint Uint;
    typedef Global::Real Real;
    typedef ItemTemplate ItemType;
    typedef util::Math::Vector Vector;
    typedef std::vector<Vertex> Vertices;

    Graph();
    Graph(Uint elements_size);
    Graph(const Graph& g);
    virtual ~Graph();

    virtual Graph& buildFrom(const Graph& g, const Vector& elem);
    virtual bool checkConsistence() const;
    virtual void clear();
    virtual Uint getElementsSize() const;
    virtual const Vector& getNeighbor(Uint i, Uint neigh) const;
    virtual const std::vector<Uint>& getNeighbors(Uint i) const;
    virtual Uint getNeighborsSize(Uint i) const;
    virtual Uint getSize() const;
    virtual typename Vertices::iterator getVertexBegin();
    virtual typename Vertices::const_iterator getVertexBegin() const;
    virtual const Vector& getVertexElement(Uint v) const;
    virtual Real& getVertexElementAt(Uint v, Uint i);
    virtual typename Vertices::iterator getVertexEnd();
    virtual typename Vertices::const_iterator getVertexEnd() const;
    virtual const ItemTemplate& getVertexItem(Uint v) const;
    virtual Uint maxDegree() const;
    virtual Graph& operator=(const Graph& g);
    virtual void print(std::ostream& os) const;
    virtual void pushBack(
        const ItemTemplate& item,
        Vector *element,
        std::vector<Uint> *neighs = NULL);
    virtual void setVertex(
        Uint i,
        const ItemTemplate& item,
        Vector *element,
        std::vector<Uint> *neighs = NULL);
    virtual void setVertexElement(Uint v, const Vector& value);
    virtual void setVertexItem(Uint v, const ItemTemplate& item);

  private:
    Vertices m_vertices;
    Uint m_elements_size;

}; // class Graph

/**
 * Class Graph::Vertex
 */
template <typename ItemTemplate>
class Graph<ItemTemplate>::Vertex
{
  public:
    typedef ItemTemplate Item;
    typedef Vector Element;
    typedef std::vector<Uint> Neighbors;

    //! Constructor
    Vertex() :
      m_item(Item()),
      m_element(new Element()),
      m_neighbors(new Neighbors())
    { }

    //! Copy constructor
    Vertex(const Vertex& ge) :
      m_item(ItemTemplate()),
      m_element(new Element()),
      m_neighbors(new Neighbors())
    {
      this->operator=(ge);
    }

    //! Destructor
    ~Vertex()
    {
      delete m_element;
      delete m_neighbors;
    }

    //! Get vertex element
    Element& getElement()
    {
      return *m_element;
    }

    //! Get vertex element
    const Element& getElement() const
    {
      return *m_element;
    }

    //! Get vertex item
    Item& getItem()
    {
      return m_item;
    }

    //! Get vertex item
    const Item& getItem() const
    {
      return m_item;
    }

    //! Get vertex neighboors
    Neighbors& getNeighbors()
    {
      return *m_neighbors;
    }

    //! Get vertex neighboors
    const Neighbors& getNeighbors() const
    {
      return *m_neighbors;
    }

    //! Assigment operator
    Vertex& operator=(const Vertex& v)
    {
      m_item = v.m_item;
      *(m_element) = *(v.m_element);
      *(m_neighbors) = *(v.m_neighbors);
      return *this;
    }

    //! Set vertex element (by pointer)
    //! NOTE: this objet will delete the pointer when no longer useful.
    void setElement(Element *element)
    {
      delete m_element;
      m_element = element;
    }

    //! Set vertex element (by copy)
    void setElement(const Element& element)
    {
      *(m_element) = element;
    }

    //! Set vertex item (by copy)
    void setItem(const ItemTemplate& item)
    {
      m_item = item;
    }

    //! Set vertex neighboors (by pointer).
    //! NOTE: this objet will delete the pointer when no longer useful.
    void setNeighbors(Neighbors *neighbors)
    {
      delete m_neighbors;
      m_neighbors = neighbors;
    }

    //! Set vertex neighboors (by copy)
    void setNeighbors(const Neighbors& neighbors)
    {
      *(m_neighbors) = neighbors;
    }

  private:
    Item m_item;
    Element *m_element;
    Neighbors *m_neighbors;

}; // class Graph::Vertex

} // namespace structure
} // namespace moka

#include "graph_impl.h"

#endif // MOKA_STRUCTURE_GRAPH_H

#ifndef MOKA_STRUCTURE_MULTILABELEDGRAPH_H
#define MOKA_STRUCTURE_MULTILABELEDGRAPH_H

#include <iostream>
#include <string>
#include <vector>
#include <moka/structure/graph.h>

namespace moka {
namespace structure {

/**
 * Class MultiLabeledGraph
 *
 * Represents a graph in which each vertex is associated with a collection
 * of indexed labels.
 *
 * Each vertex should have the same number of labels associated although this
 * is not checked, but it could be done in the future, so each label can have a
 * name (to represent its content) that can be setted and retrived using the
 * methods setLabelName and getLabelName.
 */
class MultiLabeledGraph : public Graph< std::vector<std::string> >
{
  public:
    typedef Graph<ItemType> BaseClass;

    MultiLabeledGraph() { }
    virtual ~MultiLabeledGraph() { }

    virtual void addLabelName(const std::string& name);
    virtual MultiLabeledGraph& buildFrom(
        const MultiLabeledGraph& g, const Vector& elem);
    virtual const std::string& getLabelName(size_t i) const;
    virtual std::string& getLabelName(size_t i);
    virtual Uint getLabelNameSize() const;
    virtual bool read(std::istream& is);
    virtual void setLabelName(size_t i, const std::string& name);
    virtual void write(std::ostream& os) const;

  private:
    std::vector<std::string> m_label_names;

}; // class MultiLabeledGraph

} // namespace structure
} // namespace moka

#endif // MOKA_STRUCTURE_MULTILABELEDGRAPH_H

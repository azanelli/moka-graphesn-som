#ifndef MOKA_STRUCTURE_LABELEDGRAPH_H
#define MOKA_STRUCTURE_LABELEDGRAPH_H

#include <string>
#include <moka/structure/graph.h>

namespace moka {
namespace structure {

/**
 * Class LabeledGraph
 */
class LabeledGraph : public Graph<std::string>
{
  public:
    LabeledGraph() { }
    ~LabeledGraph() { }

    virtual void write(std::ostream& os) const;

}; // class LabeledGraph

} // namespace structure
} // namespace moka

#endif // MOKA_STRUCTURE_LABELEDGRAPH_H

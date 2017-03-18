#include "labeledgraph.h"

#include <moka/exception.h>
#include <moka/global.h>
#include <moka/util/math.h>

namespace moka {
namespace structure {

using namespace moka::util;

/**
 * Method write
 *
 * Writes graph on output stream.
 */
void LabeledGraph::write(std::ostream& os) const
{
  os << this->getSize() << std::endl;
  Vertices::const_iterator vit = getVertexBegin();

  for (/* nop */; vit != this->getVertexEnd(); ++vit)
  {
    os << vit->getItem() << std::endl;
    Math::write(os, vit->getElement());
    Global::write(os, vit->getNeighbors());
  } // for

  return;
} // method write

} // namespace structure
} // namespace moka

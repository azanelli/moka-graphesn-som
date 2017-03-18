#ifndef MOKA_DATASET_VECTORDATASET_H
#define MOKA_DATASET_VECTORDATASET_H

#include <vector>
#include <moka/global.h>
#include <moka/dataset/genericdataset.h>

namespace moka {
namespace dataset {

/**
 * Class VectorDataset
 */
class VectorDataset : public GenericDataset< std::vector<Global::Real> >
{
  public:
    VectorDataset() { }
    virtual ~VectorDataset() { }

    virtual Dataset* cloneTrSet() = 0;
    virtual void load(const util::Parameters& params) = 0;
    virtual void read(std::istream& os) = 0;
    virtual void write(std::ostream& os) const = 0;

}; // class VectorDataset

} // namespace dataset
} // namespace moka

#endif // MOKA_DATASET_VECTORDATASET_H

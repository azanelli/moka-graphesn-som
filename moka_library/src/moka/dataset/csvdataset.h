#ifndef MOKA_DATASET_CSVDATASET_H
#define MOKA_DATASET_CSVDATASET_H

#include <moka/dataset/dataset.h>
#include <moka/dataset/vectordataset.h>
#include <moka/global.h>
#include <moka/util/parameters.h>

namespace moka {
namespace dataset {

/**
 * Class CsvDataset
 */
class CsvDataset : public VectorDataset
{
  public:
    CsvDataset();
    virtual ~CsvDataset();

    virtual Dataset* cloneTrSet();
    virtual void load(const util::Parameters& params);
    virtual void read(std::istream& is);
    virtual void write(std::ostream& os) const;

  private:
    void readFromCsv(std::istream& is, Uint ninputs, Uint noutputs);

}; // class CsvDataset

} // namespace dataset
} // namespace moka

#endif // MOKA_DATASET_CSVDATASET_H

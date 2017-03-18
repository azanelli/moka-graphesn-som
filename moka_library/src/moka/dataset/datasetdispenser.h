#ifndef MOKA_DATASET_DATASETDISPENSER_H
#define MOKA_DATASET_DATASETDISPENSER_H

#include <string>
#include <moka/dataset/dataset.h>
#include <moka/dataset/csvdataset.h>
#include <moka/dataset/multilabeledgraphdataset.h>

namespace moka {
namespace dataset {

/**
 * Class DatasetDispenser
 */
class DatasetDispenser
{
  public:

    /**
     * Method get
     *
     * IMPORTANT: the returned object is created in this method by the new
     * operator and MUST be deleted by the caller.
     */
    static Dataset* get(const std::string& dataset_type)
    {
      if (dataset_type == "csv")
        return new CsvDataset();
      if (dataset_type == "multi-labeled-graph")
        return new MultiLabeledGraphDataset();
      return NULL;
    } // method get

}; // class DatasetDispenser

} // namespace dataset
} // namespace moka

#endif // MOKA_DATASET_DATASETDISPENSER_H

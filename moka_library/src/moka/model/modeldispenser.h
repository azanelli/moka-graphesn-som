#ifndef MOKA_MODEL_MODELDISPENSER_H
#define MOKA_MODEL_MODELDISPENSER_H

#include <string>
#include <moka/model/model.h>
#include <moka/model/graphesnsom.h>

namespace moka {
namespace model {

/**
 * Class ModelDispenser
 *
 */
class ModelDispenser
{
  public:

    /**
     * Method get
     *
     * IMPORTANT: the returned object is created in this method by the new
     * operator and MUST be deleted by the caller.
     */
    static Model* get(const std::string& model_type)
    {
      if (model_type == "graph-esn-som")
        return new GraphEsnSom();
      return NULL;
    } // method get

}; // class ModelMakerDispenser

} // namespace model
} // namespace moka

#endif // MOKA_MODEL_MODELDISPENSER_H

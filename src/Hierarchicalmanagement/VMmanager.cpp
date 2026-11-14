#include "AI/VMmanager.h"
#include <cstddef>
#include "ggml/ggml-backend.h"

size_t VMManager::GetFreeVRAM_MB()
{
    size_t VMsize = 0;
    size_t Generalequipment = ggml_backend_dev_count();
    for (size_t i = 0; i < Generalequipment; i++) {
        ggml_backend_dev_t dev = ggml_backend_dev_get(i);
        ggml_backend_dev_props props;
        ggml_backend_dev_get_props(dev, &props);
        if (props.memory_total > 0) {
            VMsize = props.memory_free / (1024 * 1024);
            break;
        }
    }
    return VMsize;
}
bool VMManager::HasEnoughVRAM(size_t required_mb)
{
    size_t VMsize = VMManager::GetFreeVRAM_MB();
    return VMsize > required_mb;
}
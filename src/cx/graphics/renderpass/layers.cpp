#include "layers.h"

namespace cx::graphics {

void Layers::remove_layer(TypeID id) {
  for (auto it = m_stages.begin(); it != m_stages.end();) {
    if (it->second == id) {
      it = m_stages.erase(it);
    } else {
      ++it;
    }
  }
}
void Layers::render_stage(const Pipeline::Stage& stage,
                          const CommandBuffer& command_buffer) {
  for (const auto& [stage_index, id] : m_stages) {
    if (stage_index.first != stage) {
      continue;
    }

    if (auto& subrender = m_layers[id]) {
      subrender->render(command_buffer);
    }
  }
}

}  // namespace cx::graphics
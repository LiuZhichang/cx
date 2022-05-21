#pragma once

#include <map>
#include <unordered_map>

#include "cx/common/type_meta.h"
#include "cx/graphics/pipeline/pipeline.h"
#include "cx/graphics/renderpass/layer.h"

namespace cx::graphics {

class Layers {
 public:
  template <typename T>
  T* obtain() const {
    // T implement Layer
    TypeID id = TypeMeta<Layer>::ID<T>();

    auto it = m_layers.find(id);
    if (it != m_layers.end() && it->second) {
      return dynamic_cast<T*>(it->second);
    }

    return nullptr;
  }

  template <typename T, typename... Args>
  T* push(const Pipeline::Stage stage, std::unique_ptr<T>&& layer) {
    TypeID id = TypeMeta<Layer>::ID<T>();

    m_stages.insert({StageInfo(stage, m_layers.size()), id});

    m_layers[id] = std::move(layer);

    return dynamic_cast<T*>(m_layers[id].get());
  }

  template <typename T>
  void remove() {
    TypeID id = TypeMeta<Layer>::ID<T>();

    remove_layer(id);

    m_layers.erase(id);
  }

  void clear();

 private:
  void remove_layer(TypeID id);
  void render_stage(const Pipeline::Stage& stage,
                    const CommandBuffer& command_buffer);

  typedef std::pair<Pipeline::Stage, std::size_t> StageInfo;

  std::unordered_map<TypeID, std::unique_ptr<Layer>> m_layers;
  std::multimap<StageInfo, TypeID> m_stages;
};

}  // namespace cx::graphics
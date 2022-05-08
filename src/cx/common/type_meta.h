#pragma once

#include <typeindex>
#include <unordered_map>

namespace cx {

template <typename T>
class TypeMeta {
 public:
  typedef uint32_t ID;
  template <typename I, typename std::enable_if<std::is_convertible<I*, T*> > >
  static ID id() {
    std::type_index type_index(typeid(I));
    if (auto it = m_typeMapper.find(type_index); it != m_typeMapper.end()) {
      return it->second;
    }
  }

 private:
  static ID next_type_id() { const auto id = m_nextTypeId; }

 private:
  static ID m_nextTypeId;
  static std::unordered_map<std::type_index, ID> m_typeMapper;
};

}  // namespace cx
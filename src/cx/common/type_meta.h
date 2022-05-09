#pragma once

#include <type_traits>
#include <typeindex>
#include <unordered_map>

namespace cx {

/**
 * @brief 类型id，每个类型的id唯一，可在类型映射表中查询
 *
 */
typedef std::size_t TypeID;

/**
 * @brief 类型元信息记录[这里仅记录类型的id]
 *
 * @tparam T 任意类型
 */
template <typename T>
class TypeMeta {
 public:
  //  类型映射表type
  typedef std::unordered_map<std::type_index, TypeID> TypeMapper;

  /**
   * @brief 用于获取类型对应的id
   *
   * @tparam I 必须能够转为T类型的类型, 如果未记录I类型，则会进行注册
   * @return TypeID 类型id
   */
  template <typename I, typename = std::enable_if_t<std::is_base_of_v<T, I>>>
  static TypeID ID() {
    auto& mapper = GetTypeMapper();

    std::type_index type_index(typeid(I));
    if (auto it = mapper.find(type_index); it != mapper.end()) {
      return it->second;
    }

    const TypeID cur_id = m_nextTypeId++;

    mapper[type_index] = cur_id;
    return cur_id;
  }

 private:
  static TypeID m_nextTypeId;

  static TypeMapper& GetTypeMapper() {
    static TypeMapper s_typeMapper;
    return s_typeMapper;
  }
};

template <typename T>
TypeID TypeMeta<T>::m_nextTypeId = 0;

// template <typename T>
// typename TypeMeta<T>::TypeMapper TypeMeta<T>::m_typeMapper = {};

}  // namespace cx
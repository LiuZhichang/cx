/**
 * @file module.h
 * @author liuzhichang (lzc3318619633@163.com)
 * @brief 模块的注册与管理
 * @version 0.1
 * @date 2022-05-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "common.h"
#include "internal.h"
#include "log/log.h"
#include "noncopyable.h"
#include "type_meta.h"

namespace cx {

template <typename Base>
class ModuleFactory {
 public:
  virtual ~ModuleFactory() = default;

  struct TypeVal {
    std::function<std::unique_ptr<Base>()> generator;
    typename Base::Stage Stage;
    std::vector<TypeID> require;
  };

  template <typename... Args>
  struct Requires {
    std::vector<TypeID> Get() const {
      std::vector<TypeID> require;
      // 展开变参
      (require.emplace_back(TypeMeta<Base>::template ID<Args>()), ...);

      return require;
    }
  };

  /**
   * @brief 类型注册管理类
   *
   * @tparam T 任意类型
   */
  template <typename T /*, bool Raw = true*/>
  class Registrar : public Base {
    struct Generator {};

   public:
    // typedef typename std::conditional<Raw, T*, std::unique_ptr<T>>::type ptr;

    /**
     * @brief 获取类型实例指针
     *
     * @return T* 类型实例指针
     */
    static T* Get() { return s_module_instance; }

   protected:
    /**
     * @brief 类型注册
     *
     * @tparam Args 变参类型，可传入多个类型
     * @param stage 该类型所在阶段
     * @param require
     */
    template <typename... Args>
    static bool Register(typename Base::Stage stage,
                         Requires<Args...>&& require = {}) {
      // 获取注册器实例
      auto& registry = ModuleFactory::Registry();
      // 获取类型id
      TypeID id = TypeMeta<Base>::template ID<T>();

      // 类型注册，k: TypeID, v: TypeVal
      // TypeVal [generator, stage, require]
      registry[id] = {[]() {
                        s_module_instance = new T();
                        return std::unique_ptr<Base>(s_module_instance);
                      },
                      stage, require.Get()};
      return true;
    }

    // T's instance pointer
    inline static T* s_module_instance = nullptr;
  };

 public:
  typedef std::unordered_map<TypeID, TypeVal> TypeRegMapper;

 public:
  // 注册器实例
  CX_STATIC TypeRegMapper& Registry() {
    static TypeRegMapper inst{};
    return inst;
  }

 private:
};

class Module : public ModuleFactory<Module>, public Noncopyable {
 public:
  enum class Stage : uint8_t {
    eNone,
    eInvariably,
    ePre,
    eNormal,
    ePost,
    eRender
  };

  virtual ~Module() = default;

  virtual void update() = 0;
};

}  // namespace cx
/**
 * @file config.h
 * @author liuzhichang (lzc3318619633@163.com)
 * @brief 引擎配置
 * @version 0.1
 * @date 2022-05-08
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <cx/utils/sync/spink_lock.h>

#include <filesystem>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <type_traits>

namespace YAML {
class Node;
}

namespace cx {

namespace conv {
template <typename F, typename T>
class Convert {
 public:
  static_assert(std::is_convertible_v<F, T>,
                "src type must be convert to dst type.");
  T operator()(const F& val) { return F(); }
};

}  // namespace conv

/**
 * @brief 配置项基类
 *
 */
class ConfigVarBase {
 public:
  typedef std::shared_ptr<ConfigVarBase> ptr;
  ConfigVarBase(const std::string& name, const std::string& descripti)
      : m_name(name), m_descript(descripti) {}

  virtual ~ConfigVarBase() {}

  /**
   * @brief 配置项名称
   *
   * @return const std::string&  名称
   */
  const std::string& name() const { return m_name; }

  /**
   * @brief 配置项描述
   *
   * @return const std::string& 描述
   */
  const std::string& desc() const { return m_descript; }

  virtual std::string to_string() = 0;

  virtual bool from_string(const std::string& str) = 0;

 protected:
  std::string m_name;
  std::string m_descript;
};

template <typename T, class FromStr = conv::Convert<std::string, T>,
          class ToStr = conv::Convert<T, std::string>>
class ConfigVar : public ConfigVarBase {
 public:
  typedef std::function<void(const T& oldVal, const T& newVal)> callback_t;
  typedef sync::SpinkLock lock_t;
  typedef std::lock_guard<lock_t> lock_guard;

  ConfigVar(const std::string& name, const T& default_val,
            const std::string& descript = "")
      : ConfigVarBase(name, descript), m_val(default_val) {}

  std::string to_string() {
    try {
      return ToStr()(value());
    } catch (const std::exception& e) {
    }
  }

  bool from_string(const std::string& str) {
    try {
      m_val = FromStr()(str);
    } catch (const std::exception& e) {
    }

    return false;
  }

  const T& value() const {
    lock_guard lock(m_mutex);
    return m_val;
  }

  void set_value(const T& val) {
    {
      lock_guard lock(m_mutex);
      if (val == m_val) return;

      for (auto& it : m_callback_mapper) {
        it.second(m_val, val);
      }
    }
    lock_guard lock(m_mutex);
    m_val = val;
  }

  uint64_t add_listener(callback_t callback) {
    static uint64_t s_cb_id = 0;
    lock_guard lock(m_mutex);
    m_callback_mapper[s_cb_id] = callback;
    return (++s_cb_id);
  }

  void delete_listener(uint64_t id) {
    lock_guard lock(m_mutex);
    m_callback_mapper.erase(id);
  }

  callback_t get_listener(uint64_t id) {
    lock_guard lock(m_mutex);
    typename callback_mapper_t::iterator it = m_callback_mapper.find(id);
    if (it != m_callback_mapper.end()) {
      return it;
    }

    return nullptr;
  }

  void clear_listener() {
    lock_guard lock(m_mutex);
    m_callback_mapper.clear();
  }

 private:
  typedef std::map<uint64_t, callback_t> callback_mapper_t;

  T m_val;
  lock_t m_mutex;
  callback_mapper_t m_callback_mapper;
};

/**
 * @brief 引擎配置类，用于读取或配置引擎参数
 *
 */
class Config {
 public:
  typedef std::map<std::string, ConfigVarBase::ptr> var_mapper_t;
  typedef sync::SpinkLock lock_t;
  typedef std::lock_guard<lock_t> lock_guard;

  template <typename T>
  static typename ConfigVar<T>::ptr Lookup(const std::string& name,
                                           const T& default_val,
                                           const std::string& descript = "") {
    lock_guard lock(GetMutex());

    // 注册
    return check_item(name, default_val, descript);
  }

  template <typename T>
  static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
    lock_guard lock(GetMutex());
    var_mapper_t& mapper = GetMapper();
    typename var_mapper_t::iterator it = mapper.find(name);
    if (it == mapper.end()) {
      return nullptr;
    }

    return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
  }

  static ConfigVarBase::ptr LookupBase(const std::string& name);

  static void LoadFromFile(const std::string& path);

  static void LoadFromDirectory(const std::string& path);

  static void Watch(bool enable) { s_watch = enable; }

  static void Visit(const std::function<void(ConfigVarBase::ptr)>& callback);

 private:
  static void LoadFromFile(const std::filesystem::path& path);

  static void LoadFileFromNode(YAML::Node* root);

  template <typename T>
  static typename ConfigVar<T>::ptr check_item(const std::string& name,
                                               const T& default_val,
                                               const std::string& descript) {
    var_mapper_t& mapper = GetMapper();
    typename var_mapper_t::iterator it = mapper.find(name);
    if (it != mapper.end()) {
      ConfigVarBase::ptr tmp =
          std::dynamic_pointer_cast<ConfigVar<T>>(it->second);

      return tmp ? tmp : nullptr;
    }
    typename ConfigVar<T>::ptr conf_var(
        std::make_shared<ConfigVar<T>>(name, default_val, descript));

    GetMapper().insert({name, conf_var});

    return conf_var;
  }

  static lock_t& GetMutex() {
    static lock_t s_mutex;
    return s_mutex;
  }

  static var_mapper_t& GetMapper() {
    static var_mapper_t s_datas;
    return s_datas;
  }

  typedef std::map<std::size_t, time_t> file_mapper_t;
  static std::string s_config_dir;

  static file_mapper_t s_file_modified;
  static bool s_watch;
};
}  // namespace cx
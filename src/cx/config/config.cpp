#include "config.h"

#include <cx/utils/fileop/file.h>
#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <iostream>
#include <list>
#include <set>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace YAML {
class Node;
}

namespace cx::conv {

template <typename F>
class Convert<F, std::string> {
 public:
  static_assert(std::is_arithmetic_v<F>,
                "src type must be suitable std::to_string arg`s type.");
  std::string operator()(const F& val) { return std::to_string(val); }
};

template <typename T>
class Convert<std::string, T> {
 public:
  T operator()(const std::string& str) {
    std::stringstream ss;
    ss << str;

    T result;
    ss >> result;
    return result;
  }
};

template <>
class Convert<std::string, std::string> {
 public:
  std::string operator()(const std::string& str) { return str; }
};

template <>
class Convert<char*, std::string> {
 public:
  std::string operator()(char* str) { return std::string(str); }
};

// vector
template <typename T>
class Convert<std::string, std::vector<T>> {
 public:
  std::vector<T>&& operator()(const std::string& str) {
    typename std::vector<T> result;

    YAML::Node node = YAML::Load(str);
    std::stringstream ss;

    for (T& val : node) {
      ss.str("");
      ss << *val;
      result.push_back(Convert<std::string, T>()(ss.str()));
    }

    return std::move(result);
  }
};

template <typename T>
class Convert<std::vector<T>, std::string> {
 public:
  std::string operator()(const std::vector<T>& vec) {
    YAML::Node node(YAML::NodeType::Sequence);
    for (T& val : vec) {
      node.push_back(Convert<T, std::string>()(val));
    }

    std::stringstream ss;
    ss << node;

    return ss.str();
  }
};

// list
template <typename T>
class Convert<std::string, std::list<T>> {
 public:
  std::list<T>&& operator()(const std::string& str) {
    typename std::list<T> result;

    YAML::Node node = YAML::Load(str);
    std::stringstream ss;

    for (T& val : node) {
      ss.str("");
      ss << *val;
      result.push_back(Convert<std::string, T>()(ss.str()));
    }

    return std::move(result);
  }
};

template <typename T>
class Convert<std::list<T>, std::string> {
 public:
  std::string operator()(const std::list<T>& list) {
    YAML::Node node(YAML::NodeType::Sequence);
    for (T& val : list) {
      node.push_back(Convert<T, std::string>()(val));
    }

    std::stringstream ss;
    ss << node;

    return ss.str();
  }
};

// set
template <typename T>
class Convert<std::string, std::set<T>> {
 public:
  std::set<T>&& operator()(const std::string& str) {
    typename std::set<T> result;

    YAML::Node node = YAML::Load(str);
    std::stringstream ss;

    for (T& val : node) {
      ss.str("");
      ss << *val;
      result.push_back(Convert<std::string, T>()(ss.str()));
    }

    return std::move(result);
  }
};

template <typename T>
class Convert<std::set<T>, std::string> {
 public:
  std::string operator()(const std::set<T>& set) {
    YAML::Node node(YAML::NodeType::Sequence);
    for (T& val : set) {
      node.push_back(Convert<T, std::string>()(val));
    }

    std::stringstream ss;
    ss << node;

    return ss.str();
  }
};

// unorderd_set
template <typename T>
class Convert<std::string, std::unordered_set<T>> {
 public:
  std::unordered_set<T>&& operator()(const std::string& str) {
    typename std::unordered_set<T> result;

    YAML::Node node = YAML::Load(str);
    std::stringstream ss;

    for (T& val : node) {
      ss.str("");
      ss << *val;
      result.push_back(Convert<std::string, T>()(ss.str()));
    }

    return std::move(result);
  }
};

template <typename T>
class Convert<std::unordered_set<T>, std::string> {
 public:
  std::string operator()(const std::unordered_set<T>& set) {
    YAML::Node node(YAML::NodeType::Sequence);
    for (T& val : set) {
      node.push_back(Convert<T, std::string>()(val));
    }

    std::stringstream ss;
    ss << node;

    return ss.str();
  }
};

// map
template <typename T>
class Convert<std::string, std::map<std::string, T>> {
 public:
  std::map<std::string, T>&& operator()(const std::string& str) {
    typename std::map<std::string, T> result;

    YAML::Node node = YAML::Load(str);
    std::stringstream ss;

    for (T& val : node) {
      ss.str("");
      ss << val.second;
      result.push_back(val.first.Scalar(), Convert<std::string, T>()(ss.str()));
    }

    return std::move(result);
  }
};

template <typename T>
class Convert<std::map<std::string, T>, std::string> {
 public:
  std::string operator()(const std::map<std::string, T>& map) {
    YAML::Node node(YAML::NodeType::Map);
    for (T& val : map) {
      node[val.first] = YAML::Load(Convert<T, std::string>()(val.second));
    }

    std::stringstream ss;
    ss << node;

    return ss.str();
  }
};
// unordered_map
template <typename T>
class Convert<std::string, std::unordered_map<std::string, T>> {
 public:
  std::unordered_map<std::string, T>&& operator()(const std::string& str) {
    typename std::unordered_map<std::string, T> result;

    YAML::Node node = YAML::Load(str);
    std::stringstream ss;

    for (T& val : node) {
      ss.str("");
      ss << val.second;
      result.push_back(val.first.Scalar(), Convert<std::string, T>()(ss.str()));
    }

    return std::move(result);
  }
};

template <typename T>
class Convert<std::unordered_map<std::string, T>, std::string> {
 public:
  std::string operator()(const std::map<std::string, T>& map) {
    YAML::Node node(YAML::NodeType::Map);
    for (T& val : map) {
      node[val.first] = YAML::Load(Convert<T, std::string>()(val.second));
    }

    std::stringstream ss;
    ss << node;

    return ss.str();
  }
};
}  // namespace cx::conv

std::string cx::Config::s_config_dir = "config/";
bool cx::Config::s_watch = true;
cx::Config::file_mapper_t cx::Config::s_file_modified;

void ListAllMember(
    const std::string& prefix, const YAML::Node& node,
    std::list<std::pair<std::string, const YAML::Node>>& output) {
  if (!prefix.empty()) {
    output.push_back(std::make_pair(prefix, node));
  }

  if (node.IsMap())
    for (auto it : node) {
      ListAllMember((prefix.empty() ? it.first.Scalar()
                                    : (prefix + "." + it.first.Scalar())),
                    it.second, output);
    }
}

void cx::Config::LoadFileFromNode(YAML::Node* root) {
  std::list<std::pair<std::string, const YAML::Node>> all_nodes;
  ListAllMember("", *root, all_nodes);

  for (auto& i : all_nodes) {
    std::string key = i.first;
    if (key.empty()) {
      continue;
    }

    std::transform(key.begin(), key.end(), key.begin(), ::tolower);

    ConfigVarBase::ptr var = LookupBase(key);

    if (var) {
      if (i.second.IsScalar()) {
        var->from_string(i.second.Scalar());
      } else {
        std::stringstream ss;
        ss << i.second;
        var->from_string(ss.str());
      }
    }
  }
}

cx::ConfigVarBase::ptr cx::Config::LookupBase(const std::string& name) {
  lock_guard(GetMutex());
  var_mapper_t::iterator it = GetMapper().find(name);
  return it != GetMapper().end() ? it->second : nullptr;
}

void cx::Config::LoadFromFile(const std::filesystem::path& path) {
  if (!std::filesystem::is_regular_file(path)) {
    return;
  }
  std::string subfix = path.extension();
  if (subfix == ".yml" || subfix == ".yaml") {
    if (s_watch) {
      std::size_t file_hash = std::filesystem::hash_value(path);
      auto ftime = std::filesystem::last_write_time(path);
      std::time_t ctime = decltype(ftime)::clock::to_time_t(ftime);

      if (s_file_modified[file_hash] == ctime) {
        return;
      }
      s_file_modified[file_hash] = ctime;
    }
    YAML::Node node = YAML::LoadFile(path.native());
    LoadFileFromNode(&node);
  }
}

void cx::Config::LoadFromFile(const std::string& path) {
  std::filesystem::path file_path(path);
  LoadFromFile(file_path);
}

void cx::Config::LoadFromDirectory(const std::string& path) {
  FileOp::each_files(path, [](const std::filesystem::path& file_path) {
    LoadFromFile(file_path);
  });
}
void cx::Config::Visit(
    const std::function<void(ConfigVarBase::ptr)>& callback) {
  lock_guard lock(GetMutex());

  var_mapper_t& mapper = GetMapper();
  for (auto& var : mapper) {
    callback(var.second);
  }
}

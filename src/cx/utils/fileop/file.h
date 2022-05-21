#pragma once

#include <filesystem>
#include <functional>

namespace cx {

class FileOp {
 public:
  typedef std::filesystem::path fs_path_t;

  static void each_files(
      const std::string& dir,
      std::function<void(const fs_path_t& file_path)> callback);

  static void remove_file(const std::string& file);

  static bool file_exists(const std::string& file);

  static void read(const std::filesystem::path& path, std::string& buf);
};
}  // namespace cx
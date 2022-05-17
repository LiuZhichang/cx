#include "file.h"

using namespace std;

namespace cx {

void FileOp::each_files(
    const std::string& dir,
    std::function<void(const std::filesystem::path& file_path)> callback) {
  filesystem::path dir_path(dir);

  if (!filesystem::exists(dir_path) || !filesystem::is_directory(dir_path)) {
    return;
  }

  std::filesystem::recursive_directory_iterator entry(dir_path);
  std::filesystem::recursive_directory_iterator list(entry);

  for (auto& it : list) {
    callback(it.path());
  }
}

void FileOp::remove_file(const std::string& file) {
  fs_path_t path(file);
  filesystem::remove(path);
}
bool FileOp::file_exists(const std::string& file) {
  fs_path_t path(file);
  return filesystem::exists(path);
}

}  // namespace cx

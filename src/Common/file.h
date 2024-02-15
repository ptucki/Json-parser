#ifndef FILE_H
#define FILE_H

#include <vector>
#include <memory>
#include <string>

class File {
  

public:
  File(const char* path);

  bool Load();
  const std::string& GetContent() const;
  size_t size() const;

private:
  using FileHolder = std::unique_ptr<FILE, void(*)(FILE*)>;

  static void FileDeleter(FILE* f) { fclose(f); }

  std::string file_buffer_;
  FileHolder file_holder_;
  size_t file_size_;
};


#endif // !FILE_H

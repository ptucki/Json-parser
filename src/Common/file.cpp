#include <iostream>

#include "file.h"

File::File(const char* path)
  : file_buffer_{""}
  , file_holder_{ fopen(path, "rb"), File::FileDeleter }
  , file_size_{ 0 }
{
  if (file_holder_.get() != NULL) {
    std::cout << "File loaded!" << std::endl;
  }
  else {
    std::cout << "File is not loaded!" << std::endl;
  }
}

bool File::Load()
{
  bool is_file_read = false;

  // File exists?
  if (file_holder_) {
    FILE* file_ptr = file_holder_.get();

    // Read succesfully?
    if (fseek(file_ptr, 0, SEEK_END) >= 0)
    {
      file_size_ = ftell(file_ptr);

      // Has any content?
      if (file_size_ >= 0)
      {
        // Stream position correct?
        if (fseek(file_ptr, 0, SEEK_SET) >= 0)
        {
          // set size of string buffer
          file_buffer_.resize(file_size_);
          // Read data to string
          fread(file_buffer_.data(), 1, file_size_, file_ptr);
          is_file_read = true;
        }
      }
    }
  }
  return is_file_read;
}

const std::string& File::GetContent() const
{
  return file_buffer_;
}

size_t File::size() const
{
  return file_size_;
}

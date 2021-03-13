#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <cstring>


Image::Image(const std::string &a_path)
{
  if((data = (Pixel*)stbi_load(a_path.c_str(), &width, &height, &channels, 0)) != nullptr)
  {
    size = width * height * channels;
  }
}

Image::Image(int a_width, int a_height, int a_channels)
{
  data = new Pixel[a_width * a_height * a_channels]{};

  if(data != nullptr)
  {
    width = a_width;
    height = a_height;
    size = a_width * a_height * a_channels;
    channels = a_channels;
    self_allocated = true;
  }
}

int Image::Width() const {
  return width;
}

int Image::Height() const {
  return height;
}

int Image::Channels() const {
  return channels;
}

size_t Image::Size() const {
  return size;
}

Pixel* Image::Data() {
  return data;
}

void Image::UpdateBuffer() {
  if (buffer != nullptr) {
    delete [] buffer;
  }
  buffer = new Pixel[height * width * channels];
  std::memcpy(buffer, data, width * height * channels);
}

Pixel Image::GetPixel(int x, int y) {
  return data[width * y + x];
}

void Image::PutPixel(int x, int y, const Pixel &pix) {
  data[width * y + x] = pix;
}

Pixel Image::GetBufPix(int x, int y) {
  return buffer[width * y + x];
}

void Image::PutBufPix(int x, int y, const Pixel &pix) {
  buffer[width * y + x] = pix;
}

int Image::Save(const std::string &a_path)
{
  auto extPos = a_path.find_last_of('.');
  if(a_path.substr(extPos, std::string::npos) == ".png" || a_path.substr(extPos, std::string::npos) == ".PNG")
  {
    stbi_write_png(a_path.c_str(), width, height, channels, data, width * channels);
  }
  else if(a_path.substr(extPos, std::string::npos) == ".jpg" || a_path.substr(extPos, std::string::npos) == ".JPG" ||
          a_path.substr(extPos, std::string::npos) == ".jpeg" || a_path.substr(extPos, std::string::npos) == ".JPEG")
  {
    stbi_write_jpg(a_path.c_str(), width, height, channels, data, 100);
  }
  else
  {
    std::cerr << "Unknown file extension: " << a_path.substr(extPos, std::string::npos) << "in file name" << a_path << "\n";
    return 1;
  }

  return 0;
}

Image::~Image()
{
  if(self_allocated) {
    delete [] data;
    delete [] buffer;
  } else {
    stbi_image_free(data);
    stbi_image_free(buffer);
  }
}
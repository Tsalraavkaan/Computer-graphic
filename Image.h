#ifndef MAIN_IMAGE_H
#define MAIN_IMAGE_H

#include <string>

constexpr int tileSize = 32;

struct Pixel
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};

constexpr Pixel backgroundColor{0, 0, 0, 0};

struct Image
{
  explicit Image(const std::string &a_path);
  Image(int a_width = -1, int a_height = -1, int a_channels = 3);

  int Save(const std::string &a_path);

  int Width()    const;
  int Height()   const;
  int Channels() const;
  size_t Size()  const;
  Pixel* Data();
  void UpdateBuffer();

  Pixel GetPixel(int x, int y);
  void  PutPixel(int x, int y, const Pixel &pix);
  Pixel GetBufPix(int x, int y);
  void  PutBufPix(int x, int y, const Pixel &pix);

  ~Image();

private:
  int width = -1;
  int height = -1;
  int channels = 3;
  size_t size = 0;
  Pixel *data = nullptr;
  Pixel *buffer = nullptr;
  bool self_allocated = false;
};



#endif //MAIN_IMAGE_H

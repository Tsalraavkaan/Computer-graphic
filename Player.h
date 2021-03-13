#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include "Image.h"
#include <vector>
#include <GLFW/glfw3.h>

struct Point
{
  int x;
  int y;
};

enum class MovementDir
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

struct Trap {
  explicit Trap(int i, int j, GLfloat start_frame = 0, GLfloat frames = 3)
      : x(i), y(j), cur_frame(start_frame), delta_frame(frames) {};
  void CheckFlag(GLfloat frame, Image &screen, std::vector<std::string> &level, Image &, Image &);
private:
  int x;
  int y;
  GLfloat cur_frame;
  GLfloat delta_frame;
};

struct Player
{
  explicit Player(Point pos = {.x = 10, .y = 10}) :
                 coords(pos), old_coords(coords) {};

  bool Moved() const;
  bool onWall(MovementDir dir, std::vector<std::string> level, int n, int m);
  int ProcessInput(MovementDir dir, std::vector<std::string> level, GLfloat frame);
  void Draw(Image &screen);

private:
  Image skin = Image("./resources/character.png");
  Point coords {.x = 10, .y = 10};
  Point old_coords {.x = 10, .y = 10};
  Pixel color {.r = 255, .g = 255, .b = 0, .a = 255};
  int move_speed = 4;
  int img_size = 16;
  int i = 0;
  int j = 0;
  int delta_frame = 0.2;
  int cur_frame = 0;
  int player_tiles = 4;

};

#endif //MAIN_PLAYER_H

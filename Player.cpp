#include "Player.h"
#include <iostream>

void Trap::CheckFlag(GLfloat frame, Image &screen,
    std::vector<std::string> &level, Image &active, Image &unactive) {
  if (frame - cur_frame > delta_frame) {
    cur_frame = frame;
    level[y][x] = level[y][x] == 'T' ? 'A' : 'T';
    if (level[y][x] == 'T') {
      for (int l = 0; l < tileSize; l++) {
        for (int k = 0; k < tileSize; k++) {
           screen.PutPixel(x * tileSize + k, y * tileSize + l, unactive.GetPixel(k + 12 * tileSize, l));
           screen.PutBufPix(x * tileSize + k, y * tileSize + l, unactive.GetPixel(k + 12 * tileSize, l));
        }
      }
    } else {
      for (int l = 0; l < tileSize; l++) {
        for (int k = 0; k < tileSize; k++) {
           screen.PutPixel(x * tileSize + k, y * tileSize + l, active.GetPixel(k + 13 * tileSize, l));
           screen.PutBufPix(x * tileSize + k, y * tileSize + l, active.GetPixel(k + 13 * tileSize, l));
        }
      }
    }
  }
}

bool Player::Moved() const
{
  if(coords.x == old_coords.x && coords.y == old_coords.y)
    return false;
  else
    return true;
}


bool Player::onWall(MovementDir dir, std::vector<std::string> level, int n, int m) {
  bool flag = true;
  switch(dir) {
    case MovementDir::UP:
      if (level[n + 1][m] == '#' || level[n + 1][m + 1] == '#') {
        flag = false;
      }
      break;
    case MovementDir::DOWN:
      if (level[n][m] == '#' || level[n][m + 1] == '#') {
        flag = false;
      }
      break;
    case MovementDir::LEFT:
      if (level[n][m] == '#' || level[n + 1][m] == '#') {
        flag = false;
      }
      break;
    case MovementDir::RIGHT:
      if (level[n][m + 1] == '#' || level[n + 1][m + 1] == '#') {
        flag = false;
      }
      break;
    default:
      break;
  }
  return flag;
}

int Player::ProcessInput(MovementDir dir, std::vector<std::string> level, GLfloat frame) {
  int move_dist = move_speed * 1, n, m;
  switch(dir)
  {
    case MovementDir::UP:
      i = 1;
      old_coords.y = coords.y;
      n = (coords.y + move_dist) / tileSize;
      m = coords.x / tileSize;
      if (onWall(dir, level, n, m)) {
        coords.y += move_dist;
      }
      break;
    case MovementDir::DOWN:
      i = 0;
      old_coords.y = coords.y;
      n = (coords.y - move_dist) / tileSize;
      m = coords.x / tileSize;
      if (onWall(dir, level, n, m)) {
        coords.y -= move_dist;
      }
      break;
    case MovementDir::LEFT:
      i = 2;
      old_coords.x = coords.x;
      n = coords.y / tileSize;
      m = (coords.x - move_dist) / tileSize;
      if (onWall(dir, level, n, m)) {
        coords.x -= move_dist;
      }
      break;
    case MovementDir::RIGHT:
      i = 3;
      old_coords.x = coords.x;
      n = coords.y / tileSize;
      m = (coords.x + move_dist) / tileSize;
      if (onWall(dir, level, n, m)) {
        coords.x += move_dist;
      }
      break;
    default:
      break;
  }
  if (frame - cur_frame > delta_frame) {
    cur_frame = frame;
    j = (j + 1) % player_tiles;
  }
  int middle = tileSize / 2;
  int x = (coords.x + middle) / tileSize;
  int y = (coords.y + middle) / tileSize;
  if (level[y][x] == 'A' || level[y][x] == ' ') {
    return 2;
  } else if (level[y][x] == 'X'){
    return 1;
  }
  return 0;
}

void Player::Draw(Image &screen)
{
  if(Moved())
  {
    for(int y = old_coords.y; y <= old_coords.y + tileSize; ++y)
    {
      for(int x = old_coords.x; x <= old_coords.x + tileSize; ++x)
      {
        screen.PutPixel(x, y, screen.GetBufPix(x, y));
      }
    }
    old_coords = coords;
  }

  for(int y = 0; y <= img_size; ++y)
  {
    for(int x = 0; x <= img_size; ++x)
    {
      if (skin.GetPixel(x + img_size * i, img_size - y + img_size * j).a > 0.1) {
        screen.PutPixel(x * 2 + coords.x, y * 2 + coords.y, skin.GetPixel(x + img_size * i, img_size - y + img_size * j));
        screen.PutPixel(x * 2 + 1 + coords.x, y * 2 + coords.y, skin.GetPixel(x + img_size * i, img_size - y + img_size * j));
        screen.PutPixel(x * 2 + coords.x, y * 2 + 1 + coords.y, skin.GetPixel(x + img_size * i, img_size - y + img_size * j));
        screen.PutPixel(x * 2 + 1 + coords.x, y * 2 + 1 + coords.y, skin.GetPixel(x + img_size * i, img_size - y + img_size * j));
      }
      //screen.PutPixel(x, y, blend(screen.GetPixel(x, y), img.GetPixel(x - coords.x, tileSize - y + coords.y)));
    }
  }
}
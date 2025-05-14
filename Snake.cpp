#include "Snake.h"

#include <chrono>
#include <iostream>
#include <random>
#include <stdexcept>

Snake::Snake(bool border)
    : hConsole(GetStdHandle(STD_OUTPUT_HANDLE)),
      borderEnabled(border),
      rng(std::random_device{}()) {
  if (hConsole == INVALID_HANDLE_VALUE)
    throw std::runtime_error("Error: Cannot get console handle.");

  setCursorVisibility(false);
  updateConsoleSize();

  // initialize playfield occupancy
  occupied.assign(innerRows, std::vector<bool>(innerCols, false));

  // start snake in center
  int startY = innerRows / 2;
  int startX = innerCols / 2;
  snake.emplace_back(startY, startX);
  occupied[startY][startX] = true;

  spawnFruit();
}

void Snake::run() {
  while (true) {
    updateConsoleSize();
    drawFrame();

    // input
    Direction dir = pollInput();
    if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0) break;

    // only allow 180° turns when snake len > 1
    if (dir != Direction::None &&
        !(dir == Direction::Up && currentDir == Direction::Down) &&
        !(dir == Direction::Down && currentDir == Direction::Up) &&
        !(dir == Direction::Left && currentDir == Direction::Right) &&
        !(dir == Direction::Right && currentDir == Direction::Left)) {
      currentDir = dir;
    }

    if (currentDir != Direction::None) {
      auto [hy, hx] = snake.front();
      int y = hy, x = hx;
      switch (currentDir) {
        case Direction::Up:
          --y;
          break;
        case Direction::Down:
          ++y;
          break;
        case Direction::Left:
          --x;
          break;
        case Direction::Right:
          ++x;
          break;
        default:
          break;
      }

      // wrap
      if (x < 0)
        x = innerCols - 1;
      else if (x >= innerCols)
        x = 0;
      if (y < 0)
        y = innerRows - 1;
      else if (y >= innerRows)
        y = 0;

      ateFruit = (y == fruit.first && x == fruit.second);
      if (ateFruit) spawnFruit();

      snake.emplace_front(y, x);
      occupied[y][x] = true;

      if (!ateFruit) {
        auto [ty, tx] = snake.back();
        occupied[ty][tx] = false;
        snake.pop_back();
      }
    }
  }
}

void Snake::updateConsoleSize() {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
    throw std::runtime_error("Error getting console size.");

  int newRows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
  int newCols = csbi.srWindow.Right - csbi.srWindow.Left + 1;

  if (newRows != rows || newCols != cols) {
    rows = newRows;
    cols = newCols;
    if (borderEnabled) {
      innerRows = rows - 2;
      innerCols = cols - 2;
    } else {
      innerRows = rows;
      innerCols = cols;
    }

    // reset state
    snake.clear();
    occupied.assign(innerRows, std::vector<bool>(innerCols, false));
    currentDir = Direction::None;

    // re-center snake
    snake.emplace_back(innerRows / 2, innerCols / 2);
    occupied[innerRows / 2][innerCols / 2] = true;

    spawnFruit();
  }
}

void Snake::drawFrame() const {
  clearConsole();

  if (borderEnabled) {
    std::cout << '+';
    for (int c = 0; c < innerCols; ++c) std::cout << '-';
    std::cout << "+\n";
  }

  for (int r = 0; r < innerRows; ++r) {
    if (borderEnabled) std::cout << '|';
    for (int c = 0; c < innerCols; ++c) {
      if (r == fruit.first && c == fruit.second)
        std::cout << '@';
      else if (occupied[r][c])
        std::cout << '*';
      else
        std::cout << ' ';
    }
    if (borderEnabled) std::cout << "|\n";
  }

  if (borderEnabled) {
    std::cout << '+';
    for (int c = 0; c < innerCols; ++c) std::cout << '-';
    std::cout << '+';
  }
}

Snake::Direction Snake::pollInput() const {
  if (GetAsyncKeyState(VK_UP) & 0x8000) return Direction::Up;
  if (GetAsyncKeyState(VK_DOWN) & 0x8000) return Direction::Down;
  if (GetAsyncKeyState(VK_LEFT) & 0x8000) return Direction::Left;
  if (GetAsyncKeyState(VK_RIGHT) & 0x8000) return Direction::Right;
  return Direction::None;
}

void Snake::clearConsole() const {
  COORD home{0, 0};
  SetConsoleCursorPosition(hConsole, home);
}

void Snake::spawnFruit() {
  std::uniform_int_distribution<int> dx(0, innerCols - 1);
  std::uniform_int_distribution<int> dy(0, innerRows - 1);
  int fx, fy;
  do {
    fx = dx(rng);
    fy = dy(rng);
  } while (occupied[fy][fx]);
  fruit = {fy, fx};
}

void Snake::setCursorVisibility(bool visible) {
  CONSOLE_CURSOR_INFO info;
  GetConsoleCursorInfo(hConsole, &info);
  info.bVisible = visible ? TRUE : FALSE;
  SetConsoleCursorInfo(hConsole, &info);
}

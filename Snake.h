#pragma once

#include <windows.h>

#include <deque>
#include <random>
#include <utility>
#include <vector>

class Snake {
 public:
  explicit Snake(bool border = true);
  void run();

 private:
  enum class Direction { None, Up, Down, Left, Right };

  // Console
  HANDLE hConsole;
  int rows{}, cols{};
  int innerRows{}, innerCols{};
  bool borderEnabled;

  // Game state
  std::deque<std::pair<int, int>> snake;  // front() is head
  std::pair<int, int> fruit{};
  bool ateFruit{false};
  Direction currentDir{Direction::None};
  std::vector<std::vector<bool>> occupied;

  // Random
  std::mt19937 rng;

  // Methods
  void updateConsoleSize();
  void setCursorVisibility(bool visible);
  void clearConsole() const;
  void drawFrame() const;
  Direction pollInput() const;
  void spawnFruit();
};

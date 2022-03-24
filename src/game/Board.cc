
# include "Board.hh"
# include <cmath>

namespace two48 {

  Board::Board(int width, int height) noexcept:
    utils::CoreObject("board"),

    m_width(width),
    m_height(height),

    m_board()
  {
    setService("2048");

    initialize();
  }

  int
  Board::w() const noexcept {
    return m_width;
  }

  int
  Board::h() const noexcept {
    return m_height;
  }

  void
  Board::initialize() noexcept {
    // Initialize the board and any custom initialization.
    m_board = std::vector<unsigned>(w() * h(), 0u);

    const unsigned count = 15u;
    unsigned id = 0u;

    while (id < count) {
      unsigned x = std::rand() % w();
      unsigned y = std::rand() % h();

      unsigned p = std::rand() % 11u;

      m_board[linear(x, y)] = std::pow(2u, p);

      ++id;
    }
  }

  bool
  Board::empty(int x, int y) const {
    if (x >= m_width || y >= m_height) {
      error(
        "Failed to fetch board number",
        "Invalid coordinate " + std::to_string(x) + "x" + std::to_string(y)
      );
    }

    return m_board[linear(x, y)] == 0u;
  }

  unsigned
  Board::at(int x, int y) const {
    if (x >= m_width || y >= m_height) {
      error(
        "Failed to fetch board number",
        "Invalid coordinate " + std::to_string(x) + "x" + std::to_string(y)
      );
    }

    return m_board[linear(x, y)];
  }

  inline
  unsigned
  Board::linear(int x, int y) const noexcept {
    return y * m_width + x;
  }

}


# include "2048.hh"

namespace two48 {

  Game::Game(int width, int height) noexcept:
    utils::CoreObject("board"),

    m_board(width, height)
  {
    setService("2048");

    initialize();
  }

  const Board&
  Game::operator()() const noexcept {
    return m_board;
  }

  void
  Game::initialize() noexcept {
    m_board.initialize();
  }

  unsigned
  Game::moveHorizontally(bool positive) {
    return m_board.moveHorizontally(positive);
  }

  unsigned
  Game::moveVertically(bool positive) {
    return m_board.moveVertically(positive);
  }

}


# include "2048.hh"

namespace two48 {

  Game::Game(unsigned width, unsigned height) noexcept:
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
    // We want two initial tiles with a value of 2 or 4.
    // There's a strong bias towards 2 instead of 4.
    const unsigned count = 2u;
    unsigned id = 0u;

    m_board.reset();

    while (id < count) {
      unsigned v = std::rand() % 100u < 90u ? 2u : 4u;
      m_board.spawn(v);

      ++id;
    }
  }

  unsigned
  Game::moveHorizontally(bool positive) {
    // Handle the move.
    unsigned s = m_board.moveHorizontally(positive);

    // Spawn a random tile: the value is set between
    // 2 and 4 with a strong bias towards 2.
    unsigned v = std::rand() % 100u < 90u ? 2u : 4u;
    m_board.spawn(v);

    return s;
  }

  unsigned
  Game::moveVertically(bool positive) {
    // Handle the move.
    unsigned s = m_board.moveVertically(positive);

    // Spawn a random tile: the value is set between
    // 2 and 4 with a strong bias towards 2.
    unsigned v = std::rand() % 100u < 90u ? 2u : 4u;
    m_board.spawn(v);

    return s;
  }

}

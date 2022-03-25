
# include "2048.hh"

namespace two48 {

  Game::Game(unsigned width, unsigned height, unsigned depth) noexcept:
    utils::CoreObject("board"),

    m_board(width, height, depth)
  {
    setService("2048");

    initialize();
  }

  unsigned
  Game::w() const noexcept {
    return m_board.w();
  }

  unsigned
  Game::h() const noexcept {
    return m_board.h();
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

  void
  Game::undo() {
    // NOTE: We introduce a bias: as we don't save the piece
    // that was spawned, we leave the possibility to undo a
    // move until a piece is what the user expects.
    m_board.undo();
  }

  bool
  Game::canUndo() const noexcept {
    return m_board.canUndo();
  }

  unsigned
  Game::moveHorizontally(bool positive,
                         bool& valid)
  {
    // Check whether the move is valid.
    valid = m_board.canMoveHorizontally(positive);
    if (!valid) {
      return 0u;
    }

    // Handle the move.
    unsigned s = m_board.moveHorizontally(positive);

    // Spawn a random tile: the value is set between
    // 2 and 4 with a strong bias towards 2.
    unsigned v = std::rand() % 100u < 90u ? 2u : 4u;
    m_board.spawn(v);

    return s;
  }

  unsigned
  Game::moveVertically(bool positive,
                       bool& valid)
  {
    // Check whether the move is valid.
    valid = m_board.canMoveVertically(positive);
    if (!valid) {
      return 0u;
    }

    // Handle the move.
    unsigned s = m_board.moveVertically(positive);

    // Spawn a random tile: the value is set between
    // 2 and 4 with a strong bias towards 2.
    unsigned v = std::rand() % 100u < 90u ? 2u : 4u;
    m_board.spawn(v);

    return s;
  }

  bool
  Game::canMove() const noexcept {
    return
      m_board.canMoveHorizontally(true) ||
      m_board.canMoveHorizontally(false) ||
      m_board.canMoveVertically(true) ||
      m_board.canMoveVertically(false)
    ;
  }

  void
  Game::load(const std::string& file) {
    m_board.load(file);
  }

  void
  Game::save(const std::string& file,
             unsigned moves,
             unsigned score) const
  {
    m_board.save(file, moves, score);
  }

}

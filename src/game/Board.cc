
# include "Board.hh"
# include <cmath>

namespace two48 {

  Board::Board(unsigned width,
               unsigned height,
               unsigned depth) noexcept:
    utils::CoreObject("board"),

    m_width(width),
    m_height(height),

    m_board(w() * h(), 0u),

    m_undoStackDepth(depth),
    m_undoStack()
  {
    setService("2048");
  }

  unsigned
  Board::w() const noexcept {
    return m_width;
  }

  unsigned
  Board::h() const noexcept {
    return m_height;
  }

  bool
  Board::empty(unsigned x, unsigned y) const {
    if (x >= m_width || y >= m_height) {
      error(
        "Failed to fetch board number",
        "Invalid coordinate " + std::to_string(x) + "x" + std::to_string(y)
      );
    }

    return m_board[linear(x, y)] == 0u;
  }

  unsigned
  Board::at(unsigned x, unsigned y) const {
    if (x >= m_width || y >= m_height) {
      error(
        "Failed to fetch board number",
        "Invalid coordinate " + std::to_string(x) + "x" + std::to_string(y)
      );
    }

    return m_board[linear(x, y)];
  }

  bool
  Board::canMoveHorizontally(bool positive) const noexcept {
    // We have to make sure that at least one tile can be
    // moved to the corresponding direction or merged.
    bool valid = false;

    unsigned y = 0u;
    while (y < h() && !valid) {
      // Scan the line and check if:
      // - there are two consecutive digits with the same
      //   value.
      // - there is a space on the right direction compared
      //   to the sense of the move.
      unsigned lastDigit = 0u;

      unsigned x = 0u;
      while (x < w() && !valid) {
        unsigned id = (positive ? y * w() + w() - 1u - x : y * w() + x);

        // Case of same consiecutive digits.
        if (lastDigit != 0u && lastDigit == m_board[id]) {
          valid = true;
        }

        // Case of an empty space after a central digit. Note
        // that we need to make sure that it's not the first
        // digit.
        if (lastDigit == 0u && m_board[id] != 0u &&
            ((positive && id % w() != w() - 1u) || (!positive && id % w() != 0u)))
        {
          valid = true;
        }

        lastDigit = m_board[id];

        ++x;
      }

      ++y;
    }

    return valid;
  }

  bool
  Board::canMoveVertically(bool positive) const noexcept {
    // We have to make sure that at least one tile can be
    // moved to the corresponding direction or merged.
    bool valid = false;

    unsigned x = 0u;
    while (x < w() && !valid) {
      // Scan the line and check if:
      // - there are two consecutive digits with the same
      //   value.
      // - there is a space on the right direction compared
      //   to the sense of the move.
      unsigned lastDigit = 0u;

      unsigned y = 0u;
      while (y < h() && !valid) {
        unsigned id = (positive ? y * w() + x : (h() - 1u - y) * w() + x);

        // Case of same consiecutive digits.
        if (lastDigit != 0u && lastDigit == m_board[id]) {
          valid = true;
        }

        // Case of an empty space after a central digit. Note
        // that we need to make sure that it's not the first
        // digit.
        if (lastDigit == 0u && m_board[id] != 0u &&
            ((positive && id / w() != 0) || (!positive && id / w() != h() - 1u)))
        {
          valid = true;
        }

        lastDigit = m_board[id];

        ++y;
      }

      ++x;
    }

    return valid;
  }

  unsigned
  Board::moveHorizontally(bool positive) {
    // Save the current state of the board.
    saveBoard();

    // Move each row horizontally and accumulate the score.
    unsigned score = 0u;

    for (unsigned y = 0u ; y < h() ; ++y) {
      score += collapseRow(y, positive);
    }

    return score;
  }

  unsigned
  Board::moveVertically(bool positive) {
    // Save the current state of the board.
    saveBoard();

    // Move each column horizontally and accumulate the score.
    unsigned score = 0u;

    for (unsigned x = 0u ; x < w() ; ++x) {
      score += collapseColumn(x, positive);
    }

    return score;
  }

  void
  Board::reset() noexcept {
    m_board = std::vector<unsigned>(w() * h(), 0u);
    m_undoStack.clear();
  }

  bool
  Board::spawn(unsigned value) noexcept {
    // Gather available cells.
    std::vector<unsigned> availables;

    for (unsigned id = 0u ; id < m_board.size() ; ++id) {
      if (m_board[id] == 0u) {
        availables.push_back(id);
      }
    }

    if (availables.empty()) {
      return false;
    }

    // Pick a random location and spawn the number.
    unsigned id = availables[std::rand() % availables.size()];
    m_board[id] = value;

    log("Spawning " + std::to_string(value) + " at " + std::to_string(id % w()) + "x" + std::to_string(id / w()), utils::Level::Verbose);

    return true;
  }

  void
  Board::undo() noexcept {
    // In case there is no move to undo, stop here.
    if (m_undoStack.empty()) {
      log("Can't undo move, stack is empty");
      return;
    }

    log("Restoring move, still " + std::to_string(m_undoStack.size()) + " available", utils::Level::Info);

    m_board = m_undoStack.back();
    m_undoStack.pop_back();
  }

  bool
  Board::canUndo() const noexcept {
    return !m_undoStack.empty();
  }

  inline
  unsigned
  Board::linear(unsigned x, unsigned y) const noexcept {
    return y * m_width + x;
  }

  inline
  unsigned
  Board::collapseRow(unsigned y, bool positive) noexcept {
    // Aggregate the elements of the row.
    std::vector<unsigned> numbers;
    for (unsigned x = 0u ; x < w() ; ++x) {
      unsigned v = m_board[linear(x, y)];

      if (v != 0u) {
        numbers.push_back(v);
      }
    }

    // Collapse elements based on the direction we're
    // processing them. Collapsing in the positive
    // direction means that we will process elements
    // in the reverse order of the row.
    unsigned score = 0u;
    std::vector<unsigned> out;

    unsigned x = 0u;
    while (x < numbers.size()) {
      unsigned lid = (positive ? numbers.size() - 1u - x : x);
      unsigned lid2 = (positive ? numbers.size() - 1u - x - 1u : x + 1u);

      // In case we reached the last number, we can't
      // possible merge it.
      if ((positive && lid == 0u) || (!positive && lid == numbers.size() - 1u)) {
        out.push_back(numbers[lid]);
      }
      else {
        // In case both numbers are identical, merge them.
        // Otherwise, keep the first one.
        if (numbers[lid] != numbers[lid2]) {
          out.push_back(numbers[lid]);
        }
        else {
          out.push_back(numbers[lid] * 2u);
          ++x;

          // The score is the value of the new tile.
          score += (numbers[lid] * 2u);
        }
      }
      ++x;
    }

    // Put elements in order at the top or bottom of the
    // row as a result of the collapse.
    unsigned id = 0u;
    while (id < out.size()) {
      unsigned x = positive ? w() - 1u - id : id;

      m_board[linear(x, y)] = out[id];
      ++id;
    }

    // Fill the rest with empty values.
    while (id < w()) {
      unsigned x = positive ? w() - 1u - id : id;

      m_board[linear(x, y)] = 0u;
      ++id;
    }

    return score;
  }

  inline
  unsigned
  Board::collapseColumn(unsigned x, bool positive) noexcept {
    // Aggregate the elements of the column.
    std::vector<unsigned> numbers;
    for (unsigned y = 0u ; y < h() ; ++y) {
      unsigned v = m_board[linear(x, y)];

      if (v != 0u) {
        numbers.push_back(v);
      }
    }

    // Collapse elements based on the direction we're
    // processing them. Collapsing in the positive
    // direction means that we will process elements
    // in the order of the row.
    // We will also accumulate the score here.
    unsigned score = 0u;
    std::vector<unsigned> out;

    unsigned y = 0u;
    while (y < numbers.size()) {
      unsigned lid = (positive ? y : numbers.size() - 1u - y);
      unsigned lid2 = (positive ? y + 1u : numbers.size() - 1u - y - 1u);

      // In case we reached the last number, we can't
      // possible merge it.
      if ((!positive && lid == 0u) || (positive && lid == numbers.size() - 1u)) {
        out.push_back(numbers[lid]);
      }
      else {
        // In case both numbers are identical, merge them.
        // Otherwise, keep the first one.
        if (numbers[lid] != numbers[lid2]) {
          out.push_back(numbers[lid]);
        }
        else {
          out.push_back(numbers[lid] * 2u);
          ++y;

          // The score is the value of the new tile.
          score += (numbers[lid] * 2u);
        }
      }

      ++y;
    }

    // Put elements in order at the top or bottom of the
    // row as a result of the collapse.
    unsigned id = 0u;
    while (id < out.size()) {
      unsigned y = positive ? id: h() - 1u - id;

      m_board[linear(x, y)] = out[id];
      ++id;
    }

    // Fill the rest with empty values.
    while (id < h()) {
      unsigned y = positive ? id : h() - 1u - id;

      m_board[linear(x, y)] = 0u;
      ++id;
    }

    return score;
  }

  inline
  void
  Board::saveBoard() noexcept {
    // Push the current state of the board: in case we
    // already reached the maximum depth of the undo
    // stack, we have to remove the first one.
    if (m_undoStack.size() == m_undoStackDepth) {
      log("Removing first saved move", utils::Level::Verbose);
      m_undoStack.pop_front();
    }

    log("Saving state " + std::to_string(m_undoStack.size()) + "/" + std::to_string(m_undoStackDepth), utils::Level::Verbose);
    m_undoStack.push_back(m_board);
  }

}

#ifndef    BOARD_HH
# define   BOARD_HH

# include <vector>
# include <memory>
# include <deque>
# include <core_utils/CoreObject.hh>

namespace two48 {

  class Board: public utils::CoreObject {
    public:

      /**
       * @brief - Create a new board with the specified dimensions.
       * @param width - the width of the board.
       * @param height - the height of the board.
       * @param depth - the depth of the undo stack.
       */
      Board(unsigned width = 4u,
            unsigned height = 4u,
            unsigned depth = 5u) noexcept;

      /**
       * @brief - The width of the board.
       * @return - the width of the board.
       */
      unsigned
      w() const noexcept;

      /**
       * @brief - The height of the board.
       * @return - the height of the board.
       */
      unsigned
      h() const noexcept;

      /**
       * @brief - Whether or not the position at the specified coords
       *          is empty.
       * @param x - the x coordinates.
       * @param y - the y coordinates.
       * @return - `true` if the cell is empty.
       */
      bool
      empty(unsigned x, unsigned y) const;

      /**
       * @brief - Returns the number at the specified position or zero
       *          in case the cell is empty.
       * @param x - the x coordinates.
       * @param y - the y coordinates.
       * @return - the number at this place.
       */
      unsigned
      at(unsigned x, unsigned y) const;

      /**
       * @brief - Check whether a horizontal move along the specified
       *          direction is possible or not: this is defined when
       *          at least a piece would move.
       * @return - `true` if the move leads to at least one tile moving.
       */
      bool
      canMoveHorizontally(bool positive) const noexcept;

      /**
       * @brief - Check whether a vertical move along the specified
       *          direction is possible or not: this is defined when
       *          at least a piece would move.
       * @return - `true` if the move leads to at least one tile moving.
       */
      bool
      canMoveVertically(bool positive) const noexcept;

      /**
       * @brief - Move the pieces in the board with a horizontal move
       *          which along the positive or negative axis based on
       *          the value of the input boolean.
       * @param positive - whether the move is towards positive x.
       * @return - the number of points brought by the move.
       */
      unsigned
      moveHorizontally(bool positive);

      /**
       * @brief - Move the pieces in the board with a vertical move
       *          which along the positive or negative axis based on
       *          the value of the input boolean.
       * @param positive - whether the move is towards positive y.
       * @return - the number of points brought by the move.
       */
      unsigned
      moveVertically(bool positive);

      /**
       * @brief - Reset all tiles to be 0.
       */
      void
      reset() noexcept;

      /**
       * @brief - Pop a tile with the input value at a random empty
       *          location in the grid.
       * @param value - the value to spawn.
       * @return - `true` in case the tile could be spawned.
       */
      bool
      spawn(unsigned value) noexcept;

      /**
       * @brief - Undo the last move if possible.
       */
      void
      undo() noexcept;

      /**
       * @brief - Whether or not there are some moves to undo.
       * @return - `true` if there are moves to be undone.
       */
      bool
      canUndo() const noexcept;

      /**
       * @brief - Used to perform the saving of this board to the
       *          provided file.
       *          Note that to have a valid save we need to be
       *          provided the current number of moves and score.
       * @param file - the name of the file to save the board to.
       * @param moves - the current number of moves.
       * @param score - the current score.
       */
      void
      save(const std::string& file,
           unsigned moves,
           unsigned score) const;

      /**
       * @brief - Loads the content of the board defined in the
       *          input file and use it to replace the content
       *          of this board.
       * @param file - the file defining the board's data.
       */
      void
      load(const std::string& file);

    private:

      unsigned
      linear(unsigned x, unsigned y) const noexcept;

      /**
       * @brief - Move row horizontally in the specified direction.
       * @param y - the index of the row to process.
       * @param positive - whether the row should be collapsed towards
       *                   positive x or not.
       * @return - how much points the collapse brought.
       */
      unsigned
      collapseRow(unsigned y, bool positive) noexcept;

      /**
       * @brief - Move column horizontally in the specified direction.
       * @param x - the index of the row to process.
       * @param positive - whether the row should be collapsed towards
       *                   positive y or not.
       * @return - how much points the collapse brought.
       */
      unsigned
      collapseColumn(unsigned x, bool positive) noexcept;

      /**
       * @brief - Save the current state of the board and handle the
       *          undo properties.
       */
      void
      saveBoard() noexcept;

    private:

      /**
       * @brief - The width of the board.
       */
      unsigned m_width;

      /**
       * @brief - The height of the board.
       */
      unsigned m_height;

      /**
       * @brief - The current state of the board.
       */
      mutable std::vector<unsigned> m_board;

      /**
       * @brief - The depth of the undo stack.
       */
      unsigned m_undoStackDepth;

      /**
       * @brief - The list of the last moves which can be undone.
       */
      std::deque<std::vector<unsigned>> m_undoStack;
  };

  using BoardShPtr = std::shared_ptr<Board>;
}

#endif    /* BOARD_HH */

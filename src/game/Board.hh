#ifndef    BOARD_HH
# define   BOARD_HH

# include <vector>
# include <memory>
# include <core_utils/CoreObject.hh>

namespace two48 {

  class Board: public utils::CoreObject {
    public:

      explicit
      Board(unsigned width = 4u, unsigned height = 4u) noexcept;

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
       * @param value - the valeu to spawn.
       * @return - `true` in case the tile could be spawned.
       */
      bool
      spawn(unsigned value) noexcept;

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
  };

  using BoardShPtr = std::shared_ptr<Board>;
}

#endif    /* BOARD_HH */

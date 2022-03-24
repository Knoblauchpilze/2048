#ifndef    BOARD_HH
# define   BOARD_HH

# include <vector>
# include <memory>
# include <core_utils/CoreObject.hh>

namespace two48 {

  class Board: public utils::CoreObject {
    public:

      explicit
      Board(int width = 4u, int height = 4u) noexcept;

      /**
       * @brief - The width of the board.
       * @return - the width of the board.
       */
      int
      w() const noexcept;

      /**
       * @brief - The height of the board.
       * @return - the height of the board.
       */
      int
      h() const noexcept;

      /**
       * @brief - Initialize the board with a new game.
       */
      void
      initialize() noexcept;

      /**
       * @brief - Whether or not the position at the specified coords
       *          is empty.
       * @param x - the x coordinates.
       * @param y - the y coordinates.
       * @return - `true` if the cell is empty.
       */
      bool
      empty(int x, int y) const;

      /**
       * @brief - Returns the number at the specified position or zero
       *          in case the cell is empty.
       * @param x - the x coordinates.
       * @param y - the y coordinates.
       * @return - the number at this place.
       */
      unsigned
      at(int x, int y) const;

    private:

      unsigned
      linear(int x, int y) const noexcept;

    private:

      /**
       * @brief - The width of the board.
       */
      int m_width;

      /**
       * @brief - The height of the board.
       */
      int m_height;

      /**
       * @brief - The current state of the board.
       */
      mutable std::vector<unsigned> m_board;
  };

  using BoardShPtr = std::shared_ptr<Board>;
}

#endif    /* BOARD_HH */
#ifndef    GAME_2048_HH
# define   GAME_2048_HH

# include <vector>
# include <unordered_set>
# include <memory>
# include <core_utils/CoreObject.hh>
# include "Board.hh"

namespace two48 {

  class Game: public utils::CoreObject {
    public:

      Game(unsigned width = 4u, unsigned height = 4u) noexcept;

      /**
       * @brief - The width of the board attached to this game.
       * @return - the width in cells.
       */
      unsigned
      w() const noexcept;

      /**
       * @brief - The height of the board attached to this game.
       * @return - the height in cells.
       */
      unsigned
      h() const noexcept;

      /**
       * @brief - Decay the game into its board component.
       * @return - the board used by this 2048 game.
       */
      const Board&
      operator()() const noexcept;

      /**
       * @brief - Initialize the board with a new game.
       */
      void
      initialize() noexcept;

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

    private:

      /**
       * @brief - The current state of the board.
       */
      mutable Board m_board;
  };

  using GameShPtr = std::shared_ptr<Game>;
}

#endif    /* GAME_2048_HH */

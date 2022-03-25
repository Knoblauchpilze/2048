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

      /**
       * @brief - Create a new game with the specified properties.
       * @param width - the width of the board.
       * @param height - the height of the board.
       * @param depth - the undo stack depth.
       */
      Game(unsigned width = 4u,
           unsigned height = 4u,
           unsigned depth = 5u) noexcept;

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
       * @brief - Undo the last move if possible.
       */
      void
      undo();

      /**
       * @brief - Whether or not there are some moves to undo.
       * @return - `true` if there are moves to be undone.
       */
      bool
      canUndo() const noexcept;

      /**
       * @brief - Move the pieces in the board with a horizontal move
       *          which along the positive or negative axis based on
       *          the value of the input boolean.
       * @param positive - whether the move is towards positive x.
       * @param valid - output argument defining whether the move was
       *                valid. If not then the score will be `0` and
       *                the board won't be modified.
       * @return - the number of points brought by the move.
       */
      unsigned
      moveHorizontally(bool positive,
                       bool& valid);

      /**
       * @brief - Move the pieces in the board with a vertical move
       *          which along the positive or negative axis based on
       *          the value of the input boolean.
       * @param positive - whether the move is towards positive y.
       * @param valid - output argument defining whether the move was
       *                valid. If not then the score will be `0` and
       *                the board won't be modified.
       * @return - the number of points brought by the move.
       */
      unsigned
      moveVertically(bool positive,
                     bool& valid);

    private:

      /**
       * @brief - The current state of the board.
       */
      mutable Board m_board;
  };

  using GameShPtr = std::shared_ptr<Game>;
}

#endif    /* GAME_2048_HH */

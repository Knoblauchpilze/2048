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

      Game(int width = 4u, int height = 4u) noexcept;

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

    private:

      /**
       * @brief - The current state of the board.
       */
      mutable Board m_board;
  };

  using GameShPtr = std::shared_ptr<Game>;
}

#endif    /* GAME_2048_HH */

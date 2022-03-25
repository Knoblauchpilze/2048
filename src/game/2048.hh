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

      /**
       * @brief - Whether the board still have valid moves.
       * @return - `true` if a move is still possible.
       */
      bool
      canMove() const noexcept;

      /**
       * @brief - Loads the content of the board defined in the
       *          input file and use it to replace the content
       *          of this board.
       * @param file - the file defining the board's data.
       */
      void
      load(const std::string& file);

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

    private:

      /**
       * @brief - The current state of the board.
       */
      mutable Board m_board;
  };

  using GameShPtr = std::shared_ptr<Game>;
}

#endif    /* GAME_2048_HH */

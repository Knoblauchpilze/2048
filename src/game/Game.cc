
# include "Game.hh"
# include <cxxabi.h>
# include "Menu.hh"

/// @brief - The height of the main menu.
# define STATUS_MENU_HEIGHT 50

/// @brief - The width of the board in cells.
# define BOARD_WIDTH 4

/// @brief - The height of the board in cells.
# define BOARD_HEIGHT 4

/// @brief - The undo stack depth.
# define UNDO_STACK_DEPTH 5

/// @brief - The maximum width of the baord.
# define MAX_BOARD_WIDTH 8

/// @brief - The maximum height of the board.
# define MAX_BOARD_HEIGHT 8

namespace {

  pge::MenuShPtr
  generateMenu(const olc::vi2d& pos,
               const olc::vi2d& size,
               const std::string& text,
               const std::string& name,
               const olc::Pixel& color,
               bool clickable = false,
               bool selectable = false)
  {
    pge::menu::MenuContentDesc fd = pge::menu::newMenuContent(text, "", size);
    fd.color = olc::WHITE;
    fd.hColor = olc::GREY;
    fd.align = pge::menu::Alignment::Center;

    return std::make_shared<pge::Menu>(
      pos,
      size,
      name,
      pge::menu::newColoredBackground(color),
      fd,
      pge::menu::Layout::Horizontal,
      clickable,
      selectable
    );
  }

  pge::MenuShPtr
  generateMessageBoxMenu(const olc::vi2d& pos,
                         const olc::vi2d& size,
                         const std::string& text,
                         const std::string& name,
                         bool alert)
  {
    pge::menu::MenuContentDesc fd = pge::menu::newMenuContent(text, "", size);
    fd.color = (alert ? olc::RED : olc::GREEN);
    fd.align = pge::menu::Alignment::Center;

    return std::make_shared<pge::Menu>(
      pos,
      size,
      name,
      pge::menu::newColoredBackground(alert ? olc::VERY_DARK_RED : olc::VERY_DARK_GREEN),
      fd,
      pge::menu::Layout::Horizontal,
      false,
      false
    );
  }

}

namespace pge {

  Game::Game():
    utils::CoreObject("game"),

    m_state(
      State{
        true,  // paused
        true,  // disabled
        false, // terminated
      }
    ),

    m_menus(),

    m_width(BOARD_WIDTH),
    m_height(BOARD_HEIGHT),
    m_board(std::make_shared<two48::Game>(m_width, m_height, UNDO_STACK_DEPTH)),
    m_moves(0u),
    m_score(0u),
    m_canMove(true)
  {
    setService("game");
  }

  Game::~Game() {}

  std::vector<MenuShPtr>
  Game::generateMenus(float width,
                      float height)
  {
    olc::Pixel bg(250, 248, 239);
    olc::Pixel buttonBG(185, 172, 159);

    // Generate the status menu.
    MenuShPtr status = generateMenu(olc::vi2d(), olc::vi2d(width, STATUS_MENU_HEIGHT), "", "status", bg);

    olc::vi2d pos;
    olc::vi2d dims(50, STATUS_MENU_HEIGHT);
    MenuShPtr mLabel = generateMenu(pos, dims, "Moves:", "moves_label", buttonBG);
    m_menus.moves = generateMenu(pos, dims, "0", "moves", buttonBG);
    MenuShPtr sLabel = generateMenu(pos, dims, "Score:", "score_label", buttonBG);
    m_menus.score = generateMenu(pos, dims, "0", "score", buttonBG);
    m_menus.undo = generateMenu(pos, dims, "Undo", "unro", buttonBG, true);
    MenuShPtr reset = generateMenu(pos, dims, "Reset", "reset", buttonBG, true);

    m_menus.undo->setSimpleAction(
      [](Game& g) {
        g.undo();
      }
    );
    reset->setSimpleAction(
      [](Game& g) {
        g.reset();
      }
    );

    status->addMenu(mLabel);
    status->addMenu(m_menus.moves);
    status->addMenu(sLabel);
    status->addMenu(m_menus.score);
    status->addMenu(m_menus.undo);
    status->addMenu(reset);

    // Generate the board dimensions menu.
    MenuShPtr mDims = generateMenu(olc::vi2d(0, height - STATUS_MENU_HEIGHT), olc::vi2d(width, STATUS_MENU_HEIGHT), "", "dims", bg);

    m_menus.wMinus = generateMenu(pos, dims, "-", "w_minus", buttonBG, true);
    m_menus.width = generateMenu(pos, dims, "0", "width", buttonBG);
    m_menus.wPlus = generateMenu(pos, dims, "+", "w_plus", buttonBG, true);

    m_menus.hMinus = generateMenu(pos, dims, "-", "h_minus", buttonBG, true);
    m_menus.height = generateMenu(pos, dims, "0", "height", buttonBG);
    m_menus.hPlus = generateMenu(pos, dims, "+", "h_plus", buttonBG, true);

    mDims->addMenu(m_menus.wMinus);
    mDims->addMenu(m_menus.width);
    mDims->addMenu(m_menus.wPlus);
    mDims->addMenu(m_menus.hMinus);
    mDims->addMenu(m_menus.height);
    mDims->addMenu(m_menus.hPlus);

    m_menus.wMinus->setSimpleAction(
      [this](Game& g) {
        unsigned w = (m_board->w() > 2u ? m_board->w() - 1u : m_board->w());
        g.setBoardDimensions(w, m_board->h());
      }
    );
    m_menus.wPlus->setSimpleAction(
      [this](Game& g) {
        g.setBoardDimensions(m_board->w() + 1u, m_board->h());
      }
    );
    m_menus.hMinus->setSimpleAction(
      [this](Game& g) {
        unsigned h = (m_board->h() > 2u ? m_board->h() - 1u : m_board->h());
        g.setBoardDimensions(m_board->w(), h);
      }
    );
    m_menus.hPlus->setSimpleAction(
      [this](Game& g) {
        g.setBoardDimensions(m_board->w(), m_board->h() + 1u);
      }
    );

    // Generate the menu to indicate a loss.
    m_menus.lost.date = utils::TimeStamp();
    m_menus.lost.wasActive = false;
    // We display the alert for 3 seconds.
    m_menus.lost.duration = 3000;
    m_menus.lost.menu = generateMessageBoxMenu(
      olc::vi2d((width - 300.0f) / 2.0f, (height - 150.0f) / 2.0f),
      olc::vi2d(300, 150),
      "You lost !",
      "lost",
      true
    );
    m_menus.lost.menu->setVisible(false);

    // Package menus for output.
    std::vector<MenuShPtr> menus;

    menus.push_back(status);
    menus.push_back(mDims);

    menus.push_back(m_menus.lost.menu);

    return menus;
  }

  void
  Game::performAction(float /*x*/, float /*y*/) {
    // Only handle actions when the game is not disabled.
    if (m_state.disabled) {
      debug("Ignoring action while menu is disabled");
      return;
    }
  }

  bool
  Game::step(float /*tDelta*/) {
    // When the game is paused it is not over yet.
    if (m_state.paused) {
      return true;
    }

    updateUI();

    bool done = !m_canMove && !m_menus.lost.menu->visible();
    if (done) {
      pause();
      enable(!m_state.paused);
    }

    return !done;
  }

  void
  Game::togglePause() {
    if (m_state.paused) {
      resume();
    }
    else {
      pause();
    }

    enable(!m_state.paused);
  }

  void
  Game::move(int x, int y) {
    // Do nothing while the game is paused.
    if (m_state.paused) {
      return;
    }

    // Ignore invalid motions.
    if (x != 0 && y != 0) {
      warn(
        "Ignoring invalid motion",
        "Direction is " + std::to_string(x) + "x" + std::to_string(y)
      );

      return;
    }

    // Ignore any move if the user can't do antyhing anymore.
    if (!m_canMove) {
      info("Stop trying, you lost");
      return;
    }

    unsigned score = 0u;
    bool valid = false;

    if (x != 0) {
      score = m_board->moveHorizontally(x > 0, valid);
    }

    if (y != 0) {
      score = m_board->moveVertically(y > 0, valid);
    }

    // In case the move is not valid, do nothing.
    if (!valid) {
      std::string move = "to the right";
      if (x < 0) {
        move = "to the left";
      }
      else if (y > 0) {
        move = "up";
      }
      else if (y < 0) {
        move = "down";
      }

      debug("Ignoring invalid move " + move);
      return;
    }

    m_canMove = m_board->canMove();

    // Update the moves and score.
    verbose("Move " + std::to_string(m_moves) + " brought " + std::to_string(score) + " point(s)");
    ++m_moves;
    m_score += score;
  }

  void
  Game::undo() {
    // Do nothing while the game is paused.
    if (m_state.paused) {
      return;
    }

    // NOTE: We're not updating the number of moves
    // neither the score, which also gives room for
    // a potential score exploit.
    info("Undoing last move");

    m_board->undo();
  }

  void
  Game::reset() {
    info("Creating board with dimensions " + std::to_string(m_width) + "x" + std::to_string(m_height));

    // Reset variables.
    m_moves = 0u;
    m_score = 0u;
    m_board = std::make_shared<two48::Game>(m_width, m_height);
    m_canMove = true;
  }

  const two48::Board&
  Game::board() const noexcept {
    return (*m_board)();
  }

  void
  Game::setBoardDimensions(unsigned w, unsigned h) {
    // Do nothing while the game is paused.
    if (m_state.paused) {
      return;
    }

    // Ignore identical dimensions.
    if (m_board->w() == w && m_board->h() == h) {
      return;
    }

    m_width = w;
    m_height = h;

    reset();
  }

  void
  Game::load(const std::string& file) {
    // Load the board.
    m_board->load(file);

    // Update internal properties.
    m_width = m_board->w();
    m_height = m_board->h();

    m_canMove = m_board->canMove();

    // Read the score and move count from the file: this is
    // written at the beginning of the file, after the header.
    std::ifstream out(file.c_str());
    if (!out.good()) {
      error("Failed to load board from file \"" + file + "\"", "No such file");
    }

    // Read dimensions of the board.
    unsigned buf;
    out.read(reinterpret_cast<char*>(&buf), sizeof(unsigned));
    out.read(reinterpret_cast<char*>(&buf), sizeof(unsigned));

    // Read the number of moves and the score.
    out.read(reinterpret_cast<char*>(&m_moves), sizeof(unsigned));
    out.read(reinterpret_cast<char*>(&m_score), sizeof(unsigned));
  }

  void
  Game::save(const std::string& file) const {
    // Save the file including the number of moves and the score.
    m_board->save(file, m_moves, m_score);
  }

  void
  Game::enable(bool enable) {
    m_state.disabled = !enable;

    if (m_state.disabled) {
      verbose("Disabled game UI");
    }
    else {
      verbose("Enabled game UI");
    }
  }

  void
  Game::updateUI() {
    // Update moves and score.
    m_menus.moves->setText(std::to_string(m_moves));
    m_menus.score->setText(std::to_string(m_score));

    // Update the undo button.
    m_menus.undo->setEnabled(m_board->canUndo());

    // Update board dimensions.
    m_menus.width->setText(std::to_string(m_width));
    m_menus.height->setText(std::to_string(m_height));

    m_menus.wMinus->setEnabled(m_width > 2u);
    m_menus.wPlus->setEnabled(m_width < MAX_BOARD_WIDTH);
    m_menus.hMinus->setEnabled(m_height > 2u);
    m_menus.hPlus->setEnabled(m_height < MAX_BOARD_HEIGHT);

    // Update the menu indicating that the user lost.
    m_menus.lost.update(!m_canMove);
  }

  bool
  Game::TimedMenu::update(bool active) noexcept {
    // In case the menu should be active.
    if (active) {
      if (!wasActive) {
        // Make it active if it's the first time that
        // we detect that it should be active.
        date = utils::now();
        wasActive = true;
        menu->setVisible(true);
      }
      else if (utils::now() > date + utils::toMilliseconds(duration)) {
        // Deactivate the menu in case it's been active
        // for too long.
        menu->setVisible(false);
      }
      else {
        // Update the alpha value in case it's active
        // for not long enough.
        olc::Pixel c = menu->getBackgroundColor();

        float d = utils::diffInMs(date, utils::now()) / duration;
        c.a = static_cast<uint8_t>(
          std::clamp((1.0f - d) * pge::alpha::Opaque, 0.0f, 255.0f)
        );
        menu->setBackground(pge::menu::newColoredBackground(c));
      }
    }
    // Or if the menu shouldn't be active anymore and
    // it's the first time we detect that.
    else if (wasActive) {
      // Deactivate the menu.
      menu->setVisible(false);
      wasActive = false;
    }

    return menu->visible();
  }

}


# include "Game.hh"
# include <cxxabi.h>
# include "Menu.hh"

/// @brief - The height of the main menu.
# define STATUS_MENU_HEIGHT 50

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

}

namespace pge {

  Game::Game(two48::GameShPtr board):
    utils::CoreObject("game"),

    m_state(
      State{
        true,  // paused
        true,  // disabled
        false, // terminated
      }
    ),

    m_menus(),

    m_board(board),
    m_moves(0u),
    m_score(0u)
  {
    setService("game");
  }

  Game::~Game() {}

  std::vector<MenuShPtr>
  Game::generateMenus(float width,
                      float /*height*/)
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
    MenuShPtr reset = generateMenu(pos, dims, "Reset", "reset", buttonBG, true);

    reset->setSimpleAction(
      [](Game& g) {
        g.reset();
      }
    );

    status->addMenu(mLabel);
    status->addMenu(m_menus.moves);
    status->addMenu(sLabel);
    status->addMenu(m_menus.score);
    status->addMenu(reset);

    // Package menus for output.
    std::vector<MenuShPtr> menus;

    menus.push_back(status);

    return menus;
  }

  void
  Game::performAction(float /*x*/, float /*y*/) {
    // Only handle actions when the game is not disabled.
    if (m_state.disabled) {
      log("Ignoring action while menu is disabled");
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

    return true;
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

    unsigned score = 0u;

    if (x != 0) {
      score = m_board->moveHorizontally(x > 0);
    }

    if (y != 0) {
      score = m_board->moveVertically(y > 0);
    }

    // Update the moves and score.
    log("Move " + std::to_string(m_moves) + " brought " + std::to_string(score) + " point(s)", utils::Level::Verbose);
    ++m_moves;
    m_score += score;
  }

  void
  Game::reset() {
    // Do nothing while the game is paused.
    if (m_state.paused) {
      return;
    }

    // Reset variables.
    m_moves = 0u;
    m_score = 0u;
    m_board->initialize();
  }

  void
  Game::enable(bool enable) {
    m_state.disabled = !enable;

    if (m_state.disabled) {
      log("Disabled game UI", utils::Level::Verbose);
    }
    else {
      log("Enabled game UI", utils::Level::Verbose);
    }
  }

  void
  Game::updateUI() {
    // Update moves and score.
    m_menus.moves->setText(std::to_string(m_moves));
    m_menus.score->setText(std::to_string(m_score));
  }

}

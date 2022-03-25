
# include "Game.hh"
# include <cxxabi.h>
# include "Menu.hh"

/// @brief - The height of the main menu.
# define STATUS_MENU_HEIGHT 50

/// @brief - The width of the board in cells.
# define BOARD_WIDTH 3

/// @brief - The height of the board in cells.
# define BOARD_HEIGHT 5

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
    m_board(std::make_shared<two48::Game>(m_width, m_height)),
    m_moves(0u),
    m_score(0u)
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

    // Package menus for output.
    std::vector<MenuShPtr> menus;

    menus.push_back(status);
    menus.push_back(mDims);

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

    log("Creating board with dimensions " + std::to_string(m_width) + "x" + std::to_string(m_height), utils::Level::Info);

    // Reset variables.
    m_moves = 0u;
    m_score = 0u;
    m_board = std::make_shared<two48::Game>(m_width, m_height);
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

    // Update board dimensions.
    m_menus.width->setText(std::to_string(m_width));
    m_menus.height->setText(std::to_string(m_height));

    m_menus.wMinus->setEnabled(m_width > 2u);
    m_menus.wPlus->setEnabled(m_width < MAX_BOARD_WIDTH);
    m_menus.hMinus->setEnabled(m_height > 2u);
    m_menus.hPlus->setEnabled(m_height < MAX_BOARD_HEIGHT);
  }

}

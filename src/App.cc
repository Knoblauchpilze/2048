
# include "App.hh"
# include <maths_utils/ComparisonUtils.hh>

/// @brief - The border to have between cells.
# define CELL_BORDER 0.1f

/// @brief - The width of the board in cells.
# define BOARD_WIDTH 3

/// @brief - The height of the board in cells.
# define BOARD_HEIGHT 5

namespace {

  olc::Pixel
  backgroundFromNumber(unsigned number) noexcept {
    if (number <= 2u) {
      return olc::Pixel(238, 228, 218);
    }
    else if (number <= 4u) {
      return olc::Pixel(236, 224, 200);
    }
    else if (number <= 8u) {
      return olc::Pixel(238, 177, 123);
    }
    else if (number <= 16u) {
      return olc::Pixel(242, 150, 105);
    }
    else if (number <= 32u) {
      return olc::Pixel(240, 125, 98);
    }
    else if (number <= 64u) {
      return olc::Pixel(243, 95, 65);
    }
    else if (number <= 128u) {
      return olc::Pixel(233, 206, 119);
    }
    else if (number <= 256u) {
      return olc::Pixel(236, 203, 103);
    }
    else if (number <= 512u) {
      return olc::Pixel(236, 200, 89);
    }
    else if (number <= 1024u) {
      return olc::Pixel(231, 194, 87);
    }
    else if (number <= 2048u) {
      return olc::Pixel(232, 189, 77);
    }
    else {
      return olc::Pixel(44, 44, 44);
    }
  }

  olc::Pixel
  colorFromNumber(unsigned number) noexcept {
    return (number < 4u ? olc::Pixel(101, 95, 83) : olc::WHITE);
  }

  olc::vf2d
  cellToCoords(float x, float y, unsigned w, unsigned h) noexcept {
    return olc::vf2d(4.0f - w / 2.0f + x, 4.0f - h / 2.0f + y);
  }

  olc::vf2d
  coordsToCell(float x, float y, unsigned w, unsigned h) noexcept{
    return olc::vf2d(
      std::floor(x - 3.5f + w / 2.0f),
      std::floor(y - 3.5f + h / 2.0f)
    );
  }

}

namespace pge {

  App::App(const AppDesc& desc):
    PGEApp(desc),

    m_game(nullptr),
    m_state(nullptr),
    m_menus(),

    m_packs(std::make_shared<TexturePack>()),

    m_directionalState(Direction::Count, false),
    m_board(std::make_shared<two48::Game>(BOARD_WIDTH, BOARD_HEIGHT))
  {}

  bool
  App::onFrame(float fElapsed) {
    // Handle case where no game is defined.
    if (m_game == nullptr) {
      return false;
    }

    if (!m_game->step(fElapsed)) {
      log("This is game over", utils::Level::Info);
    }

    return m_game->terminated();
  }

  void
  App::onInputs(const controls::State& c,
                const CoordinateFrame& cf)
  {
    // Handle case where no game is defined.
    if (m_game == nullptr) {
      return;
    }

    // Handle menus update and process the
    // corresponding actions.
    std::vector<ActionShPtr> actions;
    bool relevant = false;

    for (unsigned id = 0u ; id < m_menus.size() ; ++id) {
      menu::InputHandle ih = m_menus[id]->processUserInput(c, actions);
      relevant = (relevant || ih.relevant);
    }

    if (m_state != nullptr) {
      menu::InputHandle ih = m_state->processUserInput(c, actions);
      relevant = (relevant || ih.relevant);
    }

    for (unsigned id = 0u ; id < actions.size() ; ++id) {
      actions[id]->apply(*m_game);
    }

    bool lClick = (c.buttons[controls::mouse::Left] == controls::ButtonState::Released);
    if (lClick && !relevant) {
      olc::vf2d it;
      olc::vi2d tp = cf.pixelCoordsToTiles(olc::vi2d(c.mPosX, c.mPosY), &it);

      m_game->performAction(tp.x + it.x, tp.y + it.y);
    }

    if (!c.keys[controls::keys::Right] && m_directionalState[Direction::Right]) {
      m_game->move(1, 0);
    }
    if (!c.keys[controls::keys::Up] && m_directionalState[Direction::Up]) {
      m_game->move(0, 1);
    }
    if (!c.keys[controls::keys::Left] && m_directionalState[Direction::Left]) {
      m_game->move(-1, 0);
    }
    if (!c.keys[controls::keys::Down] && m_directionalState[Direction::Down]) {
      m_game->move(0, -1);
    }
    m_directionalState[Direction::Right] = c.keys[controls::keys::Right];
    m_directionalState[Direction::Up] = c.keys[controls::keys::Up];
    m_directionalState[Direction::Left] = c.keys[controls::keys::Left];
    m_directionalState[Direction::Down] = c.keys[controls::keys::Down];

    if (c.keys[controls::keys::P]) {
      m_game->togglePause();
    }
    if (c.keys[controls::keys::N] || c.keys[controls::keys::R]) {
      m_game->reset();
    }
  }

  void
  App::loadData() {
    // Create the game and its state.
    m_game = std::make_shared<Game>(m_board);

    // Automatically set the game to not paused.
    m_game->togglePause();
  }

  void
  App::loadResources() {
    // Assign a specific tint to the regular
    // drawing layer so that we have a built
    // in transparency.
    // We can't do it directly when drawing
    // in the rendering function because as
    // the whole layer will be drawn as one
    // quad in opengl with an opaque alpha,
    // we will lose this info.
    // This means that everything is indeed
    // transparent but that's the only way
    // for now to achieve it.
    setLayerTint(Layer::Draw, olc::Pixel(255, 255, 255, alpha::SemiOpaque));

    log("Load app resources in the 'm_packs' attribute", utils::Level::Info);
  }

  void
  App::loadMenuResources() {
    // Generate the game state.
    m_state = std::make_shared<GameState>(
      olc::vi2d(ScreenWidth(), ScreenHeight()),
      Screen::Game
    );

    m_menus = m_game->generateMenus(ScreenWidth(), ScreenHeight());
  }

  void
  App::cleanResources() {
    if (m_packs != nullptr) {
      m_packs.reset();
    }
  }

  void
  App::cleanMenuResources() {
    m_menus.clear();
  }

  void
  App::drawDecal(const RenderDesc& res) {
    // Clear rendering target.
    SetPixelMode(olc::Pixel::ALPHA);
    Clear(olc::VERY_DARK_GREY);

    // In case we're not in the game screen, do nothing.
    if (m_state->getScreen() != Screen::Game) {
      SetPixelMode(olc::Pixel::NORMAL);
      return;
    }

    const two48::Board& b = (*m_board)();

    SpriteDesc sd = {};
    sd.loc = pge::RelativePosition::Center;
    sd.radius = std::max(b.w(), b.h()) + CELL_BORDER;

    sd.x = 3.5f;
    sd.y = 3.5f;

    sd.sprite.tint = olc::Pixel(184, 171, 158);

    // The surrounding layer.
    drawRect(sd, res.cf);

    drawBoard(res);
    drawNumbers(res);
    drawOverlays(res);

    SetPixelMode(olc::Pixel::NORMAL);
  }

  void
  App::draw(const RenderDesc& /*res*/) {
    // Clear rendering target.
    SetPixelMode(olc::Pixel::ALPHA);
    Clear(olc::Pixel(255, 255, 255, alpha::Transparent));

    // In case we're not in game mode, just render
    // the state.
    if (m_state->getScreen() != Screen::Game) {
      m_state->render(this);
      SetPixelMode(olc::Pixel::NORMAL);
      return;
    }

    SetPixelMode(olc::Pixel::NORMAL);
  }

  void
  App::drawUI(const RenderDesc& /*res*/) {
    // Clear rendering target.
    SetPixelMode(olc::Pixel::ALPHA);
    Clear(olc::Pixel(255, 255, 255, alpha::Transparent));

    // In case we're not in game mode, just render
    // the state.
    if (m_state->getScreen() != Screen::Game) {
      m_state->render(this);
      SetPixelMode(olc::Pixel::NORMAL);
      return;
    }

    // Render the game menus.
    for (unsigned id = 0u ; id < m_menus.size() ; ++id) {
      m_menus[id]->render(this);
    }

    SetPixelMode(olc::Pixel::NORMAL);
  }

  void
  App::drawDebug(const RenderDesc& res) {
    // Clear rendering target.
    SetPixelMode(olc::Pixel::ALPHA);
    Clear(olc::Pixel(255, 255, 255, alpha::Transparent));

    // In case we're not in game mode, just render
    // the state.
    if (m_state->getScreen() != Screen::Game) {
      m_state->render(this);
      SetPixelMode(olc::Pixel::NORMAL);
      return;
    }

    // Draw cursor's position.
    olc::vi2d mp = GetMousePos();
    olc::vf2d it;
    olc::vi2d mtp = res.cf.pixelCoordsToTiles(mp, &it);

    int h = GetDrawTargetHeight();
    int dOffset = 15;
    DrawString(olc::vi2d(0, h / 2), "Mouse coords      : " + toString(mp), olc::CYAN);
    DrawString(olc::vi2d(0, h / 2 + 1 * dOffset), "World cell coords : " + toString(mtp), olc::CYAN);
    DrawString(olc::vi2d(0, h / 2 + 2 * dOffset), "Intra cell        : " + toString(it), olc::CYAN);

    SetPixelMode(olc::Pixel::NORMAL);
  }

  void
  App::drawBoard(const RenderDesc& res) noexcept {
    // The colors of the cells.
    olc::Pixel empty(203, 191, 178);

    SpriteDesc sd = {};
    sd.loc = pge::RelativePosition::Center;
    // Assume that the tile size is a square and scale
    // the tiles so that they occupy one tile.
    sd.radius = 1.0f - CELL_BORDER;

    const two48::Board& b = (*m_board)();

    // Draw the board.
    for (unsigned y = 0u ; y < b.h() ; ++y) {
      for (unsigned x = 0u ; x < b.w() ; ++x) {
        olc::vf2d p = cellToCoords(x, y, b.w(), b.h());
        sd.x = p.x;
        sd.y = p.y;

        sd.sprite.tint = b.empty(x, y) ? empty : backgroundFromNumber(b.at(x, y));

        drawRect(sd, res.cf);
      }
    }
  }

  void
  App::drawNumbers(const RenderDesc& res) noexcept {
    const two48::Board& b = (*m_board)();

    // The scale for the numbers.
    olc::vf2d scale(2.0f, 2.0f);

    // Draw the numbers.
    for (unsigned y = 0u ; y < b.h() ; ++y) {
      for (unsigned x = 0u ; x < b.w() ; ++x) {
        if (b.empty(x, y)) {
          continue;
        }

        unsigned val = b.at(x, y);
        std::string str = std::to_string(val);

        olc::vf2d p = cellToCoords(x + 0.5f, y + 0.5f, b.w(), b.h());

        olc::vf2d pNum = res.cf.tileCoordsToPixels(p.x, p.y, pge::RelativePosition::Center, 1.0f);

        // More or less center the text in the cell.
        olc::vi2d sz = GetTextSize(str);
        pNum.x -= sz.x;
        pNum.y -= sz.y;

        DrawStringDecal(pNum, str, colorFromNumber(val), scale);
      }
    }
  }

  void
  App::drawOverlays(const RenderDesc& res) noexcept {
    SpriteDesc sd = {};
    sd.loc = pge::RelativePosition::Center;
    sd.radius = 1.0f - CELL_BORDER;

    const two48::Board& b = (*m_board)();

    // Draw the overlay in case the mouse is over
    // a cell with a piece.
    olc::vi2d mp = GetMousePos();
    olc::vf2d it;
    olc::vi2d mtp = res.cf.pixelCoordsToTiles(mp, &it);

    // Convert raw cells to coordinates.
    olc::vf2d cp = coordsToCell(mtp.x + it.x, mtp.y + it.y, b.w(), b.h());

    bool validX = (cp.x >= 0 && static_cast<unsigned>(cp.x) < b.w());
    bool validY = (cp.y >= 0 && static_cast<unsigned>(cp.y) < b.h());
    if (validX && validY) {
      olc::vf2d p = cellToCoords(cp.x, cp.y, b.w(), b.h());
      sd.x = p.x;
      sd.y = p.y;

      sd.sprite.tint = olc::Pixel(101, 95, 89, pge::alpha::AlmostTransparent);
      drawRect(sd, res.cf);
    }
  }

}

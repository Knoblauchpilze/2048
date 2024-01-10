
/**
 * @brief - A clone of the 2048 board game. See more info here:
 *          https://github.com/gabrielecirulli/2048
 */

# include <core_utils/log/StdLogger.hh>
# include <core_utils/log/PrefixedLogger.hh>
# include <core_utils/log/Locator.hh>
# include <core_utils/CoreException.hh>
# include "AppDesc.hh"
# include "TopViewFrame.hh"
# include "App.hh"

int
main(int /*argc*/, char** /*argv*/) {
  // Create the logger.
  utils::log::StdLogger raw;
  raw.setLevel(utils::log::Severity::DEBUG);
  utils::log::PrefixedLogger logger("pge", "main");
  utils::log::Locator::provide(&raw);

  try {
    logger.notice("Starting application");

    pge::Viewport tViewport = pge::Viewport(olc::vf2d(-1.5f, -1.5f), olc::vf2d(10.0f, 10.0f));
    pge::Viewport pViewport = pge::Viewport(olc::vf2d(0.0f, 0.0f), olc::vf2d(768.0f, 768.0f));

    pge::CoordinateFrameShPtr cf = std::make_shared<pge::TopViewFrame>(
      tViewport,
      pViewport,
      olc::vi2d(64, 64)
    );
    pge::AppDesc ad = pge::newDesc(olc::vi2d(768, 768), cf, "2048");
    ad.fixedFrame = true;
    pge::App demo(ad);

    demo.Start();
  }
  catch (const utils::CoreException& e) {
    logger.error("Caught internal exception while setting up application", e.what());
  }
  catch (const std::exception& e) {
    logger.error("Caught internal exception while setting up application", e.what());
  }
  catch (...) {
    logger.error("Unexpected error while setting up application");
  }

  return EXIT_SUCCESS;
}


target_sources (main-app_lib PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Board.cc
	${CMAKE_CURRENT_SOURCE_DIR}/2048.cc

	${CMAKE_CURRENT_SOURCE_DIR}/Game.cc
	${CMAKE_CURRENT_SOURCE_DIR}/SavedGames.cc
	${CMAKE_CURRENT_SOURCE_DIR}/GameState.cc
	)

target_include_directories (main-app_lib PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}"
	)

add_executable(qt-tests EXCLUDE_FROM_ALL
  QtTestsMain.cpp
  SignalRecorder.h
  NullConfigLoader.h
  ConfigChangeBroadcasterTest.cpp
  ConfigBinderRoundTripTest.cpp
  ConfigBinderMappedTest.cpp
  ConfigBinderFloatAndPathTest.cpp
  ConfigBinderOverrideTest.cpp
  BalloonTipFilterTest.cpp
  ConfigComplexBindingTest.cpp
  ConfigSettingRegistryTest.cpp
  EmulationStatusWidgetTest.cpp
  MainWindowUiTest.cpp
  ${CMAKE_SOURCE_DIR}/Source/Core/DolphinQt/EmulationStatusWidget.cpp
  ${CMAKE_SOURCE_DIR}/Source/Core/DolphinQt/GameCount.cpp
  ${CMAKE_SOURCE_DIR}/Source/Core/DolphinQt/QtUtils/NonAutodismissibleMenu.cpp
  ${CMAKE_SOURCE_DIR}/Source/Core/DolphinQt/MainWindow.ui
  # ToolTipStyle reads Config::MAIN_USE_HIGH_CONTRAST_TOOLTIPS, which pulls in MainSettings.cpp and
  # with it the Host_* callbacks. StubHost.cpp includes Core/Host.h, so every stub signature is
  # compiler-checked against the declaration.
  ${CMAKE_SOURCE_DIR}/Source/UnitTests/StubHost.cpp
)
set_target_properties(qt-tests PROPERTIES FOLDER Tests)
set_target_properties(qt-tests PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Tests
)
# Must be a target property, not the CMAKE_AUTOMOC variable: the variable is only
# read when a target is created, so setting it here would silently do nothing.
set_target_properties(qt-tests PROPERTIES AUTOMOC ON)
set_target_properties(qt-tests PROPERTIES AUTOUIC ON)
set_target_properties(qt-tests PROPERTIES
  AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/Source/Core/DolphinQt
)
target_include_directories(qt-tests PRIVATE
  ${CMAKE_SOURCE_DIR}/Source/Core
  ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(qt-tests PRIVATE
  dolphinqt-config-binder
  gtest::gtest
  core
  uicommon
  fmt::fmt
)
add_test(NAME qt-tests COMMAND qt-tests -platform offscreen)
add_dependencies(unittests qt-tests)

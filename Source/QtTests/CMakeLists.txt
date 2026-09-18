add_executable(qt-tests EXCLUDE_FROM_ALL
  QtTestsMain.cpp
  SignalRecorder.h
  NullConfigLoader.h
  ConfigChangeBroadcasterTest.cpp
  ConfigBinderRoundTripTest.cpp
  ConfigBinderMappedTest.cpp
  ConfigBinderFloatAndPathTest.cpp
  ConfigBinderOverrideTest.cpp
)
set_target_properties(qt-tests PROPERTIES FOLDER Tests)
set_target_properties(qt-tests PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Tests
)
# Must be a target property, not the CMAKE_AUTOMOC variable: the variable is only
# read when a target is created, so setting it here would silently do nothing.
set_target_properties(qt-tests PROPERTIES AUTOMOC ON)
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

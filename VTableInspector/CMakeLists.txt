add_executable (VTableInspector "test.cpp" "VTableInspector.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET VTableInspector PROPERTY CXX_STANDARD 20)
endif()

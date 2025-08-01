# CMakeList.txt : CMake project for event-streaming, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (event-streaming "src/main.cpp"  "src/core/eventType.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET event-streaming PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

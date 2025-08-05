# CMakeList.txt : CMake project for event-streaming, include source and define
# project specific logic here.
#

# Add source to this project's executable.

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

add_executable (event-streaming  ${SOURCES})

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(event-streaming PRIVATE nlohmann_json::nlohmann_json)

if (WIN32)
  target_link_libraries(event-streaming ws2_32.lib)
endif()
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET event-streaming PROPERTY CXX_STANDARD 20)
endif()


# TODO: Add tests and install targets if needed.

project(grapecli)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

FetchContent_Declare(
  cxxopts
  GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
  GIT_TAG        v3.1.1
)
FetchContent_MakeAvailable(cxxopts)

add_executable(grapecli main.cpp)
target_link_libraries(grapecli PRIVATE grapedb_lib cxxopts::cxxopts)
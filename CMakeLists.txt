set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB CPP_FILES "*.cpp")

include_directories(${PROJECT_SOURCE_DIR})
add_library(EXECUTOR STATIC ${CPP_FILES})
target_include_directories(EXECUTOR PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.c include/glad/*.h)
add_library(gladLib STATIC ${srcs})

target_include_directories(gladLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
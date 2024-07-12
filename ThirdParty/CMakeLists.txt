#--------------------------------------------------------------------
# Add subdirectories
#--------------------------------------------------------------------
#add_subdirectory(glad)
#set(GLAD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/glad/include" CACHE PATH "the include path of glad")
#target_include_directories(${PROJECT_NAME} PUBLIC GLAD_INCLUDE_DIR)
#target_link_libraries(${PROJECT_NAME} PUBLIC gladLib)

#--------------------------------------------------------------------
# Add subdirectories
#--------------------------------------------------------------------
#add_subdirectory(glfw-3.4)
#set(GLFW_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.4/include" CACHE PATH "the include path of glfw")
#target_include_directories(${PROJECT_NAME} PUBLIC GLFW_INCLUDE_DIR)
#target_link_libraries(${PROJECT_NAME} PUBLIC glfw)

#--------------------------------------------------------------------
# Add subdirectories
#--------------------------------------------------------------------
#add_subdirectory(googletest-main)
#set(GOOGLETEST_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/googletest-main/googletest/include" CACHE PATH "the include path of googletest-main")
#include_directories(${PROJECT_NAME} ${GOOGLETEST_INCLUDE_DIR})
#target_link_libraries(${PROJECT_NAME} PUBLIC gtest gtest_main)


add_subdirectory(glad)
set(GLAD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/glad/include" CACHE PATH "the include path of glad")

add_subdirectory(freetype-2.13.2)
set(FREETYPE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/freetype-2.13.2/include" CACHE PATH "the include path of FreeType")

add_subdirectory(tinyXML)
set(TINYXML_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tinyXML" CACHE PATH "the include path of tinyXML")

#add_subdirectory(Eigen)
set(EIGEN_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/" CACHE PATH "the include path of Eigen")
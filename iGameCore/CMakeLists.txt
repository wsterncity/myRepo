set(MODULE_NAME iGameCore_module)

file(GLOB_RECURSE IGAME_CORE_FILE CONFIGURE_DEPENDS ./*.h ./*.cpp)
add_library(${MODULE_NAME} STATIC ${IGAME_CORE_FILE})

target_include_directories(${MODULE_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Core/CellModel
        ${CMAKE_CURRENT_SOURCE_DIR}/Core/Common
        ${CMAKE_CURRENT_SOURCE_DIR}/Core/DataModel
        ${CMAKE_CURRENT_SOURCE_DIR}/Core/FilterModel
        ${CMAKE_CURRENT_SOURCE_DIR}/Core/Mapper
        ${CMAKE_CURRENT_SOURCE_DIR}/IO
        ${CMAKE_CURRENT_SOURCE_DIR}/Filters
        ${CMAKE_CURRENT_SOURCE_DIR}/Filters/*

        ${CMAKE_CURRENT_SOURCE_DIR}/Rendering
        ${CMAKE_CURRENT_SOURCE_DIR}/Rendering/Common
        ${CMAKE_CURRENT_SOURCE_DIR}/Rendering/Core
        ${CMAKE_CURRENT_SOURCE_DIR}/Rendering/Core/Interactor
        ${CMAKE_CURRENT_SOURCE_DIR}/Rendering/OpenGL

        ${EIGEN_INCLUDE_DIR}
)

message("moudule : ${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(${MODULE_NAME} PUBLIC
        gladLib
        freetype
        meshoptimizer
        tinyXML2
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${MODULE_NAME})
target_compile_definitions(${MODULE_NAME} PRIVATE ASSEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Rendering/Assests")

IF (WIN32)
    target_compile_definitions(${MODULE_NAME} PUBLIC IGAME_OPENGL_VERSION_330)
ELSEIF (APPLE)
    target_compile_definitions(${MODULE_NAME} PUBLIC IGAME_OPENGL_VERSION_330)
ELSEIF (UNIX)
    target_compile_definitions(${MODULE_NAME} PUBLIC IGAME_OPENGL_VERSION_330)
ENDIF ()
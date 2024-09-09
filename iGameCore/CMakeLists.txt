set(MODULE_NAME iGameCore_module)

## 手动设置 HDF5 库和头文件的路径
#set(HDF5_ROOT "C:/Program Files/HDF_Group/HDF5/1.14.3")
#set(HDF5_INCLUDE_DIR "${HDF5_ROOT}/include")
#set(HDF5_LIBRARY_DIR "${HDF5_ROOT}/lib")
#set(HDF5_LIBRARIES "${HDF5_LIBRARY_DIR}/hdf5.lib")
#
## 设置包含目录
#include_directories("C:/Program Files (x86)/cgns/include")
#
## 添加静态库
#add_library(cgns STATIC IMPORTED)
#set_target_properties(cgns PROPERTIES IMPORTED_LOCATION "C:/Program Files (x86)/cgns/lib/cgns.lib")
#
## 添加 HDF5 库
#add_library(hdf5 SHARED IMPORTED)
#set_target_properties(hdf5 PROPERTIES
#    IMPORTED_LOCATION "${HDF5_ROOT}/bin/hdf5.dll"  # 指向 DLL 文件
#    IMPORTED_IMPLIB "${HDF5_LIBRARIES}"            # 指向导入库
#)
#

file(GLOB_RECURSE IGAME_CORE_FILE CONFIGURE_DEPENDS ./*.h ./*.cpp)
add_library(${MODULE_NAME} STATIC ${IGAME_CORE_FILE}  "IO/OBJ/igameOBJWriter.h")

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
)
target_link_libraries(${MODULE_NAME} PUBLIC
        gladLib
        freetype
        eigen
        meshoptimizer
        tinyXML2

        #        hdf5
        #        cgns
)

if (ENABLE_CGNS_MODULE)
    target_compile_definitions(${MODULE_NAME} PRIVATE CGNS_ENABLE)
    target_link_libraries(${MODULE_NAME} PUBLIC cgns_static)
ENDIF ()
target_compile_definitions(${MODULE_NAME} PRIVATE ASSEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Rendering/Assests")

IF (WIN32)
    target_compile_definitions(${MODULE_NAME} PUBLIC IGAME_OPENGL_VERSION_460)
ELSEIF (APPLE)
    target_compile_definitions(${MODULE_NAME} PUBLIC IGAME_OPENGL_VERSION_330)
ELSEIF (UNIX)
    target_compile_definitions(${MODULE_NAME} PUBLIC IGAME_OPENGL_VERSION_330)
ENDIF ()


if (WIN32)
    #  内部库在cmake构建时定义IGAME_COMPILE_EXPORT，使得模式切换为dllexport, 否则外部库在使用编译这个动态库代码时将其模式切换为dllimport
    target_compile_definitions(${MODULE_NAME} PRIVATE IGAME_COMPILE_EXPORT)
    #  由于Windows不会搜索其他目录下的dll文件因此要使用cmake指令将dll移至exe目录
    #  将第三方模块编译成的可执行文件(.exe)、静态库(.lib)、动态库(.dll)的输出移至放入主程序源码目录
    set_property(TARGET ${MODULE_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
    set_property(TARGET ${MODULE_NAME} PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
    set_property(TARGET ${MODULE_NAME} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
    set_property(TARGET ${MODULE_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR})
    set_property(TARGET ${MODULE_NAME} PROPERTY ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR})
    set_property(TARGET ${MODULE_NAME} PROPERTY LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR})
    set_property(TARGET ${MODULE_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR})
    set_property(TARGET ${MODULE_NAME} PROPERTY ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR})
    set_property(TARGET ${MODULE_NAME} PROPERTY LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR})
endif ()

target_link_libraries(${PROJECT_NAME} PUBLIC ${MODULE_NAME})

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
add_library(${MODULE_NAME} STATIC ${IGAME_CORE_FILE}   )

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

#        ${CMAKE_SOURCE_DIR}/

        ${EIGEN_INCLUDE_DIR}
)

message("moudule : ${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(${MODULE_NAME} PUBLIC
        gladLib
        freetype
        meshoptimizer
        tinyXML2

#        hdf5
#        cgns
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${MODULE_NAME})
if(ENABLE_CGNS_MODULE)
    target_compile_definitions(${MODULE_NAME} PRIVATE CGNS_ENABLE)
    target_link_libraries(${MODULE_NAME} PUBLIC cgns_static )
ENDIF ()
target_compile_definitions(${MODULE_NAME} PRIVATE ASSEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Rendering/Assests")

IF (WIN32)
    target_compile_definitions(${MODULE_NAME} PUBLIC IGAME_OPENGL_VERSION_460)
ELSEIF (APPLE)
    target_compile_definitions(${MODULE_NAME} PUBLIC IGAME_OPENGL_VERSION_330)
ELSEIF (UNIX)
    target_compile_definitions(${MODULE_NAME} PUBLIC IGAME_OPENGL_VERSION_330)
ENDIF ()

# set(hdf5_DLLS ${HDF5_ROOT}/bin/hdf5.dll ${HDF5_ROOT}/bin/libmmd.dll)
# # 添加一个自定义命令来复制 Qt DLL 到构建目标的目录
# add_custom_command(TARGET ${MODULE_NAME} POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         ${hdf5_DLLS}
#         $<TARGET_FILE_DIR:${PROJECT_NAME}>
# )

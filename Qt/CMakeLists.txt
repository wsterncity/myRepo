# Linux平台的GNU编译器
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message(STATUS "Compiler is GCC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -O2 -pthread")
    #设置QT的目录
    set(QT_CMAKE_PATH "/home/mky/Qt/6.5.3/gcc_64/lib/cmake")
    set(CMAKE_PREFIX_PATH "/home/mky/Qt/6.5.3/gcc_64/lib/cmake")
    # # Windows的MSVC编译器
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message(STATUS "Compiler is MSVC")
    #设置QT的目录
    set(QT_CMAKE_PATH "E:/Software/Visualsoft/Qt/Qt5.14.2/5.14.2/msvc2017_64/lib/cmake")
    set(CMAKE_PREFIX_PATH "E:/Software/Visualsoft/Qt/Qt5.14.2/5.14.2/msvc2017_64/lib/cmake")
    # set(QT_CMAKE_PATH "D:/Software/Qt/5.14.2/msvc2017_64/lib/cmake")
    # set(CMAKE_PREFIX_PATH "D:/Software/Qt/5.14.2/msvc2017_64/lib/cmake")
    # set(QT_CMAKE_PATH "D:/Qt/5.14.2/msvc2017_64/lib/cmake")
    # set(CMAKE_PREFIX_PATH "D:/Qt/5.14.2/msvc2017_64/lib/cmake")
    # set(CMAKE_PREFIX_PATH "E:/Software/Visualsoft/Qt/Qt5.14.2/5.14.2/msvc2017_64/lib/cmake")
    # set(QT_CMAKE_PATH "D:/dev/Qt5.5.3/6.5.3/msvc2019_64/lib/cmake")
    # set(CMAKE_PREFIX_PATH "D:/dev/Qt5.5.3/6.5.3/msvc2019_64/lib/cmake")
    #以UTF-8格式进行编译
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    message(STATUS "Compiler is AppleClang")
    #设置QT的目录
    set(QT_CMAKE_PATH "...Your Qt Path/buildir/outDir/lib/cmake")
    set(CMAKE_PREFIX_PATH "...Your Qt Path/buildir/outDir/lib/cmake")
endif ()

set(MODULE_NAME Qt_module)

set(CMAKE_INCLUDE_CURRENT_DIR ON) #将编译目录包含到Include目录，否则编译器会找不到qt编译的这些头文件
set(CMAKE_AUTOMOC ON) # for meta object compiler
set(CMAKE_AUTORCC ON) # resource files
set(CMAKE_AUTOUIC ON) # UI files
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/Resources/UI")
# 查找 Qt

#find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL REQUIRED)
message("Qt5Widgets_FOUND  : ${Qt5Widgets_FOUND}")
#qt_standard_project_setup()
Qt5_add_resources(QRC_FILES "Resources/iGameQtMainWindow.qrc")
#set(QRC_FILES ${QRC_FILES} PARENT_SCOPE)

set(UI_FILES
        "Resources/UI/iGameQtMainWindow.ui"
        "Resources/UI/Animation.ui"
        "Resources/UI/MeshEditMode.ui"
        "Resources/UI/QualityDetection.ui"
        "Resources/UI/ScalarView.ui"
        "Resources/UI/ColorManager.ui"
        "Resources/UI/SetCustomScaleRange.ui"
        "Resources/UI/SearchInfo.ui"
        "Resources/UI/igStreamTracer.ui"
        "Resources/UI/igVector.ui"
        "Resources/UI/TensorView.ui"
        "Resources/UI/filterDialog.ui"
)
Qt5_wrap_ui(UI_HEAD_FILES ${UI_FILES})

get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(DEPLOYQT_EXECUTABLE NAMES windeployqt macdeployqt HINTS "${_qt_bin_dir}")


file(GLOB_RECURSE QT_SRCS CONFIGURE_DEPENDS src/*.cpp include/*.h)


add_library(${MODULE_NAME} SHARED
        ${QRC_FILES}
        # ${UI_FILES}
        ${UI_HEAD_FILES}
        ${QT_SRCS}
        src/IQCore/iGameFileDialog.cpp
        include/IQCore/iGameFileDialog.h
)


set_source_files_properties(${QRC_FILES} PROPERTIES GENERATED ON)

target_include_directories(${MODULE_NAME} PUBLIC
        include
        "${CMAKE_BINARY_DIR}/Qt"
)
set(LINK_LIBS)


list(APPEND LINK_LIBS
        Qt5::OpenGL
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        #        Qt5::OpenGLWidgets
        gladLib
        iGameCore_module
        #iGameRendering_module
)

set(LINK_OPTS)
if (EMSCRIPTEN)
    list(APPEND LINK_LIBS Qt5::Platform)
    list(APPEND LINK_OPTS "SHELL:-s FULL_ES2=1")
endif ()

target_link_libraries(${MODULE_NAME} PUBLIC
        ${LINK_LIBS}
)
target_link_options(${MODULE_NAME} PUBLIC ${LINK_OPTS})

target_link_libraries(${PROJECT_NAME} PUBLIC ${MODULE_NAME})

target_compile_definitions(${MODULE_NAME} PRIVATE IG_QT_COMPILE_EXPORT)

#   由于Windows不会搜索其他目录下的dll文件因此要使用cmake指令将dll移至exe目录
if (WIN32)
    #  内部库在cmake构建时定义IG_QT_COMPILE_EXPORT，使得模式切换为dllexport, 否则外部库在使用编译这个动态库代码时将其模式切换为dllimport
    target_compile_definitions(${MODULE_NAME} PRIVATE IG_QT_COMPILE_EXPORT)
    get_target_property(Qt5Core_LOCATION Qt5::Core LOCATION)
    get_filename_component(Qt5_INSTALL_DIR ${Qt5Core_LOCATION} DIRECTORY)
    # 构建 Qt 的 DLL 文件路径
    set(Qt5_DLL_DIR "${Qt5_INSTALL_DIR}")
    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(Qt5_DLLS
                ${Qt5_DLL_DIR}/Qt5Cored.dll
                ${Qt5_DLL_DIR}/Qt5OpenGLd.dll
                #                ${Qt5_DLL_DIR}/Qt5OpenGLWidgetsd.dll
                ${Qt5_DLL_DIR}/Qt5Widgetsd.dll
                ${Qt5_DLL_DIR}/Qt5Guid.dll
        )
    elseif (${CMAKE_BUILD_TYPE} STREQUAL "Release")
        set(Qt5_DLLS
                ${Qt5_DLL_DIR}/Qt5Core.dll
                ${Qt5_DLL_DIR}/Qt5OpenGL.dll
                #                ${Qt5_DLL_DIR}/Qt5OpenGLWidgets.dll
                ${Qt5_DLL_DIR}/Qt5Widgets.dll
                ${Qt5_DLL_DIR}/Qt5Gui.dll

        )
    endif ()

    set(Qt5_DLLS ${Qt5_DLLS}
            ${CMAKE_BINARY_DIR}/Qt/Qt_module.dll
    )
    # 添加一个自定义命令来复制 Qt DLL 到构建目标的目录
    add_custom_command(TARGET ${MODULE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${Qt5_DLLS}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif ()


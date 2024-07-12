cmake_minimum_required(VERSION 3.19)
project(OpeniGame)
add_definitions(/D_USE_MATH_DEFINES)
set(CMAKE_CXX_STANDARD 17)


add_executable(${PROJECT_NAME}
        main.cpp
)
#--------------------------------------------------------------------
# Add subdirectories
#--------------------------------------------------------------------
#导入第三方组件
add_subdirectory(ThirdParty)
#导入iGame核心后处理组件
add_subdirectory(iGameCore)
#导入渲染器模块
#add_subdirectory(Rendering)
#导入Qt模块
add_subdirectory(Qt)

#   由于Windows不会搜索其他目录下的dll文件因此要使用cmake指令将dll移至exe目录
if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${DEPLOYQT_EXECUTABLE} "$<TARGET_FILE:${PROJECT_NAME}>"
    )

endif ()
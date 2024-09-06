add_subdirectory(glad)
add_subdirectory(freetype-2.13.2)
add_subdirectory(tinyXML2)
#add_subdirectory(Eigen)
add_subdirectory(meshoptimizer)

# CGNS配置
set(CGNS_BUILD_SHARED OFF)
set(CGNS_ENABLE_HDF5 OFF)
set(CGNS_ENABLE_PARALLEL OFF)  # 如果不需要并行支持可以关闭
#set(CGNS_HDF5_DIR ${HDF5_ROOT})

# Configure the compilation definition according to the value of MODULE_FLAG

if(ENABLE_CGNS_MODULE)
    add_subdirectory(CGNS)
endif ()
add_subdirectory(tinyXML2)
add_subdirectory(glad)
add_subdirectory(freetype-2.13.3)
add_subdirectory(eigen-3.4.0)
add_subdirectory(meshoptimizer-0.21)

# CGNS
# Configure the compilation definition according to the value of MODULE_FLAG

if (ENABLE_CGNS_MODULE)
    #    find_package(Git QUIET)
    #    if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/../.git")
    #        # Update submodules as needed
    #        option(GIT_SUBMODULE "Check submodules during build" ON)
    #        if(GIT_SUBMODULE)
    #            message(STATUS "Submodule update")
    #            execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
    #                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #                    RESULT_VARIABLE GIT_SUBMOD_RESULT)
    #            if(NOT GIT_SUBMOD_RESULT EQUAL "0")
    #                message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    #            endif()
    #        endif()
    #    endif()

    set(CGNS_BUILD_SHARED OFF)
    set(CGNS_ENABLE_HDF5 OFF)
    set(CGNS_ENABLE_PARALLEL OFF) 
    #set(CGNS_HDF5_DIR ${HDF5_ROOT})
    add_subdirectory(CGNS)
endif ()
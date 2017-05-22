cmake_minimum_required ( VERSION 3.0.2 )

project ( IMPEngine )


# Function to assign the path of a file as it source group
# This will produce filter ("folders") in a VS project.
function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin )

set ( DEBUG )

########################################################################################
# openCL

if( NOT BITNESS )
  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BITNESS 64)
  else()
    set(BITNESS 32)
  endif()
endif()

# Select bitness for non-msvc platform. Can be specified as -DBITNESS=32/64 at command-line
if( NOT MSVC )
  set(BITNESS ${BITNESS} CACHE STRING "Specify bitness")
  set_property(CACHE BITNESS PROPERTY STRINGS "64" "32")
endif()

MESSAGE( STATUS "blabla: " ${CMAKE_SIZEOF_VOID_P} )

# Unset OPENCL_LIBRARIES, so that corresponding arch specific libs are found when bitness is changed
unset(OPENCL_LIBRARIES CACHE)

if( BITNESS EQUAL 64 )
  set(BITNESS_SUFFIX x86_64)
elseif( BITNESS EQUAL 32 )
  set(BITNESS_SUFFIX x86)
else()
  message( FATAL_ERROR "Bitness specified is invalid" )
endif()
# Find OpenCL include and libs
find_path( OPENCL_INCLUDE_DIRS 
    NAMES OpenCL/cl.h CL/cl.h
    HINTS ../../../../../include/ $ENV{AMDAPPSDKROOT}/include/
)
mark_as_advanced(OPENCL_INCLUDE_DIRS)

find_library( OPENCL_LIBRARIES
	NAMES OpenCL
	HINTS ../../../../../lib/ $ENV{AMDAPPSDKROOT}/lib
	PATH_SUFFIXES ${PLATFORM}${BITNESS} ${BITNESS_SUFFIX}
)
mark_as_advanced( OPENCL_LIBRARIES )

if( OPENCL_INCLUDE_DIRS STREQUAL "" OR OPENCL_LIBRARIES STREQUAL "")
	message( FATAL_ERROR "Could not locate OpenCL include & libs" )
endif( )
include_directories( ${OPENCL_INCLUDE_DIRS} $ENV{AMDAPPSDKROOT}/include/SDKUtil )
#####################################################################################
# GLFW
add_subdirectory(external/glfw-3.2.1)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# GLM
include_directories(external/glm)

# dirent
if (WIN32)
	include_directories(external/dirent)
endif(WIN32)

#TBB
add_subdirectory(external/tbb)

# Main code
add_subdirectory ( src )


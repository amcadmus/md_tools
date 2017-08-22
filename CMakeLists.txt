cmake_minimum_required(VERSION 3.0)
project(MOASP_TOOLS CXX)

# define build type
if ((NOT DEFINED CMAKE_BUILD_TYPE) OR CMAKE_BUILD_TYPE STREQUAL "")
   set (CMAKE_BUILD_TYPE release)
endif ()

find_package (Boost REQUIRED program_options)
include_directories (${Boost_INCLUDE_DIRS})

include_directories ("include")

set (libname moasp_tools)
file (GLOB LIB_SRC src/*.cc src/*.cpp)
add_library (${libname} ${LIB_SRC})

file (GLOB EXEC_SRC src/app/*cpp)
foreach (exec_file ${EXEC_SRC})
  get_filename_component (exec_file_name ${exec_file} NAME)
  string(REGEX REPLACE "\\.[^.]*$" "" exec_name ${exec_file_name})
  add_executable(${exec_name} ${exec_file})
  target_link_libraries(${exec_name} moasp_tools ${Boost_PROGRAM_OPTIONS_LIBRARY} )
  set_target_properties(
    ${exec_name}
    PROPERTIES
    INSTALL_RPATH "${Boost_LIBRARY_DIRS}")
  install(TARGETS ${exec_name} DESTINATION bin/)
endforeach ()


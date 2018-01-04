# add global dependencies
include_directories( ../libGPIB/include )
link_directories( ${CMAKE_BINARY_DIR}/lib )

if( ${ENABLE_FTDI} )
  add_definitions(-DFTDI)
endif()

# add executables
file(GLOB tools [a-zA-Z]*.c[a-zA-Z]*)

foreach(target ${tools})
  get_filename_component(execname ${target} NAME_WE)
  get_filename_component(srcfile ${target} NAME)

  add_executable(${execname} ${srcfile})
  target_link_libraries(${execname} -lGPIB)
endforeach()

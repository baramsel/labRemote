#
# Look for dependencies

# Find libFTDI1

#
# Prepare the library

# Add the headers
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

# Add all *.c* files as source code
set(SrcFiles AgilentPs.cpp
  AMAC.cpp
  Bk85xx.cpp
  Keithley24XX.cpp
  MojoCom.cpp
  SerialCom.cpp)

if ( ${ENABLE_FTDI} )
  set(SrcFiles ${SrcFiles}
    FTDICom.cpp)

  include_directories ( ${LIBFTDI_INCLUDE_DIR} ${LIBMPSSE_INCLUDE_DIR} )
  link_directories ( ${LIBFTDI_LIBRARY_DIRS} ${LIBMPSSE_LIBRARIES} )
else()
  message("skipping FTDICom due to missing libftdi1 or libmpsse")
  message(" libftdi1 = ${LIBFTDI_FOUND}")
  message(" libmpsse = ${LIBMPSSE_FOUND}")
endif()

add_library(GPIB SHARED ${SrcFiles}) 
target_link_libraries(GPIB mpsse ftdi1)


SET(SRCS xpt2046.cpp   xpt2046_calc.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
generate_arduino_library(${libPrefix}XPT2046
                            SRCS ${SRCS}
                            BOARD ${ARDUINO_DEFAULT_BOARD}
                        )
EXPORT_LIBRARY(xpt2046 FLAGS "" INCLUDE_PATHS "${CMAKE_CURRENT_SOURCE_DIR}" NAME "${libPrefix}XPT2046")

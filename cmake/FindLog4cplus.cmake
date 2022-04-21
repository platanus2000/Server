

find_path(Log4cplus_INCLUDE_DIR NAMES log4cplus/logger.h)
find_library(Log4cplus_LIBRARY NAMES log4cplus)
mark_as_advanced(Log4cplus_INCLUDE_DIR Log4cplus_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Log4cplus
  REQUIRED_VARS Log4cplus_INCLUDE_DIR Log4cplus_LIBRARY
  )

if(Log4cplus_FOUND)
  set(Log4cplus_INCLUDE_DIRS ${Log4cplus_INCLUDE_DIR})
  set(Log4cplus_LIBRARIES ${Log4cplus_LIBRARY})
endif()


find_path(MySQL_INCLUDE_DIR NAMES mysql/mysql.h)
find_library(MySQL_LIBRARY NAMES mysqlclient HINTS /usr/lib64/mysql)
mark_as_advanced(MySQL_INCLUDE_DIR MySQL_LIBRARY)
# message("MySQL : ${MySQL_LIBRARY}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MySQL
  REQUIRED_VARS MySQL_INCLUDE_DIR MySQL_LIBRARY
  )

if(MySQL_FOUND)
  set(MySQL_INCLUDE_DIRS ${MySQL_INCLUDE_DIR})
  set(MySQL_LIBRARIES ${MySQL_LIBRARY})
endif()
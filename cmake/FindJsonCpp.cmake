

find_path(JsonCpp_INCLUDE_DIR NAMES json/json.h)
find_library(JsonCpp_LIBRARY NAMES jsoncpp)
mark_as_advanced(JsonCpp_INCLUDE_DIR JsonCpp_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(JsonCpp
  REQUIRED_VARS JsonCpp_INCLUDE_DIR JsonCpp_LIBRARY
  )

if(JsonCpp_FOUND)
  set(JsonCpp_INCLUDE_DIRS ${JsonCpp_INCLUDE_DIR})
  set(JsonCpp_LIBRARIES ${JsonCpp_LIBRARY})
endif()

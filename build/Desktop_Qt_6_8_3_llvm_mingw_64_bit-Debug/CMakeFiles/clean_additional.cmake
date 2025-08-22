# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\appFileBrowser_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\appFileBrowser_autogen.dir\\ParseCache.txt"
  "appFileBrowser_autogen"
  )
endif()

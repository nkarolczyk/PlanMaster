# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/PlanMaster_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/PlanMaster_autogen.dir/ParseCache.txt"
  "PlanMaster_autogen"
  )
endif()

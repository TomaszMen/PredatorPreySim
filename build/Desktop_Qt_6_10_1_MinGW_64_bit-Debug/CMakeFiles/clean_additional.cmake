# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\PredatorPreySimulation_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\PredatorPreySimulation_autogen.dir\\ParseCache.txt"
  "PredatorPreySimulation_autogen"
  )
endif()

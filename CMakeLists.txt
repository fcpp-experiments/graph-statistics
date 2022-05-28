# minimum required version of CMake
cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
# avoid building internal tests of the FCPP library
option(FCPP_INTERNAL_TESTS "Build internal tests for FCPP." OFF)
# load the FCPP sub-module
add_subdirectory(./fcpp/src)
# CMake macro setting up the FCPP library
fcpp_setup()

# project description
project(
    graphstats
    VERSION 0.0.0
    DESCRIPTION "Computation of graph statistics on logical and physical networks."
)

# target declaration
fcpp_target(./run/logical_batch.cpp OFF)
fcpp_target(./run/logical_gui.cpp   ON)
fcpp_target(./run/physical_gui.cpp  ON)
fcpp_target(./extras/graph_converter.cpp OFF)

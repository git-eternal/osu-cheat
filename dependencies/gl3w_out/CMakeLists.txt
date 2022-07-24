# no idea how to use gl3w's cmake file... TODO: figure it out
# Generate this dep by running: py gl3w_gen.py --root ../gl3w_out/

cmake_minimum_required(VERSION 3.18)
project(gl3w)
add_library(gl3w STATIC "src/gl3w.c")

set(gl3w-includes "${CMAKE_CURRENT_SOURCE_DIR}/include" PARENT_SCOPE)
set_target_properties(gl3w PROPERTIES LINKER_LANGUAGE C)
#project(vectorclass)
#
#cmake_minimum_required(VERSION 3.22)
#
#include_directories(include)
#
#add_library(test OBJECT test.cpp)
#set(VECTORCLASS_OBJECT_FILES
#        ${VECTORCLASS_OBJECT_FILES} $<TARGET_OBJECTS:vectorclass>)
#
#disable_target_warnings(vectorclass)
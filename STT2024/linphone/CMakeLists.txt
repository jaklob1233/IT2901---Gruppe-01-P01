cmake_minimum_required(VERSION 3.4)
project(linphone_pybind)

set(CMAKE_BUILD_TYPE Debug)

set(pybind11_DIR "/home/herman/.local/lib/python3.10/site-packages/pybind11/share/cmake/pybind11")

find_package(pybind11 REQUIRED)

include_directories(/usr/include/linphone++)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

pybind11_add_module(linphone_module linphone_module.cpp LinphoneCoreWrapper.cpp LinphoneCallWrapper.cpp CustomCoreListener.cpp)
target_link_libraries(linphone_module PRIVATE pybind11::pybind11 /usr/lib/x86_64-linux-gnu/liblinphone++.so)

# add_custom_command(TARGET linphone_module POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy
#             $<TARGET_FILE:linphone_module>
#             ${CMAKE_CURRENT_SOURCE_DIR}/./$<TARGET_FILE_NAME:linphone_module>)
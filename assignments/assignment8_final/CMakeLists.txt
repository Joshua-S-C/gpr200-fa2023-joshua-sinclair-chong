#Transformations example

file(
 GLOB_RECURSE ASSIGNMENT8_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE ASSIGNMENT8_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)
#Copies this assignment8_final's asset folder to bin when it is built
add_custom_target(copyAssetsA8 ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${ASSIGNMENT7_INC} DESTINATION include/assignment8_final)
add_executable(assignment8_final ${ASSIGNMENT8_SRC} ${ASSIGNMENT8_INC} ${ASSIGNMENT8_ASSETS})
target_link_libraries(assignment8_final PUBLIC core IMGUI)
target_include_directories(assignment8_final PUBLIC ${CORE_INC_DIR} ${stb_INCLUDE_DIR})

#Trigger asset copy when assignment8_final is built
add_dependencies(assignment8_final copyAssetsA8)
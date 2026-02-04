
add_executable(App)

target_link_libraries(App PUBLIC Engine)

target_sources(App PUBLIC
    "EntryPoint.cpp"
    "PhysicsScene.cpp"
    "RigidBody.cpp"
	"Circle.cpp"
    "PhysicsObject.cpp"
    "Plane.cpp"
    "Box.cpp"
)

target_include_directories(App PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(TARGET App POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Shaders $<TARGET_FILE_DIR:App>/Shaders
)

if(WIN32)
	add_custom_command(TARGET App POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_CURRENT_SOURCE_DIR}/SDL3.dll $<TARGET_FILE_DIR:App>)
endif()





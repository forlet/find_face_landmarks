# Validation
if(NOT Boost_FOUND)
	message(FATAL_ERROR "Boost is missing.")
endif()
if(NOT PROTOBUF_FOUND)
	message(FATAL_ERROR "Protobuf is missing.")
endif()

# Target
if(WIN32)
	link_directories(${Boost_LIBRARY_DIRS})
else()
	link_libraries(${Boost_LIBRARIES})
endif()

add_executable(find_face_landmarks find_face_landmarks.cpp)
#target_include_directories(sfl_cache PRIVATE 
#	${Boost_INCLUDE_DIRS})
target_include_directories(find_face_landmarks PUBLIC
	${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(find_face_landmarks PUBLIC
	${OpenCV_LIBS} sequence_face_landmarks
)

# Installations
install(TARGETS find_face_landmarks DESTINATION bin)
set(FFL_TARGETS ${FFL_TARGETS} find_face_landmarks)

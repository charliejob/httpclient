
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

add_definitions(-std=c++11)

FILE(GLOB HEADER_FILES_CORE
	httpclient.h
	threadpool.h
	)
FILE(GLOB SOURCE_FILES_CORE
	main.cpp
	)

source_group("Header Files" FILES
	${HEADER_FILES_CORE}
	)
source_group("Source Files" FILES
	${SOURCE_FILES_CORE}
	)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
ADD_EXECUTABLE(httpclient
	${HEADER_FILES_CORE}
	${SOURCE_FILES_CORE}
	)

TARGET_LINK_LIBRARIES(httpclient pthread curl jsoncpp)


INSTALL(TARGETS httpclient
	RUNTIME DESTINATION bin
	)


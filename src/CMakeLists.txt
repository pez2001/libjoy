#this is just a basic CMakeLists.txt, for more information see the cmake manpage

#add definitions, compiler switches, etc.
ADD_DEFINITIONS(-Wall -O2)

#build a shared library
ADD_LIBRARY(joy SHARED libjoy.c)

#for testing
ADD_EXECUTABLE(libjoytest libjoytest.c)

#a small app to read values from joysticks
ADD_EXECUTABLE(joytool joytool.c)

#need to link to some other libraries ? just add them here
TARGET_LINK_LIBRARIES(libjoytest joy)
#TARGET_LINK_LIBRARIES(libjoytest libjoy)
TARGET_LINK_LIBRARIES(joytool joy)

#add an install target here
#INSTALL_FILES(...)
#INSTALL_PROGRAMS(...)
#INSTALL_TARGET(...)
INSTALL(TARGETS joytool libjoytest joy
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib)

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/libjoy.h DESTINATION include)
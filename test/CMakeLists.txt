# Include dirs.
include_directories(.)
include_directories(../src)
include_directories(${metasim_INCLUDE_DIRS})

# Environment-based settings.
if(APPLE)
	set(LIB_TYPE "SHARED")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++0x")
	
	if(EXISTS "${metasim_DIR}/libmetasim.dylib")
		set(metasim_LIBRARY ${CMAKE_LIBRARY_PATH} "${metasim_DIR}/libmetasim.dylib")
	elseif(EXISTS "${metasim_DIR}/Debug/libmetasim.dylib")
		set(metasim_LIBRARY ${CMAKE_LIBRARY_PATH} "${metasim_DIR}/Debug/libmetasim.dylib")
	elseif(EXISTS "${metasim_DIR}/Release/libmetasim.dylib")
		set(metasim_LIBRARY ${CMAKE_LIBRARY_PATH} "${metasim_DIR}/Release/libmetasim.dylib")
	endif()
	
elseif(UNIX)
	set(LIB_TYPE "SHARED")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++0x")
	set(metasim_LIBRARY "${metasim_DIR}/libmetasim.so")
	
elseif(WIN32)	
	if(EXISTS "${metasim_DIR}/Debug/metasim.lib")
		set(metasim_LIBRARY "${metasim_DIR}/Debug/metasim.lib")
	elseif(EXISTS "${metasim_DIR}/Release/metasim.lib")
		set(metasim_LIBRARY "${metasim_DIR}/Release/metasim.lib")
	endif()
		
endif()

# Create the executable.
add_executable(tests test_main.cpp cbs.cpp test_task.cpp test_mrt.cpp test_AVR.cpp)
add_executable(cbs_test test_main.cpp cbs2.cpp)
add_executable(test_broe test_main test_broe.cpp)
add_executable(srp_test test_main srp_test.cpp)
add_executable(test_srp_cbs test_main test_srp_cbs.cpp)
add_executable(test_hsrp test_main test_hsrp.cpp)
# Indicate that rtlib need rtlib library.
target_link_libraries(tests rtlib ${metasim_LIBRARY})
target_link_libraries(cbs_test rtlib ${metasim_LIBRARY})
target_link_libraries(test_broe rtlib ${metasim_LIBRARY})
target_link_libraries(srp_test rtlib ${metasim_LIBRARY})
target_link_libraries(test_srp_cbs rtlib ${metasim_LIBRARY})
target_link_libraries(test_hsrp rtlib ${metasim_LIBRARY})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -funroll-loops -Ofast -march=native -ffast-math -fPIC -fopenmp")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

add_executable(test main.cpp Ray.h Util.h Model.h AABB.h Scene.h Time.h) 

target_link_libraries(test ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

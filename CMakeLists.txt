# cmake_minimum_required(VERSION <specify CMake version here>)
cmake_minimum_required(VERSION 3.16)
project(feeder_dwzq_wind)

SET(CMAKE_BUILD_TYPE "Release")  # Release Debug
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

SET(ROOT_DIR "/home/work/sys/lib")

macro(SUBDIRLIST result curdir)
    file(GLOB children RELATIVE ${curdir} ${curdir}/*)
    set(dirlist "")
    foreach(child ${children})
        if(IS_DIRECTORY ${curdir}/${child})
            LIST(APPEND dirlist ${child})
        endif()
    endforeach()
    set(${result} ${dirlist})
endmacro()

SUBDIRLIST(all_sub_index ${ROOT_DIR})
foreach(child ${all_sub_index})
    list(APPEND all_include ${ROOT_DIR}/${child}/include)
    list(APPEND all_lib ${ROOT_DIR}/${child}/lib)
endforeach()

SET(TDF_VERSION "tdfapi-3.3.0")
SET(FEEDER "dwzq_wind_feeder")

include_directories(${CMAKE_CURRENT_LIST_DIR}/lib/${TDF_VERSION}/include ${all_include})
link_directories(${CMAKE_CURRENT_LIST_DIR}/lib/${TDF_VERSION}/lib ${all_lib})

add_library(libfeeder_dwzq_wind
        src/libfeeder_dwzq_wind/config.h
        src/libfeeder_dwzq_wind/define.h
        src/libfeeder_dwzq_wind/wind_support.cc
        src/libfeeder_dwzq_wind/wind_support.h
        src/libfeeder_dwzq_wind/wind_server.h
        src/libfeeder_dwzq_wind/wind_server.cc
        src/libfeeder_dwzq_wind/libfeeder_dwzq_wind.h
        src/libfeeder_dwzq_wind/config.cc
        )

add_executable(${FEEDER}
        src/dwzq_wind_feeder/main.cc
        )
target_link_libraries(${FEEDER} libfeeder_dwzq_wind TDFAPI30 feeder coral swordfish x stdc++fs yaml-cpp clickhouse-cpp-lib-static boost_date_time boost_filesystem boost_regex boost_system  boost_chrono boost_log boost_program_options boost_thread boost_iostreams z protobuf protobuf-lite sodium zmq ssl crypto iconv pthread dl)



FILE(COPY Dockerfile image.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
FILE(GLOB COMMON_LIB_NAME "${ROOT_DIR}/common_lib/lib/*so*")
FILE(COPY ${COMMON_LIB_NAME} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

FILE(GLOB LIBS_NAME "${CMAKE_CURRENT_LIST_DIR}/lib/${TDF_VERSION}/lib/*so*")
FILE(COPY ${LIBS_NAME} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

FILE(COPY ${COMMON_LIB_NAME} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/image.sh "n=$1; v=`./$n -v`; ldd $n | grep /sys/lib/ | awk '{print $3}' | xargs -i cp -L -n {} .; docker build -t $n:$v .; rm -rf \${n}_$v.docker.tar.gz; docker save $n:$v > \${n}_$v.docker.tar && gzip \${n}_$v.docker.tar")
add_custom_target(image COMMAND sh image.sh ${FEEDER} DEPENDS ${FEEDER})

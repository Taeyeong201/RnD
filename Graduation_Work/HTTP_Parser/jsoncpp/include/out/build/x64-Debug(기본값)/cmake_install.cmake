# Install script for directory: D:/VDI/AtWork/Taeyeong201/RnD/Graduation_Work/HTTP_Parser/jsoncpp/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "D:/VDI/AtWork/Taeyeong201/RnD/Graduation_Work/HTTP_Parser/jsoncpp/include/out/install/x64-Debug(기본값)")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/json/allocator.h;/json/assertions.h;/json/config.h;/json/forwards.h;/json/json.h;/json/json_features.h;/json/reader.h;/json/value.h;/json/version.h;/json/writer.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/json" TYPE FILE FILES
    "D:/VDI/AtWork/Taeyeong201/RnD/Graduation_Work/HTTP_Parser/jsoncpp/include/json/allocator.h"
    "D:/VDI/AtWork/Taeyeong201/RnD/Graduation_Work/HTTP_Parser/jsoncpp/include/json/assertions.h"
    "D:/VDI/AtWork/Taeyeong201/RnD/Graduation_Work/HTTP_Parser/jsoncpp/include/json/config.h"
    "D:/VDI/AtWork/Taeyeong201/RnD/Graduation_Work/HTTP_Parser/jsoncpp/include/json/forwards.h"
    "D:/VDI/AtWork/Taeyeong201/RnD/Graduation_Work/HTTP_Parser/jsoncpp/include/json/json.h"
    "D:/VDI/AtWork/Taeyeong201/RnD/Graduation_Work/HTTP_Parser/jsoncpp/include/json/json_features.h"
    "D:/VDI/AtWork/Taeyeong201/RnD/Graduation_Work/HTTP_Parser/jsoncpp/include/json/reader.h"
    "D:/VDI/AtWork/Taeyeong201/RnD/Graduation_Work/HTTP_Parser/jsoncpp/include/json/value.h"
    "D:/VDI/AtWork/Taeyeong201/RnD/Graduation_Work/HTTP_Parser/jsoncpp/include/json/version.h"
    "D:/VDI/AtWork/Taeyeong201/RnD/Graduation_Work/HTTP_Parser/jsoncpp/include/json/writer.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "D:/VDI/AtWork/Taeyeong201/RnD/Graduation_Work/HTTP_Parser/jsoncpp/include/out/build/x64-Debug(기본값)/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")

cmake_minimum_required (VERSION 2.6)

project (meka C CXX)

include(cmake/Android.cmake)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Profile."
      FORCE)
endif()

if( WANT_ANDROID )

  message("--------android---------")
  add_custom_target(debug DEPENDS Meka WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})

  # Put libraries into `lib'.
  set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
  
  add_subdirectory(android)

else()

  message("--------desktop---------")
  add_custom_target(debug DEPENDS meka WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})

endif()

add_subdirectory(srcs)

# Instructions
# Android:
# cmake .. -DANDROID_NDK_TOOLCHAIN_ROOT=$ANDROID_NDK_TOOLCHAIN_ROOT -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake -DANDROID_ABI=armeabi-v7a -DALLEGRO_DIRECTORY=/opt/android/allegro5 -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Linux
# cmake .. -DCMAKE_BUILD_TYPE=Debug



if ( DEFINED ANDROID_ABI )

  set( WANT_ANDROID "1" )

endif()

if( WANT_ANDROID )

  # Set root directory for allegro
  if ( "${ALLEGRO_DIRECTORY}" STREQUAL "" )

    set(ALLEGRO_DIRECTORY ${HOME}/allegro5-master )
    message( "ALLEGRO_DIRECTORY not set, using ${ALLEGRO_DIRECTORY}" )

  endif()
  
  set(ALLEGRO_INCLUDES ${ALLEGRO_DIRECTORY}/include ${AllegroPlatformDirectory}/include ${ALLEGRO_DIRECTORY}/addons/)
  
else()
  set(ALLEGRO_DIRECTORY /usr)

if ( CMAKE_BUILD_TYPE MATCHES "Debug" )

  #set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -Wno-sign-compare -Wno-multichar -Wno-implicit-fallthrough -fdiagnostics-show-option -O0 -ggdb -DDEBUG -Wno-unused-parameter")

else()
  #set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wall -O3 -ffast-math -fno-strength-reduce -funroll-all-loops -fomit-frame-pointer -x c++")
endif()

#add_definitions( "-ffast-math -fno-strength-reduce -fomit-frame-pointer -x c++" )

endif()

# warn if not found
if ( EXISTS "${ALLEGRO_DIRECTORY}/include/allegro5/allegro5.h" )
  
  message( "-- Found Allegro at ${ALLEGRO_DIRECTORY}" )
  
else( WANT_ANDROID )
  
  message( "--------------------" )
  message( "WARNING: " )
  message( "* Allegro not found expected ${ALLEGRO_DIRECTORY}" )
  message( "--------------------" )
  
endif()

set(ALLEGRO_LIBS ${PREFIX}allegro_acodec${POSTFIX} ${PREFIX}allegro_audio${POSTFIX} ${PREFIX}allegro_color${POSTFIX} 
  ${PREFIX}allegro${POSTFIX} ${PREFIX}allegro_font${POSTFIX} ${PREFIX}allegro_image${POSTFIX} ${PREFIX}allegro_main${POSTFIX} 
  ${PREFIX}allegro_memfile${POSTFIX} ${PREFIX}allegro_primitives${POSTFIX} ${PREFIX}allegro_ttf${POSTFIX})

#set(AllegroLibs allegro allegro_image allegro_ttf allegro_color allegro_font allegro_dialog allegro_primitives)

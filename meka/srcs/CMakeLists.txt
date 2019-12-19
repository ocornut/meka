include(../cmake/Common.cmake)
include(../cmake/AndroidApp.cmake)

include_directories("${CMAKE_SOURCE_DIR}/srcs" ${ALLEGRO_INCLUDES} "${ALLEGRO_DIRECTORY}/include")

set(sources g_menu_t.cpp app_game.cpp video_t.cpp db.cpp capture.cpp country.cpp inputs_f.cpp rapidfir.cpp periph.cpp
  z80marat/Debug.cpp z80marat/Z80.cpp z80marat/Z80Call.cpp z80marat/Z80DebugHelpers.cpp
  vlfn.cpp g_action.cpp areplay.cpp inputs.cpp fskipper.cpp bmemory.cpp blit.cpp machine.cpp
  vmachine.cpp fdc765.cpp app_palview.cpp app_cheatfinder.cpp app_about.cpp sportpad.cpp g_update.cpp libmisc.cpp
  message.cpp libparse.cpp eeprom.cpp saves.cpp blit_c.cpp inputs_t.cpp
  inputs_i.cpp commport.cpp fonts.cpp meka.cpp app_filebrowser.cpp gui.cpp config.cpp app_mapview.cpp desktop.cpp
  g_tools.cpp ioports.cpp beam.cpp inputs_c.cpp tools.cpp hq2x.cpp blitintf.cpp setup.cpp errors.cpp shared.cpp
  file.cpp app_tileview.cpp app_textview.cpp coleco.cpp g_box.cpp keyinfo.cpp skin_fx.cpp g_menu.cpp video_m2.cpp
  app_techinfo.cpp sdsc.cpp sf7000.cpp allegro4to5.cpp video.cpp app_options.cpp skin.cpp g_widget.cpp g_mouse.cpp
  sk1100.cpp glasses.cpp palette.cpp app_memview.cpp datadump.cpp data.cpp checksum.cpp build.cpp skin_bg.cpp
  video_m5.cpp unzip.cpp misc.cpp tvoekaki.cpp bios.cpp lightgun.cpp g_menu_i.cpp mappers.cpp patch.cpp
  sound/emu2413/emu2413.cpp sound/emu2413/mekaintf.cpp sound/sound.cpp sound/vgm.cpp
  sound/sound_logging.cpp sound/psg.cpp sound/fmeditor.cpp sound/s_misc.cpp sound/wav.cpp sound/fmunit.cpp
  vdp.cpp mainloop.cpp effects.cpp drivers.cpp inputs_u.cpp tvtype.cpp textbox.cpp sg1ksc3k.cpp debugger.cpp cpu.cpp video_c.cpp g_init.cpp)


set(sources1 test.cpp)

add_definitions( -DASM_SYMBOLS_REQUIRE_UNDERSCORE )

if(ANDROID)

  set(name Meka)
  
  link_directories( "${ALLEGRO_DIRECTORY}/lib" "${ANDROID_NDK_TOOLCHAIN_ROOT}/sysroot/usr/lib" )
  add_library(native-lib SHARED ${sources})
  target_link_libraries(native-lib ${ALLEGRO_LIBS} log z)
  
  # Add android target for app
  add_android_app(${name} "${sources};${assets}")

  # Copy allegro libs to app directory
  set(dest "${CMAKE_CURRENT_BINARY_DIR}/${name}.project/app/src/main/jniLibs/armeabi-v7a/")

  add_custom_command(TARGET ${name} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${dest})

  foreach(lib ${ALLEGRO_LIBS})
    set(src ${ALLEGRO_DIRECTORY}/lib/lib${lib}.so)
    add_custom_command(TARGET ${name} PRE_BUILD COMMAND "${CMAKE_COMMAND}" -E copy "${src}" "${dest}")
  endforeach()

  set(apk_root "${CMAKE_CURRENT_BINARY_DIR}/${name}.project/app/src/main/assets")
  set(apk_data "${apk_root}/meka" )
  set(data_src "${CMAKE_CURRENT_SOURCE_DIR}/.." )
  
  # Copy assets
  set(dest "${apk_data}/meka.sc")
  set(src "${CMAKE_CURRENT_SOURCE_DIR}/../srcs/SC3000SurvivorsMkIIMulticart_v2_31.sc")
  add_custom_command(TARGET ${name} PRE_BUILD COMMAND "${CMAKE_COMMAND}" -E copy "${src}" "${dest}")

  # Copy assets
  set(dest "${apk_data}/Data/")
  set(src "${CMAKE_CURRENT_SOURCE_DIR}/../Data/*")
  add_custom_command(TARGET ${name} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${dest} COMMAND "${CMAKE_COMMAND}" -E copy "${src}" "${dest}")

  set(dest "${apk_data}/Data/fonts")
  set(src "${CMAKE_CURRENT_SOURCE_DIR}/../Data/fonts/*")
  add_custom_command(TARGET ${name} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${dest} COMMAND "${CMAKE_COMMAND}" -E copy "${src}" "${dest}")

  set(dest "${apk_root}/fonts")
  set(src "${CMAKE_CURRENT_SOURCE_DIR}/../Data/fonts/*")
  add_custom_command(TARGET ${name} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${dest} COMMAND "${CMAKE_COMMAND}" -E copy "${src}" "${dest}")

  set(dest "${apk_data}/")
  set(src "${CMAKE_CURRENT_SOURCE_DIR}/../meka.*")
  add_custom_command(TARGET ${name} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${dest} COMMAND "${CMAKE_COMMAND}" -E copy "${src}" "${dest}")

  set(src "${CMAKE_CURRENT_SOURCE_DIR}/../resource/res/mipmap-xhdpi/ic_launcher.png")
  set(dest "${CMAKE_CURRENT_BINARY_DIR}/${name}.project/app/src/main/res/mipmap/")
  add_custom_command(TARGET ${name} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${dest} COMMAND "${CMAKE_COMMAND}" -E copy "${src}" "${dest}")

  set(src "${CMAKE_CURRENT_BINARY_DIR}/${name}.project/app/build/outputs/apk/debug/app-debug.apk")
  set(dest "${CMAKE_CURRENT_BINARY_DIR}/${name}.apk")
  set(dest2 "/media/sf_Shared/")
  add_custom_command(TARGET native-lib POST_BUILD COMMAND "${CMAKE_COMMAND}" -E copy "${src}" "${dest}"
    COMMAND "${CMAKE_COMMAND}" -E copy "${dest}" "${dest2}")

  # Add install target
  add_custom_target(push DEPENDS ${name} COMMAND adb install -r ${dest})

  add_custom_target(resource)
  
  target_link_libraries(${name} ${ALLEGRO_LIBS} log z)
  set(CMAKE_EXE_LINKER_FLAGS " -static")
  target_link_libraries(${name} -static-libgcc -static-libstdc++)
else()

  set(name meka)

  add_executable(${name} ${sources})

  target_link_libraries(${name} ${ALLEGRO_LIBS} z)
  
endif()

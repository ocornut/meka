cmake_minimum_required(VERSION 3.10)
project(meka)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# stuff for better allegro support
# Try CMake Find modules first, fall back to pkg-config
find_package(Ogg QUIET)
find_package(Vorbis QUIET)
find_package(Theora QUIET)

find_package(PkgConfig QUIET)

if (NOT Ogg_FOUND AND PkgConfig_FOUND)
    pkg_check_modules(OGG ogg QUIET)
    if (OGG_FOUND)
        set(Ogg_FOUND TRUE)
        set(Ogg_INCLUDE_DIRS ${OGG_INCLUDE_DIRS})
        set(Ogg_LIBRARIES ${OGG_LIBRARIES})
    endif()
endif()

if (NOT Vorbis_FOUND AND PkgConfig_FOUND)
    pkg_check_modules(VORBIS vorbis QUIET)
    if (VORBIS_FOUND)
        set(Vorbis_FOUND TRUE)
        set(Vorbis_INCLUDE_DIRS ${VORBIS_INCLUDE_DIRS})
        set(Vorbis_LIBRARIES ${VORBIS_LIBRARIES})
    endif()
endif()

if (NOT Theora_FOUND AND PkgConfig_FOUND)
    pkg_check_modules(THEORA theora QUIET)
    if (THEORA_FOUND)
        set(Theora_FOUND TRUE)
        set(Theora_INCLUDE_DIRS ${THEORA_INCLUDE_DIRS})
        set(Theora_LIBRARIES ${THEORA_LIBRARIES})
    endif()
endif()

if (NOT OGG_FOUND)
    message(STATUS "libogg not found. This is needed for allegro5 audio support.")
    if(WIN32)
        message(STATUS "libogg not found. If using vcpkg, run: vcpkg install libogg")
    elseif(UNIX)
        message(STATUS "libogg not found! Please install libogg-dev (e.g. sudo apt install libogg-dev)")
    elseif(APPLE)
        message(STATUS "libogg not found! Please install libogg via Homebrew (e.g. brew install libogg)")
    endif()
endif()

if (NOT VORBIS_FOUND)
    message(STATUS "libvorbis not found. This is needed for allegro5 video support.")
    if(WIN32)
        message(STATUS "libvorbis not found. If using vcpkg, run: vcpkg install libvorbis")
    elseif(UNIX)
        message(STATUS "libvorbis not found! Please install libvorbis-dev (e.g. sudo apt install libvorbis-dev)")
    elseif(APPLE)
        message(STATUS "libvorbis not found! Please install libvorbis via Homebrew (e.g. brew install libvorbis)")
    endif()
endif()

if (NOT THEORA_FOUND)
    message(STATUS "libtheora not found. This is needed for allegro5 video support.")
    if(WIN32)
        message(STATUS "libtheora not found. If using vcpkg, run: vcpkg install libtheora")
    elseif(UNIX)
        message(STATUS "libtheora not found! Please install libtheora-dev (e.g. sudo apt install libtheora-dev)")
    elseif(APPLE)
        message(STATUS "libtheora not found! Please install libtheora via Homebrew (e.g. brew install libtheora)")
    endif()
endif()

# Prefer system / vcpkg installed libs
find_package(ZLIB QUIET)
find_package(PNG QUIET)
find_package(Freetype QUIET)

if(NOT ZLIB_FOUND)
    if(WIN32)
        message(STATUS "ZLIB not found. If using vcpkg, run: vcpkg install zlib")
    elseif(UNIX)
        message(STATUS "ZLIB not found! Please install zlib1g-dev (e.g. sudo apt install zlib1g-dev)")
    elseif(APPLE)
        message(STATUS "ZLIB not found! Please install zlib via Homebrew (e.g. brew install zlib)")
    endif()
endif()

if(NOT PNG_FOUND)
    if(WIN32)
        message(STATUS "libpng not found. If using vcpkg, run: vcpkg install libpng")
    elseif(UNIX)
        message(STATUS "libpng not found! Please install libpng-dev (e.g. sudo apt install libpng-dev)")
    elseif(APPLE)
        message(STATUS "libpng not found! Please install libpng via Homebrew (e.g. brew install libpng)")
    endif()
endif()

if(NOT FREETYPE_FOUND)
    if(WIN32)
        message(STATUS "Freetype not found. If using vcpkg, run: vcpkg install freetype")
    elseif(UNIX)
        message(STATUS "Freetype not found! Please install libfreetype6-dev (e.g. sudo apt install libfreetype6-dev)")
    elseif(APPLE)
        message(STATUS "Freetype not found! Please install freetype via Homebrew (e.g. brew install freetype)")
    endif()
endif()

include(FetchContent)


# Pass Freetype info to Allegro5 before fetching/building it
set(ALLEGRO_WITH_FREETYPE ON CACHE BOOL "" FORCE)
set(FREETYPE_INCLUDE_DIR ${FREETYPE_INCLUDE_DIRS} CACHE PATH "" FORCE)
set(FREETYPE_LIBRARY freetype CACHE STRING "" FORCE)

set(WANT_DOCS OFF CACHE BOOL "" FORCE)
set(WANT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(WANT_TESTS OFF CACHE BOOL "" FORCE)

# Now fetch and build Allegro 5
FetchContent_Declare(
    allegro5
    GIT_REPOSITORY https://github.com/liballeg/allegro5.git
    GIT_TAG        master # Or specify a stable tag/release
)
FetchContent_GetProperties(allegro5)
if(NOT allegro5_POPULATED)
    FetchContent_Populate(allegro5)
    add_subdirectory(${allegro5_SOURCE_DIR} ${allegro5_BINARY_DIR})
endif()

# Platform detection
if(APPLE)
    set(PLATFORM_MACOSX TRUE)
elseif(UNIX)
    set(PLATFORM_UNIX TRUE)
elseif(WIN32)
    set(PLATFORM_WIN32 TRUE)
endif()

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/libs
    ${CMAKE_SOURCE_DIR}/../include
    ${CMAKE_SOURCE_DIR}
    ${allegro5_SOURCE_DIR}/include
    ${allegro5_BINARY_DIR}/include
)

# Collect all addon directories in allegro5
file(GLOB ADDON_DIRS "${allegro5_SOURCE_DIR}/addons/*")
foreach(addon_dir ${ADDON_DIRS})
    if(IS_DIRECTORY ${addon_dir})
        include_directories(${addon_dir})
    endif()
endforeach()

# Compiler flags
if(PLATFORM_WIN32)
    add_definitions(-DARCH_WIN32)
elseif(PLATFORM_UNIX)
    add_definitions(-DARCH_UNIX -DASM_SYMBOLS_REQUIRE_UNDERSCORE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -ffast-math -fno-strength-reduce -funroll-all-loops -fomit-frame-pointer")
elseif(PLATFORM_MACOSX)
    add_definitions(-DARCH_MACOSX -DUNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")
endif()

# zlib and png
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS} ${PNG_INCLUDE_DIRS})

# Source files (add all your .cpp files here)
set(SOURCES
    allegro4to5.cpp
    app_about.cpp
    app_cheatfinder.cpp
    app_filebrowser.cpp
    app_game.cpp
    app_mapview.cpp
    app_memview.cpp
    app_options.cpp
    app_palview.cpp
    app_techinfo.cpp
    app_textview.cpp
    app_tileview.cpp
    areplay.cpp
    beam.cpp
    bios.cpp
    blit_c.cpp
    blit.cpp
    blitintf.cpp
    bmemory.cpp
    build.cpp
    capture.cpp
    checksum.cpp
    coleco.cpp
    commport.cpp
    config.cpp
    country.cpp
    cpu.cpp
    data.cpp
    datadump.cpp
    db.cpp
    debugger.cpp
    desktop.cpp
    drivers.cpp
    eeprom.cpp
    effects.cpp
    errors.cpp
    fdc765.cpp
    file.cpp
    fonts.cpp
    fskipper.cpp
    g_action.cpp
    g_box.cpp
    g_init.cpp
    glasses.cpp
    g_menu.cpp
    g_menu_i.cpp
    g_menu_t.cpp
    g_mouse.cpp
    g_tools.cpp
    gui.cpp
    g_update.cpp
    g_widget.cpp
    hq2x.cpp
    inputs_c.cpp
    inputs.cpp
    inputs_f.cpp
    inputs_i.cpp
    inputs_t.cpp
    inputs_u.cpp
    ioports.cpp
    keyinfo.cpp
    libmisc.cpp
    libparse.cpp
    lightgun.cpp
    machine.cpp
    mainloop.cpp
    mappers.cpp
    meka.cpp
    message.cpp
    misc.cpp
    palette.cpp
    patch.cpp
    periph.cpp
    rapidfir.cpp
    saves.cpp
    sdsc.cpp
    setup.cpp
    sf7000.cpp
    sg1ksc3k.cpp
    shared.cpp
    sk1100.cpp
    skin_bg.cpp
    skin.cpp
    skin_fx.cpp
    sportpad.cpp
    textbox.cpp
    tools.cpp
    tvoekaki.cpp
    tvtype.cpp
    unzip.cpp
    vdp.cpp
    video_c.cpp
    video.cpp
    video_m2.cpp
    video_m5.cpp
    video_t.cpp
    vlfn.cpp
    vmachine.cpp
    z80marat/Debug.cpp
    z80marat/Z80.cpp
    z80marat/Z80Call.cpp
    z80marat/Z80DebugHelpers.cpp
    sound/fmeditor.cpp
    sound/fmunit.cpp
    sound/psg.cpp
    sound/s_misc.cpp
    sound/sound.cpp
    sound/sound_logging.cpp
    sound/vgm.cpp
    sound/wav.cpp
    sound/emu2413/emu2413.cpp
    sound/emu2413/mekaintf.cpp
)

# Add executable
add_executable(meka ${SOURCES})

# Link Allegro 5 (built from FetchContent)
target_link_libraries(meka
    allegro
    allegro_main
    allegro_font
    allegro_audio
    allegro_primitives
    allegro_image
    allegro_ttf
    ${ZLIB_LIBRARIES}
    ${PNG_LIBRARIES}
    # Add other libraries as needed
)

# Platform-specific frameworks (macOS)
if(PLATFORM_MACOSX)
    target_link_libraries(meka
        "-framework OpenGL"
        "-framework CoreFoundation"
        "-framework OpenAL"
        "-framework AudioToolbox"
        "-framework AppKit"
        "-framework IOKit"
        "-framework Foundation"
    )
endif()
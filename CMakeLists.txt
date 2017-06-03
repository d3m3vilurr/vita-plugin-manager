cmake_minimum_required(VERSION 2.8)

set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_C_COMPILER "arm-vita-eabi-gcc")

project(taiplgmgr)
set(TITLE_ID "TAIPLGMGR")
set(TITLE "taiHEN Plugin Manager")
set(VERSION "0.1.0")

execute_process(COMMAND
    python -c "print '%02d.%02d' % tuple(map(lambda x: int(x), '${VERSION}'.split('.')[:2]))"
    OUTPUT_VARIABLE APP_VER
)
string(REPLACE "\n" "" APP_VER "${APP_VER}")

if (NOT EXISTS "$ENV{RELEASE}")
    execute_process(COMMAND
        git describe --abbrev=7 --dirty --always
        OUTPUT_VARIABLE GIT_HASH
    )
    string(REPLACE "\n" "" GIT_HASH "${GIT_HASH}")

    set(VERSION "${VERSION}-${GIT_HASH}")
endif(NOT EXISTS "$ENV{RELEASE}")

add_definitions(-DSQLITE_OS_OTHER=1)
add_definitions(-DSQLITE_TEMP_STORE=3)
add_definitions(-DSQLITE_THREADSAFE=0)
add_definitions(-DVERSION=\"${VERSION}\")

set(CMAKE_C_FLAGS "-Wl,-q -Wall -O3 -std=c99")

include_directories(
    extern/sqlite3
    extern/getline
    extern/trim
)

add_executable(${PROJECT_NAME}.elf
    src/console.c
    src/appdb.c
    src/font.c
    src/button.c
    src/file.c
    src/menu.c
    src/config.c
    src/plugin.c
    src/main.c
    src/vita_sqlite.c

    extern/sqlite3/sqlite3.c
    extern/getline/getline.c
    extern/trim/trim.c
)

target_link_libraries(${PROJECT_NAME}.elf
    -lvita2d
    -lfreetype
    -lz
    -lm
    -lSceIofilemgr_stub
    -lSceKernelThreadMgr_stub
    -lSceCommonDialog_stub
    -lSceDisplay_stub
    -lSceGxm_stub
    -lSceAppMgr_stub
    -lSceAppUtil_stub
    -lSceSysmodule_stub
    -lSceCtrl_stub
    -lScePgf_stub
    -lSceShellSvc_stub
    -ltaihen_stub
)

add_custom_target(${PROJECT_NAME}.vpk ALL
    COMMAND vita-elf-create ${PROJECT_NAME}.elf ${PROJECT_NAME}.velf
    COMMAND vita-make-fself  -a 0x2800000000000001 -c ${PROJECT_NAME}.velf eboot.bin
    COMMAND vita-mksfoex -s TITLE_ID=${TITLE_ID} -s APP_VER=${APP_VER} "${TITLE}" param.sfo
    COMMAND vita-pack-vpk -s param.sfo -b eboot.bin
            --add ../sce_sys/icon0.png=sce_sys/icon0.png
            --add ../sce_sys/livearea/contents/bg.png=sce_sys/livearea/contents/bg.png
            --add ../sce_sys/livearea/contents/startup.png=sce_sys/livearea/contents/startup.png
            --add ../sce_sys/livearea/contents/template.xml=sce_sys/livearea/contents/template.xml
            ${PROJECT_NAME}.vpk
)

add_dependencies(${PROJECT_NAME}.vpk ${PROJECT_NAME}.elf)

set(SRCDIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
add_definitions(-DBUILDINGPPRO)

set(PPC_SRCs
    ${SRCDIR}/DelayOutPut.c
    ${SRCDIR}/Effects.c
    ${SRCDIR}/MainDriver.c
    ${SRCDIR}/FileUtils.c
    ${SRCDIR}/Interrupt.c
    ${SRCDIR}/MADDriver.c
    ${SRCDIR}/MyDebugStr.c
    ${SRCDIR}/OutPut8.c
    ${SRCDIR}/realft.c
    ${SRCDIR}/TickRemover.c
    ${SRCDIR}/stub-VSTPlugIn.c
    ${SRCDIR}/MIDI-Hardware-Stub.c
)

set(PPC_Headers
    ${SRCDIR}/MADFileUtils.h
    ${SRCDIR}/MADDriver.h
    ${SRCDIR}/MAD.h
    ${SRCDIR}/PlayerPROCore.h
    ${SRCDIR}/MADDefs.h
    ${SRCDIR}/RDriver.h
    ${SRCDIR}/VSTFunctions.h
)

if(UNIX)
    set(PPC_SRCs
        ${PPC_SRCs}
        ${SRCDIR}/Lin-PlugImport.c
    )

    add_definitions(-D__UNIX__)
endif(UNIX)

if(WIN32)
    set(PPC_SRCs
        ${PPC_SRCs}
        ${SRCDIR}/Win-PlugImport.c
        ${SRCDIR}/Win-WaveDriver.c
    )
endif(WIN32)

find_package(PulseAudio)

add_library(PlayerPROCore SHARED ${PPC_SRCs} ${PPC_Headers})
target_link_libraries(PlayerPROCore dl m)

add_subdirectory(Import-Export)



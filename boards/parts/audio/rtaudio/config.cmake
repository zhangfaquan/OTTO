if(APPLE) 
    target_compile_definitions(otto PUBLIC "__MACOSX_CORE__")
    find_library(CORE_AUDIO CoreAudio)
    find_library(CORE_MIDI CoreMIDI)
    find_library(AUDIO_TOOLBOX AudioToolbox)
    target_link_libraries(otto PUBLIC ${CORE_AUDIO}, ${CORE_MIDI}, ${AUDIO_TOOLBOX})
else()
    target_compile_definitions(otto PUBLIC "__LINUX_ALSA__")
    target_link_libraries(otto PUBLIC asound)
endif()
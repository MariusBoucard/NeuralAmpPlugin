cmake_minimum_required(VERSION 3.15)

project(NeuralAmpPlugin VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add JUCE as a subdirectory (adjust the path to where your JUCE folder is located)
# Option 1: If JUCE is in a subdirectory relative to this CMakeLists.txt
# add_subdirectory(path/to/JUCE)

# Option 2: If JUCE is installed elsewhere
# For this example, assuming JUCE is at C:/Users/Marius/Documents/JUCE/JUCE
# add_subdirectory("C:/Users/Marius/Documents/JUCE/JUCE" JUCE)

# Define the plugin
juce_add_plugin(NeuralAmpPlugin
        COMPANY_NAME "BALL"
        COMPANY_WEBSITE "http://juce.com"
        COMPANY_EMAIL ""
        PLUGIN_MANUFACTURER_CODE Ball
        PLUGIN_CODE NAmp
        FORMATS VST3 Standalone
        PRODUCT_NAME "Ballzzy Amp"
        BUNDLE_ID "com.BALL.NeuralAmpPlugin"

        # VST3 specific settings
        VST3_CATEGORIES Fx

        # Plugin characteristics
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE

        # Other settings
        COPY_PLUGIN_AFTER_BUILD FALSE
        PLUGIN_NAME "Ballzzy Amp"
        PLUGIN_DESCRIPTION "Neural Amp Modeler Plugin"
        VST3_AUTO_MANIFEST FALSE
)

# Link binary data and JUCE modules to the plugin
target_link_libraries(NeuralAmpPlugin
        PRIVATE
        NeuralAmpPluginData
        NAM
        AudioDSPTools
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Add source files
target_sources(NeuralAmpPlugin
        PRIVATE
        # Main source files
        Source/Main.cpp
        Source/AmpAudioProcessor.cpp
        Source/AmpAudioProcessor.h
        Source/AmpAudioProcessor.hpp

        # DSP files
        Source/dsp/Mappers.cpp
        Source/dsp/Mappers.h
        Source/dsp/ParameterSetup.h
        Source/dsp/Processor.cpp
        Source/dsp/Processor.h
        Source/dsp/Processor.hpp

        # DSP Bones
        Source/dsp/Bones/ToneStack.cpp
        Source/dsp/Bones/ToneStack.h
        Source/dsp/Bones/ResamplingNAM.cpp
        Source/dsp/Bones/ResamplingNAM.h

        # Look and Feel
        Source/looknfeel/GeneralLookAndFeel.h
        Source/looknfeel/StripKnob_look_n_feel.h
        Source/looknfeel/ToogleButtonLookAndFeel.h
        Source/looknfeel/TopBarLookAndFeel.h

        # UI
        Source/ui/gui.cpp
        Source/ui/gui.h
        Source/ui/gui.hpp
        Source/ui/components/TopBarComponent.cpp
        Source/ui/components/TopBarComponent.h
)

# Add binary resources (images and default models)
juce_add_binary_data(NeuralAmpPluginData
        SOURCES
        # Images
        Source/ressources/knob_strip.png
        Source/ressources/plate.png
        Source/ressources/toggleButtonStrip.png

        # Default models
        Source/ressources/defaultModels/HolyGrail.wav
        Source/ressources/defaultModels/room.wav
        Source/ressources/defaultModels/MetalLead.nam
)

# Preprocessor definitions
target_compile_definitions(NeuralAmpPlugin
        PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=1
        JUCE_STRICT_REFCOUNTEDPOINTER=1
        JUCE_MODAL_LOOPS_PERMITTED=1
        PRIVATE
        PIP_JUCE_EXAMPLES_DIRECTORY="C:/Users/Marius/Desktop/JUCE/examples"
)

# Add header search paths for NAM library
target_include_directories(NeuralAmpPlugin
        PRIVATE
        "C:/Users/Marius/Documents/JUCE/libraries/NeuralAmpModelerCore"
)

# Platform-specific settings
if(MSVC)
    target_compile_options(NeuralAmpPlugin PRIVATE /bigobj)
endif()

# Custom post-build command to copy VST3 to specific location
if(WIN32)
    set(VST3_DESTINATION_PATH "D:/vstcreation/BallzzyAmp.vst3/Contents/x86_64-win")

    add_custom_command(TARGET NeuralAmpPlugin_VST3 POST_BUILD
            # Create the necessary internal folder structure
            COMMAND ${CMAKE_COMMAND} -E make_directory "${VST3_DESTINATION_PATH}"

            # Copy the VST3 file
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:NeuralAmpPlugin_VST3>"
            "${VST3_DESTINATION_PATH}/$<TARGET_FILE_NAME:NeuralAmpPlugin_VST3>"
            COMMENT "Creating VST3 Bundle structure in D:/vstcreation"
    )
endif()
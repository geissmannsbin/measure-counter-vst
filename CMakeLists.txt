cmake_minimum_required(VERSION 3.15)

project(MeasureCounter VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE subdirectory
add_subdirectory(JUCE)

# Define the plugin target
juce_add_plugin(MeasureCounter
    COMPANY_NAME "doktrin"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Dktn
    PLUGIN_CODE Mctr
    FORMATS VST3 AU
    PRODUCT_NAME "MeasureCounter"
    DESCRIPTION "Visual measure counter for live performances"
    VST3_CATEGORIES "Fx" "Tools"
    AU_MAIN_TYPE "kAudioUnitType_Effect"
)

# Add source files
target_sources(MeasureCounter
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/MeasureDisplay.cpp
        Source/MeasureDisplay.h
)

# Compile definitions
target_compile_definitions(MeasureCounter
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
)

# Link required JUCE modules
target_link_libraries(MeasureCounter
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_plugin_client
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Quick Start Guide

## First-Time Setup

Since you don't have CMake installed yet, you'll need to install it first:

### Install CMake on macOS

**Option 1: Using Homebrew (recommended)**
```bash
# Install Homebrew if you don't have it
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install CMake
brew install cmake
```

**Option 2: Download from cmake.org**
1. Visit https://cmake.org/download/
2. Download the macOS .dmg file
3. Install and add to PATH

### Verify Installation
```bash
cmake --version
# Should show: cmake version 3.15 or higher
```

## Building the Plugin

Once CMake is installed:

```bash
# Make sure you're in the project directory
cd /Users/robingeissmann/dev/measure-counter-vst

# Create build directory
mkdir build
cd build

# Configure (this will take a few minutes the first time as JUCE builds)
cmake ..

# Build the plugin
cmake --build . --config Release

# Install to system plugin directory
# VST3:
cp -r MeasureCounter_artefacts/Release/VST3/MeasureCounter.vst3 ~/Library/Audio/Plug-Ins/VST3/

# AU:
cp -r MeasureCounter_artefacts/Release/AU/MeasureCounter.component ~/Library/Audio/Plug-Ins/Components/
```

## Testing the Plugin

1. Open your DAW (e.g., Logic Pro, Ableton Live, Reaper)
2. Rescan plugins if necessary
3. Insert "MeasureCounter" on any audio track
4. Press play in your DAW
5. The plugin should display the current bar and beat!

## Common Issues

### "JUCE not found" error
```bash
# Make sure JUCE submodule is initialized
git submodule update --init --recursive
```

### "Xcode not found" error
- Install Xcode from the App Store
- Run: `sudo xcode-select --install`

### Plugin doesn't appear in DAW
- Check the plugin was copied to the correct location
- Some DAWs cache plugin lists - try restarting the DAW
- Check Console.app for error messages

## Next Steps

- Read the full README.md for detailed information
- Customize colors and appearance in `Source/MeasureDisplay.cpp`
- Try it with different time signatures in your DAW
- Experiment with different cycle lengths (4, 8, 16, 32 bars)

Enjoy!

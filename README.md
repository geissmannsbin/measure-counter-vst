# MeasureCounter VST Plugin

<img width="1700" height="172" alt="Screenshot 2026-02-11 at 15 22 04" src="https://github.com/user-attachments/assets/b1a1f01b-c6a6-4f1a-9808-34456c4afd63" />


[![Latest Release](https://img.shields.io/github/v/release/geissmannsbin/measure-counter-vst)](https://github.com/geissmannsbin/measure-counter-vst/releases)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A JUCE-based VST3/AU plugin that provides clear visual feedback for measure counting during live performances. Perfect for musicians who need to stay synced during shows, rehearsals, or recording sessions.

## Features

- **Large, High-Contrast Display**: Easy to read from a distance, suitable for stage use
- **Real-Time Transport Sync**: Automatically syncs with your DAW's tempo, time signature, and playhead position
- **Configurable Cycle Lengths**: Choose between 4, 8, 16, or 32-bar cycles
- **Visual Feedback**:
  - Large bar number display (current bar within cycle)
  - Beat indicators showing current beat within the bar
  - Progress bars for both bar position and full cycle position
  - Color-coded warnings (yellow for second-to-last bar, red for last bar)
- **Manual Reset**: Reset the cycle to bar 1 at any time with the RESET button
- **Resizable Window**: Drag to any size, with smart scaling of all visual elements
- **Persistent Settings**: Window size and cycle length preferences are saved with your project
- **Multi-Time Signature Support**: Automatically adapts to any time signature from your DAW
- **Minimal CPU Usage**: No audio processing overhead

## Requirements

### Download Pre-Built Binaries

Pre-built plugins are available for macOS, Windows, and Linux on the [Releases page](https://github.com/geissmannsbin/measure-counter-vst/releases).

**Quick Installation:**

1. Download the `.zip` file for your platform from the latest release
2. Extract the archive
3. Copy the plugin files to your system's plugin directory:
   - **macOS**: VST3 → `~/Library/Audio/Plug-Ins/VST3/`, AU → `~/Library/Audio/Plug-Ins/Components/`
   - **Windows**: VST3 → `C:\Program Files\Common Files\VST3\`
   - **Linux**: VST3 → `~/.vst3/`
4. Restart your DAW or rescan plugins

**macOS Note:** First launch requires right-click > Open due to unsigned binaries (macOS Gatekeeper). Subsequent launches work normally.

---

### For Building

- **CMake** 3.15 or higher
- **C++17** compatible compiler
- **Git** (for cloning JUCE submodule)
- **macOS**: Xcode (for AU and VST3 builds)
- **Windows**: Visual Studio 2019 or later
- **Linux**: GCC 7+ or Clang 6+

### For Running

- A compatible DAW that supports VST3 or AU plugins
- Tested with: Logic Pro, Ableton Live, Reaper, Pro Tools

## Building the Plugin

### macOS

```bash
# Clone the repository (if not already done)
git clone https://github.com/geissmannsbin/measure-counter-vst.git
cd measure-counter-vst

# Initialize JUCE submodule
git submodule update --init --recursive

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the plugin
cmake --build . --config Release

# The built plugins will be in:
# build/MeasureCounter_artefacts/Release/VST3/MeasureCounter.vst3
# build/MeasureCounter_artefacts/Release/AU/MeasureCounter.component
```

### Windows

```bash
# Clone the repository
git clone https://github.com/geissmannsbin/measure-counter-vst.git
cd measure-counter-vst

# Initialize JUCE submodule
git submodule update --init --recursive

# Create build directory
mkdir build
cd build

# Configure with CMake (Visual Studio 2019)
cmake .. -G "Visual Studio 16 2019" -A x64

# Build the plugin
cmake --build . --config Release

# The built plugin will be in:
# build/MeasureCounter_artefacts/Release/VST3/MeasureCounter.vst3
```

### Linux

```bash
# Clone the repository
git clone https://github.com/geissmannsbin/measure-counter-vst.git
cd measure-counter-vst

# Initialize JUCE submodule
git submodule update --init --recursive

# Install dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install libasound2-dev libfreetype6-dev libx11-dev libxrandr-dev \
                     libxinerama-dev libxcursor-dev libgl1-mesa-dev

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the plugin
cmake --build . --config Release

# The built plugin will be in:
# build/MeasureCounter_artefacts/Release/VST3/MeasureCounter.vst3
```

## Installation

### macOS

**VST3:**
```bash
cp -r build/MeasureCounter_artefacts/Release/VST3/MeasureCounter.vst3 \
      ~/Library/Audio/Plug-Ins/VST3/
```

**AU:**
```bash
cp -r build/MeasureCounter_artefacts/Release/AU/MeasureCounter.component \
      ~/Library/Audio/Plug-Ins/Components/
```

### Windows

**VST3:**
Copy `MeasureCounter.vst3` to:
```
C:\Program Files\Common Files\VST3\
```

### Linux

**VST3:**
```bash
cp -r build/MeasureCounter_artefacts/Release/VST3/MeasureCounter.vst3 \
      ~/.vst3/
```

## Usage

1. **Load the Plugin**: Insert MeasureCounter on any track in your DAW (it's a pass-through effect, so it won't affect your audio)

2. **Position the Window**: Resize and position the plugin window where it's visible during performance (e.g., above your DAW interface)

3. **Select Cycle Length**: Use the dropdown menu to choose your desired cycle length (4, 8, 16, or 32 bars)

4. **Start Playback**: Press play in your DAW, and the plugin will automatically sync with the transport

5. **Reset if Needed**: Click the RESET button to restart the cycle from bar 1 at any time

## Visual Guide

### Display Layout

```
┌─────────────────────────────────────────────────────────┐
│  BAR                BEAT              PROGRESS           │
│   8          ● ○ ○ ○       Bar:    [■■■■■■□□□]           │
│  (RED)      (4/4 time)     Cycle:  [■■■■■■■■■]           │
│                                                           │
│         [RESET]     [Cycle: 8 Bars ▾]                    │
└─────────────────────────────────────────────────────────┘
```

### Color Coding

- **Cyan** (bars 1 through N-2): Normal operation
- **Yellow** (bar N-1): Warning - approaching the end of the cycle
- **Red** (bar N): Last bar - typically for fills, breaks, or transitions

## Troubleshooting

### Plugin doesn't appear in DAW

- Make sure the plugin is copied to the correct location
- Rescan plugins in your DAW
- Check that your DAW supports VST3 or AU format

### Transport not syncing

- Ensure your DAW's transport is playing
- Check that the plugin is receiving transport information (some DAWs require this to be enabled)
- Try closing and reopening the plugin window

### Display issues

- Try resizing the window - all elements scale automatically
- Make sure you're using a recent version of your DAW
- Check graphics drivers are up to date

### Build errors

- Ensure JUCE submodule is initialized: `git submodule update --init --recursive`
- Check that CMake version is 3.15 or higher
- Make sure you have all required dependencies installed

## Technical Details

- **Plugin Type**: Audio Effect (pass-through)
- **Formats**: VST3, AU
- **I/O Configuration**: Stereo in/out
- **MIDI**: Not required
- **Latency**: Zero
- **CPU Usage**: Minimal (GUI updates only)
- **State Saved**: Window size, cycle length, reset offset

## Customization

To modify the plugin (e.g., change colors, fonts, or default sizes):

1. Edit `Source/MeasureDisplay.cpp` for visual styling
2. Edit `Source/PluginEditor.h` to change default window size or resize constraints
3. Rebuild with CMake

## License

MIT License - see [LICENSE](LICENSE) file for details.

## Credits

Built with [JUCE Framework](https://juce.com/)

## Support

For issues, questions, or feature requests, please open an issue on GitHub.

---

**Enjoy your performances with MeasureCounter!** 🎵

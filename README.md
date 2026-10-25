# YI Trial

Trial VST3 audio effect (JUCE 8, CMake). It shows a temporary 16:9 test picture. The track's audio level moves the blue Sky band left and right, and the Scale and Brightness sliders change the selected layer. Audio passes through unchanged.

The shapes are placeholders. The layers and the audio binding live in one place: `makeTestScene()` in [Source/Scene.h](Source/Scene.h).

## Build (Windows)

Requirements: Visual Studio 2022 (Desktop development with C++), CMake 3.22+, git. JUCE is downloaded automatically during configure.

```
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Result: `build\YITrial_artefacts\Release\VST3\YI Trial.vst3` (a folder, copy it whole).

Every push also builds on GitHub Actions. The `YI-Trial-VST3` artifact on the run page has the same `.vst3`.

## Install in FL Studio 2026

1. Copy `YI Trial.vst3` to `C:\Program Files\Common Files\VST3\`.
2. In FL Studio: Options → Manage plugins → Find installed plugins (rescan).
3. Add **YI Trial** as an effect on a mixer track (or the master).

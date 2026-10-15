# Android Shader Parameter Editor — Implementation Plan

**Goal:** Give Android a shader parameter editor equivalent to the desktop
`ShaderParametersDialog`, with the editor's rules moved into shared, tested core code.

**Design:** [2026-10-15-android-shader-parameters-design.md](../specs/2026-10-15-android-shader-parameters-design.md)

**Method:** each task is red → green → refactor. A task is done when its tests were seen failing,
then passing, and the full `tests` / `qt-tests` suites still pass.

## Files

| File | Task |
| --- | --- |
| `Source/Core/VideoCommon/PostProcessing/ShaderParameterEditor.{h,cpp}` (new) | 1–3 |
| `Source/Core/VideoCommon/CMakeLists.txt` | 1 |
| `Source/UnitTests/VideoCommon/PostProcessing/ShaderParameterEditorTest.cpp` (new) | 1–3 |
| `Source/UnitTests/VideoCommon/CMakeLists.txt` | 1 |
| `Source/Core/DolphinQt/Config/Graphics/ShaderParametersDialog.{h,cpp}` | 4 |
| `Source/Android/jni/Config/ShaderParameterEditor.cpp` (new), `Config/PostProcessing.cpp`, `jni/CMakeLists.txt`, `AndroidCommon/IDCache.{h,cpp}` | 5 |
| `features/settings/model/ShaderParameterEditor.kt` (new), `PostProcessing.kt` | 5 |
| `features/settings/ui/shaderparameters/*` (new), `AndroidManifest.xml`, `strings.xml` | 6 |
| `features/settings/ui/SettingsFragmentPresenter.kt` | 7 |
| `src/test/.../ShaderParameterInputTest.kt` (new) | 6 |

## Task 1 — Slider mapping and availability (pure functions)

1. Write `ShaderParameterEditorTest.cpp` with:
   - `SliderMapping.WholeStepsGetOnePositionEach` (0..1 step 0.05 → 20 positions, increment 0.05)
   - `SliderMapping.CapsAtTenThousandAndStretchesIncrement` (0..1e6 step 1)
   - `SliderMapping.DegenerateRangesGetNoSlider` (step 0, step < 0, max == min, max < min,
     range narrower than one step)
   - `SliderMapping.NonMultipleRangeEndsExactlyAtMaximum` (0..1 step 0.3 → 3 positions; last = 1.0)
   - `SliderMapping.PositionNearestTheDefaultIsTheDefault`
   - `SliderMapping.PositionForValueRoundsAndClamps`
   - `Availability.*` ordering (preset, then library, then backend) and backend names
     (Vulkan/D3D/D3D12 yes; OGL/Metal/Software Renderer/Null/"" no).
2. Add the test to `Source/UnitTests/VideoCommon/CMakeLists.txt`; build; see it fail to compile.
3. Implement `MakeSliderMapping`, `SliderValueAt`, `SliderPositionFor`,
   `IsParameterEditingBackend`, `GetParameterEditingAvailability`; add files to
   `videocommon`. Tests green.

## Task 2 — Editor construction and queries

1. Tests: rows start at initial values; stored values applied; out-of-range stored value clamped;
   stored names not in the preset ignored; empty list → `NoParameters`; `Failed()` → `Error`
   with message and no rows; `IsDefault`; `FormatValue` uses `DecimalsForStep` and '.'.
2. Implement constructor, `Failed`, getters, `FormatValue`. Green.

## Task 3 — Editor mutators and persistence

1. Tests (Config fixture with a `NullLoader` base layer, as `LibrashaderParametersStorage`):
   `SetValue` clamps and persists the `name=value` string; unchanged value returns false and
   does not bump `CurrentGeneration`; `SetSliderPosition` maps through the slider; `Reset`
   removes the entry; `ResetAll` removes the key; `CollectOverrides` keeps preset order.
2. Implement mutators and `Open()` (`ResolveConfiguredPreset` → `ResolvePresetPath` →
   `KeyForPreset` → `Enumerate` → `Load`). Green.

## Task 4 — Desktop dialog onto the editor (refactor, no behaviour change)

1. `ShaderParametersDialog` keeps its widgets and holds a `std::unique_ptr<ShaderParameterEditor>`.
   Slider ranges come from `GetRows()[i].slider`, slider edits call `SetSliderPosition`, spin
   edits `SetValue`, Reset/Reset All the editor; the dialog only refreshes widgets.
2. Delete the dialog's `MAX_SLIDER_STEPS`, slider math, `ApplyOverrides`, `CollectOverrides`,
   `IsDefault`, `SaveOverrides`.
3. Build `dolphin-emu`, run `qt-tests` and `tests`.

## Task 5 — JNI bridge

1. `ShaderParameterEditor.kt`: `@Keep private val pointer`, `external` members per the design's
   §4.2 table, `companion fun open(spec)`.
2. `jni/Config/ShaderParameterEditor.cpp` implementing them; register the class and pointer
   field in `IDCache`; add the file to `jni/CMakeLists.txt`.
3. `PostProcessing.getParameterEditingAvailability(backend, spec)`.
4. Constants for status and availability shared through companion objects, mirroring the C++
   enum order (documented on both sides).

## Task 6 — Compose screen

1. `ShaderParameterInput.parse(text): Float?` + `ShaderParameterInputTest` (JVM): accepts
   `1.5`, `1,5`, `-0.25`, surrounding blanks; rejects `""`, `abc`, `1.2.3`, `NaN`, `Infinity`.
2. `ShaderParametersViewModel` (`UiState` = `Loading | Message(text) | Loaded(rows, canResetAll)`),
   `ShaderParametersScreen`, `ShaderParametersActivity` (`launch(context, preset, perGame)`,
   `onStop` saves the base layer), manifest entry, strings.

## Task 7 — Settings entry row

1. Row under the picker per design §4.3, availability from `PostProcessing`,
   `StringSetting.MAIN_GFX_BACKEND`, `StringSetting.GFX_ENHANCE_POST_SHADER`.
2. Per-game detection from `settings.isGameSpecific`.

## Task 8 — Verification

1. Windows: `dolphin-emu`, `tests`, `qt-tests` build and pass.
2. Android: `./gradlew assembleDebug testDebugUnitTest` (needs the Android SDK/NDK), then the
   manual checklist in design §5.

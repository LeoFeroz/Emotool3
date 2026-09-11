# emotool3

> **Credits and Attribution**
>
> The original code and tool are the work of **eterniti**. This repository is a separate project based on that original work and is not intended to claim ownership or authorship of the original tool.
>
> **All credits for the original tool, its code, and the original work belong to eterniti.**
>
> This project is maintained separately and contains modifications and additions made specifically for this project.

Command-line tool (C++) for manipulating files in the EMO/EMB/EMG/EMM/EMA (sssscommon) formats, with support for import/export through the Autodesk FBX SDK.

## Requirements

* **Windows 10/11**
* **Visual Studio 2022** (v17), with the **"Desktop development with C++"** workload

  * Platform Toolset: `v143`
  * Windows SDK: `10.0` (any recent version installed by VS)
* **Autodesk FBX SDK 2020.x** (tested with `2020.3.10`), **VS2022 / x64** version

The project only builds for the **x64** platform, in **Debug** and **Release** configurations.

## Installing the FBX SDK

1. Download the FBX SDK 2020.x (C++) from the Autodesk website.
2. Install it using the default installer. By default, it is installed at:

   ```
   C:\Program Files\Autodesk\FBX\FBX SDK\<version>
   ```
3. The project automatically detects the SDK in the following preferred order:

   * `C:\Program Files\Autodesk\FBX\FBX SDK\2020.3.10`
   * `C:\fbxsdk`

   If the SDK is installed in another location, manually set the `FBXSDK_DIR` variable (see below).

## Building with the IDE (Visual Studio)

1. Clone the repository.
2. Open `emotool3.sln` in Visual Studio 2022.
3. Select the configuration (`Debug` or `Release`) and the `x64` platform.
4. Build → *Build Solution* (`Ctrl+Shift+B`).

If the FBX SDK is not detected automatically, set the `FBXSDK_DIR` environment variable before opening Visual Studio, pointing to the SDK root folder (the one containing `include\` and `lib\`).

## Build Output

* Binary: `bin\x64\<Configuration>\emotool3.exe`
* Intermediate objects: `obj\x64\<Configuration>\emotool3\`

After the build, the `CopyRuntimeFiles` target automatically copies the following to the output folder:

* `libfbxsdk.dll` (from the corresponding FBX SDK configuration; Debug builds use the Release DLL as a fallback if a separate Debug DLL does not exist)
* All files from `BuildExtra\` (common to all configurations)
* All files from `BuildExtra\<Configuration>\` (specific to the current configuration, if the folder exists)

## Project Structure

```text
emotool3/
├── emotool3.sln
├── emotool3.vcxproj
├── emotool3/            # main source code (main.cpp, debug.cpp)
├── sssscommon/          # common library for reading/writing EMO/EMB/EMG/EMM/EMA/2ry formats
│   ├── tinyxml/         # third-party XML parser
│   └── vs/              # MSVC compatibility headers (e.g. dirent.h)
└── BuildExtra/          # extra files copied to the output folder after build
    ├── Debug/           # Debug configuration-specific files
    └── Release/         # Release configuration-specific files
```

## Troubleshooting

| Problem                                                    | Likely Cause                                          | Solution                                                                                                                             |
| ---------------------------------------------------------- | ----------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------ |
| `Cannot open include file: 'fbxsdk.h'`                     | FBX SDK not found                                     | Check the installation or set `FBXSDK_DIR`                                                                                           |
| `LNK1104: cannot open file 'libfbxsdk.lib'`                | Incorrect library path for the platform/configuration | Confirm that `$(FBXSDK_DIR)\lib\x64\<Config>\libfbxsdk.lib` exists                                                                   |
| Missing DLL error when running (`libfbxsdk.dll` not found) | DLL was not copied                                    | Check that the `CopyRuntimeFiles` target completed without errors; the DLL should be in `$(FBXSDK_DIR)\lib\x64\Release` (or `Debug`) |

```
```

# Repository Guidelines

## Project Structure & Module Organization

This is a Windows desktop C/C++ project for `simplewall`. Core application code lives in `src/`, with matching `.c`/`.h` pairs for major areas such as `wfp`, `profile`, `network`, `notifications`, and `listview`. `src/uwp.cpp` contains C++/WinRT integration, and `src/resource.rc` plus `src/res/` hold Win32 resources and embedded images. Runtime files, default profiles, and translations are under `bin/`; localization files are in `bin/i18n/`. Root Visual Studio files (`simplewall.sln`, `simplewall.vcxproj`, `.filters`) define the build. External sibling dependencies are declared in `.gitmodules` as `../routine` and `../builder`.

## Build, Test, and Development Commands

- `build_vc.bat`: initializes Visual Studio 2026 build tools and builds Release `x64` and `ARM64`.
- `msbuild simplewall.sln -property:Configuration=Debug -property:Platform=x64`: local debug build.
- `msbuild simplewall.sln -property:Configuration=Release -property:Platform=x64`: release build for x64.
- `build.bat`: delegates packaging/build work to the sibling `..\builder` repository.
- `build_locale.bat`: rebuilds localization assets through `..\builder`.
- `build_simplewall_rules.bat`: updates `bin/profile_internal.xml` rules through `..\builder`.

NuGet package restore is needed for `Microsoft.Windows.CppWinRT` if `packages/` is missing.

## Coding Style & Naming Conventions

Follow the existing C style: tabs for indentation, opening braces on their own line for functions and control blocks, and spaces before parenthesized calls such as `_app_profile_save (hwnd)`. Use Unicode/Win32 types consistently (`WCHAR`, `BOOLEAN`, `HRESULT`) and keep SAL annotations on function parameters. Internal functions use module prefixes, for example `_wfp_*`, `_app_*`, and `_r_*`. The project builds with warning level 3 and treats warnings as errors, so keep changes warning-clean.

## Testing Guidelines

There is no dedicated automated test suite in this repository. Validate changes by building the affected platforms with `msbuild` or `build_vc.bat`. For behavior touching WFP filters, profiles, services, UWP packages, or startup elevation, perform manual checks on a Windows system with administrator rights. Verify localization or resource changes by launching the built app and checking the relevant UI.

## Commit & Pull Request Guidelines

Recent history uses short, lower-case subjects such as `fix sdk update`, `upd project sdk`, and `code cosmetics`. Keep commits focused and concise; prefer prefixes like `fix`, `upd`, or a direct action phrase. Pull requests should describe the changed behavior, list the builds/manual checks performed, link related issues, and include screenshots for visible UI changes.

## Security & Configuration Notes

Do not commit machine-specific Visual Studio output or generated binaries beyond the tracked runtime assets already in `bin/`. Treat WFP behavior carefully: changes can affect system-wide network filtering even after the UI exits.

# Repository Guidelines

## Project Structure & Module Organization

This is a Windows desktop C/C++ project for `simplewall`. Core application code is in `src/`, with paired `.c`/`.h` modules for WFP, profiles, logging, networking, notifications, list views, search, and security. `src/uwp.cpp` contains C++/WinRT integration. Win32 resources live in `src/resource.rc` and `src/res/`. Runtime profiles, language files, and built binaries are under `bin/`, including translations in `bin/i18n/`. The root contains `simplewall.sln`, `simplewall.vcxproj`, and build helper scripts.

The sibling repositories declared by `.gitmodules` are expected at `..\routine` and `..\builder`.

## Build, Test, and Development Commands

- `build_vc.bat`: builds Release `x64` and `ARM64`; this fork supports Visual Studio 2026 Insiders.
- `cmd /c 'call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvarsall.bat" amd64 && msbuild simplewall.sln -property:Configuration=Debug -property:Platform=x64 -verbosity:minimal'`: debug x64 build.
- `cmd /c 'call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvarsall.bat" amd64 && msbuild simplewall.sln -property:Configuration=Release -property:Platform=x64 -verbosity:minimal'`: release x64 build.
- `build_locale.bat`: rebuilds localization assets through `..\builder`.
- `build_simplewall_rules.bat`: updates `bin/profile_internal.xml` through `..\builder`.

NuGet restore is required if `packages/Microsoft.Windows.CppWinRT.*` is missing.

## Fork Maintenance Workflow

This fork uses `origin` for `https://github.com/igormino104/simplewall.git` and `upstream` for `https://github.com/henrypp/simplewall.git`. Keep local fixes on `igor/vs2026-routine-compat`; do not work directly on `master`.

To update from upstream:

```powershell
git fetch upstream
git switch igor/vs2026-routine-compat
git rebase upstream/master
```

Resolve conflicts by preserving upstream changes when they replace local compatibility code. Keep this fork's compatibility layer only where current `..\routine` still requires it. After a successful rebase, build Debug and Release x64, then push with `git push --force-with-lease origin igor/vs2026-routine-compat`.

## Coding Style & Compatibility Notes

Follow the existing style: tabs, braces on separate lines for functions/blocks, Unicode Win32 types, SAL annotations, and module prefixes such as `_app_*`, `_wfp_*`, and `_r_*`. Warnings are treated as errors.

`src/global.h` contains compatibility wrappers mapping simplewall calls to the current `routine` API. Keep new wrappers minimal, named `_app_compat_*`, and remove them when upstream or `routine` no longer needs them.

## Testing Guidelines

There is no dedicated automated test suite. Validate changes with Debug and Release x64 builds. For WFP, service, profile, UWP, or elevation changes, manually test on Windows with administrator rights.

## Commit Guidelines

Keep commits focused: build support, compatibility fixes, and docs should remain separate. Use concise subjects such as `build: support Visual Studio 2026 Insiders`, `compat: adapt simplewall to current routine API`, or `docs: update repository guide`.

# ExampleMod

A starter template for Skyrim Special Edition / Anniversary Edition SKSE plugins using
[CommonLibSSE-NG](https://github.com/alandtse/CommonLibVR/tree/ng), [CMake](https://cmake.org), and [vcpkg](https://vcpkg.io).

Supports building on **Linux** (cross-compilation via `clang-cl` + [xwin](https://github.com/Jake-Shadle/xwin)) and **Windows** (MSVC).

---

<!-- nexus:start -->
## Requirements

- [SKSE64](https://skse.silverlock.org/)
- [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444)

## Installation

**Mod manager (recommended):**
1. Install the requirements above.
2. Install ExampleMod via your mod manager.
3. Launch Skyrim via SKSE.

**Manual:**
1. Install the requirements above.
2. Copy `ExampleMod.dll` to `Data\SKSE\Plugins\`.
3. Launch Skyrim via SKSE.

## Compatibility

- Compatible with Skyrim SE and AE.
- No ESP/ESL required.
<!-- nexus:end -->

---

## What the Starter Plugin Does

When loaded by Skyrim the plugin:

1. **Writes a log** to `Data/SKSE/Plugins/ExampleMod.log` via spdlog.
2. **Hooks `kDataLoaded`** (fires once all game data is loaded).
3. **Prints to the in-game console** (`~` key): `[ExampleMod] Loaded successfully!`

---

## Development

### Prerequisites

#### All platforms
- [Git](https://git-scm.com/)
- [CMake](https://cmake.org/download/) 3.21+
- [vcpkg](https://vcpkg.io/en/getting-started) — set `VCPKG_ROOT` in your environment

#### Linux
- LLVM/Clang (provides `clang-cl`, `lld-link`, `llvm-lib`, `llvm-rc`, `llvm-mt`)
- [xwin](https://github.com/Jake-Shadle/xwin) — downloads the real Windows SDK and MSVC CRT headers/libs
- [Ninja](https://ninja-build.org/)

```bash
# Arch / CachyOS
sudo pacman -S clang lld llvm ninja

# Install xwin (requires Rust/cargo)
cargo install xwin

# Fetch Windows SDK + MSVC CRT headers to ~/.xwin  (one-time, ~700 MB)
xwin splat --output ~/.xwin
```

> **Note:** On first configure, `cmake/toolchains/clang-cl-cross.cmake` creates
> TitleCase symlinks inside your xwin installation, e.g.:
> ```
> ~/.xwin/sdk/lib/um/x86_64/Advapi32.lib  ->  advapi32.lib
> ```
> lld-link is case-sensitive but CommonLibSSE-NG references libs with mixed-case names.
> The originals are untouched.

#### Windows
- [Visual Studio 2022](https://visualstudio.microsoft.com/) with **Desktop development with C++**

---

### Getting Started

#### 1. Use this template

Click **"Use this template"** on GitHub, or clone and re-initialise:

```bash
git clone https://github.com/your-org/your-mod.git
cd your-mod
```

#### 2. Run the init script

**Linux** — run interactively or pass arguments directly:

```bash
./scripts/init.sh
# or:
./scripts/init.sh "AuthorName" "ModName"
```

**Windows (PowerShell)**:

```powershell
.\scripts\init.ps1
# or:
.\scripts\init.ps1 "AuthorName" "ModName"
```

This will:
- Replace mod name and author placeholders across all files
- Initialise git submodules (CommonLibSSE-NG + vcpkg)
- Bootstrap vcpkg
- Copy `.env.example` → `.env` with a reminder to fill in your paths

#### 3. Configure deploy path

Edit the `.env` file created by the init script and set `SKYRIM_MODS_FOLDER` to your mod manager's staging folder:

```bash
# Vortex (Linux, Steam):
SKYRIM_MODS_FOLDER=$HOME/.local/share/Steam/steamapps/common/Vortex Mods/skyrimse

# MO2 (Linux):
# SKYRIM_MODS_FOLDER=$HOME/MO2/mods
```

#### 4. Build

```bash
./scripts/build.sh
# or directly:
cmake --preset release-linux
cmake --build --preset release-linux
```

The DLL lands in `build/release-linux/ExampleMod.dll`.

##### Deploy to mod manager

```bash
./scripts/deploy.sh
# or directly (deploy.sh sources .env for you; cmake does not):
source .env && cmake --workflow --preset deploy
```

This configures, builds, and copies `ExampleMod.dll` + `ExampleMod.pdb` directly into:
```
$SKYRIM_MODS_FOLDER/ExampleMod/SKSE/Plugins/
```

Vortex will detect the new mod folder automatically. Enable it in Vortex, then launch Skyrim.

On subsequent builds (no config change needed):
```bash
./scripts/deploy.sh
```

##### Windows (MSVC)

```bash
cmake --preset release-windows
cmake --build --preset release-windows
```

The DLL lands in `build/msvc/Release/ExampleMod.dll`.

#### 5. Running tests

Unit tests run as a native Windows executable via the `test-windows` preset:

```bash
cmake --preset test-windows
cmake --build --preset test-windows
ctest --preset test-windows
```

Tests live in `test/` and use [Catch2](https://github.com/catchorg/Catch2). Only pure-logic code (no RE::/SKSE:: APIs) can be tested this way. See `src/Utils.h` and `test/ExampleTests.cpp` for the pattern.

---

### Git Hooks (Lefthook)

Prerequisites:

- `go install github.com/evilmartians/lefthook@latest`
- `clang-format` (part of LLVM — already required for development)
- `clang-tidy` (part of LLVM — already required for development)
- `cmake-format` (`sudo pacman -S cmake-format` on Arch/CachyOS; `pip install cmakelang` elsewhere)
- `shellcheck` (`sudo pacman -S shellcheck` on Arch/CachyOS)

```bash
lefthook install
```

---

### Editor Setup (clangd / Neovim)

CMake writes `compile_commands.json` to the build directory automatically.
Copy or symlink it to the project root so clangd picks it up:

```bash
# After configuring:
ln -sf build/release-linux/compile_commands.json compile_commands.json
```

The `.clangd` file already sets `--target=x86_64-pc-windows-msvc` so clangd
resolves Windows headers correctly on Linux.

Recommended Neovim plugins: [nvim-lspconfig](https://github.com/neovim/nvim-lspconfig)
with `clangd`, and [clangd_extensions.nvim](https://github.com/p00f/clangd_extensions.nvim).

---

### Updating CommonLibSSE-NG

```bash
git submodule update --remote lib/commonlibsse-ng
git add lib/commonlibsse-ng
git commit -m "chore: update CommonLibSSE-NG submodule"
```

---

### Nexus Mods Page

The `<!-- nexus:start/end -->` block at the top of `README.md` is the **source of truth** for Requirements, Installation, and Compatibility. `docs/nexus-page.md` holds the rest of the BBCode page (overview, tagline, credits).

To update the Nexus page:

1. Edit Requirements/Installation/Compatibility inside the `<!-- nexus:start/end -->` block in `README.md`.
2. Edit overview, tagline, and credits directly in `docs/nexus-page.md`.
   > **Do not edit** the `<!-- generated:start/end -->` block in `docs/nexus-page.md` — it is overwritten every time the script runs.
3. Generate the combined BBCode output:

```bash
python3 scripts/generate-nexus-page.py

# Or pipe straight to the clipboard:
python3 scripts/generate-nexus-page.py | xclip -selection clipboard  # Linux
python3 scripts/generate-nexus-page.py | pbcopy                       # macOS
```

4. Paste the output into the Nexus Mods page editor.

---

### CI

| Workflow | Trigger | What it does |
|---|---|---|
| `setup.yml` | First push in a repo created from this template | Renames placeholders using the repo name, then self-deletes |
| `ci.yml` | PRs to `main` touching `src/`, `test/`, `cmake/`, `vcpkg.json`, `CMakeLists.txt`, `CMakePresets.json` | `clang-format` (ubuntu) → `test` + `build` (windows, parallel) → `clang-tidy` (windows) |
| `release.yml` | Push of a `v*` tag | Builds, packages via `scripts/package.sh`, publishes a GitHub Release with zip + PDB |
| `nexus-upload.yml` | Release published or manual `workflow_dispatch` | Downloads release zip, generates cliff release notes, uploads to Nexus Mods |
| `lint.yml` | PRs touching `scripts/` | Runs shellcheck on shell scripts |
| `pr-title.yml` | PR opened/edited/reopened/synchronize | Checks PR title follows Conventional Commits (`feat`, `fix`, `chore`, `refactor`) |

#### Nexus Mods Upload

`nexus-upload.yml` triggers automatically when a GitHub Release is published, or can be run manually via **workflow_dispatch** with a version input.

**Prerequisites (one-time setup):**
1. Upload your first file manually via the [Nexus Mods web UI](https://www.nexusmods.com) — this creates the file group.
2. Note the `file_group_id` from the URL or mod manager.
3. Add to your repository:
   - **Secret** `NEXUSMODS_API_KEY` — your Nexus Mods API key (Settings → Secrets → Actions)
   - **Variable** `NEXUSMODS_FILE_GROUP_ID` — the file group ID (Settings → Variables → Actions)

---

## License

MIT — see [LICENSE](LICENSE).


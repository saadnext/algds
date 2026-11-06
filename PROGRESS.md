# PROGRESS

## Environment
- WSL Ubuntu, VS Code + clangd, GCC 15.2 (primary), Clang 21.1.8, CMake 4.2.3, git 2.53, Ninja
- gdb 17.1, valgrind 3.26.0, clang-format/clang-tidy 21.1.8
- C11 (`-std=c11`), 4-space indent
- Repo: github.com/saadnext/algds (public), branch `main`, remote `origin`

## Current position
- Phase 0, Step 0.3: Test harness + ctest (IN PROGRESS)

## Passed steps
- 0.1 Repo, git, GitHub, editor config (PASS)
- 0.2 CMake: warnings, -std=c11, sanitizer option (PASS)

## Weak topics / concepts I struggled with
- Pointer-to-pointer (shaky)
- realloc misuse (shaky)
- Function pointers (never used)
- Debuggers (never used)
- Reading which flag a GCC warning comes from (the [-Wxxx] bracket)

## Review queue (spaced repetition)
- Git's three places (working folder / local history / GitHub) and which one is a backup
- Why .clangd needs a configured build dir (compile_commands.json) on a fresh clone
- PRIVATE vs PUBLIC in target_link_libraries / target_include_directories

## Decisions log
- Build system: CMake + Ninja generator
- Static library `algds`, opaque structs
- Symbol prefix `algds_`, macros `ALGDS_`, include path `<algds/...>`
- Folder names: include/algds/, src/, benchmark/, docs/, examples/, tests/ (created in 0.3)
- Build dirs: build-deb, build-rel, build-asan, build-clang (all ignored via build*/)
- .clangd points at build-deb
- Option ALGDS_SANITIZE enables address+undefined (-g -O1) via INTERFACE target project_sanitizers
- Warnings in INTERFACE target project_warnings, linked PRIVATE to the library
- Include dir is PUBLIC; TODO later: $<BUILD_INTERFACE:...> before `install`
- C_EXTENSIONS OFF so -std=c11 is strict (not gnu11)
- Header contracts live in .h files, not .c files
- .vscode/ ignored (personal settings); .clangd and .clang-format committed
- Testing: hand-made C harness + ctest; no Catch2 (C++). Revisit Unity/cmocka later
- CI: GitHub Actions in Step 0.4 (gcc/clang matrix, ASan/UBSan, Valgrind)
- Commits: Conventional Commits, message text supplied by mentor
- Repo is public: mentor can read by URL
- Optional/undecided: warnings on examples, -Wstrict-prototypes

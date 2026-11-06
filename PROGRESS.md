# PROGRESS

## Environment
- WSL Ubuntu, VS Code + clangd, GCC 15.2 (primary), Clang 21.1.8, CMake 4.2.3, git 2.53
- gdb 17.1, valgrind 3.26.0, clang-format/clang-tidy 21.1.8
- C11 (`-std=c11`), 4-space indent
- Repo: github.com/saadnext/algds (public), branch `main`, remote `origin`

## Current position
- Phase 0, Step 0.2: CMake (warnings, -std=c11, sanitizer option) (IN PROGRESS)

## Passed steps
- 0.1 Repo, git, GitHub, editor config (PASS)

## Weak topics / concepts I struggled with
- Pointer-to-pointer (shaky)
- realloc misuse (shaky)
- Function pointers (never used)
- Debuggers (never used)

## Review queue (spaced repetition)
- Git's three places (working folder / local history / GitHub) and which one is a backup
- Why .clangd needs build/ to exist (compile_commands.json)
- Verify format-on-save in user settings

## Decisions log
- Build system: CMake
- Static library, opaque structs
- Symbol prefix `algds_`, macros `ALGDS_`, include path `<algds/...>`
- Folder names: include/algds/, src/, benchmark/, docs/, examples/; tests/ created in Step 0.3
- .vscode/ ignored (personal settings); .clangd and .clang-format committed
- Testing: hand-made C harness + ctest; no Catch2 (C++). Revisit Unity/cmocka later
- CI: GitHub Actions in Step 0.4 (gcc/clang matrix, ASan/UBSan, Valgrind)
- Commits: Conventional Commits, message text supplied by mentor
- Repo is public: mentor can read by URL

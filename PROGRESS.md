# PROGRESS

## Environment
- WSL Ubuntu, VS Code + clangd, GCC 15.2 (primary), Clang 21.1.8, CMake 4.2.3, git 2.53, Ninja
- gdb 17.1, valgrind 3.26.0, clang-format/clang-tidy 21.1.8
- C11 (`-std=c11`), 4-space indent
- Repo: github.com/saadnext/algds (public), branch `main`, remote `origin`

## Current position
- Phase 1, Step 1.2: vector extras (pop, reserve, allocator hooks, function pointers) (NOT STARTED)

## Passed steps
- 0.1 Repo, git, GitHub, editor config (PASS)
- 0.2 CMake: warnings, -std=c11, sanitizer option (PASS)
- 0.3 Test harness + ctest (PASS)
- 0.4 GitHub Actions CI (PASS)
- 0.5 gdb basics (PASS, reduced scope: crash hunting with bt/frame/print; rest learned when needed)
- 1.1 Vector core: create/destroy/push/get/size (PASS)

## Weak topics / concepts I struggled with
- Pointer-to-pointer (improving: understood create/destroy/push distinction; recheck in 1.2)
- realloc misuse (fixed in push via tmp + local new_capacity; keep reviewing)
- Function pointers (never used; teach right before first need)
- Debuggers (used once: run, bt, frame, print)
- Reading which flag a GCC warning comes from (the [-Wxxx] bracket)
- Argument order in expected/actual style macros (swapped once)
- Build dir names are local; CI uses its own (build vs build-deb mix-up)
- Checking the right variable after malloc (*out vs out)
- Writing header contracts (mentor supplied the model in 1.1; write them yourself in 1.2)

## Review queue (spaced repetition)
- Git's three places (working folder / local history / GitHub) and which one is a backup
- Why .clangd needs a configured build dir (compile_commands.json) on a fresh clone
- PRIVATE vs PUBLIC in target_link_libraries / target_include_directories
- Why do { } while (0) in multi-statement macros
- Test exit code -> how ctest decides pass/fail
- REQUIRE's hidden `return`: only valid in int-returning functions with `failed` in scope
- Why ASan and Valgrind can't run together (both intercept malloc/free)
- push vs pull_request triggers: what each protects
- Where CMake puts executables (build dir of the CMakeLists that defines them)
- In a backtrace, the first frame in your own code is where to look (not the libc frame)
- When does a function need `**`: only when it must change the caller's pointer variable itself (create yes; destroy-by-value, push, get no)
- Overflow check form: test `a > SIZE_MAX / b` BEFORE multiplying; checking after is too late
- Why doubling is O(n) total copying while fixed +k is O(n^2)
- Passing tests do not prove correctness: R1-R4 were invisible to the test suite

## Decisions log
- Build system: CMake + Ninja generator
- Static library `algds`, opaque structs
- Symbol prefix `algds_`, macros `ALGDS_`, include path `<algds/...>`
- Folder names: include/algds/, src/, benchmark/, docs/, examples/, tests/
- Build dirs: build-deb, build-rel, build-asan, build-clang (all ignored via build*/)
- .clangd points at build-deb
- Option ALGDS_SANITIZE enables address+undefined (-g -O1) via INTERFACE target project_sanitizers
- Option ENABLE_PROJECT_WARNINGS (default ON); flags live in INTERFACE target project_warnings, linked PRIVATE to the library
- Include dir is PUBLIC; TODO later: $<BUILD_INTERFACE:...> before `install`
- C_EXTENSIONS OFF so -std=c11 is strict (not gnu11)
- Header contracts live in .h files, not .c files
- .vscode/ ignored (personal settings); .clangd and .clang-format committed
- Testing: hand-made harness in tests/test_harness.h (CHECK, CHECK_STR_EQ, REQUIRE, TEST_STATUS)
- Test counter is a local `int failed` in each test's main (no extern/global); revisit when tests are split into functions
- Revisit C frameworks (Unity/cmocka) later; no Catch2 (C++)
- CI: .github/workflows/ci.yml; push + PR to main; matrix {gcc,clang} x {Debug+ASan/UBSan, Release}; separate Valgrind job (Debug, no sanitizers, ./build/test_version)
- actions/checkout@v7 works (fallback @v6 if it ever fails)
- Commits: Conventional Commits, message text supplied by mentor
- Repo is public: mentor can read by URL (Actions logs need login, so paste failing logs)
- Optional/undecided: warnings on examples, -Wstrict-prototypes
- PACE RULE: learn tools just in time. No standalone tooling drills; function pointers and allocator hooks are taught right before their first use; API/error design is decided inside each structure's step.
- Review rule: mentor gives ONE complete issue list per review round, no new items later.
- Error style: one enum `algds_status` in include/algds/error.h {OK, ERR_INVALID_ARG, ERR_NOMEM, ERR_RANGE}; overflow reports NOMEM
- Vector: opaque handle, elem stored by value (void* + elem_size), starts at capacity 0, first growth to 4, then x2, clamped to SIZE_MAX / elem_size
- create(algds_vec **out, size_t elem_size): status return, *out = NULL on failure when out non-NULL
- destroy(algds_vec **v): frees and sets *v = NULL; NULL or *v == NULL is a no-op (option B chosen by me)
- get copies into a caller buffer (no pointer into storage); out of range -> ALGDS_ERR_RANGE
- size(NULL) == 0, documented
- Header docs pattern: purpose, params, returns per status code, notes (ownership, complexity); document promises, not implementation

## Notes for later
- Push of an element that points into the same vector (realloc may move it before memcpy): decide when adding pointer access in 1.2
- Possible `algds_vec_at` returning a pointer (faster, but invalidated by push)
- Dead `!v->data` check in get; duplicate error.h include in vector.c (optional cleanup)
- Out-of-memory paths (create malloc failure, push realloc failure) untestable until allocator hooks in 1.2
- Possible overflow helper shared by push/reserve
- Revisit opaque handle vs init/deinit with benchmark numbers in Phase 10

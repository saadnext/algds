# PROGRESS

## Environment

- Linus Mint, VS Code + clangd, GCC (primary), Clang 18.1.3, CMake, git, Ninja
- gdb, valgrind 3.22.0, clang-format/clang-tidy
- (versions re-checked from the 1.2 output; update if your machine differs)
- C11 (`-std=c11`), 4-space indent
- Repo: github.com/saadnext/algds (public), branch `main`, remote `origin`

## Current position

- Phase 1, Step 1.3: vector extras (pop, reserve, possibly pointer access) (NOT STARTED)
- Then 1.4: string builder

## Passed steps

- 0.1 Repo, git, GitHub, editor config (PASS)
- 0.2 CMake: warnings, -std=c11, sanitizer option (PASS)
- 0.3 Test harness + ctest (PASS)
- 0.4 GitHub Actions CI (PASS)
- 0.5 gdb basics (PASS, reduced scope: crash hunting with bt/frame/print; rest learned when needed)
- 1.1 Vector core: create/destroy/push/get/size (PASS)
- 1.2 Allocator hooks + function pointers + failure tests (PASS)

## Weak topics / concepts I struggled with

- Pointer-to-pointer (much better after 1.1/1.2; recheck in 1.3)
- realloc misuse (fixed in push via tmp + local new_capacity; keep reviewing)
- Function pointers (learned in 1.2: typedef syntax OK; `ctx` role needed two explanations)
- Debuggers (used once: run, bt, frame, print)
- Reading which flag a GCC warning comes from (the [-Wxxx] bracket)
- Argument order in expected/actual style macros (swapped once)
- Build dir names are local; CI uses its own (build vs build-deb mix-up)
- Checking the right variable after malloc (*out vs out)
- Leftover names after a rename (allocater -> allocator broke the build once)
- Header contracts: delegated to mentor from 1.2 on (see decisions); still read them against the code

## Review queue (spaced repetition)

- Git's three places (working folder / local history / GitHub) and which one is a backup
- Why .clangd needs a configured build dir (compile_commands.json) on a fresh clone
- PRIVATE vs PUBLIC in target_link_libraries / target_include_directories
- Why do { } while (0) in multi-statement macros
- Test exit code -> how ctest decides pass/fail
- REQUIRE's hidden bare `return;`: only valid in void functions (harness changed in 1.2)
- Why ASan and Valgrind can't run together (both intercept malloc/free)
- push vs pull_request triggers: what each protects
- Where CMake puts executables (build dir of the CMakeLists that defines them)
- In a backtrace, the first frame in your own code is where to look (not the libc frame)
- When does a function need `**`: only when it must change the caller's pointer variable itself (create yes; destroy-by-value, push, get no)
- Overflow check form: test `a > SIZE_MAX / b` BEFORE multiplying; checking after is too late
- Why doubling is O(n) total copying while fixed +k is O(n^2)
- Passing tests do not prove correctness: R1-R4 were invisible to the test suite
- Why the allocator struct is copied into the vector, and what must outlive it (ctx target)
- Why `ctx` is both a struct field (stores) and a function parameter (delivers)
- destroy order: copy the allocator out of the handle BEFORE freeing the handle
- A failed operation can leave a hidden invariant broken (capacity vs block size); test the call AFTER the failure too
- Mutation checks: break one line, the right test must go red, then revert

## Decisions log

- Build system: CMake + Ninja generator
- Static library `algds`, opaque structs (except the public fill-in struct `algds_allocator`)
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
- Harness (changed in 1.2): failure counter is a file-level `static int tg_test_failures`; tests are `static void` functions called from main; REQUIRE does a bare `return;` so it is valid only in void functions
- Revisit C frameworks (Unity/cmocka) later; no Catch2 (C++)
- CI: .github/workflows/ci.yml; push + PR to main; matrix {gcc,clang} x {Debug+ASan/UBSan, Release}; separate Valgrind job (Debug, no sanitizers) runs test_version AND test_vector
- actions/checkout@v7 works (fallback @v6 if it ever fails)
- Commits: Conventional Commits, message text supplied by mentor
- Repo is public: mentor can read by URL (Actions logs need login, so paste failing logs)
- Optional/undecided: warnings on examples, -Wstrict-prototypes
- PACE RULE: learn tools just in time. No standalone tooling drills; function pointers and allocator hooks are taught right before their first use; API/error design is decided inside each structure's step.
- Review rule: mentor gives ONE complete issue list per review round, no new items later.
- DIVISION OF WORK (from 1.2): I write all library code and make all design decisions. Mentor writes header docs and tests (fakes and helpers included). I review them, run all environments, and do mutation checks (break one thing, right test must go red) every test round.
- Error style: one enum `algds_status` in include/algds/error.h {OK, ERR_INVALID_ARG, ERR_NOMEM, ERR_RANGE}; overflow reports NOMEM
- Vector: opaque handle, elem stored by value (void* + elem_size), starts at capacity 0, first growth to 4, then x2, clamped to SIZE_MAX / elem_size
- create(algds_vec **out, size_t elem_size) is a one-line wrapper for create_ex(out, elem_size, NULL); *out = NULL on failure when out non-NULL; create_ex allocates into a local and writes *out only at the end
- destroy(algds_vec **v): frees and sets *v = NULL; NULL or *v == NULL is a no-op (option B chosen by me)
- get copies into a caller buffer (no pointer into storage); out of range -> ALGDS_ERR_RANGE; dead `!v->data` check removed (invariant: capacity > 0 implies data != NULL)
- size(NULL) == 0, documented
- Allocator: struct `algds_allocator` {alloc_fn, realloc_fn, free_fn, ctx}; all three take `void *ctx` first; free has no size parameter
- Allocator lives INSIDE each vector (copied at create_ex); the ctx target must outlive the vector
- NULL allocator in create_ex means the default malloc/realloc/free; the vector never stores NULL
- Default allocator returned BY VALUE: `algds_allocator algds_default_allocator(void)`; wrappers are static in src/allocator.c
- Spelling: `allocator` everywhere (files allocator.h / allocator.c, guard, field)
- create_ex takes `const algds_allocator *`
- Header docs pattern: purpose, params, returns per status code, notes (ownership, complexity); document promises, not implementation

## Notes for later

- Push of an element that points into the same vector (realloc may move it before memcpy): decide when adding pointer access
- Possible `algds_vec_at` returning a pointer (faster, but invalidated by push)
- create_ex does not validate NULL function pointers inside a non-NULL allocator (documented caller rule); maybe return INVALID_ARG or assert later
- Possible overflow helper shared by push/reserve
- Duplicate include of allocator.h in vector.c (vector.h already includes it): optional cleanup for .clangd
- Run mutation 1 (capacity before realloc) once under build-asan to see the heap-buffer-overflow report
- Revisit opaque handle vs init/deinit with benchmark numbers in Phase 10

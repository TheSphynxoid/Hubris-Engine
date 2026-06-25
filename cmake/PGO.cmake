# PGO.cmake
#
# Link-Time Code Generation (LTCG/LTO) and Profile-Guided Optimization (PGO)
# infrastructure for MSVC. Wired but intentionally not driven yet — see the
# "When to use this" note below and Phase_Plan.md.
#
# Three modes, selected by cache variables:
#
#   HUBRIS_ENABLE_LTO     ON  -> whole-program optimization (LTCG) on Release/RelWithDebInfo.
#                                Zero workflow cost; this is the recommended baseline.
#   HUBRIS_PGO_INSTRUMENT ON  -> instrumented build; emit .pgc profile counters on exit.
#   HUBRIS_PGO_OPTIMIZE   ON  -> rebuild using the merged .pgd profile.
#
# PGO_INSTRUMENT and PGO_OPTIMIZE are mutually exclusive. Both imply LTCG
# (PGO is layered on top of whole-program IL).
#
# Usage from the root CMakeLists.txt (before add_subdirectory):
#
#   include("${CMAKE_SOURCE_DIR}/cmake/PGO.cmake")
#   hubris_apply_optimization(HubrisEngine)
#   hubris_apply_optimization(Sandbox)
#
# ---------------------------------------------------------------------------
# When to use this (read before enabling PGO)
# ---------------------------------------------------------------------------
# PGO is only as good as the workload that generates the profile. Instrumenting
# a build that has no representative hot loop produces a *misleading* profile
# that can pessimize real code once it lands. Do NOT enable HUBRIS_PGO_* until
# there is a deterministic demo exercising real frame work (render graph
# traversal, draw-list generation, shader binding, ECS queries) — i.e. after
# the renderer (Phase 3) and scene (Phase 4) exist. Plain LTCG (HUBRIS_ENABLE_LTO)
# is safe at any time and is the recommended Release default.
#
# Full PGO workflow (MSVC), once a workload exists:
#
#   1. cmake -DHUBRIS_PGO_INSTRUMENT=ON  -DCMAKE_BUILD_TYPE=Release ...
#      cmake --build ... --config Release
#   2. Run the instrumented binary on the representative workload(s).
#      Each run writes <exe>.pgc / <exe>-<n>.pgc next to the binary.
#   3. pgomgr /merge *.pgc <exe>.pgd      # combine runs into one profile
#   4. cmake -DHUBRIS_PGO_INSTRUMENT=OFF -DHUBRIS_PGO_OPTIMIZE=ON ...
#      cmake --build ... --config Release
# ---------------------------------------------------------------------------

option(HUBRIS_ENABLE_LTO     "Enable whole-program LTCG/LTO on Release configs" ON)
option(HUBRIS_PGO_INSTRUMENT "Build with PGO instrumentation (requires a representative workload run)" OFF)
option(HUBRIS_PGO_OPTIMIZE   "Rebuild using the collected PGO profile" OFF)

if(HUBRIS_PGO_INSTRUMENT AND HUBRIS_PGO_OPTIMIZE)
    message(FATAL_ERROR
        "HUBRIS_PGO_INSTRUMENT and HUBRIS_PGO_OPTIMIZE are mutually exclusive. "
        "Instrument first, run the workload, merge the profile, then rebuild with OPTIMIZE.")
endif()

# LTCG is implied by either PGO mode.
if(HUBRIS_PGO_INSTRUMENT OR HUBRIS_PGO_OPTIMIZE)
    set(_hubris_lto_requested TRUE)
elseif(HUBRIS_ENABLE_LTO)
    set(_hubris_lto_requested TRUE)
else()
    set(_hubris_lto_requested FALSE)
endif()

# Apply LTCG via CMake's portable INTERPROCEDURAL_OPTIMIZATION property — this
# maps to /GL + /LTCG on MSVC, -flto on GCC/Clang. Must be set per-target per-config.
function(hubris_apply_lto TARGET_NAME)
    if(NOT _hubris_lto_requested)
        return()
    endif()

    # Check the toolchain actually supports it before silently dropping the flag.
    include(CheckIPOSupported)
    check_ipo_supported(RESULT _ipo_ok LANGUAGES CXX)
    if(NOT _ipo_ok)
        message(WARNING "IPO/LTO requested but not supported by this toolchain; skipping for ${TARGET_NAME}.")
        return()
    endif()

    # Release and RelWithDebInfo only — never in Debug (LTO defeats fast iteration and
    # is incompatible with /ZI EditAndContinue debug info).
    # NOTE: do NOT set the bare INTERPROCEDURAL_OPTIMIZATION property — it applies to
    # all configs including Debug. Set per-config only.
    set_target_properties(${TARGET_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE             TRUE
        INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO      TRUE
    )
    message(STATUS "LTCG/LTO enabled for ${TARGET_NAME} (Release/RelWithDebInfo).")
endfunction()

# Apply PGO instrumentation or optimization flags (MSVC only). No-op when
# neither PGO option is set, so callers can always invoke it safely.
#
# PGO is a final-link operation: /LTCG:PGINSTRUMENT and /LTCG:PGOPTIMIZE are
# link.exe flags applied when producing the final executable image, and the
# profile (.pgd) is tied to that executable's image + workload. A static library
# has no link step and is never "run", so PGO on a STATIC_LIBRARY target would
# only redundantly set /GL (already handled by hubris_apply_lto) and silently
# drop the link options. Restrict real PGO work to EXECUTABLE targets; the lib
# just needs /GL from LTCG so its bitcode participates in the exe's PGO link.
function(hubris_apply_pgo TARGET_NAME)
    if(NOT MSVC)
        if(HUBRIS_PGO_INSTRUMENT OR HUBRIS_PGO_OPTIMIZE)
            message(WARNING "PGO flags are MSVC-only; ignoring for non-MSVC ${TARGET_NAME}.")
        endif()
        return()
    endif()

    if(NOT (HUBRIS_PGO_INSTRUMENT OR HUBRIS_PGO_OPTIMIZE))
        return()
    endif()

    get_target_property(_target_type ${TARGET_NAME} TYPE)
    if(NOT _target_type STREQUAL "EXECUTABLE")
        # Only executables drive a PGO cycle; static/shared libs have no link
        # step and no workload. Their /GL is already handled by hubris_apply_lto.
        message(STATUS "PGO: skipping ${TARGET_NAME} (${_target_type}) — PGO only applies to executables.")
        return()
    endif()

    if(HUBRIS_PGO_INSTRUMENT)
        target_compile_options(${TARGET_NAME} PRIVATE /GL)
        target_link_options(${TARGET_NAME}   PRIVATE /LTCG:PGINSTRUMENT)
        message(STATUS "PGO INSTRUMENT enabled for ${TARGET_NAME}. Run the workload, then merge with pgomgr.")
    elseif(HUBRIS_PGO_OPTIMIZE)
        target_compile_options(${TARGET_NAME} PRIVATE /GL)
        target_link_options(${TARGET_NAME}   PRIVATE /LTCG:PGOPTIMIZE)
        message(STATUS "PGO OPTIMIZE enabled for ${TARGET_NAME} (using collected profile).")
    endif()
endfunction()

# Convenience: apply both LTCG and (optionally) PGO to a target.
function(hubris_apply_optimization TARGET_NAME)
    hubris_apply_lto(${TARGET_NAME})
    hubris_apply_pgo(${TARGET_NAME})
endfunction()

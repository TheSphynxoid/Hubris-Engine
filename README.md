# Hubris Engine

_A needle under the steps of giants._

Hubris Engine is a custom real-time engine project built in modern C++ with a strong focus on architecture, maintainability, and long-term scalability.

This repository represents a full rewrite of an earlier codebase (formerly Sphynx Engine), with the goal of applying cleaner boundaries, better modularity, and a more professional engineering workflow.

## Why this project exists

I built Hubris Engine to deepen practical skills in systems programming and engine architecture, not just to render a triangle. The project is designed to demonstrate:

- Thoughtful separation of core engine systems
- Strong compile-time and dependency hygiene
- Iterative design with room for future extension
- Real-world tooling around CMake, vcpkg, and Vulkan

## What this demonstrates

From an engineering perspective, this project is intended to show:

- Ability to evolve a codebase through a full rewrite with clearer boundaries
- Comfort working close to the metal in C++ and graphics-adjacent systems
- Practical build and dependency management in a multi-target CMake project
- Consistent iteration habits visible through branch and commit history

## Current technical direction

- Language standard: C++20 (with a planned move to C++26 once toolchain support is stable)
- Rendering focus: Vulkan-first
- Build system: CMake
- Dependency management: vcpkg
- Repository structure organized around engine core, IO, and sandbox/testing targets

## Roadmap

Planned capabilities include:

- Public scripting API
- C# scripting integration
- Multithreaded rendering workflows
- Plugin architecture
- OpenUSD pipeline support
- Additional graphics backends (Direct3D 12)

## Build requirements

- C++20-compatible compiler
- CMake 3.12+
- Vulkan SDK
- vcpkg

## Project status

Hubris Engine is currently in active development and should be treated as an evolving engineering portfolio project rather than a production-ready runtime.

I’m happy to discuss trade-offs, architecture decisions, and upcoming milestones.
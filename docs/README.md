# tr69hostif Documentation

This directory contains implementation-oriented documentation for the tr69hostif daemon. The goal is to keep architecture, API, build, and test information close to the source tree and grounded in the current codebase.

## Documentation Index

### Architecture

- [System Overview](architecture/overview.md) describes the daemon's major components, startup sequence, and runtime boundaries.
- [JSON Usage](architecture/json-usage.md) maps the module's JSON request, config, notification, and JSON-RPC paths and records the current robustness gaps.
- [Threading Model](architecture/threading-model.md) documents worker threads, synchronization primitives, and shutdown behavior.
- [Data Flow](architecture/data-flow.md) traces request routing, event propagation, and RFC/bootstrap precedence.

### API

- [Public API](api/public-api.md) documents the shared request envelope, dispatcher entry points, IARM-facing APIs, and event contracts.

### Integration

- [Build Setup](integration/build-setup.md) covers the autotools workflow, feature flags, runtime prerequisites, and deployment notes.
- [Testing](integration/testing.md) covers the repo's unit-test and L2 test flows, including coverage generation.

### Troubleshooting

- [Common Errors](troubleshooting/common-errors.md) summarizes the most common startup, routing, and Parodus integration failures.

## Scope

The pages in this directory are intentionally implementation-specific. They reference the current source layout under `src/hostif/`, the shipped config files under `conf/`, and the repo-maintained validation scripts such as `run_ut.sh` and `run_l2.sh`.

## Maintenance Rules

- Update the relevant page when thread ownership, feature flags, or request routing changes.
- Keep Mermaid diagrams synchronized with the current code paths.
- Prefer linking to source files and config files already present in the repository instead of copying large code blocks into docs.
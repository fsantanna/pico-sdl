# Plan: Multi-Version Doxygen Docs

Build isolated docs for each git tag + main, deploy all to
GitHub Pages as `site/<version>/`.

## Context

- Strategy A: build all versions in single CI run
- No version selector, no root index, completely isolated
- Current tags (`v0.2`, `v0.1-win`) have no `doc/` directory
    - only future tags (v0.3+) will produce docs
    - workflow skips tags that lack `doc/`
- Trigger: push to `main` or push of `v*` tag
- PRs: no build, no deploy
- Dir naming: `v0.2/`, `main/`

## Done

- [x] 1. `doc/Doxyfile`: `PROJECT_NUMBER=$(PICO_DOC_VERSION)`,
         `OUTPUT_DIRECTORY=$(PICO_DOC_OUTPUT)`
- [x] 2. `doc/build.sh`: forward env vars, default output `.`
- [x] 3. `.github/workflows/docs.yml`: multi-version build
    - fetch full history (`fetch-depth: 0`)
    - trigger on push to `main` AND on `v*` tags
    - loop over `git tag -l 'v*'`, skip without `doc/`
    - build main as `site/main/`
    - upload + deploy `site/`
    - no PR trigger

## Decisions

- No root `index.html`
- Tags without `doc/` silently skipped
- `main` always built from HEAD
- Previous CI/DI plan (single-version) superseded

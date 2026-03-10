# Plan: Doxygen CI/CD & Cleanup ✓

Deploy API docs to GitHub Pages via GitHub Actions, remove
build artifacts from git.

## Done

- [x] 1. Enable GitHub Pages on `fsantanna/pico-sdl` with
         "GitHub Actions" as source
- [x] 2. Create `.github/workflows/docs.yml`
    - triggers on push (build+deploy) and PRs (build-only)
    - paths: `doc/**`, `src/*.h`
    - installs doxygen, runs `doc/build.sh`
    - deploys via `actions/deploy-pages@v4`
- [x] 3. Add `doc/html/` to `.gitignore`
- [x] 4. Remove tracked `doc/html/` from git
    - `git rm -r --cached doc/html/`
- [x] 5. Remove dead `footer.html` + `layout-1.13.xml`
    - deleted `footer.html` (was entirely commented out)
    - deleted `layout-1.13.xml` (standardized on 1.9)
    - cleared `HTML_FOOTER` in Doxyfile
    - updated `build.sh` to use `layout-1.9.xml`
- [x] 6. Update README.md docs link
    - `fsantanna.github.io/pico-sdl/`
- [x] 7. Commit and push to main
- [x] 8. Verify workflow + docs live

## Decisions

- Standardized on `layout-1.9.xml` (works with any doxygen
  >= 1.9, ubuntu-latest ships 1.9.8)
- PRs build docs but don't deploy (catches breakage early)
- `EXTRACT_ALL = YES` kept as-is (out of scope)

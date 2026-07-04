# third_party/apps/ — local app source parking (untracked)

This directory is a **local parking spot for app source trees**. Clone any
standalone app repo here and build it; then deploy (install/copy) the built
apps to the **runtime apps directory** (`<active_root>/apps/`, e.g.
`~/desktop/apps/`) where `AppDiscoverer` scans at startup.

**Not a submodule, not tracked** (see `.gitignore`) — every developer clones
their own. The main repo stays free of app sources.

## Workflow

```bash
# 1. clone an app repo here (source parking)
git clone https://github.com/Awesome-Embedded-Learning-Studio/CFDeskit.git \
  third_party/apps/CFDeskit

# 2. build it
cmake -S third_party/apps/CFDeskit -B third_party/apps/CFDeskit/build
cmake --build third_party/apps/CFDeskit/build -j

# 3. deploy to the runtime apps dir (<active_root>/apps/)
cmake --install third_party/apps/CFDeskit/build --prefix "$HOME/desktop"
# or, for a quick copy:
cp -r third_party/apps/CFDeskit/build/apps/. "$HOME/desktop/apps/"
```

The desktop discovers apps under `<active_root>/apps/<id>/app.json` — that's
where built executables + their manifests must land. This `third_party/apps/`
directory is **only source parking**; the desktop does NOT scan here.

## One rule, all apps

Same flow for any app repo: CFDeskit today; `pdfReader` / `MediaPlayer` /
embedded tools in the future. Clone here, build, deploy to `~/desktop/apps/`.
No per-app wiring in the main repo.

## If `~/desktop/apps/` is empty

The desktop logs `INFO: No apps discovered under '.../apps'` and continues —
**no error, no crash**. Deploy an app whenever you need it.

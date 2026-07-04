# apps/ — deployment target (no source)

This directory is the **runtime deployment target** for standalone desktop apps.
App sources no longer live in this repo — they were extracted to the separate
[`CFDeskit`](https://github.com/Awesome-Embedded-Learning-Studio/CFDeskit)
repository (calculator, noter, alarm_clock, calendar, system_state).

## Deployment contract

`AppDiscoverer` (in `desktop/ui/components/launcher/`) scans
`<bin>/../apps/<id>/app.json` at startup. Each app ships as a self-contained
package:

```
<active_root>/apps/
  libquarkwidgets.so            # shared by all apps (rpath $ORIGIN/..)
  <id>/
    <id>                        # executable
    app.json                    # manifest (app_id, display_name, exec, launch_kind)
```

## How to deploy

Build CFDeskit and install (or copy) its `apps/` tree here:

```bash
# clean install:
( cd ~/CFDeskit && cmake --install build --prefix <CFDesktop deploy root> )
# quick local test against an existing CFDesktop build tree:
cp -r ~/CFDeskit/build/apps/. <CFDesktop build>/out/build_develop/apps/
```

Apps resolve `libquarkwidgets.so` via rpath `$ORIGIN/..` and runtime-verify the
ABI version (`abi_check.hpp`), so the apps' QuarkWidgets version is fully
decoupled from the desktop's `bin/libquarkwidgets.so`.

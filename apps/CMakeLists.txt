# Standalone desktop apps.
# Each entry is an independent executable that CFDesktop launches via
# AppLaunchService (QProcess) — they are not linked into the desktop shell,
# so a crash or hang in an app cannot take down the desktop.
add_subdirectory(calculator)

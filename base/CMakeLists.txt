# ============================================================
# base/ — Foundation layer
# Merges cfbase (hardware probes + HWTier + console, SHARED) with the desktop
# infrastructure (logger/config/path/filesystem/fundamental/ascii_art, STATIC)
# into a single base layer. Downstream (main/CFDesktop_shared) consumes them
# via the existing target aliases (CFDesktop::base, cfconfig, cfpath, ...).
# ============================================================
add_subdirectory(cfbase)
add_subdirectory(fundamental)
add_subdirectory(path)
add_subdirectory(logger)
add_subdirectory(file_operations)
add_subdirectory(config_manager)
add_subdirectory(ascii_art)
add_subdirectory(ipc)
add_subdirectory(crash_handler)

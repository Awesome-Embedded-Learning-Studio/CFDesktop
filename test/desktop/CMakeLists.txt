# Desktop module unit tests
log_info("test_desktop" "Configuring desktop module tests:")

# Add init tests subdirectory
add_subdirectory(init)

# Add frosted backdrop tests subdirectory
add_subdirectory(frosted_backdrop)

# Add window placement tests subdirectory
add_subdirectory(window_placement)

# Add wallpaper animation tests subdirectory
add_subdirectory(wallpaper_animation)

# Add launcher (app discovery) tests subdirectory
add_subdirectory(launcher)

# Add builtin apps registry tests subdirectory
add_subdirectory(builtin_apps)

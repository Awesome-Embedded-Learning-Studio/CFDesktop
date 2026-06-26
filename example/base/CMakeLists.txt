# NOTE: the policy_chain common example now lives in the aex submodule.

log_info("Example Base" "Start From System Configurations")
add_subdirectory(system)

log_info("Example Base" "Start From Device Configurations")
add_subdirectory(device)

log_info("Example Base" "Done!")
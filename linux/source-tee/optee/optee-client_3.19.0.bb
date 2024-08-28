require optee-client.inc

SRCREV = "140bf463046071d3ca5ebbde3fb21ee0854e1951"

EXTRA_OEMAKE_append += "-DCFG_TEE_CLIENT_LOG_LEVEL=3"

"""
File containing all paths relative from the root of the project

This is necessary to also reach folders outside our module.
Additionally we don't have to re-define the variables in each file.
"""
from pathlib import Path


class PathResolver:
    # Directories
    BASE_DIR = Path(__file__, "../../..").resolve()
    LOG_DIR = Path(BASE_DIR, "logs").resolve()
    CONFIG_DIR = Path(BASE_DIR, "python/configuration").resolve()

    # Files
    CONFIG_FILE = Path(CONFIG_DIR, "config.yaml").resolve()
    LOGGING_CONFIG = Path(CONFIG_DIR, "logger_config.yaml").resolve()

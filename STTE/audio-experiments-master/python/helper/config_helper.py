"""
File handling the reading of the config file
"""
import yaml
from helper.path_helper import PathResolver


class ConfigHelper(object):
    __instance = None

    def __new__(cls):
        if cls.__instance is None:
            cls.__instance = super(ConfigHelper, cls).__new__(cls)

            cls.__instance.__config = yaml.safe_load(open(PathResolver.CONFIG_FILE))

        return cls.__instance

    def get_config(self):
        return self.__config

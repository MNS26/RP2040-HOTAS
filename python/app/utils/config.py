''' app/utils/config.py '''

import os
import configparser
class AppConfig:
  """
  Configuration File
  """
  APP_NAME: str = "PyQt template"
  # Default is users home directory
  game_log_dir: str = str(os.path.expanduser('~'))

  # Im sure there is a better way but this works
  path: str = os.path.dirname(__file__)                               # get full path to this file
  path = path+"/../.."                                                # go up by 2... since we are at config.py instead of main.py
  path_config: str = os.path.abspath(os.path.join(path,"config.cfg")) # convert to absolute path 
  path = os.path.abspath(os.path.dirname(path_config))                # get path to main.py

  configfile = None

  @classmethod
  def initialize(cls) -> None:
    """
    Perform any necessary initializations here, e.g.:
    - Loading settings from a file
    """
    config = configparser.ConfigParser()
    print(os.path.dirname(AppConfig.path_config))

    # since we are doing file access
    try:
      # check if the file is there or not
      if os.path.exists(AppConfig.path_config):
        config.read(AppConfig.path_config)
        AppConfig.configfile = config
      else:
        with open(AppConfig.path_config, "w") as configfile:
          config.add_section('Journal')
          config.set('Journal', 'log_dir', '')
          config.write(configfile)
    except:
    # we got a error! we wil use hard coded values
      pass

  def get_var(self) -> None:
    """
    Get Var.
    """
    config = configparser.ConfigParser()

  def set_var(self) -> None:
    """
    Set Var
    """
    config = configparser.ConfigParser()

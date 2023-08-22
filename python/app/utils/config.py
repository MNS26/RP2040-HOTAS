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
  # Getting current path > go up by 2 > store config path > set path to absolute
  path: str = os.path.dirname(__file__)
  path = path+"/../.."
  path_config: str = os.path.abspath(os.path.join(path,"config.cfg"))
  path = os.path.abspath(os.path.dirname(path_config))

  configfile = None

  @classmethod
  def initialize(cls) -> None:
    """
    Perform any necessary initializations here, e.g.:
    - Loading settings from a file
    """
    config = configparser.ConfigParser()
    print(os.path.dirname(AppConfig.path_config))

    try:
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
    parser = configparser.ConfigParser()

  def set_var(self) -> None:
    """
    Set Var
    """
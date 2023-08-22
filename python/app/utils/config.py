''' app/utils/config.py '''

import os
import configparser
class AppConfig:
  """
  Configuration File
  """
  APP_NAME: str = "PyQt template"

  # Default is users home directory
  game_log_dir: str = os.path.expanduser( '~' )

  # Im sure there is a better way but this works
  # Getting current path > go up by 2 > store config path > set path to absolute
  app_path: str = os.path.dirname( __file__ )
  app_path = app_path+"/../.."
  path_config: str = os.path.abspath( os.path.join( app_path, "config.cfg" ) )
  app_path = os.path.abspath( os.path.dirname( path_config ) )

  configfile = None

  @classmethod
  def initialize(cls) -> None:
    """
    Perform any necessary initializations here, e.g.:
    - Loading settings from a file
    """

    config = configparser.ConfigParser()
    try:
      config.read( AppConfig.path_config )
      AppConfig.configfile = config
      
    except:
      with open( AppConfig.path_config, "w" ) as configfile:
        config.write( configfile )

  def get_var(self) -> None:
    """
    Get Var.
    """

  def set_var(self) -> None:
    """
    Set Var
    """
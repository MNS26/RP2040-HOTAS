''' app/utils/monitor/shipyard.py '''
from ..config import AppConfig
import os
import re
from threading import Thread
import time
import json


class Shipyard(Thread):
  def __init__(self):
    self.AppConfig = AppConfig()
    self.directory = self.AppConfig.game_log_dir
    self.monitoring_thread = Thread(target=self.monitor_file_changes,name="Shipyard.json Monitor")
    self.monitoring_thread.daemon = True
    self.is_running = False

  def start(self, initial_directory):
    self.directory = initial_directory
    self.is_running = True
    self.monitoring_thread.start()

  def stop(self):
    self.is_running = False

  def monitor_file_changes(self):
    while self.is_running:

      # Check for changes in config file
      if self.directory != self.AppConfig.read_config('Logs','directory'):
        self.directory = self.AppConfig.read_config('Logs','directory')
        #print(f"Switched to new directory: {self.directory}")

      if not os.path.exists(str(self.directory)):
        #print(f"Directory '{self.directory}' does not exist.")
        time.sleep(0.1)
        continue

      file = os.path.join(str(self.directory),'Shipyard.json') 

      if file:
        print(f"Accessing latest Shipyard.json")
      else:
        print(f"No Shipyard found in: {self.directory}")  
        pass

      time.sleep(1)


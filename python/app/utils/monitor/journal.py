''' app/utils/monitor/journal.py '''
from ..config import AppConfig
import os
import re
from threading import Thread
import time
import json


class Journal(Thread):
  def __init__(self):
    self.AppConfig = AppConfig()
    self.directory = self.AppConfig.game_log_dir
    self.latest_file = None
    self.monitoring_thread = Thread(target=self.monitor_file_changes,name="Journal Monitor")
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

      file = [_file for _file in os.listdir(self.directory) if re.match(r'Journal\.\d{4}-\d{2}-\d{2}T\d{6}\.\d{2}\.log', _file)]

      if file:
        self.latest_file = max(file)
        print(f"Accessing latest Journal: {self.latest_file}")
      else:
        print(f"No Journal found in: {self.directory}")  
        pass

      time.sleep(1)

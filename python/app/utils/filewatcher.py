''' app/utils/filewatcher.py '''

import os
import re
import json
from .config import AppConfig 
from PyQt6.QtCore import Qt, QFileSystemWatcher, pyqtSlot


class FileWatcher():
  
  def __init__(self):
    
    self.AppConfig = AppConfig()
    self.Status = None
    self.Shipyard = None
    self.directory:str  = ""
    self.latest_journal: str = ""
    self.files: list = []

  def initialize(self,directory: str = "") -> None:
    self.directory = directory
    
    journal_files = [_file for _file in os.listdir(self.directory) if re.match(r'Journal\.\d{4}-\d{2}-\d{2}T\d{6}\.\d{2}\.log', _file)]
    
    if journal_files:
      self.latest_journal: str = max(journal_files)

    self.files = [
      directory,
      os.path.join(directory, self.latest_journal),
      os.path.join(directory, "Status.json"),
      os.path.join(directory, "Shipyard.json"),
      os.path.join(directory, "Outfitting.json"),
      os.path.join(directory, "NavRoute.json"),
      os.path.join(directory, "ModulesInfo.json"),
      os.path.join(directory, "ShipLocker.json"),
      os.path.join(directory, "Market.json"),
      os.path.join(directory, "Cargo.json"),
      os.path.join(directory, "Backpack.json"),
    ]

  def start(self) -> None:
    self.fS_watcher = QFileSystemWatcher(self.files)
    self.fS_watcher.fileChanged.connect(lambda x: self.file_event_change(x))
    self.fS_watcher.directoryChanged.connect(lambda x: self.file_event_change(x))

  def stop(self) -> None:
    del self.fS_watcher


  def update_directory(self,directory) -> None:

    self.fS_watcher.removePaths(self.files)
    self.files = [
      directory,
      os.path.join(directory, self.latest_journal),
      os.path.join(directory, "Status.json"),
      os.path.join(directory, "Shipyard.json"),
      os.path.join(directory, "Outfitting.json"),
      os.path.join(directory, "NavRoute.json"),
      os.path.join(directory, "ModulesInfo.json"),
      os.path.join(directory, "ShipLocker.json"),
      os.path.join(directory, "Market.json"),
      os.path.join(directory, "Cargo.json"),
      os.path.join(directory, "Backpack.json"),
    ]
    self.fS_watcher.addPaths(self.files)     
    pass


  @pyqtSlot(str)
  def file_event_change(self, path):
    match os.path.basename(path):
      case self.latest_journal:
        print("journal to be done")

      case "Status.json"|"Shipyard.json"|"Outfitting.json"|"NavRoute.json"|"ModulesInfo.json"|"ShipLocker.json"|"Market.json"|"Cargo.json"|"Backpack.json":
      # if any of the jsons, just open them
        with open(path, 'r') as file:
          try:
            JsonObject = json.load(file)
            print(f'{os.path.basename(path)} changed')

          except:
            pass

      case _:
        #means none of the files change so something happened to the directory
        print("no file has changed")
        journal_files = [_file for _file in os.listdir(self.directory) if re.match(r'Journal\.\d{4}-\d{2}-\d{2}T\d{6}\.\d{2}\.log', _file)]
        if journal_files:
          if max(journal_files) != self.latest_journal:
            self.fS_watcher.removePaths(self.files)
            self.latest_journal: str = max(journal_files)
            self.files[1] = os.path.join(self.directory,self.latest_journal)
            self.fS_watcher.addPaths(self.files)   
      
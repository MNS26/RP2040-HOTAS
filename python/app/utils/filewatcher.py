''' app/utils/filewatcher.py '''

import os
import re
import ast
import json
import typing
from datetime import datetime
from .config import AppConfig 
from PyQt6.QtCore import Qt, QFileSystemWatcher, pyqtSignal, pyqtSlot


class FileWatcher(QFileSystemWatcher):
  Journal_event = pyqtSignal(dict)
  Status_event = pyqtSignal(dict)
  Shipyard_event = pyqtSignal(dict)
  Outfitting_event = pyqtSignal(dict)
  NavRoute_event = pyqtSignal(dict)
  NavRouteClear_event = pyqtSignal(dict)
  ModulesInfo_event = pyqtSignal(dict)
  ShipLocker_event = pyqtSignal(dict)
  Market_event = pyqtSignal(dict)
  Cargo_event = pyqtSignal(dict)
  Backpack_event = pyqtSignal(dict)
  def __init__(self):
    
    self.AppConfig = AppConfig()
    self.Status = None
    self.Shipyard = None
    self.directory: str = AppConfig.game_log_dir
    self.latest_journal: str = ""
    self.latest_journal_timestamp = datetime(1,1,1,0,0,0).strftime("%Y-%m-%dT%H:%M:%SZ")
    self.latest_journal_line = {}

    self.files: list = []


  def start(self) -> None:
    self.fS_watcher = QFileSystemWatcher(self.files)
    self.fS_watcher.fileChanged.connect(lambda x: self.file_event(x))
    self.fS_watcher.directoryChanged.connect(lambda x: self.file_event(x))

  def stop(self) -> None:
    del self.fS_watcher


  def initialize(self) -> None:
    if self.directory == '':
      self.directory:str  = str(os.path.expanduser('~')) 
    
    self.latest_journal = self.get_latest_Journal()
    self.files = [
      self.directory,
      os.path.join(self.directory, self.latest_journal),
      os.path.join(self.directory, "Status.json"),
      os.path.join(self.directory, "Shipyard.json"),
      os.path.join(self.directory, "Outfitting.json"),
      os.path.join(self.directory, "NavRoute.json"),
      os.path.join(self.directory, "ModulesInfo.json"),
      os.path.join(self.directory, "ShipLocker.json"),
      os.path.join(self.directory, "Market.json"),
      os.path.join(self.directory, "Cargo.json"),
      os.path.join(self.directory, "Backpack.json"),
    ]

  def get_latest_Journal(self) -> str:
    journal_files = [_file for _file in os.listdir(self.directory) if re.match(r'Journal\.\d{4}-\d{2}-\d{2}T\d{6}\.\d{2}\.log', _file)]
    if journal_files:
      return str(max(journal_files))
    return str(None)  
    

  def update_directory(self,directory) -> None:
    self.directory = directory
     
    self.latest_journal = self.get_latest_Journal()

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
    print(self.files[0])


  @pyqtSlot(str)
  def file_event(self, path):
    match os.path.basename(path):
      case "Status.json"|"Shipyard.json"|"Outfitting.json"|"NavRoute.json"|"ModulesInfo.json"|"ShipLocker.json"|"Market.json"|"Cargo.json"|"Backpack.json":
      # if any of the jsons, just open them
        with open(path, 'r') as file:
          try:
            JsonObject = json.load(file)
            print(JsonObject)
            match JsonObject['event']:
              case 'Status':
                self.Status_event.emit(JsonObject)
              case 'Shipyard':
                self.Shipyard_event.emit(JsonObject)
              case 'Outfitting':
                self.Outfitting_event.emit(JsonObject)
              case 'NavRoute':
                self.NavRoute_event.emit(JsonObject)
              # Thanks devs for making it a spererate event -_-
              case 'NavRouteClear':
                self.NavRouteClear_event.emit(JsonObject)
              case 'ModuleInfo':
                self.ModulesInfo_event.emit(JsonObject)
              case 'ShipLocker':
                self.ShipLocker_event.emit(JsonObject)
              case 'Market':
                self.Market_event.emit(JsonObject)
              case 'Cargo':
                self.Cargo_event.emit(JsonObject)
              case 'Backpack':
                self.Backpack_event.emit(JsonObject)
          except:
            pass
      case self.latest_journal:
        try:
          # Journal is made of single line objects but this can go into the THOUSANDS of lines
          with open(path, "r") as journal_file:
            lines = journal_file.readlines()
            for line in lines:
              try:
                JsonObject = json.loads(line)
                current_timestamp = JsonObject["timestamp"]
                if current_timestamp >= self.latest_journal_timestamp:
                  # Process the JSON data
                  self.latest_journal_timestamp = JsonObject["timestamp"]
                  self.latest_journal_line = JsonObject

                  print(JsonObject)
                  self.Journal_event.emit(JsonObject)
                else:
                  # We only care about if when its new
                  pass
              except:
                # It shouldnt fail... but just in case
                print("failed to read Json Object")
                pass
        except:
          self.get_latest_Journal()
      case _:
        #means directory change triggered (doenst always mean someting)
        if  self.get_latest_Journal() != self.latest_journal:
          print("found different Journal file")
          self.fS_watcher.removePaths(self.files)
          self.latest_journal = self.get_latest_Journal()
          self.files[1] = os.path.join(self.directory,self.latest_journal)
          self.fS_watcher.addPaths(self.files)

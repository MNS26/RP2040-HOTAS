''' app/utils/logging.py '''
from threading import Thread
import json
import sys

class JournalLogs(Thread):
  def __init__(self,dir) -> None:
    super().__init__()
    self.dir = dir
    self.running = True

  def run(self):
    while self.running: 
      pass

class StatusLog(Thread):
  def __init__(self,dir) -> None:
    super().__init__()
    self.dir = dir
    self.running = True

  def run(self):
    while self.running: 
      pass

class ShipyardLog(Thread):
  def __init__(self,dir) -> None:
    super().__init__()
    self.dir = dir
    self.running = True

  def run(self):
    while self.running: 
      pass
      
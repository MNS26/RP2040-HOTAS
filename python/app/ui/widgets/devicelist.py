''' app/ui/widgets/devicelist.py '''

import hid
from PyQt6.QtWidgets import QComboBox,QWidget


class DeviceList(QComboBox):
  def __init__(self, parent: QWidget | None = ...,  device: str = ...) -> None:
    self.device = device
    super().__init__(parent)
    if self.device != None:
      try:
        self.addItem(self.device)
      except:
        pass
    #self.activated.connect(self.refreshItems)
    self.addItem(self.refreshItems())
  
  def refreshItems(self):
    unique_devices = self.listDevices()
    self.clear()
    #self.addItem("------")
    for device_info in unique_devices:
      device_name = f"{device_info['manufacturer_string']} - {device_info['product_string']}"
      self.addItem(device_name, userData=device_info)

  def showPopup(self) -> None:
    self.refreshItems()
    return super().showPopup()

  
  
  def listDevices(self) -> list:
    devices = hid.enumerate()
    unique_devices = DeviceList.filter_unique_devices(devices)
    return unique_devices
  
  @staticmethod
  def filter_unique_devices(devices) -> list:
    unique_devices = {}

    for device_info in devices:
      key = (device_info['vendor_id'], device_info['product_id'], device_info['serial_number'])
      if key not in unique_devices:
        unique_devices[key] = device_info

    return list(unique_devices.values())
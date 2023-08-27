''' app/ui/widgets/devicelist.py '''

import hid
class DeviceList:
  @staticmethod
  def filter_unique_devices(devices) -> list:
    unique_devices = {}

    for device_info in devices:
      key = (device_info['vendor_id'], device_info['product_id'], device_info['serial_number'])
      if key not in unique_devices:
        unique_devices[key] = device_info

    return list(unique_devices.values())

  @staticmethod
  def populate_device_combo(combo_box) -> None:
    devices = hid.enumerate()

    unique_devices = DeviceList.filter_unique_devices(devices)

    for device_info in unique_devices:
      device_name = f"{device_info['manufacturer_string']} - {device_info['product_string']}"
      combo_box.addItem(device_name, userData=device_info)
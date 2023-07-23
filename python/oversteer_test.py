from api.device_manager import DeviceManager
from api.test import Test
from api.events import Events

manager = DeviceManager()
manager.start()

# devices = manager.get_devices()
# print("Devices found:")
# for device in devices:
#     print("  {}: {}".format(device.dev_name, device.name))

device = manager.first_device()
print(device)

events = Events(device)
events.start()

def callback(name='end'):
    print('Test callback: ', name)

print('==== Starting tests ====')
test = Test(device, callback)
test.test2()

manager.stop()
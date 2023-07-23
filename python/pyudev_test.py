import os
import pyudev 
import api.wheel_ids as wid

supported_wheels = {
    wid.LG_G29: 900,
    wid.LG_G920: 900,
    wid.LG_G923X: 900,
    wid.LG_G923P: 900,
    wid.LG_DF: 270,
    wid.LG_MOMO: 270,
    wid.LG_DFP: 900,
    wid.LG_G25: 900,
    wid.LG_DFGT: 900,
    wid.LG_G27: 900,
    wid.LG_SFW: 270,
    wid.LG_MOMO2: 270,
    wid.LG_WFG: 180,
    wid.LG_WFFG: 180,
    wid.TM_FFRW: 180,
    wid.TM_T80: 240,
    wid.TM_T150: 1080,
    wid.TM_T248: 900,
    wid.TM_T300RS: 1080,
    wid.TM_T300RS_FF1: 1080,
    wid.TM_T500RS: 1080,
    wid.TM_TX: 900,
    wid.FT_CSL_ELITE: 1080,
    wid.FT_CSL_ELITE_PS4: 1080,
    wid.FT_CSV2: 900,
    wid.FT_CSV25: 900,
    wid.FT_PDD1: 1080,
    wid.FT_PDD2: 1080,
    wid.FT_CSL_DD: 1080,
    wid.XX_FFBOARD: 1080,
}

device = {}

def update_list(udevice):
    seat_id = udevice.get('ID_FOR_SEAT')
    device['seat_id'] = seat_id
    if seat_id is None:
        return
    
    if 'DEVNAME' in udevice:
        if 'event' in udevice.get('DEVNAME'):
            usb_id = str(udevice.get('ID_VENDOR_ID')) + ':' + str(udevice.get('ID_MODEL_ID'))
            device['dev_name'] = udevice.get('DEVNAME')
            device['vendor_id'] = udevice.get('ID_VENDOR_ID')
            device['product_id'] = udevice.get('ID_MODEL_ID')
            device['usb_id'] = usb_id
            device['max_range'] = supported_wheels[usb_id]
    else:
        device['name'] = udevice.get('NAME').strip('"')
        device['dev_path'] = os.path.realpath(os.path.join(udevice.sys_path, 'device'))



context = pyudev.Context()
print(context.device_path)
print(context.run_path)
print(context.sys_path)
print(context._libudev._name)
print(context._libudev._handle)

for udevice in context.list_devices(subsystem='input', ID_INPUT_JOYSTICK=1):
    usb_id = str(udevice.get('ID_VENDOR_ID')) + ':' + str(udevice.get('ID_MODEL_ID'))
    print('Udevice:', udevice)
    update_list(udevice)

import json
print(json.dumps(device, sort_keys=True, indent=4))

dev = device['dev_name']
if os.access(dev, os.R_OK):
    print('ok')
    path = dev if not hasattr(dev, '__fspath__') else dev.__fspath__()
    print(path)
    try:
        fd = os.open(dev, os.O_RDWR | os.O_NONBLOCK)
        print(fd)
        print('try')
    except OSError:
        fd = os.open(dev, os.O_RDONLY | os.O_NONBLOCK)
        print('except')
    input_device = None


from ctypes import CDLL
from pyudev._ctypeslib.libudev import ERROR_CHECKERS
from pyudev._ctypeslib.libudev import SIGNATURES

library_name = 'libudev.so.1'
lib = CDLL(library_name, use_errno=True)
for funcname, signature in SIGNATURES.items():
    function = getattr(lib, funcname, None)
    if function:
        argtypes, restype = signature
        # print(funcname, argtypes, restype)
        function.argtypes = argtypes
        function.restype = restype
        errorchecker = ERROR_CHECKERS.get(funcname)
        # print(argtypes, restype, errorchecker)
        if errorchecker:
            function.errcheck = errorchecker



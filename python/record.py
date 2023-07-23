from api.device_manager import DeviceManager
from api.events import Events

from threading import Timer
from evdev import ecodes
import pandas as pd
import datetime
import keyboard
import os

manager = DeviceManager()
manager.start()
device = manager.first_device()
print(device)    

wheel_pos = 0
def on_events(events):
    global wheel_pos
    for ev in events:
        if ev.type == ecodes.EV_ABS and ev.code == ecodes.ABS_X:
            wheel_pos = (ev.value - 32768) / 32768 # TODO: normalize event.value

events = Events(device)
events.add_callback(on_events)
events.start()

MAX_RECORDING_LENGTH = 10
start_time = datetime.datetime.now()
rec_path = f'rec-{start_time}.csv'
ts, v = [], []

def save_recording():
    global ts, v
    print('== Saving recordings to file')
    recording = pd.DataFrame({'ts': ts, 'v': v})
    if os.path.isfile(rec_path):
        recording.to_csv(rec_path, mode='a', index=False, header=False)
    else: 
        recording.to_csv(rec_path, index=False)
    ts, v = [], []

def record_wheel():
    global ts, v, wheel_pos
    Timer(1.0, record_wheel).start()
    ts.append(datetime.datetime.now())
    v.append(wheel_pos)
    if len(ts) >= MAX_RECORDING_LENGTH:
        save_recording()
    
record_wheel()
keyboard.wait('space')
manager.stop()
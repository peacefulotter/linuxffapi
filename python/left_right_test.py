from api.device_manager import DeviceManager
from api.events import Events
from evdev import ecodes, ff
import keyboard
import time

manager = DeviceManager()
manager.start()
device = manager.first_device()
input_device = device.get_input_device()
print(device, '\n', input_device)

events = Events(device)
events.start()

def prepare_wheel(input_device):
    try:
        for effect_id in range(input_device.ff_effects_count):
            input_device.erase_effect(effect_id)
    except OSError:
        pass
    device.center_wheel()

def create_effect(input_device, level, dir):
    """dir=1 for right, -1 for left"""
    dir = 0x4000 if dir == -1 else 0xc000
    effect = ff.Effect(
        ecodes.FF_CONSTANT, -1, dir,
        ff.Trigger(0, 0),
        ff.Replay(0, 0),
        ff.EffectType(ff_constant_effect=ff.Constant(level=level))
    )
    effect.id = input_device.upload_effect(effect)
    return effect


prepare_wheel(input_device)
level = 6553
level_incr = 2000
left_effect = create_effect(input_device, level, -1)
right_effect = create_effect(input_device, level, 1)


def update_effect_level(incr):
    global left_effect, right_effect, level
    """incr = 1 for more, -1 for less"""
    level += incr * level_incr
    left_effect.u.ff_constant_effect.level = level
    right_effect.u.ff_constant_effect.level = level
    print(f'Applying effects at level {level}')

def upload_effect(effect):
    input_device.upload_effect(effect)
    input_device.write(ecodes.EV_FF, effect.id, 1)
    time.sleep(0.3)
    input_device.write(ecodes.EV_FF, effect.id, 0)

keyboard.add_hotkey('left', lambda: upload_effect(left_effect))
keyboard.add_hotkey('right', lambda: upload_effect(right_effect))
keyboard.add_hotkey('up', lambda: update_effect_level(1))
keyboard.add_hotkey('down', lambda: update_effect_level(-1))


keyboard.wait('space')
input_device.erase_effect(left_effect.id)
input_device.erase_effect(right_effect.id)
manager.stop()
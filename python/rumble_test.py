# REQUIRES: https://github.com/berarma/new-lg4ff


import evdev
from evdev import ecodes, InputDevice, ff

# Find first EV_FF capable event device (that we have permissions
# to use).
for name in evdev.list_devices():
    dev = InputDevice(name)
    if ecodes.EV_FF in dev.capabilities():
        break

print(dev)

duration_ms = 1000
# rumble = ff.Rumble(strong_magnitude=0x0000, weak_magnitude=0xffff)
# effect_type = ff.EffectType(ff_rumble_effect=rumble)
# effect = ff.Effect(
#     ecodes.FF_RUMBLE, -1, 0,
#     ff.Trigger(0, 0),
#     ff.Replay(duration_ms, 0),
#     effect_type
# )
#  print(ecodes.FF_CONSTANT)
constant = ff.Constant(level=100)
effect = ff.Effect(
    ecodes.FF_CONSTANT, -1, 1,
    ff.Trigger(0, 0),
    ff.Replay(duration_ms, 0),
    ff.EffectType(ff_constant_effect=constant)
)
print(effect)
repeat_count = 1
effect_id = dev.upload_effect(effect)
dev.write(ecodes.EV_FF, effect_id, repeat_count)

import time
time.sleep(10)
dev.erase_effect(effect_id)
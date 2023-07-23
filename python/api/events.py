from evdev import ecodes
from threading import Thread
import logging
import time

class Events:
    def __init__(self, device):
        self.device = device
        self.callbacks = []
    
    def add_callback(self, cb):
        self.callbacks.append(cb)

    def start(self):
        Thread(target=self.input_thread, daemon=True).start()

    def input_thread(self):
        while True:
            if self.device is not None and self.device.is_ready():
                try:
                    events = self.device.read_events(0.5)
                    if events is not None:
                        self.process_events(events)
                except OSError as e:
                    logging.debug(e)
                    time.sleep(1)
            else:
                time.sleep(1)

    def process_events(self, events):
        for cb in self.callbacks:
            cb(events)
            
            # print(event.type, event.code)

            # if event.type == ecodes.EV_ABS:
            #     if event.code == ecodes.ABS_X:
            #         self.last_wheel_axis_value = event.value
            #         if self.test and self.test.is_collecting_data():
            #             self.test.append_data(event.timestamp(), event.value)
            #         else:
            #             self.ui.safe_call(self.ui.set_steering_input, event.value)
            #     elif event.code == ecodes.ABS_Z:
            #         self.ui.safe_call(self.ui.set_accelerator_input, event.value)
            #     elif event.code == ecodes.ABS_RZ:
            #         self.ui.safe_call(self.ui.set_brakes_input, event.value)
            #     elif event.code == ecodes.ABS_Y:
            #         self.ui.safe_call(self.ui.set_clutch_input, event.value)
            #     elif event.code == ecodes.ABS_HAT0X:
            #         self.ui.safe_call(self.ui.set_hatx_input, event.value)
            #         if event.value == -1:
            #             self.on_button_press(100, 1)
            #         elif event.value == 1:
            #             self.on_button_press(101, 1)
            #     elif event.code == ecodes.ABS_HAT0Y:
            #         self.ui.safe_call(self.ui.set_haty_input, event.value)
            #         if event.value == -1:
            #             self.on_button_press(102, 1)
            #         elif event.value == 1:
            #             self.on_button_press(103, 1)
            # if event.type == ecodes.EV_KEY:
            #     button = None
            #     if event.value:
            #         delay = 0
            #         if self.test and self.test.is_awaiting_action():
            #             self.test.trigger_action()
            #     else:
            #         delay = 100

            #     button = None

            #     if event.code >= 288 and event.code <= 303:
            #         button = event.code - 288
            #     if event.code >= 704 and event.code <= 712:
            #         button = event.code - 688

            #     if button is not None:
            #         self.ui.safe_call(self.ui.set_btn_input, button, event.value, delay)
            #         self.on_button_press(button, event.value)
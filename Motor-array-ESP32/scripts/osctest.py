import argparse
import time
from pythonosc import udp_client
from nicegui import ui
rows = 5
cols = 5
mot_array = [0] * (rows*cols)
light = [0, 0]

def new_OSC_client(ip, port):
    parser = argparse.ArgumentParser()
    parser.add_argument("--ip", default=ip, help="The ip of the OSC server")
    parser.add_argument(
        "--port", type=int, default=port, help="The port the OSC server is listening on"
    )
    args = parser.parse_args()
    client = udp_client.SimpleUDPClient(args.ip, args.port)
    return client

def motorValue(val,id):
    mot_array[id] = val.value
def send_grid():
    client_mots.send_message("/mot/setarr", mot_array)
    time.sleep(1)
def send_all(speed):
    client_mots.send_message("/mot/setall", speed.value)
    time.sleep(1)
def set_all(val):
    for mot in mot_array:
        mot_array[mot] = val.value
def set_light_freq(val):
    light[0] = val.value
    print(light)
    client_LEDs.send_message("/LED", light)
def set_light_duty(val):
    light[1] = val.value
    print(light)
    client_LEDs.send_message("/LED", light)
def send_big(speed):
    client_bigmot.send_message("/bigmot", speed.value)
    time.sleep(1)
def send_pattern(mode):
    print(mode)
    client_mots.send_message("/mot/mode", mode)
    time.sleep(1)
ui.label('Set motors')
for i in range(rows):
    with ui.row():
        for j in range(cols):
            motid = (j*rows)+i
            print(motid)
            ui.label(f'Motor {motid}')
            toggle1 = ui.toggle([1, 2, 3, 4, 5], value=1, on_change=lambda e, id=motid : motorValue(e, id))
ui.button('Send Full Grid Pattern', on_click=send_grid)
with ui.row().classes('w-[800px] h-[20%] justify-between border p-0 m-0'):
    with ui.row().classes('w-[48%] h-[20%] border p-0 m-0'):
        ui.label('Set all at once')
        setall = ui.slider(min=0, max=100, value=0, on_change=send_all)
        ui.label().bind_text_from(setall, 'value')
    with ui.row().classes('w-[48%] h-[20%] border p-0 m-0'):
        ui.button('Take Snapshot', on_click=send_grid)
        ui.label().bind_text_from(globals(), 'mot_array')
with ui.row().classes('w-[800px] h-[20%] justify-between border p-0 m-0'):
    with ui.row().classes('w-[48%] h-[20%] border p-0 m-0'):
        ui.label('Set Patterns')
        ui.button('wave', on_click=lambda pattern ="wave": send_pattern(pattern),color="#EE4" )
        ui.button('stripes', on_click=lambda pattern ="stripes": send_pattern(pattern), color="#EE4")
        ui.button('random', on_click=lambda pattern ="random": send_pattern(pattern), color="#EE4")
    with ui.row().classes('w-[48%] h-[20%] border p-0 m-0'):
        ui.label('Big motor')
        bigmot_slider = ui.slider(min=0, max=100, value=0, on_change=send_big)
        ui.label().bind_text_from(bigmot_slider, 'value')
with ui.row().classes('w-[800px] h-[500px] justify-between border p-0 m-0'):
    with ui.row().classes('w-[48%] h-[20%] border p-0 m-0'):
        ui.label('Light Control - Frequency')
        freq_slider = ui.slider(min=0, max=100, value=0, on_change=set_light_freq)
        ui.label().bind_text_from(freq_slider, 'value')
    with ui.row().classes('w-[48%] h-[20%] border p-0 m-0'):
        ui.label('Light Control - Duty Cycle')
        duty_slider = ui.slider(min=0, max=100, value=0, on_change=set_light_duty)
        ui.label().bind_text_from(duty_slider, 'value')

ui.run()
client_mots = new_OSC_client("10.0.0.67", 8888) #10.0.0.67 small motor
client_bigmot = new_OSC_client("10.0.0.67", 8888)
client_LEDs = new_OSC_client("10.0.0.67", 8888)
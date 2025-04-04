import argparse
import time
from pythonosc import udp_client
from nicegui import ui
rows = 4
cols = 6
mot_array = [0] * (rows*cols) 
light = [0, 0] # freq, duty

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
    snapshot.text=mot_array
    snapshot.update()
def send_grid():
    client_mots.send_message("/mot/setarr", mot_array)
    time.sleep(1)
def send_all(speed):
    print("sending all")
    client_mots.send_message("/mot/setall", speed.value)
    time.sleep(1)
def set_all(val):
    for mot in mot_array:
        mot_array[mot] = val.value
    snapshot.text=mot_array
    snapshot.update()

def set_light_freq(val):
    light[0] = val.value
    client_LEDs.send_message("/LED", light)
    time.sleep(1)

def set_light_duty(val):
    light[1] = val.value
    client_LEDs.send_message("/LED", light)
    time.sleep(1)

def send_big(speed):
    client_bigmot.send_message("/bigmot/set", speed.value)
    time.sleep(1)

def send_pattern(mode):
    client_mots.send_message("/mot/mode", mode)
    time.sleep(1)
def compute():
    snapshot.text=mot_array
    snapshot.update()

ui.label('Set motors')
for i in range(rows):
    with ui.row():
        for j in range(cols):
            motid = (j*rows)+i
            print(motid)
            ui.label(f'Motor {motid}')
            toggle1 = ui.toggle([0, 30, 50, 70, 100], value=0, on_change=lambda e, id=motid : motorValue(e, id))
ui.button('Send Full Grid Pattern', on_click=send_grid)
with ui.row().classes('w-[800px] h-[20%] justify-between border p-0 m-0'):
    with ui.row().classes('w-[48%] h-[20%] border p-0 m-0'):
        ui.label('Set all at once')
        setall = ui.toggle([0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 ], value=0, on_change=send_all)
        #setall = ui.slider(min=0, max=100, value=0, on_change=send_all)
        ui.label().bind_text_from(setall, 'value')
with ui.row().classes('w-[48%] h-[20%] border p-0 m-0'):
    ui.button('Take Snapshot', on_click=compute)
    snapshot = ui.label()
with ui.row().classes('w-[48%] h-[20%] border p-0 m-0'):
    ui.label('Set Patterns')
    ui.button('wave', on_click=lambda pattern ="wave": send_pattern(pattern),color="#EE4" )
    ui.button('stripes', on_click=lambda pattern ="stripes": send_pattern(pattern), color="#EE4")
    ui.button('random', on_click=lambda pattern ="random": send_pattern(pattern), color="#EE4")
with ui.row().classes('w-[48%] h-[20%] border p-0 m-0'):
    ui.label('Big motor')
    bigmot_slider = ui.toggle([0, 5, 7, 10, 13, 15, 17, 20, 23, 25, 30, 40, 50, 60, 70, 80, 90, 100], value=0, on_change=send_big)
        #bigmot_slider = ui.slider(min=0, max=100, value=0, on_change=send_big)
    ui.label().bind_text_from(bigmot_slider, 'value')
# with ui.row().classes('w-[800px] h-[500px] justify-between border p-0 m-0'):
with ui.row().classes('w-[48%] h-[20%] border p-0 m-0'):
    ui.label('Light Control - Frequency in Hz')
    freq_slider = ui.toggle([20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70,75,80,85,90,95,100], value=20, on_change=set_light_freq)
    # freq_slider = ui.slider(min=0, max=100, value=0, on_change=set_light_freq)
    ui.label().bind_text_from(freq_slider, 'value')
with ui.row().classes('w-[48%] h-[20%] border p-0 m-0'):
    ui.label('Light Control - Duty Cycle in %')
    duty_slider = ui.toggle([0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70,75,80,85,90,95,100], value=0, on_change=set_light_duty)

        # duty_slider = ui.slider(min=0, max=100, value=0, on_change=set_light_duty)
    ui.label().bind_text_from(duty_slider, 'value')

ui.run()
client_mots = new_OSC_client("10.0.0.66", 8888) #10.0.0.67 small motor
client_bigmot = new_OSC_client("10.0.0.194", 8888)
client_LEDs = new_OSC_client("10.0.0.156", 8888)
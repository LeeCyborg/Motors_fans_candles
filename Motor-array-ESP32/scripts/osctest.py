import argparse
import time
from pythonosc import udp_client
from nicegui import ui
rows = 5
cols = 5
mot_array = [0] * (rows*cols)
parser = argparse.ArgumentParser()
parser.add_argument("--ip", default="10.0.0.67", help="The ip of the OSC server")
parser.add_argument(
    "--port", type=int, default=8888, help="The port the OSC server is listening on"
)
args = parser.parse_args()
client_mots = udp_client.SimpleUDPClient(args.ip, args.port)

parser = argparse.ArgumentParser()
parser.add_argument("--ip", default="10.0.0.67", help="The ip of the OSC server")
parser.add_argument(
    "--port", type=int, default=8888, help="The port the OSC server is listening on"
)
args = parser.parse_args()
client_bigmot = udp_client.SimpleUDPClient(args.ip, args.port)

def motorValue(val,id):
    mot_array[id] = val.value
def send_grid(motors):
    client_mots.send_message("/mot/setarr", mot_array)
    time.sleep(1)
def send_all(speed):
    client_mots.send_message("/mot/setall", speed.value)
    time.sleep(1)
def set_all(val):
    for mot in mot_array:
        mot_array[mot] = val.value
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
            toggle1 = ui.toggle([1, 2, 3,4,5], value=1, on_change=lambda e, id=motid : motorValue(e, id))
ui.button('Send Full Grid Pattern', on_click=send_grid)
ui.label('Set all')
setall = ui.toggle([1, 2, 3,4,5], value=1, on_change=send_all)
ui.button('Take Snapshot', on_click=send_grid)
ui.label('Set Patterns')
with ui.row():
    ui.button('wave', on_click=lambda pattern ="wave": send_pattern(pattern),color="#EE4" )
    ui.button('stripes', on_click=lambda pattern ="stripes": send_pattern(pattern), color="#EE4")
    ui.button('random', on_click=lambda pattern ="random": send_pattern(pattern), color="#EE4")

with ui.row():
    ui.label('Big motor')
    slider = ui.slider(min=0, max=100, value=50)
    ui.label().bind_text_from(slider, 'value')
    bigmotor = ui.toggle([1, 2, 3, 4, 5], value=1, on_change=send_big)
ui.run()
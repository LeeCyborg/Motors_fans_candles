import sys
import argparse
import time
from pythonosc import udp_client
from nicegui import ui, html
rows = 5
cols = 4
mot_array = [0] * (rows*cols)
ui.add_css('''
    .red {
        background: red;
    }
''')
def setupOSC(): 
    #motor esp
    parser = argparse.ArgumentParser()
    parser.add_argument("--ip", default="10.0.0.125", help="The ip of the OSC server")
    parser.add_argument(
        "--port", type=int, default=8888, help="The port the OSC server is listening on"
    )
    args = parser.parse_args()
    client_mots = udp_client.SimpleUDPClient(args.ip, args.port)
    # light esp
    parser = argparse.ArgumentParser()
    parser.add_argument("--ip", default="10.0.0.125", help="The ip of the OSC server")
    parser.add_argument(
        "--port", type=int, default=8888, help="The port the OSC server is listening on"
    )
    args = parser.parse_args()
    client_LEDs = udp_client.SimpleUDPClient(args.ip, args.port)

def motorValue(val,id):
    mot_array[id] = val.value

def send_grid(motors):
    client_mots.send_message("/mot/enable", motors)
    time.sleep(1)

def send_pattern(mode):
    print(mode)
    client_mots.send_message("/mot/mode", mode)
    time.sleep(1)
def send_lights(mode):
    print(mode)
    client_LEDs.send_message("/LED", mode)
    time.sleep(1)
ui.label('Set motors')
for i in range(rows):
    with ui.row():
        for j in range(cols):
            motid = (j*rows)+i
            print(motid)
            toggle1 = ui.toggle([1, 2, 3], value=1, on_change=lambda e, id=motid : motorValue(e, id))

ui.button('Send Full Grid Pattern', on_click=send_grid)
ui.label('Set Patterns')
with ui.row():
    ui.button('wave', on_click=lambda pattern ="wave": send_pattern(pattern),color="#EE4" )
    ui.button('stripes', on_click=lambda pattern ="stripes": send_pattern(pattern), color="#EE4")
    ui.button('random', on_click=lambda pattern ="random": send_pattern(pattern), color="#EE4")
ui.label('Set Lights')
with ui.row():
    ui.button('slow', on_click=lambda pattern ="slow": send_lights(pattern),color="#00FF00" )
    ui.button('fast', on_click=lambda pattern ="fast": send_lights(pattern), color="#00FF00")
    ui.button('balance', on_click=lambda pattern ="balance": send_lights(pattern), color="#00FF00")
ui.run()
setupOSC()




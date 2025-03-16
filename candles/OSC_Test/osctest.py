import argparse
import time
from pythonosc import udp_client

if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("--ip", default="192.168.4.2",
      help="The ip of the OSC server")
  parser.add_argument("--port", type=int, default=8888,
      help="The port the OSC server is listening on")
  args = parser.parse_args()
  client = udp_client.SimpleUDPClient(args.ip, args.port)
  mylist = [2,4,5,6,1]
  for x in range(10):
    client.send_message("/led", mylist)
    time.sleep(1)

"""Small example OSC client

This program sends 10 random values between 0.0 and 1.0 to the /filter address,
waiting for 1 seconds between each value.
"""
import argparse
import random
import time
from pythonosc import udp_client

if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("--ip", default="10.0.0.125",
      help="The ip of the OSC server")
  parser.add_argument("--port", type=int, default=8888,
      help="The port the OSC server is listening on")
  args = parser.parse_args()

  client = udp_client.SimpleUDPClient(args.ip, args.port)

  mylist = [2,4,5,6,1]
  for x in range(10):

    # client.send_message("/led", "string")
    # time.sleep(1)
    client.send_message("/led", mylist)
    time.sleep(1)

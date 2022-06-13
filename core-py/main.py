
import subprocess
import sys
from typing import Tuple
sys.path.insert(0, "/home/ben/projects/core/daemon")

from threading import Event, Thread
import traceback
import signal
import time
from core.emulator.coreemu import CoreEmu
from core.emulator.data import IpPrefixes, NodeOptions
from core.emulator.enumerations import EventTypes
from core.emulator.session import Session
from core.emulator.data import LinkOptions
from core.nodes.base import CoreNode
from core.services.coreservices import ServiceManager

import dtlsr
from generate import Configuration

def sigint_handler(signum, frame):
  # res = input("Ctrl-c was pressed. Do you really want to exit? y/n ")
  # if res == 'y':
  # log.close()
  session.shutdown()
  print("\nSession shutdown complete")
  exit(1)
 
signal.signal(signal.SIGINT, sigint_handler)

DELAY = 1000 # 1ms
# DELAY = 100_000 # 200ms

CONFIG_NAME: str = "partition_headless"
FLAP_NODES: Tuple[int, int] = (1, 2)
FLAP_NODES_OTHER: Tuple[int, int] = (2, 3)
FLAP_NODE: int = 2
UP_TIME: float = 3.0
DOWN_TIME: float = 3.0

link_up = LinkOptions(
    bandwidth=100_000_000_000,
    delay=DELAY,
    dup=0,
    loss=0.0,
    jitter=0)

link_down = LinkOptions(
    bandwidth=100_000_000_000,
    delay=DELAY,
    dup=0,
    loss=100.0,
    jitter=0)

def timer(is_up: bool):
  if is_up:
    time.sleep(UP_TIME)
  else:
    time.sleep(DOWN_TIME)

def disable_ip_forwarding(node_id: int) -> None:
  subprocess.run(["python3", "../tools/vcmd.py",
                  "n{}".format(node_id), "sysctl",
                  "-w", "net.ipv4.ip_forward=0"])

def operating_loop_timer(conf: Configuration) -> None:
  is_up = True
  timer_thread = Thread(target=timer, args=[is_up])
  timer_thread.start()
  while True:
    if not timer_thread.is_alive():
      t = time.time()
      text = None
      if is_up:
        text = "[{}] link down".format(t)
        conf.update_link(FLAP_NODES[0], FLAP_NODES[1], link_down)
      else:
        text = "[{}] link up".format(t)
        conf.update_link(FLAP_NODES[0], FLAP_NODES[1], link_up)
      is_up = not is_up
      print(text)
      timer_thread = Thread(target=timer, args=[is_up])
      timer_thread.start()

def operating_loop_timer_anticorrelated(conf: Configuration) -> None:
  is_up = True
  timer_thread = Thread(target=timer, args=[is_up])
  timer_thread.start()
  while True:
    if not timer_thread.is_alive():
      t = time.time()
      text = None
      if is_up:
        text = "[{}] switch".format(t)
        conf.update_link(FLAP_NODES[0], FLAP_NODES[1], link_down)
        conf.update_link(FLAP_NODES_OTHER[0], FLAP_NODES_OTHER[1],
                         link_up)
      else:
        text = "[{}] switch".format(t)
        conf.update_link(FLAP_NODES[0], FLAP_NODES[1], link_up)
        conf.update_link(FLAP_NODES_OTHER[0], FLAP_NODES_OTHER[1],
                         link_down)
      is_up = not is_up
      print(text)
      timer_thread = Thread(target=timer, args=[is_up])
      timer_thread.start()

def operating_loop_node_timer(conf: Configuration) -> None:
  is_up = True
  timer_thread = Thread(target=timer, args=[is_up])
  timer_thread.start()
  while True:
    if not timer_thread.is_alive():
      t = time.time()
      text = None
      if is_up:
        text = "[{}] node down".format(t)
        conf.stop_services_on_node(FLAP_NODE)
        disable_ip_forwarding(FLAP_NODE)
      else:
        text = "[{}] node up".format(t)
        conf.start_services_on_node(FLAP_NODE)
      is_up = not is_up
      print(text)
      timer_thread = Thread(target=timer, args=[is_up])
      timer_thread.start()

def operating_loop_manual(conf: Configuration) -> None:
  up = True
  while True:
    input()
    if up:
      print("link down")
      conf.update_link(1, 2, link_down)
    else:
      print("link up")
      conf.update_link(1, 2, link_up)
    up = not up

def main():
  ## create emulator instance for creating sessions and utility methods
  coreemu: CoreEmu = CoreEmu()
  global session
  session = coreemu.create_session()
  global log
  # log = open("core.log", "w")
  delay_tolerant = False
  if (len(sys.argv) > 1 and
      sys.argv[1].lower() in ["delay_tolerant", "dt"]):
    print("delay-tolerant mode")
    delay_tolerant = True
  try:
    session.service_manager.add(dtlsr.DefaultRoute2)
    session.service_manager.add(dtlsr.Heartbeat)
    session.service_manager.add(dtlsr.LSR)
    session.service_manager.add(dtlsr.DTLSR)
    session.set_state(EventTypes.CONFIGURATION_STATE)
    ## Initialise
    conf = Configuration(CONFIG_NAME, link_up,
                         session, delay_tolerant)
    print("starting services")
    conf.start_services()

    session.instantiate()

    # operating_loop_manual(conf)
    # operating_loop_node_timer(conf)
    operating_loop_timer(conf)
    # operating_loop_timer_anticorrelated(conf)
  except Exception as e:
    try:
      exc_info = sys.exc_info()
    finally:
      traceback.print_exception(*exc_info)
      del exc_info
      session.shutdown()

  input("\npress enter to shutdown")
  session.shutdown()

if __name__ == "__main__":
  main()

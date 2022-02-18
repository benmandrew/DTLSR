
from io import TextIOWrapper
import sys
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
  log.close()
  session.shutdown()
  print("\nSession shutdown complete")
  exit(1)
 
signal.signal(signal.SIGINT, sigint_handler)

link_up = LinkOptions(
    bandwidth=100_000_000_000,
    delay=1000,
    dup=0,
    loss=0.0,
    jitter=0)

link_down = LinkOptions(
    bandwidth=100_000_000_000,
    delay=1000,
    dup=0,
    loss=100.0,
    jitter=0)

FLAP_TIME: float = 2.0

def timer():
  time.sleep(FLAP_TIME)

def operating_loop_timer(conf: Configuration, log: TextIOWrapper) -> None:
  timer_thread = Thread(target=timer)
  timer_thread.start()
  up = True
  while True:
    if not timer_thread.is_alive():
      t = time.time()
      text = None
      if up:
        text = "[{}] link down\n".format(t)
        conf.update_link(1, 4, link_down)
      else:
        text = "[{}] link up\n".format(t)
        conf.update_link(1, 4, link_up)
      up = not up
      print(text, end="")
      log.write(text)
      log.flush()
      timer_thread = Thread(target=timer)
      timer_thread.start()

def operating_loop_manual(conf: Configuration) -> None:
  up = True
  while True:
    input()
    if up:
      print("link down")
      conf.update_link(1, 4, link_down)
    else:
      print("link up")
      conf.update_link(1, 4, link_up)
    up = not up

def main():
  ## create emulator instance for creating sessions and utility methods
  coreemu: CoreEmu = CoreEmu()
  global session
  session = coreemu.create_session()
  global log
  log = open("core_log", "w")
  try:
    session.service_manager.add(dtlsr.Heartbeat)
    session.service_manager.add(dtlsr.DTLSR)
    session.set_state(EventTypes.CONFIGURATION_STATE)
    ## Initialise
    conf = Configuration("box_headless", link_up, session)
    conf.start_services()

    session.instantiate()

    # operating_loop_manual(conf)
    operating_loop_timer(conf, log)
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

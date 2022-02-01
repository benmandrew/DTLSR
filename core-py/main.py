
import sys
sys.path.insert(0, "/home/ben/projects/core/daemon")

from threading import Event
from core.emulator.coreemu import CoreEmu
from core.emulator.data import IpPrefixes, NodeOptions
from core.emulator.enumerations import EventTypes
from core.emulator.session import Session
from core.emulator.data import LinkOptions
from core.nodes.base import CoreNode
from core.services.coreservices import ServiceManager

import dtlsr
from generate import Configuration

def main():
  ## create emulator instance for creating sessions and utility methods
  coreemu: CoreEmu = CoreEmu()
  session: Session = coreemu.create_session()
  session.service_manager.add(dtlsr.Heartbeat)
  session.service_manager.add(dtlsr.DTLSR)
  session.set_state(EventTypes.CONFIGURATION_STATE)
  ## Link configuration
  link_options = LinkOptions(
    bandwidth=54_000_000,
    delay=1000,
    dup=0,
    loss=0.0,
    jitter=0,
  )
  ## Initialise
  conf = Configuration("loop_headless", link_options, session)
  # conf = Configuration("loop", link_options, session)
  conf.start_services()

  session.instantiate()

  input("\npress enter to shutdown")
  session.shutdown()

if __name__ == "__main__":
  main()

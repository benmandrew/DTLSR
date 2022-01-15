
from threading import Event
from core.emulator.coreemu import CoreEmu
from core.emulator.data import IpPrefixes, NodeOptions
from core.emulator.enumerations import EventTypes
from core.emulator.session import Session
from core.emulator.data import LinkOptions
from core.nodes.base import CoreNode
from core.services.coreservices import ServiceManager

from generate import Configuration

import dtlsr

# create emulator instance for creating sessions and utility methods
coreemu: CoreEmu = CoreEmu()
session: Session = coreemu.create_session()
session.service_manager.add(dtlsr.Heartbeat)
session.service_manager.add(dtlsr.DTLSR)
session.set_state(EventTypes.CONFIGURATION_STATE)

# configuring when creating a link
options = LinkOptions(
  bandwidth=54_000_000,
  delay=1000,
  dup=5,
  loss=65.0,
  jitter=0,
)


conf = Configuration("headless", options, session)

conf.start_services()

# print("Heartbeat valid:", session.services.validate_service(n1, dtlsr.Heartbeat))
# print("DTLSR valid:", session.services.validate_service(n2, dtlsr.DTLSR))

input("press enter to shutdown")

session.shutdown()

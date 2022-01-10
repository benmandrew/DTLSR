
from threading import Event
from core.emulator.coreemu import CoreEmu
from core.emulator.data import IpPrefixes, NodeOptions
from core.emulator.enumerations import EventTypes
from core.emulator.session import Session
from core.emulator.data import LinkOptions
from core.nodes.base import CoreNode
from core.services.coreservices import ServiceManager

import dtlsr

# ip nerator for example
ip_prefixes = IpPrefixes(ip4_prefix="10.0.0.0/24")

# create emulator instance for creating sessions and utility methods
coreemu: CoreEmu = CoreEmu()
session: Session = coreemu.create_session()

session.service_manager.add(dtlsr.Heartbeat)
session.service_manager.add(dtlsr.DTLSR)

session.set_state(EventTypes.CONFIGURATION_STATE)

# create nodes
options = NodeOptions(name="n1", x=100, y=100)
n1: CoreNode = session.add_node(CoreNode, options=options)
session.services.set_service(n1.id, "DTLSR")
session.services.set_service(n1.id, "Heartbeat")
options = NodeOptions(name="n2", x=300, y=100)
n2: CoreNode = session.add_node(CoreNode, options=options)
session.services.set_service(n2.id, "DTLSR")
session.services.set_service(n2.id, "Heartbeat")

# configuring when creating a link
options = LinkOptions(
  bandwidth=54_000_000,
  delay=5000,
  dup=5,
  loss=5.5,
  jitter=0,
)

# link nodes together
iface1 = ip_prefixes.create_iface(n1)
iface2 = ip_prefixes.create_iface(n2)
session.add_link(n1.id, n2.id, iface1, iface2, options)



session.instantiate()

session.services.create_service_files(n1, dtlsr.Heartbeat)
session.services.create_service_files(n2, dtlsr.DTLSR)
session.services.startup_service(n1, dtlsr.Heartbeat)
session.services.startup_service(n2, dtlsr.DTLSR)

print("Heartbeat valid:", session.services.validate_service(n1, dtlsr.Heartbeat))
print("Heartbeat valid:", session.services.validate_service(n2, dtlsr.DTLSR))

input("press enter to shutdown")

session.shutdown()

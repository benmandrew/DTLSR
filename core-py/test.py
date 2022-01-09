from core.emulator.coreemu import CoreEmu
from core.emulator.data import IpPrefixes, NodeOptions
from core.emulator.enumerations import EventTypes
from core.emulator.session import Session
from core.emulator.data import LinkOptions
from core.nodes.base import CoreNode

import dtlsr

# ip nerator for example
ip_prefixes = IpPrefixes(ip4_prefix="10.0.0.0/24")

# create emulator instance for creating sessions and utility methods
coreemu: CoreEmu = CoreEmu()
session: Session = coreemu.create_session()

session.service_manager.add(dtlsr.Heartbeat)
session.service_manager.add(dtlsr.DTLSR)

# must be in configuration state for nodes to start, when using "node_add" below
session.set_state(EventTypes.CONFIGURATION_STATE)

# create nodes
options = NodeOptions(x=100, y=100)
n1: CoreNode = session.add_node(CoreNode, options=options)
session.services.add_services(node=n1, node_type=n1.type, services=["Heartbeat"])
# options = NodeOptions(x=300, y=100, services=["Heartbeat"])
# n2: CoreNode = session.add_node(CoreNode, options=options)



for k, v in session.service_manager.services.items():
	print(k, v)

print(session.services.get_service(n1.id, "FRRBGP"))

# configuring when creating a link
options = LinkOptions(
    bandwidth=54_000_000,
    delay=5000,
    dup=5,
    loss=5.5,
    jitter=0,
)

# link nodes together
# iface1 = ip_prefixes.create_iface(n1)
# iface2 = ip_prefixes.create_iface(n2)
# session.add_link(n1.id, n2.id, iface1, iface2, options)

# start session
session.instantiate()

# do whatever you like here
input("press enter to shutdown")

# stop session
session.shutdown()

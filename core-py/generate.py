
from core.emulator.coreemu import CoreEmu
from core.emulator.data import IpPrefixes, NodeOptions
from core.emulator.enumerations import EventTypes
from core.emulator.session import Session
from core.emulator.data import LinkOptions
from core.nodes.base import CoreNode

import dtlsr

import sys, os
from typing import List, Set, Tuple

CONFIG_BASE = "/home/ben/projects/routing/configs"

ip_prefixes: List[IpPrefixes] = []

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

class Node:
  def __init__(self, id: str) -> None:
    self.id: int = int(id)
    self.source_ips: List[str] = []
    self.neighbour_ips: List[str] = []
    self.neighbour_ids: List[int] = []
    options = NodeOptions(name="n{}".format(self.id))
    self.core_node: CoreNode = session.add_node(CoreNode, options=options)
    session.services.set_service(self.core_node.id, "DTLSR")
    session.services.set_service(self.core_node.id, "Heartbeat")

  def add_neighbour(self, source_ip: str, neighbour_ip: str, neighbour_id: int):
    self.source_ips.append(source_ip)
    self.neighbour_ips.append(neighbour_ip)
    self.neighbour_ids.append(neighbour_id)


def get_node_with_id(ns: List[Node], id: int):
  for n in ns:
    if n.core_node.id == id:
      return n.core_node

def create_links(ns: List[Node]):
  seen_pairs: Set[Tuple[int, int]] = set()
  for n in ns:
    for neighbour_id in n.neighbour_ids:
      if ((n.id, neighbour_id) not in seen_pairs
          and (neighbour_id, n.id) not in seen_pairs):
        seen_pairs.add((n.id, neighbour_id))
  for i, (a, b) in enumerate(seen_pairs):
    pref = IpPrefixes(
      ip4_prefix="10.0.{}.0/24".format(get_node_with_id(ns, i)))
    iface1 = pref.create_iface()

def decode(filename: str, rows: List[str]):
  n = Node(filename[1:])
  for row in rows:
    source_ip, neighbour_ip, neighbour_id = decode_row(row)
    n.add_neighbour(source_ip, neighbour_ip, neighbour_id)
  return n

def decode_row(row: str) -> Tuple[str, str, int]:
  l = row.split(" - ")
  return l[1], l[2], int(l[3])

def main(config):
  ns: List[Node] = []
  config_path = "{}/{}".format(CONFIG_BASE, config)
  for file in os.listdir(config_path):
    filename = os.fsdecode(file)
    if filename[0] == "n" and not filename.endswith(".imn"):
      full_path = "{}/{}".format(config_path, filename)
      with open(full_path) as f:
        rows = f.readlines()
      rows = [row.rstrip() for row in rows]
      ns.append(decode(filename, rows))
  create_links(ns)
        








if __name__ == "__main__":
  main(sys.argv[1])

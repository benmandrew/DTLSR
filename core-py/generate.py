
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

class Configuration:
  def __init__(self, config: str, link_options: LinkOptions, session: Session) -> None:
    self.session = session
    self.ns: List[Node] = []
    self.ip_prefixes: List[IpPrefixes] = []

    self._read_config(config)
    self._create_links(link_options)

  def _read_config(self, config: str):
    config_path = "{}/{}".format(CONFIG_BASE, config)
    for file in os.listdir(config_path):
      filename = os.fsdecode(file)
      if filename[0] == "n" and not filename.endswith(".imn"):
        full_path = "{}/{}".format(config_path, filename)
        with open(full_path) as f:
          rows = f.readlines()
        rows = [row.rstrip() for row in rows]
        self.ns.append(self._decode(filename, rows))

  def _create_links(self, options: LinkOptions):
    seen_pairs: Set[Tuple[int, int]] = set()
    for n in self.ns:
      for neighbour_id in n.neighbour_ids:
        if ((n.id, neighbour_id) not in seen_pairs
            and (neighbour_id, n.id) not in seen_pairs):
          seen_pairs.add((n.id, neighbour_id))
    for i, (a, b) in enumerate(seen_pairs):
      cn_a = get_node_with_id(self.ns, a)
      cn_b = get_node_with_id(self.ns, b)
      pref = IpPrefixes(
        ip4_prefix="10.0.{}.0/24".format(i))
      if_a = pref.create_iface(cn_a)
      if_b = pref.create_iface(cn_b)
      self.session.add_link(cn_a.id, cn_b.id, if_a, if_b, options)
      self.ip_prefixes.append(pref)

  def start_services(self):
    for n in self.ns:
      n.start_services(self.session)


  def _decode(self, filename: str, rows: List[str]):
    n = Node(filename[1:], self.session)
    for row in rows:
      source_ip, neighbour_ip, neighbour_id = self._decode_row(row)
      n.add_neighbour(source_ip, neighbour_ip, neighbour_id)
    return n

  def _decode_row(self, row: str) -> Tuple[str, str, int]:
    l = row.split(" - ")
    return l[1], l[2], int(l[3])

class Node:
  def __init__(self, id: str, session: Session) -> None:
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
  
  def start_services(self, session: Session):
    session.services.create_service_files(self.core_node, dtlsr.Heartbeat)
    session.services.create_service_files(self.core_node, dtlsr.DTLSR)
    session.services.startup_service(self.core_node, dtlsr.Heartbeat)
    session.services.startup_service(self.core_node, dtlsr.DTLSR)

def get_node_with_id(ns: List[Node], id: int) -> CoreNode:
  for n in ns:
    if n.core_node.id == id:
      return n.core_node


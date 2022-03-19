
from core.emulator.coreemu import CoreEmu
from core.emulator.data import IpPrefixes, NodeOptions
from core.emulator.enumerations import EventTypes
from core.emulator.session import Session
from core.emulator.data import LinkOptions
from core.nodes.base import CoreNode, CoreInterface
from core.services.utility import DefaultRouteService, IPForwardService

import dtlsr

import sys, os
from typing import Dict, List, Set, Tuple

CONFIG_BASE = "/home/ben/projects/dtlsr/configs"

class Configuration:
  def __init__(self, config: str, link_options: LinkOptions, session: Session) -> None:
    self.session = session
    self.ns: List[Node] = []
    self.ip_prefixes: List[IpPrefixes] = []
    self.iface_map: Dict[Tuple[int, int], Tuple[CoreInterface, CoreInterface]] = {}

    print("initing nodes")
    self._read_config(config)
    print("creating links")
    self._create_links(link_options)

  def _read_config(self, config: str):
    config_path = "{}/{}".format(CONFIG_BASE, config)
    ts: Dict[str, str] = {}
    for file in os.listdir(config_path):
      filename = os.fsdecode(file)
      if filename[0] == "n" and not filename.endswith(".imn"):
        full_path = "{}/{}".format(config_path, filename)
        with open(full_path) as f:
          rows = f.readlines()
        rows = [row.rstrip() for row in rows]
        self.ns.append(self._decode_node_config(filename, rows))
      elif filename == "types":
        full_path = "{}/{}".format(config_path, filename)
        with open(full_path) as f:
          rows = f.readlines()
        self._decode_types(rows, ts)
    self.init_nodes(ts)

  def _decode_node_config(self, filename: str, rows: List[str]):
    n = Node(filename[1:], self.session)
    for row in rows:
      source_ip, neighbour_ip, neighbour_id = self._decode_row(row)
      n.add_neighbour(source_ip, neighbour_ip, neighbour_id)
    return n

  def _decode_row(self, row: str) -> Tuple[str, str, int]:
    l = row.split(" - ")
    return l[1], l[2], int(l[3])
  
  def _decode_types(self, rows: List[str], ts: Dict[str, str]):
    for row in rows:
      l = row.strip().split(" - ")
      if len(l) != 2:
        print("Type decoding error")
        continue
      ts[l[0]] = l[1]
  
  def _get_node_with_id(self, id: int) -> CoreNode:
    for n in self.ns:
      if n.core_node.id == id:
        return n.core_node

  def _create_links(self, options: LinkOptions):
    link_high_delay = LinkOptions(
      bandwidth=100_000_000_000,
      delay=100000,
      dup=0,
      loss=0.0,
      jitter=0)
    seen_pairs: Set[Tuple[int, int]] = set()
    for n in self.ns:
      for neighbour_id in n.neighbour_ids:
        if ((n.id, neighbour_id) not in seen_pairs
            and (neighbour_id, n.id) not in seen_pairs):
          seen_pairs.add((n.id, neighbour_id))
    for i, (a, b) in enumerate(seen_pairs):
      cn_a = self._get_node_with_id(a)
      cn_b = self._get_node_with_id(b)
      pref = IpPrefixes(
        ip4_prefix="10.0.{}.0/24".format(i))
      if_a = pref.create_iface(cn_a)
      if_b = pref.create_iface(cn_b)
      
      # if (cn_a.id == 2 and cn_b.id == 3) or (cn_a.id == 3 and cn_b.id == 2):
      #   print("n{} -- n{} : {} eth{} -- {} eth{} : HIGH DELAY".format(
      #     a, b, if_a.ip4, if_a.id, if_b.ip4, if_b.id))
      #   self.session.add_link(cn_a.id, cn_b.id, if_a, if_b, link_high_delay)
      # else:
      print("n{} -- n{} : {} eth{} -- {} eth{}".format(
        a, b, if_a.ip4, if_a.id, if_b.ip4, if_b.id))
      self.session.add_link(cn_a.id, cn_b.id, if_a, if_b, options)
      self.iface_map[(cn_a.id, cn_b.id)] = (if_a, if_b)
      self.ip_prefixes.append(pref)

  def update_link(self, id_a: int, id_b: int, options: LinkOptions):
    if ((id_a, id_b) in self.iface_map.keys()):
      if_a, if_b = self.iface_map[(id_a, id_b)]
    else:
      if_b, if_a = self.iface_map[(id_b, id_a)]
    self.session.update_link(id_a, id_b, if_a.id, if_b.id, options)

  def init_nodes(self, ts: Dict[str, str]):
    for n in self.ns:
      t = ts["n{}".format(n.id)]
      if t == "host":
        n.init_host(self.session)
      elif t == "router":
        n.init_router(self.session)
      else:
        print("Unknown type: {}".format(t))

  def start_services(self):
    print("starting services")
    for n in self.ns:
      n.start_services(self.session)
    
  def validate_services(self):
    for n in self.ns:
      n.validate_services(self.session)

class Node:
  service_map = {
    "DTLSR" : dtlsr.DTLSR,
    "Heartbeat" : dtlsr.Heartbeat,
    "DefaultRoute2" : dtlsr.DefaultRoute2,
    "IPForward" : IPForwardService
  }

  def __init__(self, id: str, session: Session) -> None:
    self.id: int = int(id)
    self.source_ips: List[str] = []
    self.neighbour_ips: List[str] = []
    self.neighbour_ids: List[int] = []
    options = NodeOptions(name="n{}".format(self.id))
    self.core_node: CoreNode = session.add_node(CoreNode, self.id,
                                                options=options)
    self.services: List[str] = []

  def init_router(self, session: Session):
    session.services.set_service(self.core_node.id, "DTLSR")
    session.services.set_service(self.core_node.id, "Heartbeat")
    session.services.set_service(self.core_node.id, "IPForward")
    self.services.append("DTLSR")
    self.services.append("Heartbeat")
    self.services.append("IPForward")

  def init_host(self, session: Session):
    session.services.set_service(self.core_node.id, "Heartbeat")
    session.services.set_service(self.core_node.id, "DefaultRoute2")
    self.services.append("Heartbeat")
    self.services.append("DefaultRoute2")

  def add_neighbour(self, source_ip: str, neighbour_ip: str, neighbour_id: int):
    self.source_ips.append(source_ip)
    self.neighbour_ips.append(neighbour_ip)
    self.neighbour_ids.append(neighbour_id)
  
  def start_services(self, session: Session):
    for s in self.services:
      session.services.create_service_files(self.core_node, self.service_map[s])
      session.services.startup_service(self.core_node, self.service_map[s])

  def validate_services(self, session: Session):
    for s in self.services:
      session.services.validate_service(self.core_node, self.service_map[s])

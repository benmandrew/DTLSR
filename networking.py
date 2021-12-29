
"""
networking.py: defines services provided by the collection of protocols
"""
from typing import Tuple

import netaddr

from core.emane.nodes import EmaneNet
from core.emulator.enumerations import LinkTypes
from core.nodes.base import CoreNode
from core.nodes.interface import DEFAULT_MTU, CoreInterface
from core.nodes.network import PtpNet, WlanNode
from core.nodes.physical import Rj45Node
from core.services.coreservices import CoreService


def addrstr(ip: netaddr.IPNetwork) -> str:
  """
  helper for mapping IP addresses to config statements
  """
  address = str(ip.ip)
  if netaddr.valid_ipv4(address):
      return "ip address %s" % ip
  elif netaddr.valid_ipv6(address):
      return "ipv6 address %s" % ip
  else:
      raise ValueError("invalid address: %s", ip)


class Graph(CoreService):
  name: str = "Graph"
  group: str = "Networking"

  configs: Tuple[str, ...] = ("graph.id", "graphboot.sh", )

  startup: Tuple[str, ...] = ("bash graphboot.sh", )

  @classmethod
  def generate_config(cls, node: CoreNode, filename: str) -> None:
    if filename == cls.configs[0]:
      return cls.generate_graph_id(node)
    elif filename == cls.configs[1]:
      return cls.generate_graph_boot()
    else:
      raise ValueError("file name (%s) is not a known configuration: %s",
                       filename, cls.configs)

  @classmethod
  def generate_graph_id(cls, node: CoreNode) -> str:
    cfg = "{}".format(node.id)
    return cfg

  @classmethod
  def generate_graph_boot(cls) -> str:
    """
    Generate shell script used to boot the daemons
    """
    return """\
#!/bin/sh
# auto-generated by graph service (networking.py)

/home/ben/projects/routing/out/heartbeat -d

/home/ben/projects/routing/out/graph -d

"""

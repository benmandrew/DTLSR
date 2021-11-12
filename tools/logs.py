
import os, sys, re
from typing import List

class Node:
  def __init__(self, pycore_dir: str, node_dir: str) -> None:
    self.name = node_dir.split(".")[0]
    self.path = "/tmp/{}/{}".format(pycore_dir, node_dir)
    self.protocols = self._get_protocols()

  def get_log_path(self, protocol: str) -> str:
    return "{}/{}.log".format(self.path, protocol)

  def get_log(self, protocol: str) -> str:
    s = ""
    with open(self.get_log_path(protocol)) as f:
      s = f.read()
    return s
  
  def __str__(self):
    s = ["{}: ".format(self.name)]
    first = True
    for protocol in self.protocols:
      if first:
        first = False
        s.append("{}".format(protocol))
      else:
        s.append(", {}".format(protocol))
    return "".join(s)

  def _get_protocols(self) -> List[str]:
    ## Match all files/directories with '.log' suffix
    files = os.listdir(self.path)
    r = re.compile(r".*\.log")
    node_logs = list(filter(r.match, files))
    node_logs.sort()
    ## Strip '.log' file suffix
    protocols = [s[:-4] for s in node_logs]
    ## Remove Quagga 'var' that got accidentally added due
    ## to CORE virtual filesystem formatting (var/log -> var.log)
    return list(filter(lambda x: x != "var", protocols))

def get_node_dirs() -> List[Node]:
  dirs = os.listdir("/tmp")
  pycore_dir = None
  for dir in dirs:
    ## Example: pycore.32777
    if dir[:7] == "pycore.":
      pycore_dir = dir
      break
  if pycore_dir is None:
    print("pycore directory not found: Have you started running CORE?")
    quit()

  ## Filter sub-directories to get node config directories: n1.conf, n2.conf, ...
  dirs = os.listdir("/tmp/{}".format(pycore_dir))
  r = re.compile(r"n\d+\.conf")
  node_dirs = list(filter(r.match, dirs))
  node_dirs.sort()

  if len(node_dirs) == 0:
    print("No nodes found: Have you started running CORE?")
    quit()

  return [Node(pycore_dir, node_dir) for node_dir in node_dirs]

def print_available_nodes(nodes: List[Node]) -> None:
  print("Available nodes and protocols:")
  for node in nodes:
    print(node)

if __name__ == "__main__":
  nodes: List[Node] = get_node_dirs()

  if len(sys.argv) != 3:
    print_available_nodes(nodes)
    quit()

  found_node = False
  for node in nodes:
    if sys.argv[1] == node.name:
      try:
        print(node.get_log(sys.argv[2]))
        found_node = True
      except FileNotFoundError:
        print(
          "Node '{}' has no log file for '{}': is it running the correct protocol?"
          .format(node.name, sys.argv[2]))

  if not found_node:
    print()
    print_available_nodes(nodes)







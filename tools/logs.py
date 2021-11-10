
import os, sys, re
from typing import List

class Node:
  def __init__(self, pycore_dir: str, node_dir: str) -> None:
    self.name = node_dir.split(".")[0]
    self.path = "/tmp/{}/{}".format(pycore_dir, node_dir)

  def get_log_path(self) -> str:
    return "{}/log".format(self.path)

  def get_log(self) -> str:
    s = ""
    with open(self.get_log_path()) as f:
      s = f.read()
    return s

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

  dirs = os.listdir("/tmp/{}".format(pycore_dir))
  r = re.compile(r"n\d+\.conf")
  node_dirs = list(filter(r.match, dirs))
  node_dirs.sort()

  if len(node_dirs) == 0:
    print("No nodes found: Have you started running CORE?")
    quit()

  return [Node(pycore_dir, node_dir) for node_dir in node_dirs]

def print_available_nodes(nodes: List[Node]) -> None:
  print("Available nodes:")
  for node in nodes:
    print("{}, ".format(node.name), end="")
  print()

if __name__ == "__main__":
  nodes: List[Node] = get_node_dirs()

  if len(sys.argv) != 2:
    print_available_nodes(nodes)
    quit()

  found_node = False
  for node in nodes:
    if sys.argv[1] == node.name:
      try:
        print(node.get_log())
        found_node = True
      except FileNotFoundError:
        print(
          "Node '{}' has no log file: is it running the correct protocol?"
          .format(node.name))

  if not found_node:
    print("Node '{}' not found.".format(sys.argv[1]))
    print_available_nodes(nodes)







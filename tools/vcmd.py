
import os, sys, re, subprocess
from typing import List

def get_pycore_dir() -> str:
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
  return pycore_dir

def print_available_nodes(nodes: List[str]) -> None:
  print("Available nodes:", end="")
  for node in nodes:
    print(node, end=", ")
  print()

if __name__ == "__main__":
  pycore_dir: str = get_pycore_dir()
  if len(sys.argv) < 3:
    print("Wrong number of arguments")
    quit()
  try:
    node_dir = "/tmp/{}/{}".format(pycore_dir, sys.argv[1])
    cmd = ["vcmd", "-c", node_dir, "--"]
    res = subprocess.run(cmd + sys.argv[2:], capture_output=True)
    print(res.stdout.decode("utf-8"))
  except Exception as e:
    print(e)
    print(
      "Node '{}' is not able to run '{}'"
      .format(sys.argv[1], sys.argv[2]))







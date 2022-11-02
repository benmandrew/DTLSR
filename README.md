## DTLSR: Delay-Tolerant Link-State Routing

LSR and DTLSR are _routing_ protocols for running on network routers. They use a _link-state_ representation of the network topology for routing. DTLSR is LSR with modifications that allow it to perform better in unreliable, variable-delay network conditions, making it _delay-tolerant_.

This is my third-year dissertation project for the University of Cambridge Computer Science tripos. The project is to evaluate and compare the performance of LSR and DTLSR in a variety of network topologies and situations, to determine where delay-tolerant modifications improve or worsen performance. Both protocols are modelled off of standard OSPF, but implemented completely from scratch.

## CORE: Common Open Research Emulator

CORE is a tool for emulating networks on a single machine, using topologies of lightweight virtual machines running Unix. For this project I disable the included routing protocols and replace them with my implementation, which is thus fully functional on real Unix machines.

CORE also provides a Python API for scripting, which allows modifying network conditions in real time and extracting testing data. This is used extensively for evaluation.

Project Link: [nrl.navy.mil/core](https://www.nrl.navy.mil/Our-Work/Areas-of-Research/Information-Technology/NCS/CORE/)

## Quick Start

### Install

CORE install. I've tested with `7.5.2`, versions above have breaking changes.
```sh
git clone git@github.com:coreemu/core.git
cd core
git checkout release-7.5.2
./install.sh
```
DTLSR install.
```sh
git clone git@github.com:benmandrew/DTLSR.git dtlsr
sudo apt-get install check libpcap-dev tcpreplay net-tools
```
Copy `dtlsr.py` (located in `core-py/` in `dtlsr`) into `daemon/core/services/` directory in `core`

Required Python libraries:
```sh
sudo -i
pip3 install netaddr mako fabric lxml pyproj
```

### Building
```sh
cmake .
make
## Run unit tests
ctest -VV
```

### Running
Pass `dt` argument to use DTLSR instead of LSR
```sh
cd core-py
sudo python3 main.py [dt]
```

### View Node Logs
`node_id` is of the form `n1`, `n2`, etc.
```sh
cd tools
python3 logs.py [node_id] [protocol]
## example
python3 logs.py n1 dtlsr
```

### Run Command on Node
```sh
cd tools
python3 vcmd.py [node_id] [command]
## example
python3 vcmd.py n3 ping 10.0.0.6
```

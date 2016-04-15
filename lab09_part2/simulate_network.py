from mininet.net import Mininet
from mininet.cli import CLI
from mininet.node import OVSSwitch
from mininet.node import RemoteController
from mininet.link import TCLink
from mininet.topo import Topo
from leaf_spine_topo import LeafAndSpineTopo
from mininet.clean import cleanup
from argparse import ArgumentParser

# Function to parse the command line arguments
def parseOptions():

    # Create a parser object
    parser = ArgumentParser("Create a network with leaf and spine topology")

    # Add argument to the parser for nSpine, nLeaf and fanout options
    parser.add_argument('--spine', type=int, help='Number of Spine switches')
    parser.add_argument('--leaf', type=int, help='Number of Leaf switches')
    parser.add_argument('--fanout', type=int, help='Number of hosts per Lead switch')

    # Parse the command line arguments
    args = parser.parse_args()

    # Set the default values for arguments
    nSpine = 2
    nLeaf = 3
    fanout = 2

    # Change the values if passed on command line
    if args.spine:
        nSpine = args.spine

    if args.leaf:
        nLeaf = args.leaf

    if args.fanout:
        fanout = args.fanout

    # return the values
    return nSpine, nLeaf, fanout

class   SimpleTopo (Topo):

    def build(self):

        self.addSwitch('s1')
        self.addHost('h1')
        self.addHost('h2')
        self.addLink('s1', 'h1')
        self.addLink('s1', 'h2')

# Main function to create and run the network
def main():

    # Clean nodes, processes left over from previous run
    cleanup()

    # Parse the command line and get arguments (default values if not passed)
    nSpine, nLeaf, fanout = parseOptions()

    # Create a topology
    topo = LeafAndSpineTopo(nSpine, nLeaf, fanout)

    # Set the link speed to 10 Mbps
    for src, dst, k in topo.g.edges( keys=True ):

        entry = topo.g.edge[src][dst][k]
        entry['bw'] = 10

    # Set the OpenFlow protocol version 1.3
    for n in topo.g.node.keys():
        topo.g.node[n]['protocols'] = ['OpenFlow13']

    # Create the network using the leaf and spine topology
    net = Mininet ( topo=topo,
                    link=TCLink,
                    controller=RemoteController,
                    autoStaticArp=True,
                    autoSetMacs=True )

    # Set the DPIDs for all the switches
    i = 1
    for s in net.switches:
        s.dpid = hex(i)[2:]
        s.dpid = '0' * (16 - len(s.dpid)) + s.dpid
        i = i + 1

    # Looking at the topology, figure out spine, leaf switches
    spine_switches = []
    leaf_switches = []
    hostToSwitch = {}

    for h in net.hosts:
        link = h.intf().link
        if h.intf() == link.intf1:
            s = link.intf2.node
        else:
            s = link.intf1.node
        hostToSwitch[h.name] = s.name
        if s.name not in leaf_switches:
            leaf_switches.append(s.name)

    for s in net.switches:
        if s.name not in leaf_switches:
            spine_switches.append(s.name)

    # Open the topology file in write mode
    f = open(".topology.txt", "w")

    # Write the number of spine switches followed by
    # Switch name and DPID on line of its own
    f.write(str(len(spine_switches)) + '\n')
    for s in spine_switches:
        f.write(s + ' ' + net.nameToNode[s].dpid + '\n')

    # Write the number of leaf switches followed by
    # Switch name and DPID on line of its own
    f.write(str(len(leaf_switches)) + '\n')
    for s in leaf_switches:
        f.write(s + ' ' + net.nameToNode[s].dpid + '\n')

    # Write the number of hosts followed by
    # Name, Leaf Switch name, MAC, IP on line of its own
    f.write(str(len(net.hosts)) + '\n')
    for h in net.hosts:
        f.write(h.name + ' ' + hostToSwitch[h.name] + ' ' + h.MAC() + ' ' + h.IP() + '\n')

    # Write all the links in the following format
    # Name1 Port1 Name2 Port2
    for name1 in spine_switches:
        for name2 in leaf_switches:
            node1 = net.nameToNode[name1]
            node2 = net.nameToNode[name2]
            intf1, intf2 = node1.connectionsTo(node2)[0]
            f.write(name1 + ' ' + str(node1.ports[intf1]) + ' ' + name2 + ' ' + str(node2.ports[intf2]) + '\n')
    for h in net.hosts:
        sname = hostToSwitch[h.name]
        s = net.nameToNode[sname]
        intf1, intf2 = h.connectionsTo(s)[0]
        f.write(h.name + ' ' + str(h.ports[intf1]) + ' ' + sname + ' ' + str(s.ports[intf2]) + '\n')

    # Close the file
    f.close()

    # Start the network
    net.start()

    # Drop into the CLI
    CLI(net)

    # Stop the network
    net.stop()

    return

if __name__ == "__main__":
    main()

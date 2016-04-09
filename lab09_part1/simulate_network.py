from mininet.net import Mininet
from mininet.cli import CLI
from mininet.node import OVSSwitch
from leaf_spine_topo import LeafAndSpineTopo
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

# Main function to create and run the network
def main():

    # Parse the command line and get arguments (default values if not passed)
    nSpine, nLeaf, fanout = parseOptions()

    # Create the network using the leaf and spine topology
    net = Mininet ( topo=LeafAndSpineTopo(nSpine, nLeaf, fanout) )

    # Start the network
    net.start

    # Drop into the CLI
    CLI(net)

    # Stop the network
    net.stop()

    return

if __name__ == "__main__":
    main()

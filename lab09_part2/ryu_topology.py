
# Class of a general switch
class   BaseSwitch(object):

    # Initialize the object with name, datapath id and datapath object
    def __init__(self, name, dpid, *args):

        self.name = name
        self.dpid = dpid
        self.dp = None
        self.flowtable = []

    #
    # Method to add a flow in the switch
    # This method takes a dictionary of the following format
    # flow = {
    #          'eth_src' : <etherner_source_MAC>
    #          'eth_dst' : <ethernet_destination_MAC>
    #          'in_port' : <input_port_number>
    #          'output'  : <output_port_number>
    #        }
    #
    # Out of the above fields, 'output' must be present
    #
    def addFlow(self, flow):

        # Get the parser and OF protocol objects
        parser = self.dp.ofproto_parser
        ofproto = self.dp.ofproto

        print 'addFlow', ofproto
        # If the flow does not have 'output' as a key, return
        if 'output' not in flow.keys():
            return

        # Store the output port, because we are going to delete
        # this key from the flow
        output = flow['output']

        # Delete the 'output' key from the flow
        del flow['output']

        # Create a match object from the flow
        match = parser.OFPMatch(**flow)

        # Create an actions object from the output port
        actions = [parser.OFPActionOutput(port=output)]

        # Create an intructions object from the actions above
        instr = [parser.OFPInstructionActions(ofproto.OFPIT_APPLY_ACTIONS, actions)]

        # Create a FlowMod message to add the flow in the switch
        msg = parser.OFPFlowMod( datapath=self.dp,
                                 command=ofproto.OFPFC_ADD,
                                 buffer_id=ofproto.OFP_NO_BUFFER,
                                 match=match,
                                 instructions=instr )

        # Send the message to the switch to add the flow
        self.dp.send_msg(msg)

# SpineSwitch is BaseSwitch
SpineSwitch = BaseSwitch

# LeafSwitch is a subclass of BaseSwitch
# It contains a list of names of hosts connected to it
class   LeafSwitch(BaseSwitch):

    # Initialize the object using the super class's init method
    def __init__(self, name, dpid, *args):
        super(LeafSwitch, self).__init__(name, dpid, *args)

        # List of names of hosts connected to this leaf switch
        self.hosts = []

# Class for a host
class   Host(object):

    # Initializa the object with name, IP, MAC and name of
    # leaf switch to which this host is connected
    def __init__(self, name, mac, ip, switch):

        self.name = name
        self.switch = switch
        self.mac = mac
        self.ip = ip

# Class for storing the entire topology
class   Topology(object):

    def __init__(self, topofile):

        # The following dictionaries store a map from names to objects
        self.spineSwitches = {} # Name -> object
        self.leafSwitches = {} # Name -> object
        self.hosts = {} # Name -> object

        # ports is a map used to store port numbers
        self.ports = {} # (node1, node2) -> (port1, port2)

	self.numElemFile = 0
	self.numElemMininet = 0

        # Open the topology file for reading
        f = open(topofile, 'r')

        # Read the spine switches and store them in the topology
        nSpine = int(f.readline())
        for s in range(nSpine):
            name, dpid = f.readline().rstrip().split(' ')
            self.spineSwitches[name] = SpineSwitch(name, dpid)
            print self.spineSwitches[name].dpid

        # Read the lead switches and store them in the topology
        nLeaf = int(f.readline())
        for s in range(nLeaf):
            name, dpid = f.readline().rstrip().split(' ')
            self.leafSwitches[name] = LeafSwitch(name, dpid)

        # Read the hosts and store them in the topology
        nHost = int(f.readline())
        for h in range(nHost):
            name, switch, mac, ip = f.readline().rstrip().split(' ')
            self.hosts[name] = Host(name, mac, ip, switch)
            self.leafSwitches[switch].hosts.append(name)

        # Read all the links and store the port numbers associated
        # with the links in the topology
        for line in f:
            n1, p1, n2, p2 = line.rstrip().split(' ')
            self.ports[(n1, n2)] = int(p1), int(p2)
            self.ports[(n2, n1)] = int(p2), int(p1)

	self.numElemFile = nSpine + nLeaf

def main():

    topo = Topology('topology.txt')

    print topo.spineSwitches
    print topo.leafSwitches
    print topo.hosts
    print topo.ports

if __name__ == "__main__":
    main()

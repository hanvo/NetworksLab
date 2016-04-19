from mininet.topo import Topo

# Class specifying the leaf and spine topology
class   LeafAndSpineTopo (Topo):

    # This function builds a leaf and spine topology
    # Parameters passed to this function are
    #
    #    nSpine - Number of Spine switches
    #    nLeaf  - Number of Leaf switches
    #    fanout - Number of hosts per leaf switch
    #
    def build(self, nSpine, nLeaf, fanout):

        # Add spine switches, named spine1, spine2 ...
        for s in range(1, nSpine+1):
            self.addSwitch('spine%s' % s)

        # Counter for host name
        i = 1

        # Add leaf switches, named leaf1, leaf2 ...
        # Add hosts, named h1, h2 ... such that
        # h1, h2, ... hF are connected to leaf1,
        # hF+1, hF+2, ... h2F are connected to leaf2,
        # and so on ... (F = fanout)
        for s in range(1, nLeaf+1):
            # Add a leaf switch
            self.addSwitch('leaf%s' % s)

            # Connect to leaf switch to all the spine switches
            for p in range(1, nSpine+1):
                self.addLink('leaf%s' % s, 'spine%s' % p)

            # Add fanout number of hosts and connect them to the
            # leaf switch created above
            for h in range(1, fanout+1):
                self.addHost('h%s' % i)
                self.addLink('leaf%s' % s, 'h%s' % i)

                # Update the host name counter
                i = i + 1

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
        listSpine = list()
        listLeaf = list()

        for spine in range(nSpine): 
            switch = self.addSwitch('s%s' % (spine + 1))
            listSpine.append(switch)

        for leaf in range(nLeaf):
            leaf = self.addSwitch('l%s' % (leaf + 1))
            listLeaf.append(leaf)

        for spine in listSpine:
            for leaf in listLeaf:
                self.addLink(spine, leaf)

        counter = 1
        for leaf in listLeaf:
            for host in range(fanout):
                host = self.addHost('h%s' % counter)
                self.addLink(host, leaf)
                counter += 1


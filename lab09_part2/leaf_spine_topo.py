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

        pass

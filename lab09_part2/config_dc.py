from ryu.base import app_manager
from ryu.topology.switches import Switches
from ryu.topology import event
from ryu.controller.handler import set_ev_cls
from ryu_topology import Topology

class   ConfigDC(app_manager.RyuApp):

    _CONTEXTS = {
            'switches' : Switches
    }

    def __init__(self, *args, **kwargs):
        super(ConfigDC, self).__init__(*args, **kwargs)

	self.switches = kwargs['switches']
        self.topo = Topology('.topology.txt')

    # Set a handler for 'Switch Enter' event
    @set_ev_cls(event.EventSwitchEnter)
    def event_switch_enter_handler(self, ev):

        # Find this entered switch in our topology..
        for s in self.topo.spineSwitches.values() + self.topo.leafSwitches.values():
            if ev.switch.dp.id == int(s.dpid):

                # Update the datapath object for this switch
                s.dp = ev.switch.dp

                print 'Switch %s entered' % s.dpid

                # Update the number of mininet elements
                self.topo.numElemMininet = self.topo.numElemMininet + 1

                # If all the switches have entered, add flows in them..
                if self.topo.numElemMininet == self.topo.numElemFile:
                    self.add_flows_in_switches(self.topo)

    #
    # Method to add flows in the all the switches
    # This method is called by the controller, when all
    # the switches are connected to the controller..
    #
    # topo is an object of class Topology.
    # It contains all the switch and host objects and
    # information related to port numbers, etc.
    # Refer Section 3.1 in the lab handout for more details
    #
    # High - Level OverView
    # - Read the host_tenant_assignments.txt file and store the mapping of hosts to tenant IDs.
    # - For each pair of hosts,
    #     If both the hosts belong to the same tenant,
    #        Add flows in the switches according to sections 4.2 and 4.3

    def add_flows_in_switches(self, topo):
        host_tenant_dict = {}

        with open('host_tenant_assignments.txt') as ht_file:
            for line in ht_file:
                (host, tenant) = line.split()

                if tenant in host_tenant_dict:
                    host_tenant_dict[tenant].append(host)
                else:
                        templist = []
                        templist.append(host)
                        host_tenant_dict[tenant] = templist

        print host_tenant_dict

        # Split out the dictionary
        for tenant, hostList in host_tenant_dict.iteritems():
            #print tenant, hostList
            # For each dictionary key it will extract the list of Hosts
            for hostName in hostList:
               
                host = topo.hosts[hostName]
                start = hostList.index(hostName)

                for connectHost in hostList[start + 1:]:
                    print hostName + '->' + connectHost

                    hostB = topo.hosts[connectHost]
                    # Figure out if they are under the same host 
                    # else go through the stems
                    if host.switch == hostB.switch:
                        inPort, p = topo.ports[host.switch, hostName]
                        outPort, p = topo.ports[host.switch, connectHost]
                        flow = {
                            'in_port' : inPort,
                            'eth_src' : topo.hosts[hostName].mac,
                            'eth_dst' : topo.hosts[connectHost].mac,
                            'output'  : outPort
                        }
                        print flow
                        topo.leafSwitches[host.switch].addFlow(flow)
                        flow = {
                            'in_port' : outPort,
                            'eth_src' : topo.hosts[connectHost].mac,
                            'eth_dst' : topo.hosts[hostName].mac,
                            'output'  : inPort
                        }
                        topo.leafSwitches[host.switch].addFlow(flow)
                    else:
                        # Host Leaf Switch -> Tenant Spine
                        inPort, p = topo.ports[host.switch, hostName]
                        outPort, p = topo.ports[host.switch, 'spine%s' % tenant]
                        flow = {
                            'in_port' : inPort,
                            'eth_src' : topo.hosts[hostName].mac,
                            'eth_dst' : topo.hosts[connectHost].mac,
                            'output'  : outPort
                        }
                        print flow
                        topo.leafSwitches[host.switch].addFlow(flow)
                        flow = {
                            'in_port' : outPort,
                            'eth_src' : topo.hosts[connectHost].mac,
                            'eth_dst' : topo.hosts[hostName].mac,
                            'output'  : inPort
                        }
                        topo.leafSwitches[host.switch].addFlow(flow)

                        # Tenant Spine -> Destination Leaf Switch
                        inPort, p = topo.ports['spine%s' % tenant, host.switch]
                        outPort, p = topo.ports['spine%s' % tenant, hostB.switch]
                        flow = {
                            'in_port' : inPort,
                            'eth_src' : topo.hosts[hostName].mac,
                            'eth_dst' : topo.hosts[connectHost].mac,
                            'output'  : outPort
                        }
                        print flow
                        topo.spineSwitches['spine%s' % tenant].addFlow(flow)
                        flow = {
                            'in_port' : outPort,
                            'eth_src' : topo.hosts[connectHost].mac,
                            'eth_dst' : topo.hosts[hostName].mac,
                            'output'  : inPort
                        }
                        topo.spineSwitches['spine%s' % tenant].addFlow(flow)

                        # Destination Leaf Switch -> Host 
                        inPort, p = topo.ports[hostB.switch, 'spine%s' % tenant]
                        outPort, p = topo.ports[hostB.switch, connectHost]
                        flow = {
                            'in_port' : inPort,
                            'eth_src' : topo.hosts[hostName].mac,
                            'eth_dst' : topo.hosts[connectHost].mac,
                            'output'  : outPort
                        }
                        print flow
                        topo.leafSwitches[hostB.switch].addFlow(flow)
                        flow = {
                            'in_port' : outPort,
                            'eth_src' : topo.hosts[connectHost].mac,
                            'eth_dst' : topo.hosts[hostName].mac,
                            'output'  : inPort
                        }
                        topo.leafSwitches[hostB.switch].addFlow(flow)

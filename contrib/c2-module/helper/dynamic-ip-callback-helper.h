/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */


#include "ns3/net-device-container.h"
#include "ns3/ipv4-interface-address.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/node.h"
#include "ns3/ipv4-static-routing.h"
#include "ns3/c2-reaction.h"

#include <map>

namespace ns3 {

class DynamicIpCallbackHelper {
public:
	DynamicIpCallbackHelper(
        Ptr<Ipv4StaticRouting> IpRoute,
        uint32_t IpRouteIfIndex,
        NetDeviceContainer NetDeviceContainer,
        std::set<uint16_t> Port_c2,
        Ptr<DynamicIpServer> C2SererApp
        );
    void setNetDeviceContainer(NetDeviceContainer NetDeviceContainer);
    void setPort(std::set<uint16_t> Port_c2);
    void setReaction(C2Reaction);
    void SetToCallback(Ptr<Node> a_node, DynamicIpCallbackHelper &thisobject);
    void setIpv4addr_c2(Ipv4Address Ipv4addr_c2);
    void setC2ServerApp(Ptr<DynamicIpServer> C2SererApp);
private:
    void IpAddRoute(Ipv4Address Ipv4addr);
    void IpChanger(Ipv4Address DestinationIpv4Addr, uint32_t DeviceNumber);
    void IpChanger(uint32_t interface, Ipv4Address DestinationIpv4Addr, uint32_t DeviceNumber);
    void IpChanger(Ptr<Ipv4> ipv4proto, uint32_t interface, Ipv4Address DestinationIpv4Addr);
    void callback(Ptr<const Packet> p, Ptr<Ipv4> protcol, uint32_t interface);
    std::map<uint16_t, Ipv4Address> RegisteredIpv4addr;
    std::set<uint16_t> m_Port_c2;
    Ptr<Ipv4StaticRouting> m_IpRoute;
    uint32_t m_IpRouteIfIndex;
    Ipv4Address m_Ipv4addr_c2;
    C2Reaction m_Reaction_c2;
    NetDeviceContainer m_NetDeviceContainer;
    Ptr<DynamicIpServer> m_C2SererApp;
};
}
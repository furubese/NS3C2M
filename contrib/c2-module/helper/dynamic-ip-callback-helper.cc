#include "ns3/dynamic-ip-server.h"

#include "ns3/net-device-container.h"
#include "ns3/ipv4-interface-address.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/ptr.h"
#include "dynamic-ip-callback-helper.h"
#include "ns3/node.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-static-routing.h"

namespace ns3 {
    DynamicIpCallbackHelper::DynamicIpCallbackHelper(
        Ptr<Ipv4StaticRouting> IpRoute, 
        uint32_t IpRouteIfIndex,
        NetDeviceContainer NetDeviceContainer, 
        std::set<uint16_t> Port_c2,
        Ptr<DynamicIpServer> C2SererApp)
        {
            this -> setNetDeviceContainer(NetDeviceContainer);
            this -> setPort(Port_c2);
            this -> setC2ServerApp(C2SererApp);
            m_Ipv4addr_c2 = "0.0.0.0";
            m_IpRouteIfIndex = IpRouteIfIndex;
            m_IpRoute = IpRoute;
        }
    void DynamicIpCallbackHelper::setNetDeviceContainer(NetDeviceContainer NetDeviceContainer){ m_NetDeviceContainer = NetDeviceContainer; }
    void DynamicIpCallbackHelper::setPort(std::set<uint16_t> Port_c2) { m_Port_c2 = Port_c2; };
    void DynamicIpCallbackHelper::setReaction(C2Reaction Reaction_c2){m_Reaction_c2 = Reaction_c2;};
    void DynamicIpCallbackHelper::setIpv4addr_c2(Ipv4Address Ipv4addr_c2){
        m_Ipv4addr_c2 = Ipv4addr_c2;
        IpAddRoute(Ipv4addr_c2);
        IpChanger(Ipv4addr_c2, 2);
    }
    void DynamicIpCallbackHelper::setC2ServerApp(Ptr<DynamicIpServer> C2SererApp){m_C2SererApp = C2SererApp;};
    void DynamicIpCallbackHelper::SetToCallback(Ptr<Node> a_node, DynamicIpCallbackHelper &thisobject)
    {
        Ptr<Ipv4L3Protocol> ipv4Proto = a_node->GetObject<Ipv4L3Protocol> ();
        ipv4Proto-> TraceConnectWithoutContext("Rx", MakeCallback(&DynamicIpCallbackHelper:: callback, &thisobject));
    }


    void DynamicIpCallbackHelper::callback(Ptr<const Packet> p, Ptr<Ipv4> protcol, uint32_t interface)
    {
        Ipv4Header header;
        p->PeekHeader (header);
        /* UDP & TCP only */
        if(header.GetProtocol() != 0x6 and header.GetProtocol() != 0x11){
            return;
        }
        // MDNS Cut
        if(header.GetDestination () == "224.0.0.251"){
            return;
        }
        /*
        NS_LOG_UNCOND ("\033[7m===========RECV==========\033[m");
        NS_LOG_UNCOND (" || Src:" << header.GetSource() );
        NS_LOG_UNCOND (" || Dst:" << header.GetDestination ());
        NS_LOG_UNCOND (" || Protocol:" << C2Reaction::uint8_tToString(header.GetProtocol()));
        */  
        int packet_size = p->GetSize();
        uint8_t received_message[packet_size];
        memset(received_message, 0, packet_size);
        p->CopyData (received_message, packet_size);
        Ipv4Header header_test;
        Packet L4_Packet = Packet(received_message, packet_size);
        L4_Packet.RemoveHeader(header_test);
        TcpHeader Tcpheader;
        UdpHeader Udpheader;
        bool Tcp = true;
        if(header.GetProtocol() == 0x6){
            L4_Packet.RemoveHeader (Tcpheader);
        }
        if(header.GetProtocol() == 0x11){
            L4_Packet.RemoveHeader (Udpheader);
            Tcp = false;
        }
        uint16_t DestinatioPort;
        if(Tcp){
            DestinatioPort = Tcpheader.GetDestinationPort();
        }else{
            DestinatioPort = Udpheader.GetDestinationPort();
        }
        // steal //
        if(m_Port_c2.empty()){
            NS_LOG_UNCOND("portempty");
            int p_size = L4_Packet.GetSize();
            uint8_t mes[p_size];
            memset(mes, 0, p_size);
            L4_Packet.CopyData (mes, p_size);
            std::string hex;
            for(int i=0; i<p_size; i++){
                char hexstr[4];
                char str[4];
                sprintf(hexstr, "%02x", mes[i]);
                hex += hexstr;
                //hex += " ";
                sprintf(str, "%c", mes[i]);
            }
            if(!m_Reaction_c2.getReaction(hex).empty()){
                m_Ipv4addr_c2 =  header.GetDestination ();
                IpAddRoute(m_Ipv4addr_c2);
            }
        }
        // m_C2SererApp

        if(m_Ipv4addr_c2 == header.GetDestination ()){
            //C2Server L3
            NS_LOG_UNCOND (" || \033[31m || \033[mC2Server:" << DestinatioPort);
            if(m_C2SererApp->getPort() != DestinatioPort){
                NS_LOG_UNCOND (" || \033[31m || \033[mChangePort:" << m_C2SererApp->getPort() << "->" << DestinatioPort);
                m_C2SererApp->ChangePort(DestinatioPort);
            }
            //NS_LOG_UNCOND (" || \033[31m ||\t\033[mTCP - (" << TcpHeader::FlagsToString(Tcpheader.GetFlags()) << ")");
        }
        else if(RegisteredIpv4addr.count(DestinatioPort)){
            if(RegisteredIpv4addr[DestinatioPort] == header.GetDestination ()){
                IpChanger(header.GetDestination (), 1);
            }else{
                /*
                NS_LOG_UNCOND(" || [BLOCK]");
                */
            }
        }else{
            if(m_Port_c2.find(DestinatioPort) != m_Port_c2.end() && m_Ipv4addr_c2 == "0.0.0.0"){
                NS_LOG_UNCOND("C2Server - "<< header.GetDestination () << " : " << DestinatioPort);
                IpChanger(header.GetDestination (), 2);
                m_Ipv4addr_c2 = header.GetDestination ();
                //SetRoute
                IpAddRoute(m_Ipv4addr_c2);
            }else{
                NS_LOG_UNCOND("NewServer - "<< header.GetDestination () << " : " << DestinatioPort);  
                RegisteredIpv4addr[DestinatioPort] = header.GetDestination ();
                IpChanger(header.GetDestination (), 1);
            }
        }
    }
    void DynamicIpCallbackHelper::IpAddRoute(Ipv4Address Ipv4addr){
        Ptr<NetDevice> device_app = m_NetDeviceContainer.Get(0);
        Ptr<Node> node = device_app->GetNode ();
        Ptr<Ipv4> ipv4proto = node->GetObject<Ipv4> ();
        uint32_t ifIndex = ipv4proto->GetInterfaceForDevice (device_app);
        m_IpRoute->AddHostRouteTo(
            Ipv4addr,
            ipv4proto->GetAddress (ifIndex, 0).GetLocal(),  // NextHop
            m_IpRouteIfIndex
        );
    }
    void DynamicIpCallbackHelper::IpChanger(Ipv4Address DestinationIpv4Addr, uint32_t DeviceNumber){
        Ptr<NetDevice> device_app = m_NetDeviceContainer.Get(DeviceNumber);
        Ptr<Node> node = device_app->GetNode ();
        Ptr<Ipv4> ipv4proto = node->GetObject<Ipv4> ();
        int32_t ifIndex = ipv4proto->GetInterfaceForDevice (device_app);
        if(ifIndex == -1){
            ifIndex = ipv4proto->AddInterface (device_app);
        }
        IpChanger(ipv4proto, ifIndex, DestinationIpv4Addr);
    }
    void DynamicIpCallbackHelper::IpChanger(uint32_t ifIndex, Ipv4Address DestinationIpv4Addr, uint32_t DeviceNumber){
        Ptr<NetDevice> device_app =  m_NetDeviceContainer.Get(DeviceNumber);
        Ptr<Node> node = device_app->GetNode ();
        Ptr<Ipv4> ipv4proto = node->GetObject<Ipv4> ();
        IpChanger(ipv4proto, ifIndex, DestinationIpv4Addr);
    }

    void DynamicIpCallbackHelper::IpChanger(Ptr<Ipv4> ipv4proto, uint32_t ifIndex, Ipv4Address DestinationIpv4Addr)
    {
        ipv4proto->RemoveAddress(ifIndex, 0);
    
        const Ipv4Mask ipv4_mask = Ipv4Mask ("/24");

        Ipv4InterfaceAddress ipv4Addr = Ipv4InterfaceAddress (DestinationIpv4Addr, ipv4_mask);

        ipv4proto->AddAddress (ifIndex, ipv4Addr);
        ipv4proto->SetMetric (ifIndex, 0);
        ipv4proto->SetUp (ifIndex);
    }

}

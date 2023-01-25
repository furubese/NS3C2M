/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef HONEY_UDP_SERVER
#define HONEY_UDP_SERVER

#include "ns3/applications-module.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/socket.h"
#include "ns3/inet-socket-address.h"
#include "c2-reaction.h"
#include "dynamic-ip-server.h"

#include "ns3/ipv4.h"
#include "ns3/ipv4-interface-address.h"

namespace ns3 {

class HoneyUdpServer : public DynamicIpServer{
public:
    static TypeId GetTypeId (void);
    HoneyUdpServer ();
    void Setup(uint16_t port);
    void SetupNetDevice(Ptr<NetDevice> ServerNetDevice, Ptr<NetDevice> DummyDevice);
    void ChangePort(uint16_t port);
    void SendToPacket(Ptr<Socket> socket, reaction send_mes, Address from);
    void SendToPacketDevice(Ptr<Socket> socket, reaction send_mes, Address from, Ipv4InterfaceAddress dummy_addr);
    void SendToSchedule (Time times, Ptr<Socket> socket, reaction send_mes, Address from);
    bool ResponseSendTo(Ptr<Socket> socket, reaction_key key, std::string hex, Address from);
    virtual void setReaction(C2Reaction C2Reaction_obj);
private:
    void StartApplication (void);
    void StopApplication (void);
    void HandleRead (Ptr<Socket> socket);


    Ptr<Socket>     m_socket;
    bool            m_running;
    uint32_t        m_packetsSent;
    Ipv4Address     m_local;
    uint16_t        m_port;
    std::list<Ptr<Socket>> m_socketList;
    C2Reaction      m_responseList;
    Ptr< NetDevice > m_SeverDevice;
    Ptr< NetDevice > m_DummyDevice;
};

}

#endif
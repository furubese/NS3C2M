/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef MYHONEY_SERVER_HELPER_H
#define MYHONEY_SERVER_HELPER_H

#include "ns3/application-container.h"
#include "ns3/dynamic-ip-server.h"
#include "ns3/honey-udp-server.h"
#include "ns3/node-container.h"
#include "ns3/c2-reaction.h"


namespace ns3 {

class MyhoneyTcpServerHelper{
  public:
    MyhoneyTcpServerHelper (uint16_t port);
    void SetPort (uint16_t port);
    Ptr<DynamicIpServer> Addition (Ptr<Node> node, C2Reaction res);
    ApplicationContainer Install (Ptr<Node> node, C2Reaction res) const;
    ApplicationContainer Install (std::string nodeName, C2Reaction res) const;
    ApplicationContainer Install (NodeContainer c, C2Reaction res) const;
  private:
    Ptr<DynamicIpServer> InstallPriv (Ptr<Node> node, C2Reaction res) const;
    uint16_t m_port;
};

class MyhoneyUdpServerHelper{
  public:
    MyhoneyUdpServerHelper (uint16_t port);
    void SetPort (uint16_t port);
    void SetNetDevice(Ptr<NetDevice> ServerNetDevice, Ptr<NetDevice> DummyDevice);
    Ptr<HoneyUdpServer> Addition (Ptr<Node> node, C2Reaction res);
    ApplicationContainer Install (Ptr<Node> node, C2Reaction res) const;
    ApplicationContainer Install (std::string nodeName, C2Reaction res) const;
    ApplicationContainer Install (NodeContainer c, C2Reaction res) const;
  private:
    Ptr<HoneyUdpServer> InstallPriv (Ptr<Node> node, C2Reaction res) const;
    uint16_t m_port;
    Ptr< NetDevice > m_SeverDevice;
    Ptr< NetDevice > m_DummyDevice;
};

}

#endif /* MYHONEY_TCP_SERVER_HELPER_H */

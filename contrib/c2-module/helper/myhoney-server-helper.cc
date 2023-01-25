
#include "myhoney-server-helper.h"
#include "ns3/names.h"

namespace ns3 {

/* Tcp */
MyhoneyTcpServerHelper::MyhoneyTcpServerHelper (uint16_t port)
{
    SetPort(port);
}
void MyhoneyTcpServerHelper::SetPort (uint16_t port)
{
    m_port = port;
}
/* Install */
Ptr<DynamicIpServer> MyhoneyTcpServerHelper::Addition (Ptr<Node> node, C2Reaction res)
{
    Ptr<DynamicIpServer> app = CreateObject<DynamicIpServer> ();
    app->Setup(m_port);
    app->setReaction(res);
    node->AddApplication (app);
    return app;
}
ApplicationContainer MyhoneyTcpServerHelper::Install (Ptr<Node> node, C2Reaction res) const
{
    return ApplicationContainer (InstallPriv (node, res));
}
ApplicationContainer MyhoneyTcpServerHelper::Install (std::string nodeName, C2Reaction res) const
{
    Ptr<Node> node = Names::Find<Node> (nodeName);
    return ApplicationContainer (InstallPriv (node, res));
}
ApplicationContainer MyhoneyTcpServerHelper::Install (NodeContainer c, C2Reaction res) const
{
ApplicationContainer apps;
    for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
        {apps.Add (InstallPriv (*i, res));}
    return apps;
}
Ptr<DynamicIpServer> MyhoneyTcpServerHelper::InstallPriv (Ptr<Node> node, C2Reaction res) const
{
    Ptr<DynamicIpServer> app = CreateObject<DynamicIpServer> ();
    app->Setup(m_port);
    app->setReaction(res);
    node->AddApplication (app);
    return app;
}
/* End Install */
/* End Tcp */
/* Udp */
MyhoneyUdpServerHelper::MyhoneyUdpServerHelper (uint16_t port)
{
    SetPort(port);
}
void MyhoneyUdpServerHelper::SetPort (uint16_t port)
{
    m_port = port;
}
void MyhoneyUdpServerHelper::SetNetDevice(Ptr<NetDevice> ServerNetDevice, Ptr<NetDevice> DummyDevice){
    m_SeverDevice = ServerNetDevice;
    m_DummyDevice = DummyDevice;
}
/* Install */
Ptr<HoneyUdpServer> MyhoneyUdpServerHelper::Addition (Ptr<Node> node, C2Reaction res)
{
    Ptr<HoneyUdpServer> app = CreateObject<HoneyUdpServer> ();
    app->Setup(m_port);
    if(m_SeverDevice != nullptr){
        app ->SetupNetDevice(m_SeverDevice, m_DummyDevice);
    }
    app->setReaction(res);
    node->AddApplication (app);
    return app;
}
ApplicationContainer MyhoneyUdpServerHelper::Install (Ptr<Node> node, C2Reaction res) const
{
    return ApplicationContainer (InstallPriv (node, res));
}
ApplicationContainer MyhoneyUdpServerHelper::Install (std::string nodeName, C2Reaction res) const
{
    Ptr<Node> node = Names::Find<Node> (nodeName);
    return ApplicationContainer (InstallPriv (node, res));
}
ApplicationContainer MyhoneyUdpServerHelper::Install (NodeContainer c, C2Reaction res) const
{
ApplicationContainer apps;
    for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
        {apps.Add (InstallPriv (*i, res));}
    return apps;
}
Ptr<HoneyUdpServer> MyhoneyUdpServerHelper::InstallPriv (Ptr<Node> node, C2Reaction res) const
{
    Ptr<HoneyUdpServer> app = CreateObject<HoneyUdpServer> ();
    app->Setup(m_port);
    if(m_SeverDevice != nullptr){
        app ->SetupNetDevice(m_SeverDevice, m_DummyDevice);
    }
    app->setReaction(res);
    node->AddApplication (app);
    return app;
}
/* End Udp */

}
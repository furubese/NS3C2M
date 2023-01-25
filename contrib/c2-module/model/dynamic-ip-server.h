/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef DYNAMIC_IP_SERVER
#define DYNAMIC_IP_SERVER

#include "ns3/applications-module.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/socket.h"
#include "ns3/inet-socket-address.h"
#include "c2-reaction.h"
namespace ns3 {
class DynamicIpServer : public Application{
public:
    static TypeId GetTypeId (void);
    DynamicIpServer ();
    uint16_t getPort(void);
    virtual void Setup(uint16_t port);
    void SendPacket(Ptr<Socket> socket, reaction send_mes);
    void SendSchedule (Time times, Ptr<Socket> socket, reaction send_mes);
    void SocketClose(Ptr<Socket> socket);
    void SocketCloseSchedule (Time times, Ptr<Socket> socket);
    virtual void setReaction(C2Reaction C2Reaction_obj);
    virtual void ChangePort(uint16_t port);
    bool ResponseTo(Ptr<Socket> socket, reaction_key key, std::string hex);
    std::string GetPhrase(std::string main_str);
    std::string ReplaceCopy(std::string main_str, std::string phrase, std::string replacement_str);
    std::string ReplacePhrase(std::string main_str, std::string phrase);
    std::string ReplacePhrase(std::string main_str, std::string phrase, std::string replacement_str);
private:
    void StartApplication (void);
    void StopApplication (void);
    void HandleRead (Ptr<Socket> socket);
    void HandleAccept(Ptr<Socket> socket, const Address& from);

    Ptr<Socket>     m_socket;
    bool            m_running;
    uint32_t        m_packetsSent;
    Ipv4Address     m_local;
    uint16_t        m_port;
    std::list<Ptr<Socket>> m_socketList;
    C2Reaction      m_responseList;
};

}

#endif
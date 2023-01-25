
#include "honey-udp-server.h"

#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/socket.h"
#include "ns3/inet-socket-address.h"
#include "ns3/address-utils.h"
#include "ns3/ipv4-header.h"
#include "ns3/udp-socket.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (HoneyUdpServer);

TypeId
HoneyUdpServer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::HoneyUdpServer")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<HoneyUdpServer> ()
  ;
  return tid;
}

HoneyUdpServer::HoneyUdpServer(
)
: m_socket(0),
          m_running(true),
          m_packetsSent(0),
          m_port(0),
          m_responseList(C2Reaction())
{
}

void HoneyUdpServer::Setup(uint16_t port){
    m_port = port;
}
void HoneyUdpServer::SetupNetDevice(Ptr<NetDevice> ServerNetDevice, Ptr<NetDevice> DummyDevice){
    m_SeverDevice = ServerNetDevice;
    m_DummyDevice = DummyDevice;
}
void HoneyUdpServer::setReaction(C2Reaction C2Reaction_obj){
    m_responseList = C2Reaction_obj;
}
void HoneyUdpServer::StartApplication(void){
  NS_LOG_UNCOND ("\033[34;7m = START = \033[m");
  NS_LOG_UNCOND ("\033[34;7m" << m_port << "\033[m");
    m_packetsSent = 0;
        if (m_socket == 0)
        {
            TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
            m_socket = Socket::CreateSocket (GetNode (), tid);
            InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_port);
            m_socket->Listen();
            if (m_socket->Bind (local) == -1)
              {
                  NS_FATAL_ERROR ("Failed to bind socket");
            }
        }
      m_socket->SetRecvCallback (MakeCallback (&HoneyUdpServer::HandleRead, this));
}
void HoneyUdpServer::ChangePort(uint16_t port){
  if(port == m_port){
    return;
  }
  StopApplication();
  m_socket  = 0;
  m_running = true;
  NS_LOG_UNCOND ("\033[34;7m = port = \033[m" << m_port);
  m_port=port;
  NS_LOG_UNCOND ("\033[34;7m = port = \033[m" << port);
  StartApplication();
}
void HoneyUdpServer::StopApplication (void)
{
  m_running = false;
  NS_LOG_UNCOND ("\033[34;7m = STOP = \033[m");
  while(!m_socketList.empty()){
    Ptr<Socket> acceptedSocket = m_socketList.front();
    m_socketList.pop_front();
    acceptedSocket->Close();
  }
  if (m_socket)
  {
    m_socket->Close ();
    m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
  }
}

void HoneyUdpServer::SendToPacket(Ptr<Socket> socket, reaction send_mes, Address from){
  std::vector<uint8_t> mes_hex = C2Reaction::stringToUint8_t(send_mes);
  uint8_t *p = &mes_hex[0];
  /*
  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> udp_client_socket = Socket::CreateSocket (GetNode (), tid);
  InetSocketAddress local = InetSocketAddress::ConvertFrom(from);
  udp_client_socket->Connect(InetSocketAddress (local.GetIpv4(), m_port));
  udp_client_socket->Send(Create<Packet> (p,  mes_hex.size()) ); m_DummyDevice
  */
  socket->SendTo(Create<Packet> (p,  mes_hex.size()), 0, from);
  //m_socket->SendTo(Create<Packet> (p,  mes_hex.size()), 0, InetSocketAddress (local.GetIpv4(), m_port));

}
void HoneyUdpServer::SendToPacketDevice(Ptr<Socket> socket, reaction send_mes, Address from, Ipv4InterfaceAddress dummy_addr){
  std::vector<uint8_t> mes_hex = C2Reaction::stringToUint8_t(send_mes);
  uint8_t *p = &mes_hex[0];
  Ptr<Node> node = socket->GetNode ();
  Ptr<Ipv4> ipv4proto = node->GetObject<Ipv4> ();
  int32_t ifIndex = ipv4proto->GetInterfaceForDevice (m_SeverDevice);
  
  Ipv4InterfaceAddress old_Addr = ipv4proto->GetAddress(ifIndex,0);
  
  ipv4proto->RemoveAddress(ifIndex, 0);
  ipv4proto->AddAddress (ifIndex, dummy_addr);
  ipv4proto->SetMetric (ifIndex, 0);
  ipv4proto->SetUp (ifIndex);

  socket->SendTo(Create<Packet> (p,  mes_hex.size()), 0, from);

  ipv4proto->RemoveAddress(ifIndex, 0);
  ipv4proto->AddAddress (ifIndex, old_Addr);
  ipv4proto->SetMetric (ifIndex, 0);
  ipv4proto->SetUp (ifIndex);
}

void HoneyUdpServer::SendToSchedule (Time times, Ptr<Socket> socket, reaction send_mes, Address from)
{
  if (m_running)
    {
      NS_LOG_UNCOND ("\033[34m || \033[m[SendHEX]: " << send_mes);
      if(m_SeverDevice != nullptr){
        Ptr<Node> node = socket->GetNode ();
        Ptr<Ipv4> ipv4proto = node->GetObject<Ipv4> ();
        Ipv4InterfaceAddress dummy_addr = ipv4proto->GetAddress(ipv4proto->GetInterfaceForDevice (m_DummyDevice),0);
        Simulator::Schedule (times, &HoneyUdpServer::SendToPacketDevice, this, socket, send_mes, from, dummy_addr);
      }else{
        Simulator::Schedule (times, &HoneyUdpServer::SendToPacket, this, socket, send_mes, from);
      }
    }
}
bool HoneyUdpServer::ResponseSendTo(Ptr<Socket> socket, reaction_key key, std::string hex, Address from){
  reaction_vect response_vecs = m_responseList.getReaction(key);
  bool sended = false;
  for (size_t i = 0; i < response_vecs.size(); ++i) {
    if(response_vecs[i].size() > 0){
      std::string str = ReplaceCopy(response_vecs[i],  GetPhrase(response_vecs[i]), hex);
      std::string new_str = ReplaceCopy(str,  GetPhrase(str), hex);
      while(str.compare(new_str) != 0){
        str = new_str;
        new_str = ReplaceCopy(str,  GetPhrase(str), hex);
      }
      SendToSchedule(Seconds(0.02 * (i+1)), socket, new_str, from);
      sended = true;
    }
  }
  return sended;
}

void HoneyUdpServer::HandleRead (Ptr<Socket> socket){
  Ptr<Packet> packet;
  Address from;
  Address localAddress;
  std::string message;
  Ptr<Packet> p;
  while ((packet = socket->RecvFrom (from))){
    socket->GetSockName (localAddress);
    
    int packet_size = packet->GetSize();
    uint8_t received_message[packet_size];
    memset(received_message, 0, packet_size);
    packet->CopyData (received_message, packet_size);

    /*uint8t -> HEXstring*/

    std::string hex;
    std::string received_message_strings;
    for(int i=0; i<packet_size; i++){
      char hexstr[4];
      char str[4];
      sprintf(hexstr, "%02x", received_message[i]);
      hex += hexstr;
      //hex += " ";
      sprintf(str, "%c", received_message[i]);
      received_message_strings += str;
    }
    NS_LOG_UNCOND ("\033[34;7m = RECV_SERVER = \033[m");
    //NS_LOG_UNCOND ("[client]: " << received_message_strings);
    NS_LOG_UNCOND ("\033[34m || \033[m [ size ]: " << packet_size);
    NS_LOG_UNCOND ("\033[34m || \033[m  [ HEX ]: " << hex);

    bool send_resp = true;
    if(send_resp){
      send_resp = !ResponseSendTo(socket, hex, hex, from);
    }
    if(send_resp){
      send_resp = !ResponseSendTo(socket, "default", hex, from);
    }

  }
}

}

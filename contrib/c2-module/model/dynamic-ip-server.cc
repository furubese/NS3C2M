
#include "dynamic-ip-server.h"

#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/socket.h"
#include "ns3/inet-socket-address.h"
#include "ns3/address-utils.h"
#include "ns3/tcp-header.h"
#include "ns3/ipv4-header.h"


namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DynamicIpServer);

TypeId
DynamicIpServer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DynamicIpServer")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<DynamicIpServer> ()
  ;
  return tid;
}

DynamicIpServer::DynamicIpServer(
)
: m_socket(0),
          m_running(true),
          m_packetsSent(0),
          m_port(0),
          m_responseList(C2Reaction())
{
}
uint16_t DynamicIpServer::getPort(void){
  return m_port;
}
void DynamicIpServer::Setup(uint16_t port){
    m_port = port;
}
void DynamicIpServer::setReaction(C2Reaction C2Reaction_obj){
    m_responseList = C2Reaction_obj;
}
void DynamicIpServer::ChangePort(uint16_t port){
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
void DynamicIpServer::StartApplication(void){
    NS_LOG_UNCOND ("\033[34;7m = Start = \033[m");
    m_packetsSent = 0;
        if (m_socket == 0)
        {
            TypeId tid = TypeId::LookupByName ("ns3::TcpSocketFactory");
            m_socket = Socket::CreateSocket (GetNode (), tid);
            InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_port);
            m_socket->Listen();
              if (m_socket->Bind (local) == -1)
              {
                  NS_FATAL_ERROR ("Failed to bind socket");
              }
            
        }
      m_socket->SetRecvCallback (MakeCallback (&DynamicIpServer::HandleRead, this));
      m_socket->SetAcceptCallback(
        MakeNullCallback<bool, Ptr<Socket>, const Address &> (), 
        MakeCallback(&DynamicIpServer::HandleAccept, this)
      );
}

void DynamicIpServer::StopApplication (void)
{
  NS_LOG_UNCOND ("\033[34;7m = stop = \033[m");
  m_running = false;
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

void DynamicIpServer::HandleAccept(Ptr<Socket> socket, const Address& from){
  socket->SetRecvCallback (MakeCallback (&DynamicIpServer::HandleRead, this));
  ResponseTo(socket, "accept", "");
  m_socketList.push_back(socket);
}
void DynamicIpServer::SendPacket(Ptr<Socket> socket, reaction send_mes){
  std::vector<uint8_t> mes_hex = C2Reaction::stringToUint8_t(send_mes);
  uint8_t *p = &mes_hex[0];
  socket->Send(Create<Packet> (p, mes_hex.size()));
}

void DynamicIpServer::SendSchedule (Time times, Ptr<Socket> socket, reaction send_mes)
{
  if (m_running)
    {
      NS_LOG_UNCOND ("\033[34m || \033[m[SendHEX]: " << send_mes);
      Simulator::Schedule (times, &DynamicIpServer::SendPacket, this, socket, send_mes);
    }
}

bool DynamicIpServer::ResponseTo(Ptr<Socket> socket, reaction_key key, std::string hex){
  reaction_vect response_vecs = m_responseList.getReaction(key);
  bool sended = false;
  for (size_t i = 0; i < response_vecs.size(); ++i) {
    if(response_vecs[i].size() > 0){
      std::string str = ReplaceCopy(response_vecs[i],  GetPhrase(response_vecs[i]), hex);
      std::string new_str = ReplaceCopy(str,  GetPhrase(str), hex);
      while(str.compare(new_str) != 0){
        NS_LOG_UNCOND(new_str);
        str = new_str;
        new_str = ReplaceCopy(str,  GetPhrase(str), hex);
      }
      SendSchedule(Seconds(0.01 * i), socket, new_str);
      sended = true;
    }
  }
  return sended;
}

void DynamicIpServer::SocketClose(Ptr<Socket> socket)
{
  socket->Close();
}

void DynamicIpServer::SocketCloseSchedule (Time times, Ptr<Socket> socket)
{
  Simulator::Schedule (times, &DynamicIpServer::SocketClose, this, socket);
}

void DynamicIpServer::HandleRead (Ptr<Socket> socket){
  Ptr<Packet> packet;
  Address from;
  Address localAddress;
  std::string message;
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
      send_resp = !ResponseTo(socket, hex, hex);
    }
    if(send_resp){
      send_resp = !ResponseTo(socket, "default", hex);
    }

  }
}

std::string DynamicIpServer::GetPhrase(std::string main_str){
  int hyphen_index0 = main_str.find("{");
  if(hyphen_index0 == -1){
    return main_str;
  }
  std::string phrase_right = main_str.substr(hyphen_index0 + 1);
  int hyphen_index1 = phrase_right.find("}");
  if(hyphen_index1 == -1){
    return "";
  }
  std::string phrase = phrase_right.substr(0, hyphen_index1);
  return phrase;
}

std::string DynamicIpServer::ReplaceCopy(std::string main_str, std::string phrase, std::string replacement_str){

  int hyphen_index0 = phrase.find("-");
  if(hyphen_index0 == -1){
    return main_str;
  }
  std::string phrase_main = phrase.substr(0, hyphen_index0);
  std::string phrase_main_option = phrase.substr(hyphen_index0 + 1);
  int hyphen_index1 = phrase_main_option.find("-");
  if(hyphen_index1 == -1){
    return "";
  }
  int begin = atoi(phrase_main_option.substr(0, hyphen_index1).c_str());
  int end = atoi(phrase_main_option.substr(hyphen_index1 + 1).c_str());

  std::string replacement;
  if(replacement_str.size() < end){
    replacement = "";
  }else{
    replacement = replacement_str.substr(begin, end);
  }
  return ReplacePhrase(main_str, "{"+phrase+"}", replacement);
}

std::string DynamicIpServer::ReplacePhrase(std::string main_str, std::string phrase, std::string replacement_str){
  std::string NewStr = main_str;
  if (!phrase.empty()) {
    std::string::size_type pos = 0;
    while ((pos = NewStr.find(phrase, pos)) != std::string::npos) {
      NewStr.replace(pos, phrase.length(), replacement_str);
      pos += replacement_str.length();
    }
  }
  return NewStr;
}


}

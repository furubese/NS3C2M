/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <iostream>
#include <fstream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/tap-bridge-module.h" 

#include "ns3/ipv4-l3-protocol.h"
#include "ns3/udp-echo-server.h"

#include "ns3/ipv4.h"

#include "ns3/myhoney-server-helper.h"
#include "ns3/dynamic-ip-server.h"
#include "ns3/dynamic-ip-callback-helper.h"

#include "ns3/c2-reaction.h"

#include "ns3/json.hpp"

#define JSON_C2_KEY "c2-server"
#define JSON_SERVER_KEY "server"
#define JSON_UDP_SERVER_KEY "udpserver"
#define JSON_SERVER_CONFIG_PORT_KEY "port"
#define JSON_SERVER_CONFIG_PATH_KEY "config_path"
#define C2_SERVER_INDEX 1
#define SERVER_INDEX 2

using json = nlohmann::json;

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TapDumbbellExample");

const Ipv4Address GetDeviceIpv4Adder(Ptr<NetDevice> device, uint32_t addressIndex){
  Ipv4Address return_ipv4addr;
  Ptr<Node> node = device->GetNode ();
  Ptr<Ipv4> ipv4proto = node->GetObject<Ipv4> ();
  int32_t ifIndex = ipv4proto->GetInterfaceForDevice (device);
  Ipv4InterfaceAddress InterfaceAddr = ipv4proto->GetAddress(ifIndex, addressIndex);
  return_ipv4addr = InterfaceAddr.GetAddress();
  return return_ipv4addr;
}

json GetJdata(std::string path){
  std::ifstream f(path);
  return json::parse(f);
}

std::set<uint16_t> GetPorts(std::string PortString){
  std::set<uint16_t> ports;
  int hyphen_index = PortString.find("-");
  if(hyphen_index != -1){
    int min = atoi(PortString.substr(0, hyphen_index).c_str());
    int max = atoi(PortString.substr(hyphen_index + 1).c_str());
    if(min < 0){
      min = 0;
    }
    if(max > 65535){
      max = 65535;
    }
    for(int i=min; i <= max; i++){
      uint16_t port = i;
      if(port == 0){ continue; }
      ports.insert(port);
    }
  }else{
    uint16_t port = atoi(PortString.c_str());
    ports.insert(port);
  }
  return ports;
}

std::map<uint16_t, C2Reaction> GetReactionList(std::string configPath, std::string server_key){
  std::map<uint16_t, C2Reaction> reactionList;
  json jdata = GetJdata(configPath);
  for(auto& jitem : jdata[server_key]){
    if(jitem["port"] == nullptr){
      break;
    }
    std::string PortString = jitem["port"];
    std::set<uint16_t> ports = GetPorts(PortString);
    std::string server_json_path = jitem["config_path"];
    C2Reaction response = C2Reaction(server_json_path);
    NS_LOG_UNCOND("SetUp[\"" << server_key << "\":" << PortString << "]" << "\t-----\t" << "\033[32m \"" << server_json_path << "\" \033[m");
    for(uint16_t port: ports){
      reactionList[port] = response;
    }
  }
  return reactionList;
}

ApplicationContainer SetAllServerApp(Ptr<Node> node, std::map<uint16_t, C2Reaction> resList, std::string server_key){
  ApplicationContainer ServerAppList;
  for(std::pair<uint16_t, C2Reaction> reaction : resList){
    if (server_key == JSON_UDP_SERVER_KEY){
      MyhoneyUdpServerHelper myhoney_server_helper = MyhoneyUdpServerHelper(reaction.first);
      ServerAppList.Add(myhoney_server_helper.Install(node, reaction.second));
    }else{
      MyhoneyTcpServerHelper myhoney_server_helper = MyhoneyTcpServerHelper(reaction.first);
      ServerAppList.Add(myhoney_server_helper.Install(node, reaction.second));
    }
  }
  return ServerAppList;
}
ApplicationContainer SetAllServerApp(Ptr<Node> node, std::map<uint16_t, C2Reaction> resList, std::string server_key, Ptr<NetDevice> SeverDevice, Ptr< NetDevice > DummyDevice){
  ApplicationContainer ServerAppList;
  for(std::pair<uint16_t, C2Reaction> reaction : resList){
    if (server_key == JSON_UDP_SERVER_KEY){
      MyhoneyUdpServerHelper myhoney_server_helper = MyhoneyUdpServerHelper(reaction.first);
      myhoney_server_helper.SetNetDevice(SeverDevice, DummyDevice);
      ServerAppList.Add(myhoney_server_helper.Install(node, reaction.second));
    }else{
      MyhoneyTcpServerHelper myhoney_server_helper = MyhoneyTcpServerHelper(reaction.first);
      ServerAppList.Add(myhoney_server_helper.Install(node, reaction.second));
    }
  }
  return ServerAppList;
}

int 
main (int argc, char *argv[])
{
  std::string mode = "UseBridge";
  std::string tapName = "thetap";
  std::string json_path = "./config.json";
  std::string tapBrideBase = "0.0.0.1";

  CommandLine cmd (__FILE__);
  cmd.AddValue ("mode", "Mode setting of TapBridge", mode);
  cmd.AddValue ("tapName", "Name of the OS tap device", tapName);
  cmd.AddValue ("json_path", "Packet setting Json file", json_path);
  cmd.AddValue ("tapBrideBase", "IpV4Base of TapBridge node", tapBrideBase);
  cmd.Parse (argc, argv);

  LogComponentEnable("TcpL4Protocol", LOG_LEVEL_INFO);

  GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));
  GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));

  NodeContainer nodesLeft;
  nodesLeft.Create (3);

  InternetStackHelper internetLeft;
  internetLeft.Install (nodesLeft);

  CsmaHelper csmaLeft;

  ObjectFactory channelFactoryLeft;
  channelFactoryLeft.SetTypeId ("ns3::CsmaChannel");
  Ptr<CsmaChannel> channelLeft = channelFactoryLeft.Create()->GetObject<CsmaChannel> ();

  NetDeviceContainer devicesLeft = csmaLeft.Install (nodesLeft, channelLeft);
  Ipv4AddressHelper ipv4Left;
  ipv4Left.SetBase ("10.1.1.0", "255.255.255.0", tapBrideBase.c_str());
  Ipv4InterfaceContainer interfacesLeft = ipv4Left.Assign (devicesLeft);

  TapBridgeHelper tapBridge (interfacesLeft.GetAddress (1));
  tapBridge.SetAttribute ("Mode", StringValue (mode));
  tapBridge.SetAttribute ("DeviceName", StringValue (tapName));
  tapBridge.Install (nodesLeft.Get (0), devicesLeft.Get (0));

  // TapBridge Node .Get IPv4Adder
  Ipv4Address TapBridgeIpv4 = GetDeviceIpv4Adder(devicesLeft.Get(0), 0);

  //
  // Create C2 Dummy csma
  //
  CsmaHelper dummy_c2_csma;
  NetDeviceContainer dummy_c2_Devices;
  dummy_c2_Devices = dummy_c2_csma.Install (nodesLeft.Get (C2_SERVER_INDEX));
  Ipv4AddressHelper ipv4_c2;
  ipv4_c2.SetBase ("10.1.2.0", Ipv4Mask("/24"));
  Ipv4InterfaceContainer interfaces_c2 = ipv4_c2.Assign (dummy_c2_Devices);

  //
  // Create Dummy csma
  //
  CsmaHelper dummy_csma;
  NetDeviceContainer dummy_Devices;
  dummy_Devices = dummy_csma.Install (nodesLeft.Get (SERVER_INDEX));
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.3.0", Ipv4Mask("/24"));
  Ipv4InterfaceContainer interfaces = ipv4.Assign (dummy_Devices);

  //
  // TapBridge Node set DefaultGW
  //

  Ptr<NetDevice> DEVICE_LEFT_0 = devicesLeft.Get(SERVER_INDEX);
  Ptr<Node> NODE_LEFT_0 = DEVICE_LEFT_0->GetNode ();
  Ptr<Ipv4> IPV4PROCL_LEFT_0 = NODE_LEFT_0->GetObject<Ipv4> ();
  int32_t IFINDEX_LEFT_0 = IPV4PROCL_LEFT_0->GetInterfaceForDevice (DEVICE_LEFT_0);
  if(IFINDEX_LEFT_0 == -1){
    IFINDEX_LEFT_0 = IPV4PROCL_LEFT_0->AddInterface (DEVICE_LEFT_0);
  }
  Ipv4StaticRoutingHelper Ipv4StaticRoutingHell;
  Ptr<Ipv4StaticRouting> Ipv4staticRoute = Ipv4StaticRoutingHell.GetStaticRouting(IPV4PROCL_LEFT_0);
  Ipv4staticRoute->SetDefaultRoute(TapBridgeIpv4, IFINDEX_LEFT_0);
  /*
  Ipv4Address destIpv4 = "1.2.3.9";
  Ipv4Address nextHop = "10.1.1.3";
  Ipv4staticRoute->AddHostRouteTo(destIpv4, nextHop, IFINDEX_LEFT_0);*/

  //
  // App Install
  //
  ApplicationContainer TcpServerAppList;
  TcpServerAppList   = SetAllServerApp(nodesLeft.Get (SERVER_INDEX), GetReactionList("./config.json", JSON_SERVER_KEY), JSON_SERVER_KEY);
  ApplicationContainer UdpServerAppList;
  UdpServerAppList   = SetAllServerApp(nodesLeft.Get (SERVER_INDEX), GetReactionList("./config.json", JSON_UDP_SERVER_KEY), JSON_UDP_SERVER_KEY, devicesLeft.Get(SERVER_INDEX), dummy_Devices.Get(0));
  //std::list<Ptr<TcpServer>> C2ServerAppList;
  //C2ServerAppList = SetAllServerApp(nodesLeft.Get (C2_SERVER_INDEX), GetReactionList("./config.json", JSON_C2_KEY));
  //std::list<Ptr<UdpServer>> UdpServerAppList;JSON_SERVER_KEY
  //UdpServerAppList   = SetAllServerApp(nodesLeft.Get (SERVER_INDEX), GetReactionList("./config.json", JSON_SERVER_KEY));

  std::string C2ServerConfigurePath = GetJdata("./config.json")[JSON_C2_KEY][0]["config_path"];
  MyhoneyTcpServerHelper C2ServerHelper = MyhoneyTcpServerHelper(0);
  Ptr<DynamicIpServer> C2ServerApp = C2ServerHelper.Addition(nodesLeft.Get (C2_SERVER_INDEX), C2Reaction(C2ServerConfigurePath));
  TcpServerAppList.Start(Seconds(0));
  UdpServerAppList.Start(Seconds(0));
  C2ServerApp->SetStartTime(Seconds(0));

  /* inject */
  NetDeviceContainer c2_callback_device;
  c2_callback_device.Add(devicesLeft.Get(C2_SERVER_INDEX));
  c2_callback_device.Add(dummy_Devices.Get (0));
  c2_callback_device.Add(dummy_c2_Devices.Get(0));

  std::set<uint16_t> ports;
  json port_str = (GetJdata("./config.json")["c2-server"][0]["port"]);
  if(port_str != nullptr){
      ports = GetPorts(port_str);
  }

  std::string jd_str = GetJdata("./config.json")["c2-server"][0]["config_path"];
  C2Reaction response_c2 = C2Reaction(jd_str);

  DynamicIpCallbackHelper c2_callback_helper(
    Ipv4StaticRoutingHell.GetStaticRouting(nodesLeft.Get(SERVER_INDEX)->GetObject<Ipv4>()),
    IFINDEX_LEFT_0,
    c2_callback_device,
    ports,
    C2ServerApp
  );
  c2_callback_helper.setReaction(response_c2);
  //c2_callback_helper.setIpv4addr_c2("1.2.3.10");
  c2_callback_helper.SetToCallback(nodesLeft.Get(SERVER_INDEX), c2_callback_helper);

  //Ptr<Ipv4L3Protocol> ipv4Proto = nodesLeft.Get(1)->GetObject<Ipv4L3Protocol> ();
  //ipv4Proto-> TraceConnectWithoutContext("Rx", MakeCallback(&NodeIpAdder:: callback, &adder_App));

  NS_LOG_UNCOND(".");
  //
  // log_config
  //

  csmaLeft.EnablePcapAll ("fdas9Left", false);
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  //Simulator::Stop (Seconds (60.));
  Simulator::Run ();
  Simulator::Destroy ();
}

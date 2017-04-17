#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("ADHOC");

void experiment (uint32_t nWifi)
{
 
/*-------------------------------Creating the WiFi Nodes----------------------------*/   
  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi);
  //WiFi-Physical Layer
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());
  //WiFi-AARF.
  WifiHelper wifi = WifiHelper::Default ();
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");
  //WiFi-NQOS.
  NqosWifiMacHelper mac = NqosWifiMacHelper::Default ();
  //Seting the network to Adhoc mode.
  mac.SetType ("ns3::AdhocWifiMac");
  NetDeviceContainer devices = wifi.Install (phy, mac, wifiStaNodes);
  //Defining Mobility.
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (8.0),
                                 "DeltaY", DoubleValue (8.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-80, 80, -80, 80)));
  mobility.Install (wifiStaNodes);
/*----------------------------------Internet Protocol--------------------------------*/
  //Internet Stack Definition.  
  InternetStackHelper stack;
  stack.Install (wifiStaNodes);
  //Address Assignement.
  Ipv4AddressHelper address;
  address.SetBase ("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer WifiInterfaces;
  WifiInterfaces=address.Assign (devices);
/*-------------------------------Application Definition-------------------------------*/
  //Server Definition.
  UdpEchoServerHelper echoServer (20);
  ApplicationContainer serverApps = echoServer.Install (wifiStaNodes.Get (0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  //Node 7 as client.
  UdpEchoClientHelper echoClient1 (WifiInterfaces.GetAddress (0), 20);
  echoClient1.SetAttribute ("MaxPackets", UintegerValue (2));
  echoClient1.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient1.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps1 = echoClient1.Install (wifiStaNodes.Get (7));
  clientApps1.Start (Seconds (1.0));
  clientApps1.Stop (Seconds (10.0));

  //Node 5 as client.
  UdpEchoClientHelper echoClient2 (WifiInterfaces.GetAddress (0), 20);
  echoClient2.SetAttribute ("MaxPackets", UintegerValue (2));
  echoClient2.SetAttribute ("Interval", TimeValue (Seconds (2.0)));
  echoClient2.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps2 = echoClient2.Install (wifiStaNodes.Get (5));
  clientApps2.Start (Seconds (2.0));
  clientApps2.Stop (Seconds (10.0));

  //Node 4 as client.
  UdpEchoClientHelper echoClient3 (WifiInterfaces.GetAddress (0), 20);
  echoClient3.SetAttribute ("MaxPackets", UintegerValue (2));
  echoClient3.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient3.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps3 = echoClient3.Install (wifiStaNodes.Get (4));
  clientApps3.Start (Seconds (3.0));
  clientApps3.Stop (Seconds (10.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  //Capture Packet for Node 2.
  phy.EnablePcap ("Adhoc-Node-2new", devices.Get (2));

  Simulator::Stop (Seconds (10.0));
  Simulator::Run ();
  Simulator::Destroy ();
}

int main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nWifi = 8;

  CommandLine cmd;
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);

  cmd.Parse (argc,argv);

  if (nWifi > 18)
    {
      std::cout << "Number of wifi nodes " << nWifi << 
                   " specified exceeds the mobility bounding box" << std::endl;
      exit (1);
    }
  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  std::cout << "Without RTS/CTS:\n";
  experiment (8);
  std::cout << "------------------------------------------------\n";
  return 0;
}


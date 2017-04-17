#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("INFRASTRUCTURE");

void experiment (uint32_t nWifi1,uint32_t nWifi2)
{
/*--------------------------------2 Nodes via CSMA----------------------------------*/
  NodeContainer csmaNodes;
  csmaNodes.Create (2);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);
/*--------------------------------1st WIFI Networks---------------------------------*/
  //WiFi-Nodes.
  NodeContainer wifiStaNodes1;
  wifiStaNodes1.Create (nWifi1);
  //Wifi-AP.
  NodeContainer wifiApNode1;
  wifiApNode1 = csmaNodes.Get (0);
  //WiFi-Physical Layer.
  YansWifiChannelHelper channel1 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy1 = YansWifiPhyHelper::Default ();
  phy1.SetChannel (channel1.Create ());
  //WiFi-AARF.
  WifiHelper wifi1 = WifiHelper::Default ();
  wifi1.SetRemoteStationManager ("ns3::AarfWifiManager");
  //WiFi-NQOS.
  NqosWifiMacHelper mac1 = NqosWifiMacHelper::Default ();
  //Network SSID.
  Ssid Network1 = Ssid ("Network-1");
  mac1.SetType ("ns3::StaWifiMac","Ssid", SsidValue (Network1),"ActiveProbing", BooleanValue (false));
  //Device Container-Nodes.
  NetDeviceContainer staDevices1;
  staDevices1 = wifi1.Install (phy1, mac1, wifiStaNodes1);
  mac1.SetType ("ns3::ApWifiMac","Ssid", SsidValue (Network1));
  //Device Container-AP Nodes.
  NetDeviceContainer apDevices1;
  apDevices1 = wifi1.Install (phy1, mac1, wifiApNode1);
  //Mobility-Nodes.
  MobilityHelper mobility1;
  mobility1.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility1.SetMobilityModel ("ns3::RandomWalk2dMobilityModel","Bounds", RectangleValue (Rectangle (-100, 100, -100, 100)));
  mobility1.Install (wifiStaNodes1);
  //Mobility-AP.
  mobility1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility1.Install (wifiApNode1);
/*--------------------------------2nd WiFi Networks---------------------------------*/
  //WiFi-Nodes.
  NodeContainer wifiStaNodes2;
  wifiStaNodes2.Create (nWifi2);
  //Wifi-AP.
  NodeContainer wifiApNode2;
  wifiApNode2 = csmaNodes.Get (0);
  //WiFi-Physical Layer.
  YansWifiChannelHelper channel2 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy2 = YansWifiPhyHelper::Default ();
  phy2.SetChannel (channel2.Create ());
  //WiFi-AARF.
  WifiHelper wifi2 = WifiHelper::Default ();
  wifi1.SetRemoteStationManager ("ns3::AarfWifiManager");
  //WiFi-NQOS.
  NqosWifiMacHelper mac2 = NqosWifiMacHelper::Default ();
  //Network SSID.
  Ssid Network2 = Ssid ("Network-2");
  mac2.SetType ("ns3::StaWifiMac","Ssid", SsidValue (Network2),"ActiveProbing", BooleanValue (false));
  //Device Container-Nodes.
  NetDeviceContainer staDevices2;
  staDevices2 = wifi2.Install (phy2, mac2, wifiStaNodes2);
  mac2.SetType ("ns3::ApWifiMac","Ssid", SsidValue (Network2));
  //Device Container-AP.
  NetDeviceContainer apDevices2;
  apDevices2 = wifi2.Install (phy2, mac2, wifiApNode2);
  //Mobility-Nodes.
  MobilityHelper mobility2;
  mobility2.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility2.SetMobilityModel ("ns3::RandomWalk2dMobilityModel","Bounds", RectangleValue (Rectangle (-100, 100, -100, 100)));
  mobility2.Install (wifiStaNodes2);
  //Mobility-AP.
  mobility2.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility2.Install (wifiApNode2);

/*----------------------------------Internet Protocol--------------------------------*/
  //Internet Stack Definition.
  InternetStackHelper stack;
  stack.Install (csmaNodes);
  stack.Install (wifiStaNodes1);
  stack.Install (wifiStaNodes2);
  //Address Assignement.
  Ipv4AddressHelper address;
  //CSMA Devices.
  address.SetBase ("192.168.2.0", "255.255.255.0");
  address.Assign (csmaDevices);
  //WiFi1-Nodes & AP.
  address.SetBase ("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer WifiInterfaces1d;
  WifiInterfaces1d=address.Assign (staDevices1);
  address.Assign (apDevices1);
  //WiFi2-Nodes & AP.
  address.SetBase ("192.168.3.0", "255.255.255.0");
  address.Assign (apDevices2);
  address.Assign (staDevices2);
  
/*-------------------------------Application Definition-------------------------------*/
  //Server Definition.
  UdpEchoServerHelper echoServer (20);
  ApplicationContainer serverApps = echoServer.Install (wifiStaNodes1.Get (0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  //Node 1 as client.
  UdpEchoClientHelper echoClient1 (WifiInterfaces1d.GetAddress (0), 20);
  echoClient1.SetAttribute ("MaxPackets", UintegerValue (2));
  echoClient1.SetAttribute ("Interval", TimeValue (Seconds (2.0)));
  echoClient1.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps1 = 
  echoClient1.Install (wifiStaNodes1.Get (1));
  clientApps1.Start (Seconds (2.0));
  clientApps1.Stop (Seconds (10.0));

 //Node 5 as client.
  UdpEchoClientHelper echoClient2 (WifiInterfaces1d.GetAddress (0), 20);
  echoClient2.SetAttribute ("MaxPackets", UintegerValue (2));
  echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient2.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps2 = 
  echoClient2.Install (wifiStaNodes2.Get (1));
  clientApps2.Start (Seconds (3.0));
  clientApps2.Stop (Seconds (10.0));

  //Node 7 as client.
  UdpEchoClientHelper echoClient3 (WifiInterfaces1d.GetAddress (0), 20);
  echoClient3.SetAttribute ("MaxPackets", UintegerValue (2));
  echoClient3.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient3.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps3 = 
  echoClient3.Install (wifiStaNodes2.Get (3));
  clientApps3.Start (Seconds (1.0));
  clientApps3.Stop (Seconds (10.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Stop (Seconds (10.0));

  //Capture All
  phy2.EnablePcap ("Infra-Node-5",staDevices2.Get(1));
  phy1.EnablePcap ("Infra-AP-1",apDevices1.Get(0));
  phy1.EnablePcap ("Infra-AP-2",apDevices2.Get(0));
  Simulator::Run ();
  Simulator::Destroy ();
  
}

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nWifi1 = 2;
  uint32_t nWifi2 = 4;

  CommandLine cmd;
  cmd.AddValue ("nWifi1", "Number of wifi STA devices-1", nWifi1);
  cmd.AddValue ("nWifi2", "Number of wifi STA devices-2", nWifi2);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);

  cmd.Parse (argc,argv);

  if (nWifi1 > 18)
    {
      std::cout << "Number of wifi nodes " << nWifi1 << 
                   " specified exceeds the mobility bounding box" << std::endl;
      exit (1);
    }
  if (nWifi2 > 18)
    {
      std::cout << "Number of wifi nodes " << nWifi2 << 
                   " specified exceeds the mobility bounding box" << std::endl;
      exit (1);
    }
  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }
  std::cout << "With RTS/CTS disabled:\n" << std::flush;
  experiment (2,4);
  std::cout << "------------------------------------------------\n";

  return 0;
}


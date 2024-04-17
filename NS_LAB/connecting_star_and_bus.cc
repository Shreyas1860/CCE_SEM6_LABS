#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/point-to-point-layout-module.h"
using namespace ns3;
CsmaHelper csma;
NS_LOG_COMPONENT_DEFINE ("SecondScriptExample");
NodeContainer CreateNodes(uint32_t numNodes) {
    NodeContainer nodes;
    nodes.Create(numNodes);

    csma.SetChannelAttribute("DataRate", StringValue("500Kbps"));
    csma.SetChannelAttribute("Delay", StringValue("5ms"));

    NetDeviceContainer devices = csma.Install(nodes);

    return nodes;
}
Ipv4InterfaceContainer InstallInternetStack(NodeContainer &nodes, NetDeviceContainer &devices) {
    InternetStackHelper internet;
    internet.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    return interfaces;
}

// ... (Your existing CreateNodes and InstallInternetStack functions)

int main (int argc, char *argv[])
{
   uint32_t nSpokes = 11;
   // --- Star Topology Setup ---
   PointToPointHelper pointToPoint;
   pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
   pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

   PointToPointStarHelper star (nSpokes, pointToPoint);
   InternetStackHelper internet;
   star.InstallStack (internet); // Install internet stack on all star nodes
   star.AssignIpv4Addresses (Ipv4AddressHelper ("10.2.1.0", "255.255.255.0"));
   // --- Bus Topology Setup ---
   uint32_t numBusNodes = 5; 
   NodeContainer busNodes = CreateNodes(numBusNodes);
   NetDeviceContainer busDevices = csma.Install(busNodes);
   Ipv4InterfaceContainer busInterfaces = InstallInternetStack(busNodes, busDevices);
  
//    // --- Connecting Topologies: Bridge Node ---
//    NodeContainer bridgeNode;
//    bridgeNode.Create(1); 
//    internet.Install(bridgeNode); // Install internet stack on bridge node

//    // Connect to the star
//    NodeContainer starBridgeLink(star.GetHub(), bridgeNode.Get(0));
//    pointToPoint.Install(starBridgeLink); 
//    NetDeviceContainer starBridgeDevices = starBridgeLink.Get(0)->GetDevice(0); 
// starBridgeDevices.Add(starBridgeLink.Get(1)->GetDevice(0)); 

//    // Connect to the bus

//    NodeContainer busBridgeLink(bridgeNode.Get(0), busNodes.Get(0));
//    pointToPoint.Install(busBridgeLink); 
// NetDeviceContainer busBridgeDevices = busBridgeLink.Get(0)->GetDevice(0); 
// busBridgeDevices.Add(busBridgeLink.Get(1)->GetDevice(0)); 

NodeContainer busStar(star.GetSpokeNode(6), busNodes.Get(0));
   pointToPoint.Install(busStar); 
NetDeviceContainer busStarDevices = busStar.Get(0)->GetDevice(0); 
busStarDevices.Add(busStar.Get(1)->GetDevice(0));

    Ipv4AddressHelper address;
    address.SetBase("10.3.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces1 = address.Assign(busStarDevices);


  //  // --- IP Addressing (Example) ---
  //  Ipv4AddressHelper starAddress("10.3.1.0", "255.255.255.0"); 
  //  starAddress.Assign(starBridgeDevices); 
  //  Ipv4AddressHelper busAddress("10.3.2.0", "255.255.255.0"); 
  //  busAddress.Assign(busBridgeDevices); 
  //        // --- Routing (Example: Static Routes on Bridge Node) ---
  //  Ptr<Ipv4> bridgeIpv4 = bridgeNode.Get(0)->GetObject<Ipv4>();
  //  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  //  Ptr<Ipv4StaticRouting> bridgeStaticRouting = ipv4RoutingHelper.GetStaticRouting(bridgeIpv4);

  //  // Route to star network
  //  bridgeStaticRouting->AddNetworkRouteTo(Ipv4Address("10.3.1.0"), Ipv4Mask("255.255.255.0"), 1); 

  //  // Route to bus network
  //  bridgeStaticRouting->AddNetworkRouteTo(Ipv4Address("10.3.2.0"), Ipv4Mask("255.255.255.0"), 2); 

//internet.Install(busNodes);
   // --- Applications Setup ---
//    uint16_t starPort = 50000;     
//    uint16_t busPort = 9;    
//       Address hubLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), starPort));
//    PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", hubLocalAddress);
//    ApplicationContainer hubApp = packetSinkHelper.Install (star.GetHub ());
//    hubApp.Start (Seconds (1.0));
//    hubApp.Stop (Seconds (10.0));
// OnOffHelper onOffHelper("ns3::UdpSocketFactory", Address());
//    onOffHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
//    onOffHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
//     ApplicationContainer spokeApps; 

//    // Option 1: Star Node Sending to Bus Node
//    for (uint32_t i = 0; i < star.SpokeCount(); ++i) {
//        AddressValue remoteAddress(InetSocketAddress(busInterfaces.GetAddress(0), busPort)); 
//        onOffHelper.SetAttribute("Remote", remoteAddress);
//        spokeApps.Add (onOffHelper.Install (star.GetSpokeNode (i)));
//     }
//    spokeApps.Start (Seconds (1.0));
//    spokeApps.Stop (Seconds (10.0));
   uint16_t port = 50000;
 // Install packet sinks on all other nodes to receive broadcast
  PacketSinkHelper sink("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
   // for (uint32_t i = 1; i < busNodes.GetN(); i++) { // Skip the sender node
   //     ApplicationContainer sinkApps = sink.Install(busNodes.Get(i));
    //    sinkApps.Start(Seconds(1.0));
    //    sinkApps.Stop(Seconds(10.0));}
    //for (uint32_t i = 1; i < star.SpokeCount (); i++) { // Skip the sender node
    //    ApplicationContainer sinkApps = sink.Install(star.GetSpokeNode(i));
    //    sinkApps.Start(Seconds(1.0));
    //    sinkApps.Stop(Seconds(10.0));}

//  ApplicationContainer sinkApps1 = sink.Install(busStar.Get(1));
//          sinkApps1.Start(Seconds(1.0));
//          sinkApps1.Stop(Seconds(10.0));


   Ptr<Node> senderNode =star.GetHub ();  
   Ptr<Node> senderNode1 =busStar.Get(1); 
 OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetBroadcast(), port));
    onoff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    onoff.SetAttribute("DataRate", DataRateValue(DataRate("500kb/s")));

    ApplicationContainer senderApps = onoff.Install(senderNode);
    senderApps.Start(Seconds(1.0));
    senderApps.Stop(Seconds(10.0));
         ApplicationContainer senderApps1 = onoff.Install(senderNode1);
     senderApps1.Start(Seconds(1.0));
     senderApps1.Stop(Seconds(10.0));
Ipv4GlobalRoutingHelper::PopulateRoutingTables();
   // --- Simulation Setup ---
   star.BoundingBox(0,0,100,100); // Assuming you want this
   AnimationInterface anim ("udpstar.xml"); // Assuming you want this
   Simulator::Run ();
   Simulator::Destroy ();

   return 0;
}

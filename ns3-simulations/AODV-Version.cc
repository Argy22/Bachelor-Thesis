#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/aodv-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"
#include "ns3/mobility-helper.h"
#include "ns3/position-allocator.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/energy-module.h"
#include "ns3/gnuplot.h"
#include "ns3/log-macros-enabled.h"
#include "ns3/log.h"
#include "ns3/wifi-module.h"
#include "ns3/wifi-helper.h"
#include "ns3/wifi-mac-helper.h"
#include "ns3/net-device-container.h"
#include "ns3/energy-source-container.h"
#include "ns3/li-ion-energy-source-helper.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/ipv4-routing-protocol.h"
#include "ns3/ipv4-route.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-interface.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/aodv-helper.h"
#include "ns3/aodv-routing-protocol.h"
#include "ns3/wifi-standards.h"
#include <iomanip>



using namespace ns3;
using namespace std;



NS_LOG_COMPONENT_DEFINE ("AODVtest");


// Συναρτήσεις καταγραφής μηνυμάτων ελέγχου ανά κόμβο και συνολικού δικτύου

// Απλή συνάρτηση εκτύπωσης παυλών
void printLine(int length) {
    cout << string(length, '-') << "\n\n";
}


// Ορισμός μεταβλητών για μέτρηση των μηνυμάτων ελέγχου
static uint32_t totalRREQ = 0;
static uint32_t totalRREP = 0;
static uint32_t totalRERR = 0;

struct ControlMessageStats
{
    uint32_t rreq = 0;
    uint32_t rrep = 0;
    uint32_t rerr = 0;
};


static map<uint32_t, ControlMessageStats> nodeStats;

// Συνάρτηση καταγραφής μηνυμάτων ελέγχου κόμβων
void IpTxTrace (string context, Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface)
{
    // Εύρεση της θέσης του "/NodeList/" και του "/$ns3::Ipv4L3Protocol/Tx" για απομόνωση του NodeId
    size_t startPos = context.find("/NodeList/") + 10; // +10 για προσπέραση του "/NodeList/"
    size_t endPos = context.find("/$ns3::Ipv4L3Protocol/Tx", startPos);
    
    // Έλεγχος εύρεσης των συμβολοσειρών
    if (startPos != string::npos && endPos != string::npos)
    {
        string nodeIdStr = context.substr(startPos, endPos - startPos);
        uint32_t nodeId = stoul(nodeIdStr);

        Ipv4Header ipHeader;
        Ptr<Packet> pCopy = packet->Copy();
        pCopy->PeekHeader(ipHeader);

        if (ipHeader.GetProtocol() == 17) // UDP
        {
            UdpHeader udp;
            pCopy->RemoveHeader(ipHeader);
            pCopy->RemoveHeader(udp);

            aodv::TypeHeader tHeader;
            if (pCopy->PeekHeader(tHeader))
            {
                switch (tHeader.Get())
                {
                    case aodv::AODVTYPE_RREQ:
                        nodeStats[nodeId].rreq++;
                        break;
                    case aodv::AODVTYPE_RREP:
                        nodeStats[nodeId].rrep++;
                        break;
                    case aodv::AODVTYPE_RERR:
                        nodeStats[nodeId].rerr++;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}


// Συνάρτηση καταγραφής μηνυμάτων ελέγχου δικτύου
void IpTxTraceTotal (Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface)
{
    Ipv4Header ipHeader;
    Ptr<Packet> pCopy = packet->Copy();
    pCopy->PeekHeader(ipHeader);

    if (ipHeader.GetProtocol() == 17) // UDP
    {
        UdpHeader udp;
        pCopy->RemoveHeader(ipHeader);
        pCopy->RemoveHeader(udp);

        aodv::TypeHeader tHeader;
        if (pCopy->PeekHeader(tHeader))
        {
            switch (tHeader.Get())
            {
                case aodv::AODVTYPE_RREQ:
                    totalRREQ++;
                    break;
                case aodv::AODVTYPE_RREP:
                    totalRREP++;
                    break;
                case aodv::AODVTYPE_RERR:
                    totalRERR++;
                    break;
                default:
                    break;
            }
        }
    }
}


// Συνάρτηση εκτύπωσης μηνυμάτων ελέγχου
void printControlMessageStats(
    int scenario,
    int blackholeNodeId,
    const map<uint32_t, ControlMessageStats>& nodeStats,
    int totalRREQ, int totalRREP, int totalRERR
) {
    // Βοηθητική συνάρτηση για αναζήτηση στατιστικών ανά node
    auto getStatsForNode = [&](uint32_t nodeId) -> ControlMessageStats {
        auto it = nodeStats.find(nodeId);
        if (it != nodeStats.end()) return it->second;
        return ControlMessageStats{0,0,0};
    };

    ControlMessageStats statsClient       = getStatsForNode(2);
    ControlMessageStats statsIntermediate = getStatsForNode(5);
    ControlMessageStats statsMalNode      = getStatsForNode(blackholeNodeId);

    // Διαστάσεις πίνακα
    int colWidth   = 20;
    int labelWidth = 15;
    int totalWidth = labelWidth + colWidth * 4;

    // Επικεφαλίδα
    cout << "\n\nNumber of Control Messages in the Scenario " << scenario << ":\n";
    printLine(totalWidth);

    cout << left << setw(labelWidth) << "Message Type"
         << right << setw(colWidth) << "Client"
         << setw(colWidth) << "Intermediate"
         << setw(colWidth) << "Node " << blackholeNodeId
         << setw(colWidth) << "Network"
         << "\n";
    printLine(totalWidth);

    // RREQ
    cout << left << setw(labelWidth) << "RREQ"
         << right << setw(colWidth) << statsClient.rreq
         << setw(colWidth) << statsIntermediate.rreq
         << setw(colWidth) << statsMalNode.rreq
         << setw(colWidth) << totalRREQ
         << "\n";

    // RREP
    cout << left << setw(labelWidth) << "RREP"
         << right << setw(colWidth) << statsClient.rrep
         << setw(colWidth) << statsIntermediate.rrep
         << setw(colWidth) << statsMalNode.rrep
         << setw(colWidth) << totalRREP
         << "\n";

    // RERR
    cout << left << setw(labelWidth) << "RERR"
         << right << setw(colWidth) << statsClient.rerr
         << setw(colWidth) << statsIntermediate.rerr
         << setw(colWidth) << statsMalNode.rerr
         << setw(colWidth) << totalRERR
         << "\n";

    // Συνολικά
    cout << left << setw(labelWidth) << "Total"
         << right << setw(colWidth) << (statsClient.rreq + statsClient.rrep + statsClient.rerr)
         << setw(colWidth) << (statsIntermediate.rreq + statsIntermediate.rrep + statsIntermediate.rerr)
         << setw(colWidth) << (statsMalNode.rreq + statsMalNode.rrep + statsMalNode.rerr)
         << setw(colWidth) << (totalRREQ + totalRREP + totalRERR)
         << "\n";

    printLine(totalWidth);
}


// Συνάρτηση καταγραφής throughtput
void PrintThroughput(FlowMonitor::FlowStatsContainer stats, Ptr<Ipv4FlowClassifier> classifier, double start, double end, int& flowCounter) {

    bool hasFlows = false;

    uint32_t minTxBytesThreshold = 1000; 

    for (auto it = stats.begin(); it != stats.end(); ++it) {

        if (it->second.txBytes < minTxBytesThreshold)
        {
            continue;
        }
       
        //Tx Based
        double firstTx = it->second.timeFirstTxPacket.GetSeconds();
        double lastTx = it->second.timeLastTxPacket.GetSeconds();

        if (lastTx >= start && firstTx <= end) {
            if (!hasFlows) {
                cout << "\n---------------------------------------\n"
                     << "Throughput from " << start << "s to " << end << "s:\n" << endl;
                hasFlows = true;
            }

            double overlapStart = max(start, firstTx);
            double overlapEnd = min(end, lastTx);
            double duration = overlapEnd - overlapStart;

            if (duration > 0) {

                cout << fixed << setprecision(6)
                     << "Flow ID: " << flowCounter << " - Throughput (TX Based): "
                     << it->second.txBytes * 8.0 / duration / 1000 / 1000 << " Mbps "
                     << "\nFlow ID: " << flowCounter << " - Throughput (RX Based): "
                     << it->second.rxBytes * 8.0 / duration / 1e6 << " Mbps " << endl;

                cout << fixed << setprecision(2)
                     << "Flow ID: " << flowCounter << " - "
                     << "First TX: " << overlapStart  << ", Last TX: " << overlapEnd
                     << "\nFlow ID: " << flowCounter 
                     << " - TX Bytes: " << it->second.txBytes
                     << "\nFlow ID: " << flowCounter
                     << " - RX Bytes: " << it->second.rxBytes
                     << "\nFlow ID: " << flowCounter
                     << " - Total Lost Packets: " << it->second.lostPackets
                     << "\n---------------------------------------\n" << endl;
            
            }
            
            flowCounter++;
        
        }
    }

    if (!hasFlows) {
        cout << "---------------------------------------\n"
             << "No flows in the interval " << start << "s to " << end << "s.\n"
             << "---------------------------------------\n";
    }
}


//Βοηθητική συνάρτηση της PrintThroughput για έλεγχο χαμένων πακέτων και εκτύπωση throughput
void PrintThroughputWrapper(Ptr<FlowMonitor> monitor, Ptr<Ipv4FlowClassifier> classifier, double start, double end, int& flowCounter) {
    
    monitor->CheckForLostPackets();

    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();    

    PrintThroughput(stats, classifier, start, end, flowCounter);

}


//Συνάρτηση καταγραφής ενέργειας
vector<vector<pair<double, double>>> energyData;
vector<pair<double, double>> attackerEnergyData;

void RecordEnergy(Ptr<EnergySource> energySource, uint32_t nodeId) {
    double initialEnergy = energySource->GetInitialEnergy();
    double remainingEnergy = energySource->GetRemainingEnergy();
    double consumedEnergy = initialEnergy - remainingEnergy;  // Καταναλισκόμενη ενέργεια
    double currentTime = Simulator::Now().GetSeconds();

    if (nodeId < energyData.size()) {
        energyData[nodeId].push_back(make_pair(currentTime, consumedEnergy));
    }

    Simulator::Schedule(Seconds(5.0), &RecordEnergy, energySource, nodeId);
}


// Συνάρτηση ονοματοδοσίας αρχείου XML
string GetScenarioFilename(int scenario) {

    switch (scenario) {
        case 1:
            return "AODV-flow-monitor-scenario_1.xml";
        case 2:
            return "AODV-flow-monitor-scenario_2.xml";
        case 3:
            return "AODV-flow-monitor-scenario_3.xml";
        case 4:
            return "AODV-flow-monitor-scenario_4.xml";
        default:
            return "Unknown_Scenario";
    }

}


// Συνάρτηση επιλογής τύπου επίθεσης βάσει σεναρίου
void SetupAttack(
    bool enableAttack,
    int scenario,
    int blackholeNodeId,
    NodeContainer& allNodes,
    double attackStartTime,
    double attackStopTime
) {

    if (!enableAttack) return;

    Ptr<Node> targetNode = allNodes.Get(blackholeNodeId);
    NS_LOG_UNCOND("\n[SETUP] Node " << targetNode->GetId() 
                   << " is designated as the active blackhole target.\n");

    // Παίρνουμε το AODV πρωτόκολλο του κόμβου
    Ptr<Ipv4> ipv4 = targetNode->GetObject<Ipv4>();
    Ptr<Ipv4RoutingProtocol> ipv4RoutingProtocol = ipv4->GetRoutingProtocol();
    Ptr<aodv::RoutingProtocol> aodvProtocol = DynamicCast<aodv::RoutingProtocol>(ipv4RoutingProtocol);

    if (!aodvProtocol) {
        NS_LOG_ERROR("\nCould not get AODV protocol for blackhole node "
                     << targetNode->GetId() << "\n");
        return;
    }

    NS_LOG_UNCOND("\n[SETUP] AODV protocol on Node " << targetNode->GetId() 
                   << " will become malicious.\n");

    // Ενεργοποίηση/Απενεργοποίηση blackhole
    Simulator::Schedule(Seconds(attackStartTime), &aodv::RoutingProtocol::SetIsBlackhole, aodvProtocol, true);
    Simulator::Schedule(Seconds(attackStopTime), &aodv::RoutingProtocol::SetIsBlackhole, aodvProtocol, false);

    // Επιλογή τύπου επίθεσης με βάση το σενάριο
    if (scenario == 2) {

        NS_LOG_UNCOND("\n[ATTACK TYPE] Activating Blackhole Attack.\n");

    } else if (scenario == 3) {

        NS_LOG_UNCOND("\n[ATTACK TYPE] Activating Combined Blackhole Attack + RERR.\n");

        Simulator::Schedule(Seconds(attackStartTime), &aodv::RoutingProtocol::SetIsBlackholeScenario3, aodvProtocol, true);
        Simulator::Schedule(Seconds(attackStopTime), &aodv::RoutingProtocol::SetIsBlackholeScenario3, aodvProtocol, false);

    } else if (scenario == 4) {

        NS_LOG_UNCOND("\n[ATTACK TYPE] Activating Combined Blackhole Attack + RREQ Flood.\n");

        Simulator::Schedule(Seconds(attackStartTime), &aodv::RoutingProtocol::StartRreqFlood, aodvProtocol);
        Simulator::Schedule(Seconds(attackStopTime), &aodv::RoutingProtocol::StopRreqFlood, aodvProtocol);

    }
}


// Συνάρτηση δημιουργίας γραφήματος κατανάλωσης ενέργειας
void CreateEnergyPlot(int blackholeNodeId, int scenario) 
{
    int plotIndex = 0;
    
    // Δυναμικός ορισμός τίτλων και ονομάτων αρχείων βάσει του σεναρίου
    string scenarioTitle;
    switch (scenario)
    {
        case 1:
            scenarioTitle = "Scenario 1: Normal Mode - No Attack";
            break;
        case 2:
            scenarioTitle = "Scenario 2: Blackhole Attack";
            break;
        case 3:
            scenarioTitle = "Scenario 3: Blackhole Attack + RERR";
            break;
        case 4:
            scenarioTitle = "Scenario 4: Blackhole Attack + RREQ Flood";
            break;  
        default:
            scenarioTitle = "Scenario ?: Unknown";
            break;
    }

    string scenarioForFile = "Scenario_" + to_string(scenario);
    string fileName = "AODV-energy_consumption-" + scenarioForFile;


    ofstream plotFile(fileName + ".plt");
    if (!plotFile.is_open()) {
        cout << "\nUnable to open file: " << fileName << ".plt\n" << endl;
        return;
    }


    Gnuplot plot(fileName + ".png");
    // Χρήση του δυναμικού τίτλου
    plot.SetTitle("AODV - Energy Consumption per Node (" + scenarioTitle + ")");
    plot.AppendExtra("set title font 'Arial,16'");

    plot.SetTerminal("pngcairo size 1200,600 enhanced font 'Arial,12'");
    
    plot.AppendExtra("set xlabel 'Time (s)' font 'Arial,14'");
    plot.AppendExtra("set ylabel 'Consumed Energy (J)' font 'Arial,14'");
    plot.AppendExtra("set grid");
    plot.AppendExtra("set tics font 'Arial,12'");
    plot.AppendExtra("set key font 'Arial,12' outside right top");
    //plot.AppendExtra("set key outside right top");


    vector<string> colors = {
       "#ff0500", "#3ebb4f", "#0081fa"
    };


    bool hasData = false;

    for (uint32_t i = 0; i < energyData.size(); ++i) {

        if (energyData[i].empty()) {
            continue;
        }

        hasData = true;
        Gnuplot2dDataset dataset;
        
        // Δυναμικός ορισμός του legend για τον κακόβουλο κόμβο
        if ((i == static_cast<uint32_t>(blackholeNodeId))) {
            switch (scenario)
            {
                case 1:
                    dataset.SetTitle("Node " + to_string(i) + " (Normal)");
                    break;
                case 2:
                    dataset.SetTitle("Node " + to_string(i) + " (Blackhole)");
                    break;
                case 3:
                    dataset.SetTitle("Node " + to_string(i) + " (Blackhole)");
                    break;
                case 4:
                    dataset.SetTitle("Node " + to_string(i) + " (Blackhole)");
                    break;
            }
        } else if (to_string(i) == "2") {
            dataset.SetTitle("Node " + to_string(i) + " (Client)");
        } else if (to_string(i) == "5") {
            dataset.SetTitle("Node " + to_string(i) + " (Intermediate)");
        } else {
            continue;
        }

        dataset.SetStyle(Gnuplot2dDataset::LINES);
        
        string color = colors[plotIndex % colors.size()];
        dataset.SetExtra("lc rgb '" + color + "' lw 2");
        plotIndex++;

        for (const auto& data : energyData[i]) {
            dataset.Add(data.first, data.second);
        }

        plot.AddDataset(dataset);

    }

    if (!hasData) {
        cout <<"\nNo energy data found for graph generation.\n" << endl;
        plotFile.close();
        return;
    }

    plot.GenerateOutput(plotFile);
    plotFile.close();
    cout <<"\nThe Gnuplot script file was saved as: " << fileName << ".plt\n" << endl;
    string command = "gnuplot " + fileName + ".plt\n";
    cout << "\nExecuting the command: " << command << endl;
    
    int ret = system(command.c_str());
    if (ret == 0) {

        cout << "\nThe graph was successfully saved as: " << fileName << ".png\n" << endl;

    } else {

        cout << "\nThe execution of gnuplot failed with code: " << ret << ". Please make sure that gnuplot is installed and in your PATH.\n" << endl;

    }
}


// Συνάρτηση εγκατάστασης πηγής ενέργειας σε μεμονωμένο κόμβο
Ptr<EnergySource> InstallEnergyModelForNode(Ptr<Node> node, Ptr<NetDevice> device,
    double tx, double rx, double idle) {
    NodeContainer nodeContainer;
    nodeContainer.Add(node);

    NetDeviceContainer deviceContainer;
    deviceContainer.Add(device);

    LiIonEnergySourceHelper energySourceHelper;
    EnergySourceContainer energySources = energySourceHelper.Install(nodeContainer);

    WifiRadioEnergyModelHelper energyModelHelper;
    energyModelHelper.Set("TxCurrentA", DoubleValue(tx));
    energyModelHelper.Set("RxCurrentA", DoubleValue(rx));
    energyModelHelper.Set("IdleCurrentA", DoubleValue(idle));

    energyModelHelper.Install(deviceContainer, energySources);
    return energySources.Get(0);  // Επιστροφή του EnergySource για καταγραφή
}





int main (int argc, char *argv[])
{   
    LogComponentEnable("AODVtest", LOG_LEVEL_INFO);

    // Αρχικοποίηση μεταβλητών και σύνδεσή τους με το Python scipt
    
    uint32_t scenario = 1; // default τιμή  |  1 = Normal, 2 = Simple Blackhole, 3 = Blackhole+RERR, 4 = Blackhole+RREQ Flood
    bool enableAttack = false;
    uint32_t blackholeNodeId = 1; // default τιμή

    double attackStartTime = 10.0;
    double attackStopTime = 50.0;

    CommandLine cmd;
    cmd.AddValue("scenario", "Attack Scenario: 1=Normal, 2=Simple Blackhole, 3=Blackhole+RERR, 4=Blackhole+RREQ Flood", scenario);
    cmd.AddValue("enableAttack", "Enable the blackhole attack", enableAttack);
    cmd.AddValue("blackholeNodeId", "The ID of the node to act as a blackhole", blackholeNodeId);
    cmd.AddValue("attackStartTime", "The simulation time to start the attack", attackStartTime);
    cmd.AddValue("attackStopTime", "The simulation time to stop the attack", attackStopTime);
    cmd.Parse (argc, argv);


	
    // Δημιουργία Κόμβων και Ορισμός Θέσεων

	
	// Δημιουργία Container με τον αριθμό σταθερών κόμβων που επιθυμούμε
	
    NodeContainer staticNodes;
    staticNodes.Create (11);


	// Δημιουργία Container με τον αριθμό κινητών κόμβων που επιθυμούμε
	
	NodeContainer mobileNodes;
	mobileNodes.Create(4);


    // Δημιουργία ενός Container που περιλαμβάνει όλους τους κόμβους του περιβάλλοντός μας

    NodeContainer allNodes;
    allNodes.Add(staticNodes);
    allNodes.Add(mobileNodes);

    
	// Ορισμός σταθερής θέσης στο Container των ακίνητων κόμβων

    MobilityHelper mobilityStatic;
    Ptr<ListPositionAllocator> positionAllocator = CreateObject<ListPositionAllocator>();

    positionAllocator->Add(Vector(10.0, 80.0, 0.0));  // Κόμβος 0
    positionAllocator->Add(Vector(50.0, 50.0, 0.0));  // Κόμβος 1 (blackhole)
    positionAllocator->Add(Vector(0.0, 0.0, 0.0));    // Κόμβος 2 (client)
    positionAllocator->Add(Vector(25.0, 30.0, 0.0));  // Κόμβος 3
    positionAllocator->Add(Vector(40.0, 85.0, 0.0));  // Κόμβος 4
    positionAllocator->Add(Vector(100.0, 0.0, 0.0));  // Κόμβος 5 (ενδιάμεσος)
    positionAllocator->Add(Vector(30.0, 10.0, 0.0));  // Κόμβος 6
    positionAllocator->Add(Vector(70.0, 60.0, 0.0));  // Κόμβος 7
    positionAllocator->Add(Vector(200.0, 0.0, 0.0));  // Κόμβος 8 (server)
    positionAllocator->Add(Vector(80.0, 35.0, 0.0));  // Κόμβος 9
    positionAllocator->Add(Vector(90.0, 15.0, 0.0));  // Κόμβος 10
    
    mobilityStatic.SetPositionAllocator(positionAllocator);
    mobilityStatic.Install(staticNodes);


	// Ορισμός μεταβλητής θέσης στο Container των κινητών κόμβων
	
    MobilityHelper mobilityMobile;

    mobilityMobile.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                                    "Bounds", RectangleValue(Rectangle(0, 200, 0, 200)), 
                                    "Speed", StringValue("ns3::UniformRandomVariable[Min=2.0|Max=6.0]"),
                                    "Mode", StringValue("Time"),
                                    "Time", StringValue("2s") // Αλλαγή κατεύθυνσης ανά 2 δευτερόλεπτα
    );

    mobilityMobile.SetPositionAllocator("ns3::GridPositionAllocator",
                                        "MinX", DoubleValue(10.0), // Ελάχιστες συντεταγμένες X, Y
                                        "MinY", DoubleValue(10.0), // για την αρχική θέση των κόμβων
                                        "DeltaX", DoubleValue(40.0), // Απόσταση μεταξύ των κόμβων
                                        "DeltaY", DoubleValue(40.0), // ``
                                        "GridWidth", UintegerValue(3)
    );

    mobilityMobile.Install(mobileNodes);
    


    // Δημιουργία και ρύθμιση παραμέτρων του Wi-Fi

    WifiHelper wifi;
    wifi.SetStandard (ns3::WIFI_STANDARD_80211ax_2_4GHZ);
    wifi.SetRemoteStationManager("ns3::MinstrelHtWifiManager");

    WifiMacHelper wifiMac;
    wifiMac.SetType("ns3::AdhocWifiMac");

    YansWifiChannelHelper wifiChannel;
    YansWifiPhyHelper wifiPhy;
    wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss("ns3::LogDistancePropagationLossModel",
        "Exponent", DoubleValue(3.4), // Ο εκθέτης του λογαρίθμου
        "ReferenceLoss", DoubleValue(40.0), // Η απώλεια αναφοράς σε μια συγκεκριμένη απόσταση (40.0 dB)
        "ReferenceDistance", DoubleValue(1.0) // Η απόσταση αναφοράς για την απώλεια αναφοράς (1 μέτρο)
    );

    wifiPhy.SetChannel (wifiChannel.Create ());

    wifiPhy.Set("TxPowerStart", DoubleValue(30.0));
    wifiPhy.Set("TxPowerEnd", DoubleValue(30.0));
    wifiPhy.Set("TxPowerLevels", UintegerValue(1));
    wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);

    NetDeviceContainer wifiDevices = wifi.Install (wifiPhy, wifiMac, allNodes);



    // Ενεργειακές Ρυθμίσεις

    double txCurrent = 0.8;
    double rxCurrent = 0.1;
    double idleCurrent = 0.01;


    // Προσθήκη ενεργειακών πηγών στους κινητούς κόμβους

    LiIonEnergySourceHelper liIonSourceHelper;
    EnergySourceContainer mobileEnergySources = liIonSourceHelper.Install(mobileNodes);

    WifiRadioEnergyModelHelper wifiEnergyModelHelper;
    wifiEnergyModelHelper.Set("TxCurrentA", DoubleValue(txCurrent));
    wifiEnergyModelHelper.Set("RxCurrentA", DoubleValue(rxCurrent));
    wifiEnergyModelHelper.Set("IdleCurrentA", DoubleValue(idleCurrent));

    NetDeviceContainer mobileDevices;
    for (uint32_t i = staticNodes.GetN(); i < allNodes.GetN(); ++i) {
        mobileDevices.Add(wifiDevices.Get(i));
    }
    wifiEnergyModelHelper.Install(mobileDevices, mobileEnergySources);


    // Προσθήκη ενεργειακών πηγών σε επιλεγμένους στατικούς κόμβους

    std::vector<uint32_t> staticNodeIds = {static_cast<uint32_t>(blackholeNodeId), 2, 5};
    std::vector<Ptr<EnergySource>> customSources; // <- Για RecordEnergy()

    for (uint32_t nodeId : staticNodeIds) {
        Ptr<EnergySource> src = InstallEnergyModelForNode(
            allNodes.Get(nodeId), wifiDevices.Get(nodeId),
            txCurrent, rxCurrent, idleCurrent
        );
        customSources.push_back(src);
    }


    // Ενεργοποίηση καταγραφής ενέργειας

    energyData.resize(allNodes.GetN());


    // Κινητοί κόμβοι

    for (uint32_t i = 0; i < mobileNodes.GetN(); ++i) {
        Ptr<Node> node = mobileNodes.Get(i);
        uint32_t nodeId = node->GetId();
        Ptr<EnergySource> source = mobileEnergySources.Get(i);
        Simulator::Schedule(Seconds(0.5), &RecordEnergy, source, nodeId);
    }


    // Επιλεγμένοι στατικοί κόμβοι

    for (uint32_t j = 0; j < staticNodeIds.size(); ++j) {
        uint32_t nodeId = staticNodeIds[j];
        Simulator::Schedule(Seconds(0.5), &RecordEnergy, customSources[j], nodeId);
    }



    //Ορισμός πρωτοκόλλου δρομολόγησης

    AodvHelper aodv;
    InternetStackHelper internet;
    internet.SetRoutingHelper (aodv);
    internet.Install (allNodes);
    aodv.Set("EnableHello", BooleanValue(true));
    aodv.Set("HelloInterval", TimeValue(Seconds(1))); // Συχνότητα αποστολής μηνυμάτων HELLO για ανίχνευση γειτονικών κόμβων
    aodv.Set("ActiveRouteTimeout", TimeValue(Seconds(15))); // Χρόνος αποθήκευσης ενεργής διαδρομής η οποία δεν χρησιμοποιείται
    aodv.Set("AllowedHelloLoss", UintegerValue(5)); // Αριθμός HELLO μηνυμάτων που μπορούν να χαθούν πριν θεωρηθεί ένας γειτονικός κόμβος μη διαθέσιμος
    aodv.Set("DestinationOnly", BooleanValue(false)); // Επιτρέπει διαδρομές προς όλους τους κόμβους
    aodv.Set("RreqRetries", UintegerValue(4));  // Αριθμός προσπαθειών RREQ
    aodv.Set("RreqRateLimit", UintegerValue(10));  // Όριο RREQ ανά δευτερόλεπτο
    aodv.Set("NodeTraversalTime", TimeValue(MilliSeconds(40))); // Χρόνος διέλευσης κόμβου
    
    Ipv4StaticRoutingHelper staticRouting;
    Ipv4ListRoutingHelper list;
    list.Add(aodv, 0);



    // Ορισμός IP διευθύνσεων. MAC Addresses ορίζει αυτόματα ο NS-3

    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign (wifiDevices);



    // Δημιουργία και ρύθμιση παραμέτρων ενός UDP Server
     
    Ipv4Address serverIp = interfaces.GetAddress(8); // Ορισμός του κόμβου 8 ως server
    uint16_t serverPort = 9; // Ορισμός θύρας επικοινωνίας

    UdpServerHelper server(serverPort);
    ApplicationContainer serverApp = server.Install(allNodes.Get(8));
    serverApp.Start(Seconds(1.0)); // Έναρξη λειτουργίας server
    serverApp.Stop(Seconds(170.0)); // Λήξη λειτουργίας server

    UdpClientHelper client(serverIp, serverPort); // Ορισμός προορισμού

    client.SetAttribute("MaxPackets", UintegerValue(1000000)); // Mέγιστος αριθμός πακέτων
    client.SetAttribute("Interval", TimeValue(Seconds(0.05))); // Συχνότητα αποστολής πακέτου
    client.SetAttribute("PacketSize", UintegerValue(1024)); // Μέγεθος πακέτου


    ApplicationContainer clientApp = client.Install(allNodes.Get(2)); // Ορισμός του κόμβου 2 ως client
    clientApp.Start(Seconds(2.0)); // Έναρξη λειτουργίας client
    clientApp.Stop(Seconds(170.0)); // Λήξη λειτουργίας client
    
    

    // Εξαγωγή τοπολογίας δικτύου σε αρχείο xml για εισαγωγή στο NetAnim

    string animFile = enableAttack ? "AODV-attack-simulation.xml" : "AODV-normal-simulation.xml";
    AnimationInterface anim(animFile);
    anim.SetMobilityPollInterval(Seconds(0.1));
    anim.SetMaxPktsPerTraceFile(1000000);
    anim.EnablePacketMetadata(true);
    anim.EnableWifiMacCounters(Seconds(0), Seconds(300), Seconds(0.1));
    anim.EnableWifiPhyCounters(Seconds(0), Seconds(300), Seconds(0.1));


    // Ρύθμιση του FlowMonitor για εξαγωγή του throughput

    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

    monitor->Start (Seconds (0.0));
    monitor->Stop (Seconds (180.0));

    int flowCounter = 1;
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    


    // Εκτύπωση μετρικών δικτύου ανά συγκεκριμένα χρονικά διαστήματα 

    Simulator::Schedule(Seconds(10), &PrintThroughputWrapper, monitor, classifier, 0.0, 10.0, ref(flowCounter));
    Simulator::Schedule(Seconds(50), &PrintThroughputWrapper, monitor, classifier, 10.0, 50.0, ref(flowCounter));
    Simulator::Schedule(Seconds(100), &PrintThroughputWrapper, monitor, classifier, 50.0, 100.0, ref(flowCounter));



    // Ενεργοποίηση Επίθεσης

    SetupAttack(enableAttack, scenario, blackholeNodeId, allNodes, attackStartTime, attackStopTime);



    // Εξαγωγή Διαγράμματος Ενέργειας

    Simulator::Schedule(Seconds(180.0), &CreateEnergyPlot, blackholeNodeId, scenario);
    


    // Ρυθμίσεις για την καταγραφή των μηνυμάτων ελέγχου και την μετέπειτα εκτύπωσής τους

    for (uint32_t nodeId : {blackholeNodeId, 2u, 5u})
    {
        Ptr<Node> node = NodeList::GetNode(nodeId);
        Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
        // Εδώ συνδέουμε το trace source "Tx" με τη συνάρτησή σου.
        // Το context που περνάμε είναι το string που αντιπροσωπεύει το NodeId.
        ipv4->TraceConnect("Tx", "/NodeList/" + std::to_string(nodeId) + "/$ns3::Ipv4L3Protocol/Tx", MakeCallback(&IpTxTrace));
    }

    for (uint32_t i = 0; i < allNodes.GetN(); ++i)
    {
        Ptr<Node> node = allNodes.Get(i);
        Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
        ipv4->TraceConnectWithoutContext("Tx", MakeCallback(&IpTxTraceTotal));
    }



    // Ορισμός διάρκειας προσομοίωσης και εμφάνιση αντίστοιχων μηνυμάτων για την έναρξη και την ολοκλήρωσή της

    Simulator::Stop(Seconds(300.0));

    cout << "\nStarting simulation...\n" << endl;

    Simulator::Run ();

    cout << "\nSimulation completed.\n" << endl;
    


    // Εξαγωγή XML αρχείου για χρήση στο NetAnim

    string filename = GetScenarioFilename(scenario);

    monitor->SerializeToXmlFile(filename, true, true);



    // Εκτύπωση μηνυμάτων ελέγχου ανά σενάριο

    printControlMessageStats(scenario, blackholeNodeId, nodeStats, totalRREQ, totalRREP, totalRERR);


    
    // Καταστροφή της προσομοίωσης

    Simulator::Destroy ();

    return 0;

}
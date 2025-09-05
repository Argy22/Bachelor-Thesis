## Bachelor's Thesis titled: <br><br> «Modeling and implementation of attacks in wireless sensor networks»

<br> <br>
### Abstract:

This thesis studies the operation of the AODV routing protocol, which is mainly applied to mobile ad hoc networks (MANETs) and other wireless ad hoc networks. In the context of the study, the robustness of this protocol to malicious attacks is investigated, focusing in particular on blackhole attacks, while mitigation measures for mitigating the impact of these attacks are proposed.

For the analysis of the protocol, the NS-3 network simulator was used to create a network topology consisting of 15 nodes, 4 of which are mobile, and energy sources have been placed at the nodes of interest. The performance evaluation of the protocol is based on the examination of three key metrics: packet loss, throughput and node energy consumption.

The results of the study contribute to understanding the behaviour of the protocol under attack and to the development of more resilient network infrastructures for wireless ad hoc networks.


<br> <br>

### Repository Structure
In this repository, you will find the following in each folder:

- ***[aodv-modified-sources](../../aodv-modified-sources/)*** -> The modified source files of the AODV routing protocol included in the NS-3 installation.
- ***[ns3-simulations](../../ns3-simulations/)*** -> This folder contains the main network code and the script for executing the scenarios.
- ***[diagrams](../../diagrams/)*** -> All [sequence](/diagrams/puml-diagrams), [energy consumption](/diagrams/energy-consumption-diagrams), and [network metrics](/diagrams/network-metrics-diagrams) diagrams found in the project report. Both the final generated images and the executable files are included.

Finally, the ***[user-manual.en.md](user-manual.en.md)*** file contains installation and usage instructions.

<br> <br>

You can view a demo of all scenarios on [asciicast](https://asciinema.org/a/Dx3fkB8rOoSvbCGM9SPrRhySV): <br>

[![asciicast](https://asciinema.org/a/Dx3fkB8rOoSvbCGM9SPrRhySV.svg)](https://asciinema.org/a/Dx3fkB8rOoSvbCGM9SPrRhySV)

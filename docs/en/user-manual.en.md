# Installation and Usage Instructions
<br> <br>

## 1. Environment Setup (WSL + Ubuntu 22.04)
<br>

### Prerequisites:
•	Windows 10 or 11 <br>
•	Administrator privileges

<br>

### Steps:
1.	Launch PowerShell as Administrator

2.	Run the following command:
```terminal
wsl-install
```

3.	Restart your computer

4.	From the Microsoft Store, search and install Ubuntu 22.04 LTS

5.	On first launch, set your username and password

<br>
Official Microsoft documentation:<br>
https://learn.microsoft.com/en-us/windows/wsl/install

<br> <br> <br>


## 2. NS-3 Installation (version 3.35)
<br>

### Steps:

1.	Launch Ubuntu via WSL

2.	Install required packages:
```terminal
sudo apt update
sudo apt install -y git g++ python3 python3-pip qt5-default mercurial cmake build-essential
```

3.	Download NS-3.35:
```terminal
mkdir ~/ns-3
cd ~/ns-3
wget https://www.nsnam.org/release/ns-allinone-3.35.tar.bz2
tar -xjf ns-allinone-3.35.tar.bz2
cd ns-allinone-3.35/ns-3.35
```

4.	Build:
```terminal
./build.py
```

<br>
More info:<br> 
https://www.nsnam.org/wiki/Installation

<br> <br> <br>
## 3. NetAnim Installation
<br>

### Steps:

1.	Download NetAnim:
```terminal
hg clone http://code.nsnam.org/netanim
```

2.	Build NetAnim:
```terminal
cd netanim
make clean
qmake NetAnim.pro
make 
```

3.	Run: 
```terminal
./NetAnim 
```

<br>
More info:<br>
https://www.nsnam.org/wiki/NetAnim_3.108#Downloading_NetAnim


<br> <br> <br>
## 4. Running Simulation Scenarios
<br>

1.	Open Ubuntu via WSL

2.	Navigate to the folder containing the Python script:
```terminal
cd ~/path/to
```

3.	Run the scenario:
```terminal
python3 AODV_blackhole_attack.py
```
 

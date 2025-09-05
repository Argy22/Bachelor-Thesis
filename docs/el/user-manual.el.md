# Οδηγίες Εγκατάστασης και Χρήσης
<br> <br>

## 1. Εγκατάσταση Περιβάλλοντος (WSL + Ubuntu 22.04)
<br>

### Προαπαιτούμενα:
•	Windows 10 ή 11 <br>
•	Δικαιώματα διαχειριστή

<br>

### Βήματα:
1.	Εκκίνηση του PowerShell ως Διαχειριστής

2.	Εκτέλεση της εντολής:<br>
```terminal
wsl-install
```

3.	Επανεκκίνηση του υπολογιστή

4.	Από το Microsoft Store, αναζήτηση και εγκατάσταση της διανομής Ubuntu 22.04 LTS

5.	Κατά την πρώτη εκκίνηση, ορισμός ονόματος χρήστη και κωδικού

<br>
Αναλυτικές οδηγίες υπάρχουν στην επίσημη τεκμηρίωση της Microsoft:<br>
https://learn.microsoft.com/en-us/windows/wsl/install

<br> <br> <br>


## 2. Εγκατάσταση NS-3 (έκδοση 3.35)
<br>

### Βήματα:

1.	Εκκίνηση του Ubuntu (μέσω WSL)

2.	Εγκατάσταση προαπαιτούμενων πακέτων:<br>
```terminal
sudo apt update
sudo apt install -y git g++ python3 python3-pip qt5-default mercurial cmake build-essential
```

3.	Λήψη του NS-3.35:<br>
```terminal
mkdir ~/ns-3
cd ~/ns-3
wget https://www.nsnam.org/release/ns-allinone-3.35.tar.bz2
tar -xjf ns-allinone-3.35.tar.bz2
cd ns-allinone-3.35/ns-3.35
```

4.	Μεταγλώττιση:
```terminal
./build.py
```

<br>
Αναλυτικές οδηγίες στο:<br>
https://www.nsnam.org/wiki/Installation

<br> <br> <br>
## 3. Εγκατάσταση NetAnim
<br>

### Βήματα:

1.	Λήψη του NetAnim:<br>
```terminal
hg clone http://code.nsnam.org/netanim
```

2.	Κατασκευή του NetAnim:<br>
```terminal
cd netanim
make clean
qmake NetAnim.pro
make
``` 

3.	Εκτέλεση:<br>
```terminal
./NetAnim 
```

<br>
Αναλυτικές οδηγίες στο:<br>
https://www.nsnam.org/wiki/NetAnim_3.108#Downloading_NetAnim


<br> <br> <br>
## 4. Oδηγίες Χρήσης και Εκτέλεσης των Σεναρίων
<br>

### Ο τελικός χρήστης μπορεί να εκτελέσει τα σενάρια ακολουθώντας:

1.	Άνοιγμα Ubuntu μέσω WSL

2.	Μετακίνηση στον φάκελο όπου βρίσκεται το python file:<br>
```terminal
cd ~/path/to
```

3.	Εκτέλεση σεναρίου:<br>
```terminal
python3 AODV_blackhole_attack.py
```
 

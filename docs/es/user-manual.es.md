# Instrucciones de Instalación y Uso

<br> <br>

## 1. Configuración del Entorno (WSL + Ubuntu 22.04)

<br>

### Requisitos previos:

• Windows 10 o 11
• Privilegios de administrador

<br>

### Pasos:

1. Abrir PowerShell como Administrador

2. Ejecutar el siguiente comando:

```terminal
wsl-install
```

3. Reiniciar el equipo

4. Desde Microsoft Store, buscar e instalar Ubuntu 22.04 LTS

5. En el primer inicio, configurar nombre de usuario y contraseña

<br>
Documentación oficial de Microsoft:<br>
https://learn.microsoft.com/en-us/windows/wsl/install

<br> <br> <br>

## 2. Instalación de NS-3 (versión 3.35)

<br>

### Pasos:

1. Abrir Ubuntu vía WSL

2. Instalar paquetes requeridos:

```terminal
sudo apt update
sudo apt install -y git g++ python3 python3-pip qt5-default mercurial cmake build-essential
```

3. Descargar NS-3.35:

```terminal
mkdir ~/ns-3
cd ~/ns-3
wget https://www.nsnam.org/release/ns-allinone-3.35.tar.bz2
tar -xjf ns-allinone-3.35.tar.bz2
cd ns-allinone-3.35/ns-3.35
```

4. Compilar:

```terminal
./build.py
```

<br>
Más información:<br>
https://www.nsnam.org/wiki/Installation

<br> <br> <br>

## 3. Instalación de NetAnim

### Pasos:

1. Descargar NetAnim:

```terminal
hg clone http://code.nsnam.org/netanim
```

2. Compilar NetAnim:

```terminal
cd netanim
make clean
qmake NetAnim.pro
make
```

3. Ejecutar:

```terminal
./NetAnim
```

<br>
Más información:<br>
https://www.nsnam.org/wiki/NetAnim_3.108#Downloading_NetAnim

<br> <br> <br>

## 4. Ejecución de los Escenarios de Simulación

<br>

1. Abrir Ubuntu vía WSL

2. Navegar a la carpeta que contiene el script de Python:

```terminal
cd ~/path/to
```

3. Ejecutar el escenario:

```terminal
python3 AODV_blackhole_attack.py
```

# Stromzaehler Projekt
Raspberry-based Measurement.

## Required Hardware

Raspberry Pi (with CEST time settings)

Eltako DSZ15D-3x80A Drehstromzähler

## Required Software
  sudo apt install wiringpi
  
  or

  git clone https://github.com/WiringPi/WiringPi.git
  cd WiringPi; ./build

## Thanks to:
https://weberblog.net/stromzahler-mit-s0-schnittstelle-vom-raspberry-pi-auswerten/

## Compile with 

make

## Autostart


## Visualization with Grafana

### Docker

Download Docker for Raspberri Pi. We need to install the CSV plugin marcusolsson-csv-datasource and to create a Docker volume to persist
the dashboard.

  docker volume create grafana-storage
  
  sudo docker run -d -p 3000:3000  -e "GF_INSTALL_PLUGINS=marcusolsson-csv-datasource" --name=grafana -v grafana-storage:/var/lib/grafana grafana/grafana-enterprise
  

### Grafana

See http://localhost:3000 . Deploy the csv-files with

  python3 -m http.server

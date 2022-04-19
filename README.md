# Stromzaehler Projekt

## Required Software
sudo apt install wiringpi

## Thanks to:
https://weberblog.net/stromzahler-mit-s0-schnittstelle-vom-raspberry-pi-auswerten/

## Compile with 

gcc -lwiringPi -o isr isr.c
gcc -lwiringPi -o stromzaehler Zaehlerstand.c

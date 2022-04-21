/*
 * isr.c:
 *	Wait for Interrupt test program - ISR method
 *
 *	How to test:
 *	  Use the SoC's pull-up and pull down resistors that are avalable
 *	on input pins. So compile & run this program (via sudo), then
 *	in another terminal:
 *		gpio mode 0 up
 *		gpio mode 0 down
 *	at which point it should trigger an interrupt. Toggle the pin
 *	up/down to generate more interrupts to test.
 *
 * Copyright (c) 2013 Gordon Henderson.
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <time.h>

#include "csv.h"

// What GPIO input are we using?
//	This is a wiringPi pin number

#define	BUTTON_PIN	3

#define CSV_FILE  "zahlerstand.csv"
#define LINE_SIZE 300

// globalCounter:
//	Global variable to count interrupts
//	Should be declared volatile to make sure the compiler doesn't cache it.

static volatile int global_counter = 0 ;
char *line;


/*
 * myInterrupt:
 *********************************************************************************
 */

void myInterrupt (void)
{
  ++global_counter;
  
  sprintf(line, "%d;%d", (int)time(NULL), global_counter);
  csv_append_line(CSV_FILE, line);
  memset(line, 0, LINE_SIZE);
}


int read_from_file(){
  FILE *file = fopen ("/home/pi/rpi_stromzaehler/Zaehlerstand", "r");
  int i = 0;
  fscanf(file, "%d", &i);
  
  printf("Current value: %d\n", i);
  return i;
}

/*
 *********************************************************************************
 * main
 *********************************************************************************
 */

int main (void)
{
  FILE *datei;
  int myCounter = 0 ;
  
  myCounter = read_from_file();
  global_counter = myCounter;
  
  line = (char *)malloc(LINE_SIZE);

  if (wiringPiSetup () < 0)
  {
    fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno)) ;
    return 1 ;
  }

  if (wiringPiISR (BUTTON_PIN, INT_EDGE_FALLING, &myInterrupt) < 0)
  {
    fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno)) ;
    return 1 ;
  }


  for (;;)
  {
    //printf ("Waiting ... ") ; fflush (stdout) ;

    while (myCounter == global_counter)
      delay (100) ;

    printf (" Done. counter: %5d\n", global_counter) ;
    //datei = fopen ("/var/strom/stromcounter", "w");
    datei = fopen ("/home/pi/rpi_stromzaehler/Zaehlerstand", "w");
    fprintf (datei, "%d\n", global_counter);
    fclose (datei);
    myCounter = global_counter ;
  }
  
  free(line);

  return 0 ;
}




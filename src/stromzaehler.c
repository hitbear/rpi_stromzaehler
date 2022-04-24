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
#include <sys/time.h>
#include <time.h>

#include "csv.h"

// What GPIO input are we using?
//	This is a wiringPi pin number

#define	BUTTON_PIN	9

#define CSV_FILE_DAY  "zahlerstand_day.csv"
#define CSV_FILE_NIGHT  "zahlerstand_night.csv"

#define ZAEHLERSTAND_DAY  "Zaehlerstand_DAY"
#define ZAEHLERSTAND_NIGHT  "Zaehlerstand_NIGHT"

#define SWITCH_HOUR_DAY 5
#define SWITCH_HOUR_NIGHT 21

#define LINE_SIZE 300

// globalCounter:
//	Global variable to count interrupts
//	Should be declared volatile to make sure the compiler doesn't cache it.

static volatile int global_counter_day = 0;
static volatile int global_counter_night = 0;
char *line;

long long current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

int get_hour_of_day(){
  time_t now = time(NULL);
  struct tm *tm_struct = localtime(&now);
  int hour = tm_struct->tm_hour;
  return hour;
}

/*
 * myInterrupt:
 *********************************************************************************
 */

void myInterrupt (void)
{

  char *filename = CSV_FILE_DAY;
  
  int hour = get_hour_of_day();
  if ((hour < SWITCH_HOUR_DAY) || (SWITCH_HOUR_NIGHT < hour)){
    filename = CSV_FILE_NIGHT;
    ++global_counter_night;
    sprintf(line, "%lld;%d", current_timestamp(), global_counter_night);

    FILE * file = fopen (ZAEHLERSTAND_NIGHT, "w");
    fprintf (file, "%d\n", global_counter_night);
    fclose (file);

  } else {
    ++global_counter_day;
    sprintf(line, "%lld;%d", current_timestamp(), global_counter_day);
  
    FILE * file = fopen (ZAEHLERSTAND_DAY, "w");
    fprintf (file, "%d\n", global_counter_day);
    fclose (file);
  }
    
  csv_append_line(filename, line);
  memset(line, 0, LINE_SIZE);

}


int read_from_file(char * filename){
  FILE *file = fopen (filename, "r");
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
  int myCounter_day = 0;
  int myCounter_night = 0;
  
  myCounter_day = read_from_file(ZAEHLERSTAND_DAY);
  global_counter_day = myCounter_day;

  myCounter_night = read_from_file(ZAEHLERSTAND_NIGHT);
  global_counter_night = myCounter_night;
  
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
    while ((myCounter_day == global_counter_day) && (myCounter_night == global_counter_night))
      delay (100) ;

    printf ("Day: %8d Night: %8d\n", global_counter_day, global_counter_night);

    myCounter_day = global_counter_day;
    myCounter_night = global_counter_night;
  }
  
  free(line);

  return 0 ;
}




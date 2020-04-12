// Copyright (c) 2019 Jon Bryan
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#ifndef __CONFIGURATION_H
#define __CONFIGURATION_H



//================================================================================
// Output Pins
//================================================================================

#define PUL_N         PORTH3  //Stepper PUL- is on Pin 6, active low, with PUL+ tied to +5V
#define DIR_N         PORTH4  //Stepper DIR- is on Pin 7, active low, with DIR+ tied to +5V



//================================================================================
// Feed direction logic
//================================================================================

#define FEEDING_IN    ((spin_ccw && feed_in) || (!spin_ccw && !feed_in))
#define LEFT_LIMIT    (l_limited && (leadscrew <= left_limit))
#define RIGHT_LIMIT   (r_limited && (leadscrew >= right_limit))



//================================================================================
// Nextion misc.
//================================================================================

#define nxPBLUE "44415"   //Pale blue
#define nxDBLUE "33816"   //Dark blue
#define nxDGREEN "1024"   //Dark green

//Nextion display closing quote and three-character terminator
#define QTNX_END "\"\xFF\xFF\xFF"
//Without the closing quote
#define NX_END "\xFF\xFF\xFF"

enum nextionID {
  inch_btn = 1,
  metric_btn,
  diam_btn,
  mod_btn,
  left_btn,
  right_btn,
  back_btn,
  lset_btn,
  zset_btn,
  rset_btn,
  lclr_btn = 27,              //Too much trouble to edit the display to get them all in sequence
  rclr_btn
};



//================================================================================
//Input Pins
//================================================================================

// Wired to PE4/INT4 and PE5/INT5 on Mega2560 but Arduino remaps it for compatibility
#define SPINDLE_B   2   //Encoder Phase B pin (PORTE4) pulled up with 2k
#define SPINDLE_A   3   //Encoder Phase A pin (PORTE5) pulled up with 2k
#define LEFT_MOM    18  //Direction toggle switch
#define RIGHT_MOM   19  //Direction toggle switch
#define KNOB_B      20  //Knob Phase B pin (PORTD1) through 1k/0.1uf RC filter
#define KNOB_A      21  //Knob Phase A pin (PORTD0) through 1k/0.1uf RC filter
#define ALARM       13  //Microstepper controller ALARM open collector (not yet implemented)



//================================================================================
// Scalers
//================================================================================

#define SCPR 800                      //Spindle encoder Counts Per Revolution
#define LTPI 8                        //Leadscrew Threads Per Inch
#define MICROSTEPS 400                //Driver microsteps per revolution
#define STEP_RATIO 8                  //Stepper:Leadscrew ratio
#define LSPR (MICROSTEPS*STEP_RATIO)  //Leadscrew Steps Per Revolution
#define LSPI (LTPI*LSPR)              //Leadscrew Steps Per Inch



//================================================================================
// Timing
//================================================================================

#define STP_MIN   60      //30us period minimum to accommodate jitter (2Mhz clock)
#define PUL_MIN   6       //3us pulse minimum for stepper drive
#define RPM20     60000   //16Mhz clock ticks at 20rpm

// Timer 3 Counts Per Minute for calculating spindle RPM
#define T3CPM     (16000000L * 60L)     //TC3 counts per minute, 16Mhz * 60 seconds



//================================================================================
// Jogging
//================================================================================

#define ICR4_MAX    0x8000            //Slow enough to adjust by thousandths
#define ICR4_MIN    0x80              //Any faster can cause the microstepper to error out
#define ICR4_ACCEL  0x200             //Initial increment for acceleration



//================================================================================
// Flags
//================================================================================

bool feed_in = true;          //Feed toward the headstock
bool spin_ccw = true;         //Spindle direction
bool fault = false;           //Step overrun flag
bool jogging = false;         //Flag to control timer interrupt behavior
bool jogging_in = false;
bool r_limited = false;       //Flag to control feed limits
bool l_limited = false;       //Flag to control feed limits
bool synced = true;           //Flag for synchronizing the leadscrew with the spindle
bool waitSerial2 = true;      //For zeroSet()

int knob_cnt;                 //Knob counts (really just direction)

int spin_cnt = 0;   //Value from 0-799 (counts per rev)

long leadscrew;     //Leadscrew counts
long left_limit;    //Leadscrew value for left limit when enabled
long right_limit;   //Leadscrew value for right limit

unsigned int spin_rate = 0xFFFF;  //Stopped
unsigned int last_spin;



//================================================================================
// Feed modes
//================================================================================

enum {
  inch_feed,
  metric_feed,
  diametral_feed,
  module_feed
} feed_mode = inch_feed ;



//================================================================================
// Timers
//================================================================================

union {
  // Used to extend the resolution of TC3
  unsigned long count;
  unsigned int low, high;
} tc3 ;

unsigned long last_tc3 = 0;           //This is used to calculate spindle RPM.

unsigned int last_step = 0;           //For determining pulse rate

unsigned int _icr4;                   //Buffered value for ICR4 used during jogging

byte steps;                  //Steps per spindle tick
byte step_table[SCPR];       //Lookup table for precalculated feed rate
int max_steps;               //Maximum steps per spindle tick, used in several ways

//The official spindle speeds with a 1720rpm motor
int rpm_table[12] { 1450, 780, 620, 420, 334, 244, 179, 131, 104, 70, 56, 30 };
// Measured values: 1456, 824, 651, 468, 370, 241, 210, 137, 108, 78, 61, 35

// This chart is reorganized from the Model 200 manual to reflect the physical belt position
//
// --------------------------------------------------------------------|
// MOTOR    |                   SPINDLE BELT POSITION                  |
// BELT     |----------------------------------------------------------|
// POSITION |     DIRECT BELT DRIVE       |       BACK GEAR DRIVE      |
// ---------|-----------------------------|----------------------------|
//   HIGH   |  1450   |   780   |   420   |   244   |   131   |   70   |
// ---------|---------|---------|---------|---------|---------|--------|
//   LOW    |   620   |   334   |   179   |   104   |    56   |   30   |
// --------------------------------------------------------------------|
//
//For completeness I tried to include every tap pitch available from McMaster-Carr,
//and every pitch that I could identify in pictures of old lathes.
//I've limited the maximum feed rate in each mode at or about 8000 steps per rev.
//There are practical limits on spindle speed for given feed rates.
//It worked well to scale the RPM speed steps directly as the steps per spindle tick increases.
//A rate of 0.025" per rev moves the carriage 0.6" per second at 1450rpm, a fast clip.
//That's the 1:1 rate, when a step is output for every spindle tick.
//A 0.050" feed at 780rpm is moving 0.65" per second, 0.075" at 620rpm is 0.775" etc.
//At the max inch rate of 0.25" per rev at 70rpm, that's fallen off to about 0.29" per rev.
//That seems sensible, since the forces go up with the deeper corresponding cut.
//We'll see how it works out in practice.
//
//I experimented with dynamically varying the step period, but it just got too fussy.
//Maximum steps per spindle tick is 11, a carriage movement of 0.00034375" (fine enough for me).
// Period is roughly scaled to feed rate, for smoothing and to mitigate resonances.
// Maximum number of motor steps per spindle count is 11.
// 2Mhz / ((RPM*SCPR)/60) / max_steps
//int period_list[12] { STP_MIN, STP_MIN, 96, 81, 90, 90, 69, 120, 143, 160, 214, 244 };
// 80% of 2Mhz / ((RPM*SCPR)/60) / max_steps

int period_list[12] {
  STP_MIN, STP_MIN, STP_MIN, STP_MIN, STP_MIN, STP_MIN,
  STP_MIN, STP_MIN,      83,     104,     155,     194 };


#endif // __CONFIGURATION_H

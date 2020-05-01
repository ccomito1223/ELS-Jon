# ELS-Jon
Electronic Lead Screw - By  Jon Bryan

This repository is an attempt to document the development and implementation of a microcontroller-
based Electronic Lead Screw specifically intended for a 1946 Logan Model 200 lathe, but the
fundamental design principles have much broader potential applications.

The goal was to implement a design that would provide the user with a nearly-unlimited selection
of inch and metric thread pitches and feeds and obviate the need for change gears, while requiring
limited or no permanent modifications to the lathe.

The resulting system is based on an Arduino Mega 2560 microcontroller, an incremental rotary encoder
for sensing the position of the lathe spindle, and a closed-loop microstepping motor/driver to precisely 
drive the lathe lead screw at the desired rate relative to the rotation of the lathe spindle.

The user interface is provided by a Nextion NX4024K032 3.2" touch screen liquid crystal display, 
a 20 pulse-per-revolution encoder knob, and an optional single-pole, double-throw momentary toggle
switch for a conveniently-located alternative control of feed direction.

An attempt was made to implement every pitch available on a standard Logan lathe quick-change gearbox,
every inch and metric tap and die size available from McMaster-Carr up to 4tpi and 6.5mm, and
every legible pitch shown in photographs of the feed plates on lathes found through internet searches,
as well as a large selection of diametral and module worm gear pitches.  The maximum rates were limited 
for practical reasons, this being a relatively light-duty lathe.

Version 1.0 Features:

Inch feeds and threads from 0.0005" to 0.250" (4tpi)

Metric from 0.01mm to 6.5mm (including standard 0-10 British Association pitches)

Inch Diametral worm gear pitches from 120 to 12

Metric Module worm gear pitches from 0.2 to 2

Programmable left and right feed limits

A manual jogging function for precise positioning of the programmed limits

Display of the lathe spindle RPM

Display of the highest recommended lathe spindle RPM for a selected feed rate

Please visit and subscribe @ https://www.youtube.com/channel/UCw5ORyB1lIvQSxHoaDNEYuA

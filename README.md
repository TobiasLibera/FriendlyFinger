# FriendlyFinger

A lovely stress relief device I designed while having a week off from work.<br></br>


## Description ##

The FriendlyFinger is a machine which rotates a 3D model of a middle finger from one side to the other, while having two LEDs shining a little lightshow to it.\
It's based on an ESP32 Development Board and is designed for remote control via WebServer or UDP.\
You need a 3D printer for building this project as well as you need to be willing to put some effort into it.
For example you will need to solder a little curcuit.<br></br>
I had a lot of fun developing and building the Friendly Finger.\
I do hope you gain something from building it, too.<br></br>

A Video can be found here:\
[Youtube Video](https://youtu.be/8vTFOdNh7Bw)<br></br>
Thingiverse:\
[Printing Data](https://www.thingiverse.com/thing:5678932)<br></br>


## Printing ##

### Before you start the Printing ###

<p><strong>
  There is a "FF_Template_Fits" part, which has all fits in it, that are needed for the assembly.
  The dimensions of the holes are the ones, that I use to get good fits.
  I strongly recommend to print this part at least once to check if all fits are working properly before printing the actual parts.
</strong><br>
</p>

The fits in the ```FF_Template_Fits``` part are:

<ol>
  <li>Fit with Nema17 Stepper motor
    <ol>
      <li>Nominal:    22mm</li>
      <li>My Value:   22.4mm</li>
    </ol>
  </li>
  <li>Fit to screw in a M3 screw directly into the printing part
    <ol>
      <li>Nominal:    3mm</li>
      <li>My Value:   2.93mm</li>
    </ol>
  </li>
  <li>Fit for RGB LED with an 28° angle to the vertical axis
    <ol>
      <li>Nominal:    5mm</li>
      <li>My Value:   5mm</li>
    </ol>
  </li>    
  <li>Countersunk hole for M3 screw (standard: ISO 4762)</li>
</ol>

Feel free to modify the corresponding STEP files to make this fits work properly.


### Cura Settings ###

<p><strong>
  Change the "Overhang Angle" in the Cura Support settings to 46 degrees.
</strong><br>
</p>

There is a BOM for the printing parts in the Documentations directory where you can find the Cura settings I used, how long the print took and how much filament was needed.
As mentioned, that are my settings. I am very sure there are a lot of folks out there who can find better, more effective settings for their prints.<br></br>



## Electronics ##

### Curcuit Board ###

<p><strong>
  I don't own any degrees in electronics!
  So I won't make any further advises on how to design the curcuit itself.
  All I do is giving an example without any warranty for safety and functionality.
</strong><br>
</p>

There is a wiring diagram in the "Documentation" directory, where the basics of the curcuit are shown in the way I planned them for my own work. It's missing headers for the RGB-LEDs as it's missing headers for the Step-Down voltage regulator.
  
  As I like JST headers and Dupoint headers, I used them for my own version.
Feel free to use whatever floats your boat to realize this connections.


### Fit with LM2596 DC-DC Step Down Module ###

There is a need to drill two holes into the breadboard to fit the LM2596 to it.
The ```FF_Washer_Distance``` part is for spacing between the breadboard and the LM2596.
The ```FF_Washer_ScrewIn``` part works as a nut for the M3x8 socket-head screws. It should be also possible to use standard M3 nuts (ISO 4032 | ISO 4033) for this.


### Connection with Base ###

There are screw holes in the ```FF_Base``` part which have a distance of ***71.12mm*** to each other. That's is the exact distance of 29 drillings in a breadboard with a ***2.54mm spacing***, which means, if you use this drillings and widen them to, maybe, 3.5mm, you can use them to connect the curcuit board to the ```FF_Base``` part with 4 screws.
It's designed to work with M3x8 socket head screws (ISO 4762).


### Fit into the Body ###

To fit a 100x100mm Breadboard into the base, there is the need of getting rid of the edges.
Therefor is the ```FF_Template_Edge``` part, which can be hold to each edge of the curcuit board. Then draw a line along the radius and grind away all the material thats behind that line.
That should be sufficient to fit the curcuit board into the ```FF_Body``` part.<br></br>



## Code ##

### Arduino IDE ###

For uploading the Code to the ESP32 Development Board I recommend the Arduino IDE.
You need to have the ESP32 support installed. Look on the World Wide Web for instructions how to do that. It's fairly simple.


### Dependencies ###

The Code has dependencies for some Standard Libraries of the Arduino IDE + ESP32 Standard Libraries.

There is one single additional dependcy. The ```analogWrite.h``` lib, which is designed for ESP32 and published under MIT licence.<br></br>
[Library description on www.arduino.cc](https://www.arduino.cc/reference/en/libraries/esp32-analogwrite/)<br></br>
You can install this library by using the Library Manager of the Arduino IDE.

### Adjustments to the Code ###

There is a pretty big ```#define``` section on top of the code, where you can do some settings.
Necessary is the IP address you want to use for the ESP32 as well as the password to your local network.

You can also define, if you want to control the FriendlyFinger by a webserver hosted on the ESP32. Or if you want the ESP32 as a UDP receiver for controlling. That's the setting I use, cause I control the Friendly Finger with a website in my local network which is hosted on a Raspberry Pi.

The other settings can be set as it's pleasing to you. Feel free to mess around a little.<br></br>



## Assembling the FriendlyFinger ##

*"It's a little bit tricky here and there..."*

### LEDs to Body ###

I won't lie to you. It's really pain in the a** to fit them in. I used WD40 as lube and still had problems to to this. Also you need to lead the cables down to the curcuit board while the stepper motor and the middle finger are already in the assembly.\
But it's possible, so I think you will get this done.

### Axis to Middle Finger ###

You need to glue the ```FF_Axis``` part to the ```FF_MiddleFinger``` part. I used fast drying epoxy glue which did the job quite well.<br></br>


## Credits ##

### Middle Finger ###

The original 3D model for the middle finger itself was desgined by ```Dmitrij Drandarov``` alias ```DahakaMVI```.
The file can be found on [Thingiverse](https://www.thingiverse.com/thing:4923022).

### GrabCAD ###

I used a lot of 3D models from [GrabCAD](https://grabcad.com).
I can absolutely recommend this website for 3D Models for standard parts, especially for electronic parts.

### ESP32 Webserver ###

As I didn't had much experience with webservers hosted on an ESP32, I used a tutorial as a base for my version of the code.
The tutorial I used was the [ESP32 Webserver Tutorial](https://lastminuteengineers.com/creating-esp32-web-server-arduino-ide/).<br></br>



## Donations ##

I want to dedicate this work to organizations which commit themselfs to helping people with mental illness.
Therefor I want to ask you to donate a little tip for one of the following organizations on www.betterplace.org, as long as you feel comfortable with that.\
I do not gain anything from it, and the organizations don't even know, I am sharing their links.
<br></br>
[Irrsinnig Menschlich e.V.](https://www.betterplace.org/de/organisations/2508-irrsinnig-menschlich-e-v)\
Translated Name: *Insanely Humane* (registered association)
<br></br>
[Stiftung Deutsche Depressionshilfe](https://www.betterplace.org/de/organisations/24384-stiftung-deutsche-depressionshilfe)\
Translated Name: *German Depression Help* (foundation)
<br></br>
***Thank you very much!***

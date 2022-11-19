# FriendlyFinger

Stress relief device.



## Electronics ##

### Curcuit Board ###

<strong>
  I don't own any degrees in electronics!
  So I won't make any further advises on how to design the curcuit itself.
  All I do is giving an example without any warranty for safety and functionality.
</strong>

  There is a curcuit layout in the "Documentation" directory, where the basics of the curcuit are shown in the way I planned them for my own work. It's missing headers for the RGB-LEDs as it's missing headers for the Step-Down voltage regulator.
  
  As I like JST headers and Dupoint headers, I used them for my own version.
Feel free to use whatever floats your boat to realize this connections.


### Connection with Base ###

There are screw holes in the ```FF_Base``` part which have a distance of ***71.12mm*** to each, which is the exact distance of 29 drillings in a curcuit with a ***2.54mm spacing***. Which means, if you use this drillings and widen them to, maybe, 3.4mm, you can use them to connect the curcuit board to the ```FF_Base``` part with 4 screws.
It's designed to work with M3x8 screws (ISO 4762).


### Fit into the Body ###

To fit a 100x100mm Breadboard into the base, there is the need of getting rid of the edges.
Therefor is the ```FF_Template_Edge``` part, which can be hold to each edge of the curcuit board. Then draw a line along the radius and grind away all the material thats behind that line.
That should be sufficient to fit the curcuit board into the ```FF_Body``` part.

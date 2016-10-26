# Portal Gun
My attempt at a replica portal gun from the TV show Rick and Morty for my Halloween costume

# Inspiration
I found this design, which appears to be the best available free design:
https://www.thingiverse.com/thing:1100601

# Electronics
I am using the same display as the Thingiverse design, but most of the other parts came from my parts bins.  The full BOM:
* Arduino Mini (eBay)
* Adafruit Alphanumeric display with backpack (Amazon)
* Optical rotary encoder w/pushbutton (scrap bin)
* 10mm Green LEDs w/ shrouds (Amazon)
* Custom Lipo pack - 4x 380mAh cells in 2S2P configuration (scrap bin)
* Salvaged RC 5V BEC (scrap bin)

Measured current with LEDs on and Display at low brightness (still quite bright) is max 75mA.  This means we should have plenty of runtime for the night with the 760mAh battery.

# Mechanical Design
My roommate took the Thingiverse model and recreated it in Solidworks with my components.  It will be posted on Thingiverse at a later time.

Most of the parts are 3D printed, with a few exceptions:
* Encoder knob - I bought a nice metal one from Amazon.  Cheaper than a whole roll of black filament!
* "Lightning" tube - I bought a set of large plastic test tubes meant for kids.  They're clear, so not the green of the actual portal gun, but it will look much nicer than a 3D printed version (and, once again, cheaper than a whole roll of transparent green filament...)

# Remaining Work
* Code
    * Optimize portal LED sequence to fade out at the very least, possibly fade in?
    * Add flashing routine for "lightning" tube LED (RNG to analog write?)
    * Add sleep functionality in for long-press of rotary encoder button
* Documentation
    * Could use some links for the items I bought
    * Schematic

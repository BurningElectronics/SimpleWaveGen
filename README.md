# SimpleWaveGen
A simple waveform generator using Arduino nano and a few more components

The components used are:

1. Arduino nano
2. SSD1306 32x128 OLED display
3. AD9833 waveform generator module
4. 2x Screw terminal (2P 5.0mm)
5. RL207 diode, you can use anyone you want, as long as it fits
6. SS12F44G4 Switch
7. 50V 100uF Capacitor (D8.0mm, P3.50mm), symbolic value
8. 3x 2-Pin Pushbuttons
9. Some female headers
10. 8x 10k 1/4W resistors, you can use any reasonable value for pull-up/down purposes

<br/>
<br/>
<br/>

How does it work?

The arduino nano sends the waveform type and frequency via SPI to the AD9833, that generates the requested waveform.

While doing this, the OLED display (SSD1306) is updated by the arduino via I2C, with dsplayed frequency, waveform type and digit position.

To set the frequency an encoder it's used.

To navigate between digits two pushbuttons, LEFT (<-) and RIGHT (->), are used.

To change Waveform type a third pushbutton (MODE) it's used.

To reset the digit press the encoder button.

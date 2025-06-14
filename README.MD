
# Dostihy a sázky
**FYI this is supposed to be for a school project**

* Works on an Arduino Mega _(hopefully)_.

A silly game where you watch LED arrays, and whichever array lights up first, wins! _(Inspired by literal horse racing, don't ask me how **we** got this idea...)_
I could've used a LED matrix instead of this, but I remember that I was trying to learn how to use one and it left me confused until this day.
Hopefully I'll be able to assemble it all toghether in school and nothing blows up in my face...

This was made in like **2 and a half hours**. It did take a mental toll on me because of the troubleshooting, but overall I think I did well with this project.
I used Wokwi for diagram ilustration and simulation, because as of now, I don't have the necessarry parts for the project to make it in real life. If you have the Wokwi extension on VS Code, you'll be able to run this. I also used PlatformIO IDE because I find it easy to manage the projects, and because it can be integrated right into VS Code. _(Tbh I don't know, I just find it easy to import libraries with it, and also was necessarry for me to make Wokwi run.)_

## How to start the game?

_(Assuming you've connected & assembled everything...)_
Hook up the Arduino to a PC _(preferably, not necessarry)_, and wait for the I2C display to light up. Once the backlight is on and the text "READY" appears, press the button that is connected to pin 51, and the race should start. Alternatively, you can press any key while the serial monitor is in focus.

## Can I use this in my project?

**Yes, you can. Honestly, I don't really care if you don't credit me.**
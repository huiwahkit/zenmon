## zenmon

### Introduction
`zenmon` is a top-like CLI monitoring tool for AMD Ryzen CPUs written in C. It uses `zenpower` to display the electrical and temperature statistics of the CPU in a way that is pleasing to the eye, at least for me.

This started as a personal project after I was left unsatisfied with the tools available for monitoring voltages and temperatures on the Ryzen 3000 CPUs.

Late in the development process, after it started looking like an actual top-like tool, I have decided that I might as well make it public. Take it as is, it's not intended to be the end-all system monitor, just a tailored solution for myself.

At some point I would like to fill the empty, unused, terminal space with:
* CPU load
* CPU frequency
* RAM usage
* maybe GPU information
* maybe storage information, but the I am interested in the more dynamic statistics

---
### Preview
<p align="center">
<img src=./res/zenmon.gif width="800" height="480">
</p>

* The CCDs (Core Chiplet Dies) have been renamed to CDs (Core Dies) for better text alignment
* The voltage is measured in milivolts, but the symbol is kept as V, again, for better text alignment
* This preview is running at 100ms/sample.
* In the graphs labels, after the symbol, the white number is the value of a dot and the red one is the value at which the graph turns red.

---
### Buliding
#### Dependencies
* GNU `make`
* `zenpower` (https://github.com/ocerman/zenpower)
    * without this, `k10temp` only exposes the die and ctl temperatures
* A Ryzen CPU (see the section below)
* `ncurses` is **not** needed; I did the cursor jumping myself

#### Compatible CPUs
* Supported out-of-the-box:
    * AMD Ryzen 9 3950X
    * AMD Ryzen 9 3900X
* Supported after changing (but mostly removing) some code:
    * Any other Ryzen CPU that `zenpower` supports

#### More on the topic of compatible CPUs
* It all depends on the files `zenpower` exposes; for the supported CPUs the files are as follows:
    * core voltage: in1_input
    * core current: curr1_input
    * core power: power1_input
    * soc voltage: in2_input
    * soc current: curr2_input
    * soc power: power2_input
    * die temperature: temp1_input
    * ctl temperature: temp2_input
    * ccd1 temperature: temp3_input
    * ccd2 temperature: temp4_input
* In case of other Ryzen CPUs, whatever `zenpower` decides to expose can be read by this program
* The only Ryzen CPU I have access to is my 3950X, so this is what I can test it on

#### Modifications needed for unsupported CPUs
* Remove (or add if you have a Threadripper supported by `zenpower`) the extra file(s) handling. `zenmon-cfg.h` and `zenmon-types.h` are good starting points; go top-down from there. I am talking about defines and structure members
* Change the window sizes to better fit the new content. `zenmon-cfg.h` has everything you need for this
* Remove unused labels from the `SCI2` sensor window from `zenmon-box.c`

#### Steps
> make

---
### Usage
Just call it and it will run with a sample rate of 1 second. Otherwise, use it like this:
> ./zenmon -r \<sample rate in ms\>

---
### Limitations
I might get to these at some point. It depends on how much I'll actually use this and the free time I'll have.
* As mentioned above, the supported CPU list barely exceedes 1 item and is not auto-detected
* The terminal window needs to be big enough; it does not dynamically resize itself
* Average values calculations start after 10 samples (this is an easy fix, but it's intentional)
* There are more I have thought of over the time, but they escape me right now

---
### License
Copyright © 2020 Denis Isai

Licensed under the GPLv3: http://www.gnu.org/licenses/gpl-3.0.html

---
### Credits
The Braille character handling from braille-lib.h and braille-lib.c have been adapted from https://github.com/dcat/tplot

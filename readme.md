## zenmon

### Introduction
`zenmon` is a top-like CLI monitoring tool for AMD Ryzen CPUs written in C. It uses `zenpower` to display the electrical and temperature statistics of the CPU in a way that is pleasing to the eye, at least for me. It also displays generic information related to the overall system load:
* Uptime
* Number of processes
* CPU usage for the past 1, 5 and 15 minutes
* RAM usage
* CPU thread usage
* CPU core MHz

This started as a personal project after I was left unsatisfied with the tools available for monitoring voltages and temperatures on the Ryzen 3000 CPUs.

Late in the development process, after it started looking like an actual top-like tool, I have decided that I might as well make it public. Take it as is, it's not intended to be the end-all system monitor, just a tailored solution for myself.

At this point I would consider it feature-complete. I will certainly maintain/improve its stability/quality, but additional features are not on my roadmap, at least for now.

---
### Preview
<p align="center">
<img src=./res/zenmon.gif width="800" height="440">
</p>

* This preview is running at 100ms/sample
* For the display I use centivolts instead of millivolts for better text alignment; in the code they are millivolts, and the variables associated with them are as named as volts
* The labeling for the usage and MHz bars are a bit messy, but that's the best way I have found to make them work
* The RAM usage bar is there, and it gets red at 50% load; it's just that rendering in Blender to get the other stats to rise and fall doesn't use that much RAM to get that bar going
* For each Braille graph, the scales, value of each dot and the point at which is turns red can be found in the `cfg` directory. By default they are set to something reasonable, that also works well with the limitations imposed by working with 2x4 "pixels" Braille characters.
* The minimum frequency that I get with my setup is 2.2 GHz; this is the reason the frequency bars stay high

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
    * AMD Ryzen 9 3900X (for the most part; see below)
* Supported after changing some code:
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
    * ccd0 temperature: temp3_input
    * ccd1 temperature: temp4_input
* In case of other Ryzen CPUs, whatever `zenpower` decides to expose can be read by this program
* At run-time, all the required system files are checked; if any are missing, the program will notify and exit gracefully
* The only Ryzen CPU I have access to is my 3950X, so this is what I can test it on

#### Modifications needed for unsupported CPUs
* In case of the 3900X, the MHz and usage bars for the last 4 cores will contain invalid values; otherwise it works as intended. The core/thread config can be changed in `zenmon-cfg-load.h` and the windows can be re-structured in `zenmon-num-load.c`
* For other CPUs:
    * remove (or add if you have a Threadripper supported by `zenpower`) the extra file(s) handling
    * the `cfg` directory and `zenmon-types.h` are good starting points; go top-down from there; I am talking about defines (especially for the system files from where I acquire various data) and structure members
    * then see my previous point about the 3900X
* Restructure the content of the windows; see the `zenmon-box`, `zenmon-num` and `zenmon-dot` modules' APIs
* In the `doc` directory there is a basic block diagram of the app structure that might help

#### Steps
> make

---
### Usage
Just call it from the newly-created `out` directory and it will run with a sample rate of 1 second. Otherwise, use one of these:
> ./zenmon -h
>
> ./zenmon -r \<sample rate in ms\>

---
### Issues
* The terminal window stays renamed to `zenmon` even after exiting the program; I would need to save the current name before changing it, but from what I understand, the way a terminal is named varies from one terminal emulator to another (`$TERM`, for example, is useless for `Alacritty`)

---
### Limitations
I might get to these at some point:
* As mentioned above, the supported CPU list barely exceedes 1 item and is not auto-detected
* Currently `zenpower` only reports half of the current and power being used by the processor; `zenmon` fixes this by doubling the acquired amount, but if `zenpower` gets fixed, this will need to be updated (in `zenmon-num-svi2.c`)
* Used memory is calculated as (Total - Free - Buffers - Cached), which also means:
    * `tmpfs` usage is not taken into account
    * I did not get the memory info from `sysinfo` because it has no data on cached RAM
    * other monitoring tools might report slightly different values, but still relatively close

The next are all intentional, mostly because of the `KISS principle` (_and because I am lazy..._ but seriously, I did try to keep things as simple as possible; I guess you will be the judge of that):
* Average values calculations start after 10 samples; until then they are 0
* In the first polling cycle, CPU thread usage is calculated since startup; this is because of the way `/proc/stat` works

---
### License
Copyright © 2020 Denis Isai

Licensed under the GPLv3: http://www.gnu.org/licenses/gpl-3.0.html

---
### Credits
* The Braille character handling from braille-lib.h and braille-lib.c have been adapted from https://github.com/dcat/tplot

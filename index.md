---
title: '吉米匠作 JimmyCraft Projects'
---

JimmyCraft ![](https://i.imgur.com/xJBHLFT.jpg =64x64)

===
 [![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)        [![Build Status](https://travis-ci.com/jmysu/R3Esp32Grbl.svg?branch=master)](https://travis-ci.com/jmysu/R3Esp32Grbl)

![issues](https://img.shields.io/github/issues/jmysu/R3Esp32Grbl.svg) ![cmmit](https://img.shields.io/github/last-commit/jmysu/R3Esp32Grbl.svg) ![ver](https://img.shields.io/github/manifest-json/v/jmysu/R3Esp32Grbl.svg) [![Gitter](https://badges.gitter.im/吉米匠作/community.svg)](https://gitter.im/吉米匠作/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)
<br>

#### Table of Contents
##### [TOC]

---
### About 

JimmyCraft Open Source Projects!
> * Mega32Grbl :zap: https://github.com/jmysu/R3Esp32Grbl
    >ESP32(r) Grbl Board w/ Arduino(r) form factor.![](https://i.imgur.com/FJrdVFN.png =300x200)
>
> * Mega32Proto :zap: https://github.com/jmysu/R3Esp32Grbl
    >ESP32(r) Proto Shield w/ Arduino(r) form factor.
>
    
> * GRBL :zap: https://github.com/jmysu/R3Esp32Grbl/tree/master/GRBL <br>
    >Add GRBL1.1 setttings.![](https://i.imgur.com/xyaw2NE.png =200x240)
 
> * Mega32 WeatherClock :zap: https://github.com/jmysu/R3Esp32Grbl/tree/master/Firmware/WeatherClock <br>
>Add NTP DarkSky WeatherClock! <img src="https://i.imgur.com/35qJUVX.gif" width="100" height="50" /><br>
>SSL to link with DarkSky Weather data(realtime ＋ two days forecast).
And timer corrected with NTP!

> * Mega32 WebSocket :zap: https://github.com/jmysu/R3Esp32Grbl/tree/master/Firmware/WebSocket <br>
>Add WebSocket for sending binary/text/Json![](https://i.imgur.com/aSz4Vmo.png =260x200)

> * Modbus Master App :zap: https://github.com/jmysu/R3Esp32Grbl/tree/master/Software <br>
> Add Qt ModbusMaster App for accessing hardware! <br>
> (Coil/HoldingRegister only) ![](https://i.imgur.com/y9BHWFH.png =320x200)


---
### News :newspaper: 
<details><summary>2019-07-04, Github Page website hosted.
  </summary>
  
>Both Mega32Grbl and Mega32Shield were hosted on Github-Page: https://jmysu.github.io/Mega32
>
>MK12 vapourware archived: https://github.com/jmysu/MK12
</details>
  
---
### ToDO List :heavy_check_mark: 
- [x] PCB Design/Layout
- [x] PCB Assembly
- [ ] PCBA Test
- [ ] Firmware integration (ESP32-Grbl tailoring)
- [ ] Documetation
...


---
### Project Timeline :alarm_clock: 
```mermaid
gantt
axisFormat  %m/%d
    title  Mega32 時程規劃

    section Hardware
    Mega32 PCB Design/Layout     :a1, 19-06-01, 45d
    PCB Debug                    :a2, after a1, 30d
    PCB 2nd RUN                  :after a2, 15d
    
    section Firmware
    ESP32/PlatformIO     :19-07-01, 45d
    ESP32/WebUI          :19-07-01, 60d
    
    section Doc
    User Guide           :after a1, 30d
```
```Read more about mermaid here: http://knsv.github.io/mermaid/

```
> I choose a lazy person to do a hard job. Because a lazy person will find an easy way to do it. [name=Bill Gates] 
> 我找了個懶人做硬活. 因為懶人會搞軟活.[name=比爾 大門口]
> :laughing: :100: 


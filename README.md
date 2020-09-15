# Automated Pump Controller

Developed for Joliet Junior College Natural Science Research Group. Ultra-efficient battery powered pump for dispersing amino acid based feed, driven by relay via AtMega 328. Utilizes DS3231 RTC with variable alarms and an SD card reader for logging every interval. 

![image](https://user-images.githubusercontent.com/38547951/93243584-edbd5a80-f777-11ea-8cbc-bb3355933b32.jpg)

# Use Case

We used this pump to disperse a specially formulated amino acid based feed at consistent, known intervals. We hydraulically pressed amino acids and other feed into pucks that we placed in the loading chamber of a chlorine float, then positioned the pump controller on top with a small aquarium pump facing downwards towards the feed. As the pump turned on and off, it would pump filtered water onto the feed, resulting in feed emanating from the device at a constant rate. This was used for a federal research study, hosted by the USGS, to test the efficacy of using amino acid based feed to attract invasive asain carp, with the hopes that the rates which they are caught would rise.


## Behavior

The pump controller works in a very straightforward way. The controller turns on, checks to see if it needs to turn the pump on or off, turns the pump on or off by flipping a latching relay, writing a status log to the sd card, checks the next time that it needs to turn the pump on or off, then puts itself in deep sleep mode after instructing the RTC to wake it up when necessary. 

![image](https://user-images.githubusercontent.com/38547951/93249645-2ada1a80-f781-11ea-84ca-81a6743b44e5.gif)

## Setting Intervals

I was the lead on this project and my advisors had never been so much as introduced to the arduino environment, so I realized the necessity of creating an intuitive interface. I did this using python to create a GUI that would save the desired data to an sd card. This sd card was then inserted into the pump controller and would be parsed automatically.

![image](https://user-images.githubusercontent.com/38547951/93251665-02075480-f784-11ea-9c2f-0ea96af058d9.png)

## Related Pictures

### Feed pucks
![image](https://user-images.githubusercontent.com/38547951/93249678-34fc1900-f781-11ea-9587-1ac7de300f6d.jpg)

### Pump apparatuses deployed on testing pond
![image](https://user-images.githubusercontent.com/38547951/93249692-39c0cd00-f781-11ea-8c5d-b2c6c498796c.jpg)

### A picture of the sonar software used to gather data
![image](https://user-images.githubusercontent.com/38547951/93249704-3d545400-f781-11ea-9d3d-c2636211c383.jpg)

Aside: I had no experience with electronics nor arduino when starting this, but 6 months of obessive toiling left me with a proficent understanding of arduino and enough electronics knowledge to build most practical devices that I desire. While I enjoyed my time with embedded devices, my intrests have moved to data science, specifically machine/deep learning. I crunched whatever data I could get my hands on with the project usuing python to analyze and plot.






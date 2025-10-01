# VATATS
Virtual Aircraft Tracking And Toolbox System<br>
App made for [**Summer of making**](https://summer.hackclub.com/).<br>
VATATS contains useful features for MSFS flight simmers like:
- Flight Tracking
- Charts
- CPDLC
- VATSIM network map

## How to use?
1) Download the newest [release](https://github.com/WiktorKociuba/VATATS/releases)
2) Run **VATATS.exe**

## Module overview:
### Flight tracking:
![Tracking Demo](https://github.com/WiktorKociuba/VATATS/blob/main/readmeFiles/demo.gif)
To track your flghts, you either need to run msfs locally, or run it on a different machine (you can findinstructions on how to setup Simconnect over internet in the "settings" tab).<br>
To start tracking the flight you need to click "Start Tracking" and to end it "Stop Tracking".<br>
From the dropdown you can select saves, that can be displayed with clicking "Show Last Flight" after selection.<br>
To clear the map use "Clear Map".<br>
Map shows current flight or one of the loaded saves.<br>
Chart below the map shows altitude AGL.<br>
### Charts:
**To use this module, you first need to authorize with ChartFox from settings (just click the button and login)**<br>
![Charts demo](https://github.com/WiktorKociuba/VATATS/blob/main/readmeFiles/charts.png)
On the top there is an input line, fill it in with ICAO airport code and click search - if the charts are available, the list will show up on the left.<br>
To display a chart, click on it from the list and you can view it on the right (you can also zoom in and out)<br>
### CPDLC:
**To use CPDLC on VATSIM network, you need to get a Hoppie secret (register [here](https://www.hoppie.nl/acars/system/register.html)), you also need to put your VATSIM CID. You can fill it in the settings**<br>
![CPDLC Demo](https://github.com/WiktorKociuba/VATATS/blob/main/readmeFiles/cpdlc.png)
Connect/Disconnect from Hoppie using corresponding buttons.<br>
Callsign will show you your current callsign, current station - the station that you are currently connected to.<br>
Click Predep to fill in the predeparture clearance (PDC).<br>
Click Request logon, to connect to specified station.<br>
Click logoff to disconnect from station.<br>
To send a message: choose the request, fill in the fields and click send.<br>
Received messages will display in the "Messages" box.<br>
Available CPDLC requests:
| Category | Request |
| --- | --- |
| CLIMB | Request climb to __ |
| CLIMB | At __ request climb to __ |
| DESCENT | Request descent to __ |
| DESCENT | At __ request descent to __ |
| FREE | Free text |
| OFFSET | Request offset \[direction\] \[offset\] of route |
| OFFSET | At __ request offset \[direction\] \[offset\] of route |
| SPEED | Request \[speed\] |
| VOICE CONTACT | Request voice contact |
| ROUTE MODIFICATION | Request direct to __ |
| ROUTE MODIFICATION | Request \[procedure name\] |
| ROUTE MODIFICATION | Request weather deviation up to \[direction\]\[distance offset\] of route |
| ROUTE MODIFICATION | Request weather deviation up to __ via __ |
| ROUTE MODIFICATION | Request heading __ |
| ROUTE MODIFICATION | Request ground track __ |
| NEGOTIATION | When can we expect __ |
| NEGOTIATION | When can we expect back on route |
| NEGOTIATION | When can we expect lower altitude |
| NEGOTIATION | When can we expect higher altitude |
| NEGOTIATION | When can we expect cruise climb to __ |
| ADITIONAL MESSAGES | We can accept __ at __ |
| ADITIONAL MESSAGES | We cannot accept __ |
| ADITIONAL MESSAGES | When can we expect climb to __ |
| ADITIONAL MESSAGES | When can we expect descent to __ |

Available TELEX messages:
- Free text

Available Info requests (click Inforeq, fill the ICAO and click send on desired position):<br>
- METAR
- TAF
- SHORTTAF
- VATSIM ATIS
###  VATSIM map:
It is a map of current live data from VATSIM network.<br>
To get aircraft callsign, you can hover on the icon. To get more data (including saved route), click on it.<br>
![Plane Demo](https://github.com/WiktorKociuba/VATATS/blob/main/readmeFiles/plane.png)<br>
CTR/FSS active areas are displayed with blue polygons. To view more data, hover on the label.<br>
![CTR Demo](https://github.com/WiktorKociuba/VATATS/blob/main/readmeFiles/fir.png)<br>
Active APP/DEP areas are shown as a circle. Hover on the label to get more info.<br>
![APP Demo](https://github.com/WiktorKociuba/VATATS/blob/main/readmeFiles/app.png)<br>
Active DEL/GND/TWR/ATIS positions are displayed with labels above the airports, hover on the label to get more data.<br>
![ATIS Demo](https://github.com/WiktorKociuba/VATATS/blob/main/readmeFiles/atis.png)<br>
### Settings:
Use it to provide:
- Simconnect IP and port
- Hoppie secret
- VATSIM CID
- ChartFox account
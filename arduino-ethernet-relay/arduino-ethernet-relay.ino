/*
* Rui Santos
* Complete Project Details https://randomnerdtutorials.com
*/

#include <SPI.h>
// The Ethernet library isn't compatible with the ENC28J60 (ref: https://forum.arduino.cc/index.php?topic=576358.0)
// Use UPIEthernet instead
//#include <Ethernet.h>
#include <UIPEthernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,12,115);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

// Relay state and pin
String relay1State = "Off";
const int relay = 2;

// Power state pin
int pwrState = 4;
int pwrStateVal = 0;

// Client variables
char linebuf[80];
int charcount=0;

void setup() {
// Relay module prepared
pinMode(relay, OUTPUT);
pinMode(pwrState, INPUT);
digitalWrite(relay, HIGH);
// Open serial communication at a baud rate of 9600
Serial.begin(9600);
// start the Ethernet connection and the server:
Ethernet.begin(mac, ip);
server.begin();
Serial.print(F("server is at "));
Serial.println(Ethernet.localIP());
}

// Display dashboard page with on/off button for relay
// It also print Temperature in C and F
void dashboardPage(EthernetClient &client) {
client.println(F("<!DOCTYPE HTML><html><head>"));
client.println(F("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><body>"));
client.println(F("<h3>Arduino Web Server - <a href=\"/\">Refresh</a></h3>"));
// Generates buttons to control the relay
client.println("<h4>Relay 1 - State: " + relay1State + "</h4>");
//Prints the logic state of input pin 4
client.print(F("<h4>Power state pin: "));
client.print(pwrStateVal);
client.println(F("</h4>"));
// If relay is off, it shows the button to turn the output on
if(relay1State == "Off"){
client.println(F("<a href=\"/relay1on\"><button>ON</button></a>"));
}
// If relay is on, it shows the button to turn the output off
else if(relay1State == "On"){
client.println(F("<a href=\"/relay1off\"><button>OFF</button></a>"));
}
client.println(F("<a href=\"relay1pulse05\"><button>Pulse 0.5s</button></a>"));
client.println(F("<a href=\"relay1pulse5\"><button>Pulse 5s</button></a>"));
client.println(F("</body></html>"));
}


void loop() {
// listen for incoming clients
EthernetClient client = server.available();
if (client) {
Serial.println(F("new client"));
memset(linebuf,0,sizeof(linebuf));
charcount=0;
// an http request ends with a blank line
boolean currentLineIsBlank = true;
while (client.connected()) {
if (client.available()) {
char c = client.read();
//read char by char HTTP request
linebuf[charcount]=c;
if (charcount<sizeof(linebuf)-1) charcount++;
// if you've gotten to the end of the line (received a newline
// character) and the line is blank, the http request has ended,
// so you can send a reply
if (c == '\n' && currentLineIsBlank) {
  pwrStateVal = digitalRead(pwrState);
  dashboardPage(client);
  break;
}
if (c == '\n') {
if (strstr(linebuf,"GET /relay1off") > 0){
digitalWrite(relay, HIGH);
relay1State = "Off";
}
else if (strstr(linebuf,"GET /relay1on") > 0){
digitalWrite(relay, LOW);
relay1State = "On";
}
else if (strstr(linebuf,"GET /relay1pulse05") > 0){
digitalWrite(relay, LOW);
relay1State = "On";
delay(500);
digitalWrite(relay, HIGH);
relay1State = "Off";
}
else if (strstr(linebuf,"GET /relay1pulse5") > 0){
digitalWrite(relay, LOW);
relay1State = "On";
delay(5000);
digitalWrite(relay, HIGH);
relay1State = "Off";
}
// you're starting a new line
currentLineIsBlank = true;
memset(linebuf,0,sizeof(linebuf));
charcount=0;
}
else if (c != '\r') {
// you've gotten a character on the current line
currentLineIsBlank = false;
}
}
}
// give the web browser time to receive the data
delay(1);
// close the connection:
client.stop();
Serial.println(F("client disonnected"));
}
}

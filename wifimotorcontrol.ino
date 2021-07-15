// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "MakerspaceCT_2G";
const char* password = "makestuff";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliary variables to store the current output state
String rightMotorState = "off";
String leftMotorState = "off";

/* Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;*/

// Motor 1 - Right Motor
int RMP1 = 16; 
int RMP2 = 17; 
int enable1Pin = 4;

// Motor 2 - Left Motor
int LMP1 = 5; 
int LMP2 = 18; 
int enable2Pin = 19;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);

  // sets the pins as outputs:
  pinMode(RMP1, OUTPUT);
  pinMode(RMP2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);

  // sets the pins as outputs:
  pinMode(LMP1, OUTPUT);
  pinMode(LMP2, OUTPUT);
  pinMode(enable2Pin, OUTPUT);
  
  // Set outputs to LOW so motors are stopped
  digitalWrite(RMP1, LOW);
  digitalWrite(RMP2, LOW);
  digitalWrite(LMP1, LOW);
  digitalWrite(LMP2, LOW);


  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            // If RMP1 is on and RMP2 is off go forward
            // If RMP1 is off and RMP2 is on go backward
            // If LMP1 is on and LMP2 is off go forward
            // If LMP1 is off and LMP2 is on go backward
            // If both pins are off, stop.

            //Go Forward LM1=HIGH LM2=LOW RM1=HIGH RM2=LOW

            //Go Backward LM1=LOW LM2=HIGH RM1=LOW RM2=HIGH

            //turn right LM1=HIGH LM2=LOW RM1=LOW RM2=HIGH

            //turn left LM1=LOW LM2=HIGH RM1=LOW RM2=HIGH

            //stop LM1=LOW LM2=LOW RM1=LOW RM2=LOW


            if (header.indexOf("GET /forward") >= 0) {
              Serial.println("Forward");
              rightMotorState = "forward";
              leftMotorState = "forward";
              digitalWrite(RMP1, HIGH);
              digitalWrite(RMP2, LOW);
              digitalWrite(LMP1, HIGH);
              digitalWrite(LMP2, LOW);
            } else if (header.indexOf("GET /backward") >= 0) {
              Serial.println("Backward");
              rightMotorState = "backward";
              leftMotorState = "backward";
              digitalWrite(RMP1, LOW);
              digitalWrite(RMP2, HIGH);
              digitalWrite(LMP1, LOW);
              digitalWrite(LMP2, HIGH);
            } else if (header.indexOf("GET /turnright") >= 0) {
              Serial.println("Left motor forward, right motor backward");
              rightMotorState = "backward";
              leftMotorState = "forward";
              digitalWrite(RMP1, HIGH);
              digitalWrite(RMP2, LOW);
              digitalWrite(LMP1, LOW);
              digitalWrite(LMP2, HIGH);
            } else if (header.indexOf("GET /turnleft") >= 0) {
              Serial.println("left motor backward, right motor forward");
              rightMotorState = "forward";
              leftMotorState = "backward";
              digitalWrite(RMP1, LOW);
              digitalWrite(RMP2, HIGH);
              digitalWrite(LMP1, HIGH);
              digitalWrite(LMP2, LOW);
            } else if (header.indexOf("GET /stop") >= 0) {
              Serial.println("stop both");
              digitalWrite(RMP1, LOW);
              digitalWrite(RMP2, LOW);
              digitalWrite(LMP1, LOW);
              digitalWrite(LMP2, LOW);
              rightMotorState = "off";
              leftMotorState = "off";
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>State of Right Motor: " + rightMotorState + "</p>");
            client.println("<p>State of Left Motor: " + leftMotorState + "</p>");
            // If the output26State is off, it displays the ON button       

            client.println("<p><a href=\"/forward\"><button class=\"button\">FORWARD</button></a></p>");

            client.println("<p><a href=\"/backward\"><button class=\"button\">BACKWARD</button></a></p>");

            client.println("<p><a href=\"/turnright\"><button class=\"button\">TURN RIGHT</button></a></p>");
            
            client.println("<p><a href=\"/turnleft\"><button class=\"button\">TURN LEFT</button></a></p>");

            client.println("<p><a href=\"/stop\"><button class=\"button\">STOP</button></a></p>");
            
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

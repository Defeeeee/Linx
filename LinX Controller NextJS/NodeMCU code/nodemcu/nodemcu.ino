#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* apSSID = "LinX";
const char* apPassword = "linxrobot";

ESP8266WebServer server(80);
String movement = "Stop";
bool ledState = false;

String getHTMLContent() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><title>Robot Control</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>"; 
  html += "</head><body>";
  html += "Access Point Information will be displayed here (You can add this to your nextjs app instead):<br>";
  html += "SSID: " + String(apSSID) + "<br>";
  html += "Password: " + String(apPassword) + "<br>";
  html += "</body></html>";
  return html;
}

void handleRoot() {
  server.sendHeader("Access-Control-Allow-Origin", "*"); 
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST");
  if (server.args() > 0) { 
    if (server.hasArg("direction")) {
      String newMovement = server.arg("direction");
      if (newMovement != movement) { // Only update if the movement has changed
        movement = newMovement;
        Serial.println(movement);  
      }
    }
  }
  server.send(200, "text/html", getHTMLContent());
}

void handleLEDToggle() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST");
  ledState = !ledState;   
  Serial.println(ledState ? "LED_ON" : "LED_OFF");
  digitalWrite(2, ledState);
  server.send(200, "application/json", "{\"state\":\"" + String(ledState ? "ON" : "OFF") + "\"}"); 
}

// Handle LED state requests
void handleLEDState() {
  server.send(200, "application/json", "{\"state\":\"" + String(ledState ? "ON" : "OFF") + "\"}");
}

// Handle OPTIONS requests for preflight checks
void handleOptions() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type"); 
  server.send(204); 
}

void setup() {
  Serial.begin(9600); 
  pinMode(2, OUTPUT);

  WiFi.softAP(apSSID, apPassword);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  Serial.print("AP SSID: ");
  Serial.println(apSSID);
  Serial.print("AP Password: ");
  Serial.println(apPassword);

  server.on("/", handleRoot); 
  server.on("/toggleLED", HTTP_POST, handleLEDToggle).on("/toggleLED", HTTP_OPTIONS, handleOptions); 
  server.on("/ledState", handleLEDState);  
  server.begin();
  Serial.println("HTTP server started");

}

void loop() {
  server.handleClient();
}


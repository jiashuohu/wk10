#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
const char* ssid = "IOT_LAB";
const char* password = "password";
String website;
WebServer server(80);

void buildWeb() {
  website += "<html>";
  website += "<head>\n";
  website += "<meta charset='UTF-8'/>\n";
  website += "<script src=\"https://code.jquery.com/jquery-2.2.4.min.js\"></script>\n";
  website += "<script>";
  website += "setInterval(requestData, 500);";
  website += " function requestData(){";
  website += "$.get(\"/sensors\")\n";
  website += ".done(function(data){ ";
  website += " if(data){";
  website += " $(\"#resis\").text(data.vr);}\n";
  website += "else {";
  website += " $(\"#resis\").text(\"?\");";
  website += " }\n";
  website += "})\n";
  website += ".fail(function(){";
  website += "console.log(\"fail\");";
  website += " });\n";
  website += "}\n";
  website += "</script>\n";
  website += " <title>VR Reader</title>\n";
  website += "</head>\n";
  website += "<body>";
  website += "<div class=\"container\">";
  website += "<p>Variable Resistor = <span id=\"resis\"></span></p>";
  website += "</div>\n";
  website += "</body>";
  website += "</html>";
 }


void handleRoot() {
  server.send(200, "text/html", website);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void HSD() {
  int sensor=analogRead(A0);
  String json = "{\"vr\":";
  json += sensor;
  json +="}";
  server.send(200, "application/json", json);
  }
  
void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  // Wait for connection
  buildWeb();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/sensors", HSD);
  
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}

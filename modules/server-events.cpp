#include <Preferences.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer http_server(5000);
AsyncEventSource left_event_source("/left");
AsyncEventSource right_event_source("/right");

void configure_wifi()
{
    const char *wifi_name = "";
    const char *wifi_password = "";

    WiFi.begin(wifi_name, wifi_password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }

    Serial.println(WiFi.localIP());
}

void server_root_response(AsyncWebServerRequest *request)
{
    request->send_P(200, "application/json", "{\"message\": \"hello world!\"");
}

void configure_server()
{
    http_server.on("/", HTTP_GET, server_root_response);
    http_server.addHandler(&left_event_source);
    http_server.addHandler(&left_event_source);
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    http_server.begin();
}

void send_server_event(AsyncEventSource event_source, int scan_sample)
{
    event_source.send(String(scan_sample).c_str());
}

//
// Created by Avi Zrachya on 02/12/2016.
//


#include "WebServer.h"
#include "Event.h"
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <FS.h>
#include "Configuration.h"


WebServer::WebServer() : server(80) {
    Log.notice("WebServer Created\n");
    Event::subscribe("WebServer::onSetup", ON_SETUP, std::bind(&WebServer::onSetup, this, std::placeholders::_1, std::placeholders::_2));
    Event::subscribe("WebServer::onLoop", ON_LOOP, std::bind(&WebServer::onLoop, this, std::placeholders::_1, std::placeholders::_2));
}

void WebServer::onSetup(int eventId, void *data) {
    server.on("/var", HTTP_POST, [](){
        WebServer::getInstance().onHandleSetVar();
    });

    server.on("/key/toggle/1", HTTP_POST, [](){
                Event::publish(ON_KEY_TOGGLE, (void *)"1");
    });

    server.serveStatic("/index.htm", SPIFFS, "/index.htm");
    server.serveStatic("/fonts", SPIFFS, "/fonts", "max-age=86400");
    server.serveStatic("/js", SPIFFS, "/js");
    server.serveStatic("/css", SPIFFS, "/css", "max-age=86400");
    server.serveStatic("/images", SPIFFS, "/images", "max-age=86400");
    server.serveStatic("/", SPIFFS, "/index.htm");

    server.onNotFound([]() {
        WebServer::getInstance().onHandleNotFound();
    });
    server.begin();
}

void WebServer::onLoop(int eventId, void *data) {
    server.handleClient();
}

void WebServer::onHandleSetVar() {
    String name = server.arg("name");
    String value = server.arg("value");
    Log.notice("onHandleSetVar [name:%s] [value:%s]\n", name.c_str(), value.c_str());

    Config.fields.wifi_password.setValue("dada");
    Config.save();
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();

   // JsonObject& data = json.createNestedObject("data");



    String responsJson;
    json.printTo(responsJson);

    server.send ( 200, "text/json", responsJson);
}


void WebServer::onHandleNotFound() {
    Log.notice("onHandleNotFound called\n");
    server.send ( 404, "text/plain", "not found" );
}






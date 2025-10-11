#pragma once

#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include "log.h"
#include "config.h"
#include "controllers.h"

#define LOG_TAG "webserver"

class BBRXWebServer {
private:
    WebServer server;
    bool is_running;

public:
    BBRXWebServer(int port = 80);
    void begin();
    void handle();
    void stop();
    bool isRunning() { return is_running; }

private:
    void handleRoot();
    void handleUpload();
    void handleDownload();
    void handleEdit();
    void handleSave();
    void handleDelete();
    void handleList();
    void handleNotFound();
    String getContentType(String filename);
};
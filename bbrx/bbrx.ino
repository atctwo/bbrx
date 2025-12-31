// #include <cstdint>
#include <Arduino.h>
#include "controllers.h"
#include "event_manager.h"
#include "status_led.h"
#include "log.h"
#include "config.h"

#ifdef ENABLE_WEB_SERVER
#include <WiFi.h>
#include "web_server.h"
#include "feedback.h"
BBRXWebServer* webServer = nullptr;
#endif

#define LOG_TAG "main"

void setup() {

    // setup serial
    Serial.begin(115200);
    delay(1500);
    logi(LOG_TAG, "\n=== bbrx! ===");
    logi(LOG_TAG, "version:     %s", VERSION_STRING);
    logi(LOG_TAG, "build time:  %s %s", __DATE__, __TIME__);
    #ifdef GIT_HASH
        logi(LOG_TAG, "commit:      %s", GIT_HASH);
    #endif
    logi(LOG_TAG, "");

    // setup the status led
    leds_setup();
    leds_set_state(LED_LOADING);

#ifdef ENABLE_WEB_SERVER
    // Setup WiFi
    logi(LOG_TAG, "Starting WiFi...");
    
    #if AP_MODE_ENABLED
        // Custom IP configuration (optional - comment out to use default 192.168.4.1)
        IPAddress local_IP(192, 168, 4, 1);      // AP IP address
        IPAddress gateway(192, 168, 4, 1);       // Gateway (usually same as IP)
        IPAddress subnet(255, 255, 255, 0);      // Subnet mask
        
        // Configure the soft AP with custom IP
        WiFi.softAPConfig(local_IP, gateway, subnet);
        
        // Start the access point
        WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
        
        IPAddress IP = WiFi.softAPIP();
        logi(LOG_TAG, "AP Mode - SSID: %s", WIFI_SSID);
        logi(LOG_TAG, "Password: %s", WIFI_PASSWORD);
        logi(LOG_TAG, "IP Address: %s", IP.toString().c_str());
    #else
        // Station mode - connect to existing WiFi
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        logi(LOG_TAG, "Connecting to WiFi...");
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            Serial.print(".");
            attempts++;
        }
        Serial.println("");
        if (WiFi.status() == WL_CONNECTED) {
            logi(LOG_TAG, "Connected! IP: %s", WiFi.localIP().toString().c_str());
        } else {
            logw(LOG_TAG, "Failed to connect to WiFi");
        }
    #endif

    // Start web server
    if (WiFi.status() == WL_CONNECTED || AP_MODE_ENABLED) {
        webServer = new BBRXWebServer(WEB_SERVER_PORT);
        webServer->begin();
        
        #if AP_MODE_ENABLED
            logi(LOG_TAG, "Web UI available at: http://%s", WiFi.softAPIP().toString().c_str());
        #else
            logi(LOG_TAG, "Web UI available at: http://%s", WiFi.localIP().toString().c_str());
        #endif
    }
#endif

    // this will load the user config from the configured filesystem
    // (or will just leave the defaults if this fails).
    // either way, once this function finishes, a bindset will be set up but not initialised
    load_config();

    // setup controller library
    controller_setup();

    // setup event manager and hardware needed by actions
    event_manager_setup();

    // finally, initialise each registered binding
    for (auto b : bindings) {
        initialise_binding(b);
    }

    // done!  now set the status led to idle
    leds_set_state(LED_IDLE);

    logi(LOG_TAG, "Setup complete!");
    logi(LOG_TAG, "==================================");
#ifdef ENABLE_WEB_SERVER
    #if AP_MODE_ENABLED
        logi(LOG_TAG, "Web UI: http://%s", WiFi.softAPIP().toString().c_str());
        logi(LOG_TAG, "SSID: %s", WIFI_SSID);
        logi(LOG_TAG, "Password: %s", WIFI_PASSWORD);
    #else
        if (WiFi.status() == WL_CONNECTED) {
            logi(LOG_TAG, "Web UI: http://%s", WiFi.localIP().toString().c_str());
        }
    #endif
#endif
    logi(LOG_TAG, "==================================");
}

void loop() {

#ifdef ENABLE_WEB_SERVER
    // Handle web server requests
    if (webServer && webServer->isRunning()) {
        webServer->handle();
    }
#endif

    // parse controller input and perform bound actions
    event_manager_update();

    // let the status led update
    leds_update();

}
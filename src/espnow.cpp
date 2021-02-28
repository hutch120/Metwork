#include "config.h"
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <TTGO.h>
#include "esp_wifi_types.h" // RSSI signal strength
#include "power.h"

int recv_count = 0;
int sent_count = 0;

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//uint8_t broadcastAddress[] = {0xC4, 0x4F, 0x33, 0x7F, 0xCE, 0xF9};

const wifi_promiscuous_filter_t filt = {
    .filter_mask=WIFI_PROMIS_FILTER_MASK_ALL
};

void refreshScreen()
{
    if (sent_count > 99)
    {
        sent_count = 1;
    }

    if (recv_count > 99)
    {
        recv_count = 1;
    }

    TTGOClass *ttgo = TTGOClass::getWatch();
    ttgo->tft->fillScreen(TFT_RED);
    // ttgo->tft->fillRect(20, 100, 200, 85, TFT_RED);
    ttgo->tft->setTextColor(TFT_WHITE);

    ttgo->tft->drawString(F("METWORK"), 1, 1);

    ttgo->tft->drawString(F("S"), 10, 45);
    ttgo->tft->drawNumber(sent_count, 80, 40, 6);

    ttgo->tft->drawString(F("R"), 10, 135);
    ttgo->tft->drawNumber(recv_count, 80, 130, 6);

    int battery_percentage = ttgo->power->getBattPercentage();
    String battPer = "";
    battPer += battery_percentage;
    battPer += "%";
    ttgo->tft->drawString(battPer, 160, 1, 2);
}

void formatMacAddress(const uint8_t *macAddr, char *buffer, int maxLength)
{
    snprintf(buffer, maxLength, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
}

void onDataReceived(const uint8_t *macAddr, const uint8_t *data, int dataLen)
{
    log_i("Received message.");

    // if (isSleeping())
    // {
    //     return;
    // }

    // // only allow a maximum of 250 characters in the message + a null terminating byte
    // char buffer[ESP_NOW_MAX_DATA_LEN + 1];
    // int msgLen = min(ESP_NOW_MAX_DATA_LEN, dataLen);
    // strncpy(buffer, (const char *)data, msgLen);

    // // make sure we are null terminated
    // buffer[msgLen] = 0;

    // // format the mac address
    // char macStr[18];
    // formatMacAddress(macAddr, macStr, 18);

    // log_i("Received message from: %s - %s", macStr, buffer);

    recv_count++;
    refreshScreen();
}

// callback when data is sent
void onDataSent(const uint8_t *macAddr, esp_now_send_status_t status)
{
    if (isSleeping())
    {
        return;
    }

    log_i("ESPNow broadcast reports %s", status == ESP_NOW_SEND_SUCCESS ? "send success" : "send fail");

    char macStr[18];
    formatMacAddress(macAddr, macStr, 18);
    log_i("Last message sent to %s", macStr);

    if (status == ESP_NOW_SEND_SUCCESS)
    {
        sent_count++;
        refreshScreen();
    }
}

void broadcast()
{
    if (isSleeping())
    {
        return;
    }

    log_i("Broadcasting ...");

    int n = 1;
    esp_err_t result = esp_now_send(broadcastAddress, (const uint8_t *)&n, sizeof(int));

    if (result == ESP_OK)
    {
        log_i("Broadcast message success");
    }
    else if (result == ESP_ERR_ESPNOW_NOT_INIT)
    {
        log_i("ESPNOW not initialized.");
    }
    else if (result == ESP_ERR_ESPNOW_ARG)
    {
        log_i("Invalid argument.");
    }
    else if (result == ESP_ERR_ESPNOW_INTERNAL)
    {
        log_i("Internal error.");
    }
    else if (result == ESP_ERR_ESPNOW_NO_MEM)
    {
        log_i("No memory.");
    }
    else if (result == ESP_ERR_ESPNOW_NOT_FOUND)
    {
        log_i("Peer not found.");
    }
    else
    {
        log_i("Unknown error.");
    }
}

void promiscuous_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type)
{
    log_i("Received packet.");

    // All espnow traffic uses action frames which are a subtype of the mgmnt frames so filter out everything else.
    // if (type != WIFI_PKT_MGMT)
    //     return;

    // static const uint8_t ACTION_SUBTYPE = 0xd0;
    // static const uint8_t ESPRESSIF_OUI[] = {0x18, 0xfe, 0x34};

    // const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buf;
    // const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
    // const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

    // // Only continue processing if this is an action frame containing the Espressif OUI.
    // if ((ACTION_SUBTYPE == (hdr->frame_ctrl & 0xFF)) &&
    //     (memcmp(hdr->oui, ESPRESSIF_OUI, 3) == 0))
    // {
    //     int rssi = ppkt->rx_ctrl.rssi;
    // }
}

void onStartAP(WiFiEvent_t event, WiFiEventInfo_t info)
{
    log_i("WiFi AP started");

    // log_i("Setting WiFi to low power");
    // WiFi.setTxPower(WIFI_POWER_MINUS_1dBm); // WIFI_POWER_MINUS_1dBm WIFI_POWER_19_5dBm

    // int powerTx = WiFi.getTxPower();
    // log_i("WiFi power set to %d", powerTx);
}

void onStartSTA(WiFiEvent_t event, WiFiEventInfo_t info)
{
    log_i("WiFi STA started.");

    log_i("Setting WiFi to low power.");
    WiFi.setTxPower(WIFI_POWER_5dBm); // WIFI_POWER_MINUS_1dBm WIFI_POWER_19_5dBm

    int powerTx = WiFi.getTxPower();
    log_i("WiFi power set to %d.", powerTx);
}

void onWiFiReady(WiFiEvent_t event, WiFiEventInfo_t info)
{
    log_i("WiFi ready.");
}

void espnow_setup()
{
    log_i("MAC Address: %s", WiFi.macAddress().c_str());
    WiFi.mode(WIFI_MODE_STA);
    //WiFi.enableSTA(true);
    //WiFi.enableAP(true);

    WiFi.onEvent(onStartAP, SYSTEM_EVENT_AP_START);
    WiFi.onEvent(onStartSTA, SYSTEM_EVENT_STA_START);
    WiFi.onEvent(onWiFiReady, SYSTEM_EVENT_WIFI_READY);

    // log_i("Disconnecting WiFi from access point.");
    // WiFi.disconnect();

    //esp_wifi_set_promiscuous(true);
    //esp_wifi_set_promiscuous_filter(&filt);
    //esp_wifi_set_promiscuous_rx_cb(&promiscuous_rx_cb);
    // WiFi.channel(0);

    if (esp_now_init() == ESP_OK)
    {
        log_i("ESPNow initialized successfully.");

        // set encryption key
        //const String &key = "000000000";
        //esp_now_set_pmk( (const uint8_t *)key.c_str());

        // add a peer for broadcast
        esp_now_peer_info_t peerInfo = {};
        memcpy(&peerInfo.peer_addr, broadcastAddress, 6);
        peerInfo.channel = 1;
        peerInfo.ifidx = WIFI_IF_STA;
        peerInfo.encrypt = false;
        
        if (!esp_now_is_peer_exist(broadcastAddress))
        {
            log_i("Adding peer 0xffffffffff");
            esp_now_add_peer(&peerInfo);
        }

        // register callbacks
        esp_now_register_recv_cb(onDataReceived);
        esp_now_register_send_cb(onDataSent);

        log_i("ESPNow callbacks registered.");

        refreshScreen();
    }
    else
    {
        log_i("ESPNow initialization failed.");
    }
}

int touchCount = 0;

void espnow_loop()
{
    /* 
        protocol??
        [0-9] - user touching screen ticks ?? OR RATE OF TICK.. where tick rate increases over time.
        [network id] - Exterally created?? WIFI names?
    */

   // esp_now_unregister_recv_cb();
   // esp_now_register_recv_cb(onDataReceived);

    // log_i("WiFi.isConnected = %d", WiFi.isConnected());
    broadcast();
}
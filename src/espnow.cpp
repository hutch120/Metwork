#include "config.h"
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <TTGO.h>
#include "esp_wifi_types.h" // RSSI signal strength
#include "button.h"

int recv_count = 0;
int sent_count = 0;

void RefreshScreen()
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

void receiveCallback(const uint8_t *macAddr, const uint8_t *data, int dataLen)
{
    if (isSleeping())
    {
        return;
    }
    // only allow a maximum of 250 characters in the message + a null terminating byte
    char buffer[ESP_NOW_MAX_DATA_LEN + 1];
    int msgLen = min(ESP_NOW_MAX_DATA_LEN, dataLen);
    strncpy(buffer, (const char *)data, msgLen);
    // make sure we are null terminated
    buffer[msgLen] = 0;
    // format the mac address
    char macStr[18];
    formatMacAddress(macAddr, macStr, 18);
    // debug log the message to the serial port
    log_i("Received message from: %s - %s\n", macStr, buffer);

    recv_count++;
    RefreshScreen();
}

// callback when data is sent
void sentCallback(const uint8_t *macAddr, esp_now_send_status_t status)
{
    if (isSleeping())
    {
        return;
    }
    char macStr[18];
    formatMacAddress(macAddr, macStr, 18);
    log_i("Last Packet Sent to: %s", macStr);
    log_i("Last Packet Send Status: %s", status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");

    if (status == ESP_NOW_SEND_SUCCESS)
    {
        sent_count++;
        RefreshScreen();
    }
}

void broadcast(const String &message)
{
    if (isSleeping())
    {
        return;
    }
    // this will broadcast a message to everyone in range
    uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    esp_now_peer_info_t peerInfo = {};
    memcpy(&peerInfo.peer_addr, broadcastAddress, 6);
    if (!esp_now_is_peer_exist(broadcastAddress))
    {
        esp_now_add_peer(&peerInfo);
    }
    esp_err_t result = esp_now_send(broadcastAddress, (const uint8_t *)message.c_str(), message.length());
    // and this will send a message to a specific device
    /*
        uint8_t peerAddress[] = {0x3C, 0x71, 0xBF, 0x47, 0xA5, 0xC0};
        esp_now_peer_info_t peerInfo = {};
        memcpy(&peerInfo.peer_addr, peerAddress, 6);
        if (!esp_now_is_peer_exist(peerAddress))
        {
            esp_now_add_peer(&peerInfo);
        }
        esp_err_t result = esp_now_send(peerAddress, (const uint8_t *)message.c_str(), message.length());
    */
    if (result == ESP_OK)
    {
        log_i("Broadcast message success");
    }
    else if (result == ESP_ERR_ESPNOW_NOT_INIT)
    {
        log_i("ESPNOW not Init.");
    }
    else if (result == ESP_ERR_ESPNOW_ARG)
    {
        log_i("Invalid Argument");
    }
    else if (result == ESP_ERR_ESPNOW_INTERNAL)
    {
        log_i("Internal Error");
    }
    else if (result == ESP_ERR_ESPNOW_NO_MEM)
    {
        log_i("ESP_ERR_ESPNOW_NO_MEM");
    }
    else if (result == ESP_ERR_ESPNOW_NOT_FOUND)
    {
        log_i("Peer not found.");
    }
    else
    {
        log_i("Unknown error");
    }
}

/*void promiscuous_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type)
{
    // All espnow traffic uses action frames which are a subtype of the mgmnt frames so filter out everything else.
    if (type != WIFI_PKT_MGMT)
        return;

    static const uint8_t ACTION_SUBTYPE = 0xd0;
    static const uint8_t ESPRESSIF_OUI[] = {0x18, 0xfe, 0x34};

    const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buf;
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
    const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

    // Only continue processing if this is an action frame containing the Espressif OUI.
    if ((ACTION_SUBTYPE == (hdr->frame_ctrl & 0xFF)) &&
        (memcmp(hdr->oui, ESPRESSIF_OUI, 3) == 0))
    {

        int rssi = ppkt->rx_ctrl.rssi;
    }
}*/

void espnow_setup()
{
    //Set device in STA mode to begin with
    WiFi.mode(WIFI_STA);
    log_i("My MAC Address is: %s", WiFi.macAddress().c_str()); // https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/c_str/
    WiFi.disconnect();

    //esp_wifi_set_promiscuous(true);
    //esp_wifi_set_promiscuous_rx_cb(&promiscuous_rx_cb)

    if (esp_now_init() == ESP_OK)
    {
        log_i("ESPNow Init Success ... register callbacks...");
        esp_now_register_recv_cb(receiveCallback);
        esp_now_register_send_cb(sentCallback);

        RefreshScreen();
    }
    else
    {
        log_i("ESPNow Init Failed");
    }
}

int touchCount = 0;

void espnow_loop()
{
    log_i("broadcast...");
    /* 
        protocol
        [0-9] - user touching screen ticks ?? OR RATE OF TICK.. where tick rate increases over time.
        [network id] - Exterally created?? WIFI names?
    */
    broadcast("5"); // touchCount
}
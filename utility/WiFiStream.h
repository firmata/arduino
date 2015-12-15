#ifndef WIFI_STREAM_H
#define WIFI_STREAM_H

#include <inttypes.h>
#include <Stream.h>
#include <IPAddress.h>
#include <WiFi.h>
#include <SPI.h>


class WiFiStream : public Stream
{
  private:
    WiFiServer _server = WiFiServer(23);
    WiFiClient _client;
    
    //configuration members
	IPAddress _local_ip;
    uint16_t _port = 0;
    uint8_t _key_idx = 0;           //WEP
    const char *_key = nullptr;           //WEP
	const char *_passphrase = nullptr;    //WPA
    char *_ssid = nullptr;
    
    int connect_client();
    bool is_ready();

  public:       
    WiFiStream();
    
    /* dynamic IP (DHCP) configurations */
    
    //OPEN networks
    int begin(char *ssid, uint16_t port);
    
    //WEP-encrypted networks
    int begin(char *ssid, uint8_t key_idx, const char *key, uint16_t port);
    
    //WPA-encrypted networks
    int begin(char *ssid, const char *passphrase, uint16_t port);
    
    /* static IP configurations */

    //OPEN networks with static IP
    int begin(char *mac_address, IPAddress local_ip, uint16_t port);
    
    //WEP-encrypted networks with static IP
    int begin(char *ssid, IPAddress local_ip, uint8_t key_idx, const char *key, uint16_t port);
    
    //WPA-encrypted networks with static IP
    int begin(char *ssid, IPAddress local_ip, const char *passphrase, uint16_t port);
    
    // allows another way to configure a static IP before begin is called
    void config(IPAddress local_ip);

    // get DCHP IP
    IPAddress localIP();

    // overriden Stream class functions
    virtual size_t write(uint8_t byte);
    virtual int read();
    virtual int available();
    virtual void flush();
    virtual int peek();
    virtual void stop();
    virtual bool maintain();
};

#endif

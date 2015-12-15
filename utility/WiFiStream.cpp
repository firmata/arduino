/******************************************************************************
 * Includes
 ******************************************************************************/

#include "WiFiStream.h"


/******************************************************************************
 * Definitions
 ******************************************************************************/

 WiFiStream::WiFiStream()
 {
    
 }

int WiFiStream::available()
{
    return connect_client() ? _client.available() : 0;
}

int WiFiStream::begin( char *ssid, uint16_t port )
{
    if( !is_ready() ) return 0;
    
    _ssid = ssid;
    _port = port;
    int result = WiFi.begin( ssid );
    if( result == 0 ) return 0;
    
    _server = WiFiServer( port );
    _server.begin();
    return result;
}

int WiFiStream::begin( char *ssid, uint8_t key_idx, const char *key, uint16_t port )
{
    if( !is_ready() ) return 0;
    
    _ssid = ssid;
    _port = port;
    _key_idx = key_idx;
    _key = key;
    
    int result = WiFi.begin( ssid, key_idx, key );
    if( result == 0 ) return 0;
    
    _server = WiFiServer( port );
    _server.begin();
    return result;
}

int WiFiStream::begin( char *ssid, const char *passphrase, uint16_t port )
{
    if( !is_ready() ) return 0;
    
    _ssid = ssid;
    _port = port;
    _passphrase = passphrase;
    
    int result = WiFi.begin( ssid, passphrase);
    if( result == 0 ) return 0;
    
    _server = WiFiServer( port );
    _server.begin();
    return result;
}

int WiFiStream::begin( char *ssid, IPAddress local_ip, uint16_t port )
{
    if( !is_ready() ) return 0;
    
    _ssid = ssid;
    _port = port;
    _local_ip = local_ip;
    
    WiFi.config( local_ip );
    int result = WiFi.begin( ssid );
    if( result == 0 ) return 0;
    
    _server = WiFiServer( port );
    _server.begin();
    return result;
}

int WiFiStream::begin( char *ssid, IPAddress local_ip, uint8_t key_idx, const char *key, uint16_t port )
{
    if( !is_ready() ) return 0;
    
    _ssid = ssid;
    _port = port;
    _local_ip = local_ip;
    _key_idx = key_idx;
    _key = key;
    
    WiFi.config( local_ip );
    int result = WiFi.begin( ssid, key_idx, key );
    if( result == 0 ) return 0;
    
    _server = WiFiServer( port );
    _server.begin();
    return result;
}

int WiFiStream::begin( char *ssid, IPAddress local_ip, const char *passphrase, uint16_t port )
{
    if( !is_ready() ) return 0;
    
    _ssid = ssid;
    _port = port;
    _local_ip = local_ip;
    _passphrase = passphrase;
    
    WiFi.config( local_ip );
    int result = WiFi.begin( ssid, passphrase);
    if( result == 0 ) return 0;
    
    _server = WiFiServer( port );
    _server.begin();
    return result;
}

void WiFiStream::config( IPAddress local_ip )
{
    _local_ip = local_ip;
    WiFi.config( local_ip );
}

IPAddress WiFiStream::localIP()
{
    return WiFi.localIP();
}

int WiFiStream::connect_client() 
{
    if( !( _client && _client.connected() ) )
    {
        WiFiClient newClient = _server.available();
        if( !newClient )
        {
            return 0;
        }

        _client = newClient;
    }
    return 1;
}

void WiFiStream::flush()
{
    if( _client ) _client.flush();
}

bool WiFiStream::is_ready()
{
    uint8_t status = WiFi.status();
    return !( status == WL_NO_SHIELD || status == WL_CONNECTED );
}

bool WiFiStream::maintain()
{
    if( connect_client() ) return true;

    stop();
	int result = 0;
    if( WiFi.status() != WL_CONNECTED )
    {
        if( _local_ip )
        {
            WiFi.config( _local_ip );
        }
      
        if( _passphrase )
        {
            result = WiFi.begin( _ssid, _passphrase);
        }
        else if( _key_idx && _key )
        {
            result = WiFi.begin( _ssid, _key_idx, _key );
        }
	    else
	    {
	  	    result = WiFi.begin( _ssid );
	    }
    }
    if( result == 0 ) return false;
  
    _server = WiFiServer( _port );
    _server.begin();
    return result;
}

int WiFiStream::peek()
{
    return _client ? _client.peek(): 0;
}

int WiFiStream::read()
{
    return connect_client() ? _client.read() : 0;
}

void WiFiStream::stop()
{
    _client.stop();
}

size_t WiFiStream::write(uint8_t outgoingByte)
{  
    if( connect_client() ) _client.write(outgoingByte);
}

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "WiFi101Stream.h"


/******************************************************************************
 * Definitions
 ******************************************************************************/

 WiFi101Stream::WiFi101Stream()
 {
	 
 }

int WiFi101Stream::available()
{
    return connect_client() ? _client.available() : 0;
}

int WiFi101Stream::begin( char *ssid, uint16_t port )
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

int WiFi101Stream::begin( char *ssid, uint8_t key_idx, const char *key, uint16_t port )
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

int WiFi101Stream::begin( char *ssid, const char *passphrase, uint16_t port )
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

int WiFi101Stream::begin( char *ssid, IPAddress local_ip, uint16_t port )
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

int WiFi101Stream::begin( char *ssid, IPAddress local_ip, uint8_t key_idx, const char *key, uint16_t port )
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

int WiFi101Stream::begin( char *ssid, IPAddress local_ip, const char *passphrase, uint16_t port )
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

void WiFi101Stream::config( IPAddress local_ip )
{
    _local_ip = local_ip;
    WiFi.config( local_ip );
}

IPAddress WiFi101Stream::localIP()
{
    return WiFi.localIP();
}

int WiFi101Stream::connect_client() 
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

void WiFi101Stream::flush()
{
    if( _client ) _client.flush();
}

bool WiFi101Stream::is_ready()
{
    uint8_t status = WiFi.status();
    return !( status == WL_NO_SHIELD || status == WL_CONNECTED );
}

bool WiFi101Stream::maintain()
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

int WiFi101Stream::peek()
{
    return connect_client() ? _client.peek(): 0;
}

int WiFi101Stream::read()
{
    return connect_client() ? _client.read() : -1;
}

void WiFi101Stream::stop()
{
    _client.stop();
}

size_t WiFi101Stream::write(uint8_t outgoingByte)
{  
    if( connect_client() ) _client.write(outgoingByte);
}

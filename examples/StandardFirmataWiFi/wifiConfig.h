/*==============================================================================
 * WIFI CONFIGURATION
 *
 * You must configure your particular hardware. Follow the steps below.
 *============================================================================*/

// STEP 1 [REQUIRED]
// Uncomment / comment the appropriate set of includes for your hardware (OPTION A, B or C)
// Option A is enabled by default.

/*
 * OPTION A: Configure for Arduino WiFi shield
 *
 * To configure StandardFirmataWiFi to use the Arduino WiFi shield based on the HDG204 Wireless LAN 802.11b/g
 * leave the #define below uncommented.
 */
#define ARDUINO_WIFI_SHIELD

//do not modify these next 4 lines
#ifdef ARDUINO_WIFI_SHIELD
#include "utility/WiFiStream.h"
WiFiStream stream;
#endif

/*
 * OPTION B: Configure for WiFi 101
 *
 * To configure StandardFirmataWiFi to use the WiFi 101 library, either for the WiFi 101 shield or
 * any boards that include the WiFi 101 chip (such as the MKR1000), comment out the '#define ARDUINO_WIFI_SHIELD'
 * under OPTION A above, and uncomment the #define WIFI_101 below.
 *
 * IMPORTANT: You must have the WiFI 101 library installed. To easily install this library, opent the library manager via:
 * Arduino IDE Menus: Sketch > Include Library > Manage Libraries > filter search for "WiFi101" > Select the result and click 'install'
 */
//#define WIFI_101

//do not modify these next 4 lines
#ifdef WIFI_101
#include "utility/WiFi101Stream.h"
WiFi101Stream stream;
#endif

/*
 * OPTION C: Configure for HUZZAH
 */

//------------------------------
 //TODO
//------------------------------
//#define HUZZAH_WIFI


// STEP 2 [REQUIRED for all boards and shields]
// replace this with your wireless network SSID
char ssid[] = "your_network_name";

// STEP 3 [OPTIONAL for all boards and shields]
// if you want to use a static IP (v4) address, uncomment the line below. You can also change the IP.
// if this line is commented out, the WiFi shield will attempt to get an IP from the DHCP server
// #define STATIC_IP_ADDRESS 192,168,1,113

// STEP 4 [REQUIRED for all boards and shields]
// define your port number here, you will need this to open a TCP connection to your Arduino
#define SERVER_PORT 3030

// STEP 5 [REQUIRED for all boards and shields]
// determine your network security type (OPTION A, B, or C)

/*
 * OPTION A: Open network (no security)
 *
 * To connect to an open network, leave WIFI_NO_SECURITY uncommented and
 * do not uncomment the #define values under options B or C
 */
//#define WIFI_NO_SECURITY

/*
 * OPTION B: WEP
 *
 * Uncomment the #define below and set your wep_index and wep_key values appropriately
 */
//#define WIFI_WEP_SECURITY

#ifdef WIFI_WEP_SECURITY
byte wep_index = 0;
char wep_key[] = "your_wep_key";
#endif  //WIFI_WEP_SECURITY

/*
 * OPTION C: WPA / WPA2
 *
 * Uncomment the #define below and set your passphrase appropriately
 */
#define WIFI_WPA_SECURITY

#ifdef WIFI_WPA_SECURITY
char wpa_passphrase[] = "your_wpa_passphrase";
#endif  //WIFI_WPA_SECURITY

/*==============================================================================
 * CONFIGURATION ERROR CHECK (don't change anything here)
 *============================================================================*/

#if ((defined(ARDUINO_WIFI_SHIELD) && (defined(WIFI_101) || defined(HUZZAH_WIFI))) || (defined(WIFI_101) && defined(HUZZAH_WIFI)))
#error "you may not define more than one wifi device type."
#endif //WIFI device type check

#if !(defined(ARDUINO_WIFI_SHIELD) || defined(WIFI_101) || defined(HUZZAH_WIFI))
#error "you must define a wifi device type."
#endif

#if ((defined(WIFI_NO_SECURITY) && (defined(WIFI_WEP_SECURITY) || defined(WIFI_WPA_SECURITY))) || (defined(WIFI_WEP_SECURITY) && defined(WIFI_WPA_SECURITY)))
#error "you may not define more than one security type at the same time."
#endif  //WIFI_* security define check

#if !(defined(WIFI_NO_SECURITY) || defined(WIFI_WEP_SECURITY) || defined(WIFI_WPA_SECURITY))
#error "you must define a wifi security type."
#endif  //WIFI_* security define check

/*==============================================================================
 * PIN IGNORE MACROS (don't change anything here)
 *============================================================================*/

// ignore SPI pins, pin 5 (reset WiFi101 shield), pin 7 (WiFi handshake) and pin 10 (WiFi SS)
// also don't ignore SS pin if it's not pin 10
// TODO - need to differentiate between Arduino WiFi1 101 Shield and Arduino MKR1000
#define IS_IGNORE_WIFI101_SHIELD(p)  ((p) == 10 || (IS_PIN_SPI(p) && (p) != SS) || (p) == 5 || (p) == 7)

// ignore SPI pins, pin 4 (SS for SD-Card on WiFi-shield), pin 7 (WiFi handshake) and pin 10 (WiFi SS)
#define IS_IGNORE_WIFI_SHIELD(p)     ((IS_PIN_SPI(p) || (p) == 4) || (p) == 7 || (p) == 10)


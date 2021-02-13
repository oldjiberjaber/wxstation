/*********************************************
   WiFi Specifics
 *********************************************/
const char* ssid = "Your SSID";
const char* password = "your password";

#ifdef SANDPIT
const char* WIFIID = "WXSTtest";
const String mqtt_ID ="WXSTtest";
#else
const char* WIFIID = "WXSTATION";
const String mqtt_ID ="WXSTN";
#endif

const char* mqtt_server = "192.168.1.6";

//---------------------WU PSW ID-------------------
const char* WUID    = "Your WUID";
const char* WUPASS   = "Your WUPASSWORD";

// replace with your channel’s thingspeak API key,
String apiKey = "Your API Key";

// replace with your thingspeak thingtweet API key,
String thingtweetAPIKey  = "your api key";

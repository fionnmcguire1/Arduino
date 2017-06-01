/*
IMPORTANT NOTE about TembooAccount.h

TembooAccount.h contains your Temboo account information and must be included
alongside your sketch. To do so, make a new tab in Arduino, call it TembooAccount.h,
and copy this content into it. 
*/

#define TEMBOO_ACCOUNT "fionnmcguire"  // Your Temboo account name 
#define TEMBOO_APP_KEY_NAME "tempplushumid"  // Your Temboo app key name
#define TEMBOO_APP_KEY "cwmIw6Gd4ULAUNCi8hEb9DsfAjf6JQ1a"  // Your Temboo app key

#define WIFI_SSID "TP-LINK_37C598"
#define WPA_PASSWORD "25729559"

/* 
The same TembooAccount.h file settings can be used for all Temboo SDK sketches.  
Keeping your account information in a separate file means you can share the 
main .ino file without worrying that you forgot to delete your credentials.
*/

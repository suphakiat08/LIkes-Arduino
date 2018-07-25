#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "Display.h"

#define mqtt_server "10.0.0.195"
#define mqtt_port 1883

WiFiClient espClient;
PubSubClient client(espClient);

byte mac[5];
char serial_char[17];
String serial_number;

const char* ssid     = "Embedded_2.4G";
const char* password = "smartembedded802";
//const char* ssid  = "Scerets";
//const char* password = "025325017";

void callback(char* topic, byte* payload, unsigned int length);
String month(String str);

void connect_wifi() {
  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  WiFi.macAddress(mac);
  
  serial_number = String(mac[0]) + String(mac[1]) + String(mac[2]) + String(mac[3]) + String(mac[4]) + String(mac[5]);
  serial_number = serial_number.substring(0, 16);
  
  Serial.println("serial number: " + serial_number);

  serial_number.toCharArray(serial_char, 17);
  display_init(serial_char);
}

void connect_mqtt() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      String pathStr = "/devices/" + serial_number;
      char pathChar[pathStr.length() + 1];
      pathStr.toCharArray(pathChar, pathStr.length() + 1);

      client.subscribe(pathChar);
      Serial.println(pathChar);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
      return;
    }
  }
}

void client_loop() {
  if(client.state() < 0) {
    client.disconnect();
    connect_mqtt(); 
  }
  client.loop();
}

String before_update;
int likes_before = -1;
int shares_before = -1;

void updateStatus() {
  char serial[serial_number.length() + 1];
    serial_number.toCharArray(serial, serial_number.length() + 1);
    
    String pathStr = "/devices/heartbeat";
    char pathChar[pathStr.length() + 1];
    pathStr.toCharArray(pathChar, pathStr.length() + 1);
    client.publish(pathChar, serial);
}

void callback(char* topic, byte* payload, unsigned int length) {
  
  String json = String((char*)payload);
  boolean change_data = false;
  boolean isLikes_change = false;
  boolean isShares_change = false;
  boolean isPromotion = false;

  if(before_update != json) {
     change_data = true;
  }
  
  before_update = String((char*)payload);
  
  Serial.print("\nMessage arrived in topic: ");
  Serial.println(topic);

  StaticJsonBuffer<512> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);

  if(change_data) {
    if(root["switch"]) {
      updateStatus();
      if(likes_before != -1 && shares_before != -1) {
        if(likes_before != root["likes"]) {
          isLikes_change = true;
        }
        if(shares_before != root["shares"]) {
          isShares_change = true;
        }
      }
  
      String prod_name_str = root["prod_name"].as<String>();
      char prod_name[prod_name_str.length() + 1];
      prod_name_str.toCharArray(prod_name, sizeof(prod_name));
  
      String price_str = root["price"];
      for (int i = price_str.length() - 3; i > 0 ; i -= 3) {
        price_str = price_str.substring(0, i) + "," + price_str.substring(i, price_str.length());
      }
      char price[price_str.length() + 1];
      price_str.toCharArray(price, sizeof(price));
    
      likes_before = root["likes"];
      char likes[String(likes_before).length() + 1];
      String(likes_before).toCharArray(likes, sizeof(likes));
    
      shares_before = root["shares"];
      char shares[String(shares_before).length() + 1];
      String(shares_before).toCharArray(shares, sizeof(shares));
     
      int stock = root["icons"]["stock"] ? true : false;
      int limited = root["icons"]["limited"] ? true : false;
      boolean hot_sale = root["icons"]["hot_sale"] ? true : false;
    
      String promotion[4];
      if (root["icons"]["promotion"]) {
        String date = root["icons"]["promotion"]["date"].as<String>();
          isPromotion = true;
          date = date.substring(3, 5) + " " + month(date.substring(0, 2)) + " " + date.substring(6, 10);
        
          promotion[0] = root["icons"]["promotion"]["amount"].as<String>();
          promotion[1] = root["icons"]["promotion"]["unit"].as<String>();
          promotion[2] = date;
          promotion[3] = promotion[1] == "baht" ?
             (int)root["price"] - (int)root["icons"]["promotion"]["amount"] :
             (int)((int)root["price"] - ((double)root["icons"]["promotion"]["amount"] / 100) * (int)root["price"]);
    
          for (int i = promotion[3].length() - 3; i > 0 ; i -= 3) {
            promotion[3] = promotion[3].substring(0, i) + "," + promotion[3].substring(i, promotion[3].length());
          }
      }
  
      int option;
      if(isPromotion) {
        option = 4;
        hot_sale ?
          option += 1:
        limited > 0 ?
          option += 2:
        stock > 0 ?
          option += 3: 0;
          
      } else if(hot_sale) {
        option = 1;
      } else if(limited) {
        option = 2;
      } else if (stock) {
        option = 3;
      } else {
        option = 0;
      }
      if(isLikes_change || isShares_change) {
        if(option == 0) {
          if(isLikes_change) display_refresh_likes_share(0, likes);
          if(isShares_change) display_refresh_likes_share(1, shares);
        } else {
          if(isLikes_change) display_refresh_likes(likes);
        }
      } else {
        display_page(prod_name, price, likes, shares, promotion, option);
      }
    } else {
      before_update = "";
      likes_before = -1;
      shares_before = -1;
      
      display_init(serial_char);
    }
  } else {
    if(root["switch"]) {
      updateStatus();
    }
  }
}

String month(String str) {
  switch (str.toInt()) {
    case 1:
      str = "Jan"; break;
    case 2:
      str = "Feb"; break;
    case 3:
      str = "Mar"; break;
    case 4:
      str = "Apr"; break;
    case 5:
      str = "May"; break;
    case 6:
      str = "Jun"; break;
    case 7:
      str = "Jul"; break;
    case 8:
      str = "Aug"; break;
    case 9:
      str = "Sep"; break;
    case 10:
      str = "Oct"; break;
    case 11:
      str = "Nov"; break;
    case 12:
      str = "Dec"; break;
  }
  
  return str;
}



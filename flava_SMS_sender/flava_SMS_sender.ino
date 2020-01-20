#include <WiFi.h>
#include <PubSubClient.h>
 
#define PrintSerialMonitor  //if define it, print on serial monitor 
//#define DEBUG
const char* ssid = "RTXMaster"; // Enter your WiFi name
const char* password =  "RTX_net_passC0de#"; // Enter WiFi password
const char* mqttServer = "soldier.cloudmqtt.com";
const int mqttPort = 12729;
const char* mqttUser = "ciyajbsc";
const char* mqttPassword = "je1T5MADj3Wi";

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  Serial.begin(9600);
  Serial2.begin(9600); // for gsm
  
//setup wifi
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
  client.publish("flava", "hello"); //Topic name
  client.subscribe("flava1");
 
  
//Initialize GSM--------------------
  int gsmInitFlag=0;
  while(gsmInitFlag==0)
  {
    Serial.println("Initialize GSM");
    if(InitGsm()==1)
    {
    Serial.print("GSM OK");
    gsmInitFlag=1;
    }else
    {
    Serial.println("GSM Fail");
    delay(500);
    }
  }
  //Sendsms("0779178744","helo");
//check network status---------------
  int checkNetworkFlag=0;
 // while(checkNetworkFlag==0)
  //{
  
    Serial.print("strength :- ");
    Serial.println(GetsingalStrength());
    Serial.println("Connectig..");
    if(CheckNetwork()==1)
    {
    Serial.println("Network conected");
    checkNetworkFlag=1;
    }else
    {
    Serial.println("Network Fail");
    delay(500);
    }
  //}

}


void loop()
{
   client.loop();
   client.publish("flava", "hello");
   delay(1000);
  
}
  
  
  
  
  
  
  
  
  
  
  
  
  
// mqtt callback

// void callback(char* topic, byte* payload, unsigned int length) {
  
  // char msgCharArray[length];
  // Serial.print("Message arrived in topic: ");
  // Serial.println(topic);
 
  // Serial.print("Message:");
  // for (int i = 0; i < length; i++)
  // {
    // //Serial.print((char)payload[i]);
  // msgCharArray[i] = (char)payload[i];
  // }
 
  // Serial.println(msgCharArray);
  // Serial.println("-----------------------");
 
// }
 

void callback(char* topic, byte* payload, unsigned int length)
{
 
  #ifdef DEBUG
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  #endif
  
  char* dataArry = (char*)malloc(length);
 
  #ifdef DEBUG
  Serial.print("Message:");
  #endif
  for (int i = 0; i < length; i++)
  {
    dataArry[i] = (char)payload[i];

  }

  #ifdef DEBUG
  Serial.print("length ");
  Serial.println(length);
  #endif

  Serial.println(dataArry);
  
   // start converting
  String *strData;
  strData = new String[length];

  int count =0;
  char * pch;
  pch = strtok (dataArry,",");
  
  while (pch != NULL)
  {
    String tempst= pch;
    strData[count] = tempst;
    #ifdef DEBUG
    printf ("%s\n",pch);
    #endif
    pch = strtok (NULL, ",");
    count++;
  }
  
  Serial.print("data1 :- ");
  Serial.println(strData[0]);
  Serial.print("data2 :- ");
  Serial.println(strData[1]);

 free(dataArry);
}
 


int GetsingalStrength()
{

  Serial2.print("AT\r\n");
  delay(500);
  while(!Serial2.available());  // for ready gsm
  
  while(Serial2.available())
  {
    //Serial.print((char)Serial2.read());
    int temp = (char)Serial2.read();  // get garbadge values
    
  }
    
  Serial2.print("AT+CSQ\r\n");
  delay(500);
  
  while(!Serial2.available());
  
  String response;
  while(Serial2.available()){
  response.concat((char)Serial2.read());
  
  }
  
  //Serial.println(response);   // strength format  +CSQ: 4,0
  int a = response.indexOf(' ');
  int b = response.indexOf(',');
  
  String strength = response.substring(a+1, b);   // strength value
 #if defined(PrintSerialMonitor)
  Serial.print("Signal strength:- ");
  Serial.println(strength);
 #endif
  
  return strength.toInt();
}


int CheckNetwork()
{

  Serial2.print("AT\r\n");
  delay(500);
  while(!Serial2.available());    //ready gsm
  
  while(Serial2.available())
  {
    //Serial.print((char)Serial2.read());
    int temp = (char)Serial2.read();    // get garbadge values
  }
    
  Serial2.print("AT+CREG?\r\n");
  delay(500);
  while(!Serial2.available());

  String response;
  while(Serial2.available())
  {
    response.concat((char)Serial2.read());
  }
    
  //Serial.println(response);     // responce format +CREG: 1,1
  //Serial.println(response.charAt(18));
  //Serial.println(response.charAt(20));
  
  if(response.charAt(18)=='0' or response.charAt(18)=='6')
  { //char 18 and 20 became 0,1 on home network
    if(response.charAt(20)=='1')
  {
      #if defined(PrintSerialMonitor)
      Serial.println("Network Ok");
      #endif
      return 1;
        
    }else
  {
      #if defined(PrintSerialMonitor)
      Serial.println("Network Fail");
      #endif
      return 0;
    }
      
  }else
  {
    #if defined(PrintSerialMonitor)
    Serial.println("Network Fail");
    #endif
    return 0;
  
  }
    
}


int InitGsm()
{
  int gsm=0;
  
  Serial2.print("AT\r\n");
  delay(500);
  while(!Serial2.available());  //wait for responce gsm
  
  String response;
  while(Serial2.available())
  {
    response.concat((char)Serial2.read());  //responce string have AT +OK 
  }
    
  String cmd = response.substring(6,8); //substring OK part
  if(!(cmd.indexOf("OK")==-1))
  {   //if true Gsm return ok
    gsm=1;
  }
    
  return gsm;
  
}


int Sendsms(String num, String msg){

  Serial2.print("AT\r\n");
  delay(500);
  while(!Serial2.available());  //ready gsm
  
  while(Serial2.available())
  {   //garbadge collection
    //Serial.print((char)Serial2.read());
    int temp = (char)Serial2.read();
  }
    
  Serial2.print("AT+CMGF=1\r\n");   // Text Mode
  delay(1000);
  
  Serial2.print("AT+CMGS=\"");
  Serial2.print(num);
  Serial2.print("\"\r\n");
  delay(1000);
  
  Serial2.println(msg);
  delay(500);
  Serial2.println((char)26);    //CTRL+Z  for end msg
  delay(500);
    
  #if defined(PrintSerialMonitor)
  Serial.print("Msg sent to ");
  Serial.print(num);
  Serial.print(" \r\n");
  #endif
  
  return 1;
  
}

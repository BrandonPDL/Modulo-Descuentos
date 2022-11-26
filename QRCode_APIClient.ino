#include <ArduinoJson.h> //Libreria para Json
#include <SoftwareSerial.h> //Libreria para conexion Serial
#include <ArduinoJson.hpp>
#include <WiFi.h>
#include <HTTPClient.h>
#include <analogWrite.h>


//Definicion de pines TX Y RX
#define MYPORT_TX 1 
#define MYPORT_RX 3
SoftwareSerial mySerial(MYPORT_RX, MYPORT_TX);

//Definicion de variables conexion WiFi
const char* ssid = "Doffice"; //Red WiFi
const char* password = "dicon15491"; //Contraseña WiFi

//Definicion pines led
int pin_red = 15;
int pin_green = 4;
int pin_blue = 0;
int pin_buzzer = 2;


String qr = ""; //Variable QR
String ns = "0EC0FF0C19E3D554"; // Variable Numero serie 
int countcad=0;
char data; //Variable de byte QR

String bufferSD, ServerResponse, ServerResponseSD, bufferSL;
int response, responseSD; 
String token, result;



void setup()
{
  mySerial.begin(9600);
  Serial.begin(9600);

  pinMode(pin_red, OUTPUT);
  pinMode(pin_green, OUTPUT);
  pinMode(pin_blue, OUTPUT);
  pinMode(pin_buzzer, OUTPUT);
  analogWrite(pin_red, 255);
  analogWrite(pin_green, 255);
  analogWrite(pin_blue, 255);
  //analogWrite(pin_buzzer, 0);

  mySerial.println("Hello world?");
  //Inicializacion Wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connectando...");
  while(WiFi.status() != WL_CONNECTED) 
  {
    analogWrite(pin_buzzer, 0);
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado WiFi");
  //Espera a que wifi se conecte 
  HttpClientRequest ();
}
void SerializeObjectLogin() //Funcion para crear Json
{
  StaticJsonDocument<100> docs; 
  docs["user"]="admin";
  docs["password"]="4321";
  docs["signature"]= "0EC0FF0C19E3D554";
  serializeJson(docs, bufferSL);
  Serial.println(bufferSL);
}

void SerializeObject() //Funcion para crear Json 
{
  
  //Serial.println(token);
  StaticJsonDocument<300> doc;
  doc["qrcode"]=qr;
  doc["token"]=token;
  doc["signature"]=ns;
  bufferSD = "";
  serializeJson(doc, bufferSD);
  Serial.print("bufferSD :");
  Serial.println(bufferSD);
}

void HttpClientRequest ()
{
  if (WiFi.status() == WL_CONNECTED) 
  {
    analogWrite(pin_blue, 0);
    analogWrite(pin_buzzer, 0);
    //Inicializa Cliente HTTP
    HTTPClient http;
    SerializeObjectLogin();
    //Comenzar ectLogin();
    http.begin("http://192.168.2.53:9874/seguripass/movil/login");
    http.addHeader("Content-Type", "application/json");
    response = http.POST(bufferSL);+
    Serial.print("response1: ");
    Serial.println(response);
    if(response == 201)
    {
      analogWrite(pin_blue, 255);
      analogWrite(pin_green, 0);
      analogWrite(pin_buzzer, 255);
      delay(300);
      analogWrite(pin_green, 255);
      analogWrite(pin_buzzer, 0);
      ServerResponse = http.getString();
      Serial.print("ServerResponse: ");
      Serial.println(ServerResponse);
      DeserializacionObject();
    }
  else 
    {
      analogWrite(pin_blue, 255);
      analogWrite(pin_red, 0);
      analogWrite(pin_buzzer, 255);
      delay(800);
      analogWrite(pin_red, 255);
      analogWrite(pin_buzzer, 0);  
    }
  http.end();
  }
  else 
  {
    Serial.println("Error de conexion WiFi");
  }
}

void DeserializacionObject()
{
  StaticJsonDocument<300> docd;
  DeserializationError error = deserializeJson(docd, ServerResponse);
  Serial.println(ServerResponse);
  if (error) 
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    //return "error";
  }
  const char* toke;
  toke = docd["token"];
  Serial.print("toke :");
  Serial.println(toke);
  String tok(toke);
  token =tok;
  Serial.print("token: ");
  Serial.println(token);
  //analogWrite(pin_blue, 255);
  //return token;
}
void DeserializacionObjectSD()
{
  StaticJsonDocument<300> docd;
  DeserializationError error = deserializeJson(docd, ServerResponseSD);
  if (error) 
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    //return "error";
  }
  const char* resul;
  resul = docd["result"];
  String tok(resul);
  result =tok;
}
void HttpClientRequestSD ()
{
  analogWrite(pin_blue, 0);
  delay(5);
  if (WiFi.status() == WL_CONNECTED) 
  {
    delay(500);
    //Inicializa Cliente HTTP
    HTTPClient http;
    SerializeObject();
    //Comenzar solicitud HTTP   
    http.begin("http://192.168.2.53:9874/seguripass/movil/descuento");
    http.addHeader("Content-Type", "application/json");
    responseSD = http.POST(bufferSD);
    analogWrite(pin_blue, 255);
    if (responseSD == 201)
    {
      ServerResponseSD = http.getString();
      Serial.print("ServerResponseSD :");
      Serial.println(ServerResponseSD);
      DeserializacionObjectSD();
      Serial.print("result :");
      Serial.println(result);
      int led = result.toInt();
      Serial.println(led);
      if (led == 1)
      {
          //verde
        analogWrite(pin_green, 0);
        analogWrite(pin_buzzer, 255);
        delay(150);
        analogWrite(pin_green, 255);
        analogWrite(pin_buzzer, 0);
        delay(25);
        analogWrite(pin_green, 0);
        analogWrite(pin_buzzer, 255);
        delay(150);
        analogWrite(pin_green, 255);
        analogWrite(pin_buzzer, 0);
      }
      else
      {
        analogWrite(pin_red, 0);
        analogWrite(pin_buzzer, 255);
        delay(500);
        analogWrite(pin_red, 255);
        analogWrite(pin_buzzer, 0);
        delay(150);
        analogWrite(pin_red, 0);
        analogWrite(pin_buzzer, 255);
        delay(500);
        analogWrite(pin_red, 255);
        analogWrite(pin_buzzer, 0);
      }
    }
    else 
    {
      analogWrite(pin_blue, 255);
      analogWrite(pin_red, 0);
      analogWrite(pin_buzzer, 255);
      delay(800);
      analogWrite(pin_red, 255);
      analogWrite(pin_buzzer, 0);  
    }
    http.end();
  }
  else 
  {
    Serial.println("Error de conexion WiFi");
  }
}
void loop()
{
  if(mySerial.available()>0)
  {
    qr ="";
    countcad=0;
    while((mySerial.available()>0) && (countcad < 49))
    {
      data = mySerial.read();
      qr += data;
      countcad++;
      delay(2);
      //mySerial.print("dentro:");
      //mySerial.println(qr);
    }
    Serial.println(countcad);
    if(countcad >48)
    {
      HttpClientRequestSD ();
    }
  }
}


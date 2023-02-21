#include <SoftwareSerial.h>
#include <avr/wdt.h>
#include <EEPROM.h>
#include "SIM900.h"
#include "inetGSM.h"
//#include "sms.h"   ////para sms/////
//SMSGSM sms;         ////para sms////
InetGSM inet;
const int  Vf1 = A0, Vf2=A1, Vf3=A2, Cf1=A4, Cf2=A5, Cf3=A6, Lev=A7, Vdc=A14, Vbat=A15;
float Volf1,Volf2, Volf3, Corf1, Corf2, Corf3,VolRec, Level, VolBat, Frecuencia, Energia, FpA, FpB, FpC;
int Discret,reset=0;
unsigned long Contador=0, Tiempo=0, ultima=0, TimeEnergia=0;
char end_c[2];
bool AlarmaReal=false, started=false,PtaPrin, PtaGenDC,PtaChelf, Mov,Vibra, GenOn, GenOnLoad,FaultGen,AcFail,SiteOnBatery,LuzObs,Otra;
char Cel11[11], Cel22[11], site22[20], msg[50];
//String msg1="Activacion de Sensores en: ";

void setup() 
{
     end_c[0]=0x1a;
     end_c[1]='\0';
Serial.begin(9600);
pinMode(2, INPUT_PULLUP); // interrucion FRECUENCIA
pinMode(3, INPUT_PULLUP); // entrada 5
pinMode(4, INPUT_PULLUP); // entrada 6
pinMode(5, INPUT_PULLUP); // entrada 7
pinMode(6, INPUT_PULLUP); // entrada 8
pinMode(7, INPUT_PULLUP); // entrada 9
pinMode(8, INPUT_PULLUP); // entrada 10
pinMode(9, INPUT_PULLUP); // entrada 11
pinMode(10, INPUT_PULLUP); // entrada 12
pinMode(11, INPUT_PULLUP); // run config
pinMode(14, INPUT_PULLUP); // entrada 4
pinMode(15, INPUT_PULLUP); // entrada 3
pinMode(16, INPUT_PULLUP); // entrada 2
pinMode(17, INPUT_PULLUP); // entrada 1
pinMode(20, INPUT_PULLUP); // Interruptor de frecuencia
pinMode(21, OUTPUT); // rst de modulo
pinMode(42, OUTPUT); // Alarma Mayor
pinMode(43, OUTPUT); // Alarma Critica
//EEPROM.put(0,0.0000);//para inicializar configurar en cero antes de enviar
EEPROM.get(0,Energia);
EEPROM.get(300,Cel11);
EEPROM.get(400,Cel22);
EEPROM.get(500,site22);
//msg1.concat(site22);
//msg1.toCharArray(msg, 50);
 //Serial.println(msg);
 // wdt_disable(); // Desactivar el watchdog mientras se configura
 //wdt_enable(WDTO_8S); // Configurar a dos segundos
 while (digitalRead(11)==1){configurar();}

  activardatos();
  
};
/////////////////////hasta aqui el inicio.////////////////////////////////////////

void loop() 
{
void LeerSensores();
bool enviardata();
bool DetectarEvento(int);
  LeerSensores();
  if (enviardata()==false){
    activardatos();
    }
  if(DetectarEvento(200)==true){ //220 Ciclo 2 minutos
    enviardata();
    }
 
 if(millis()-TimeEnergia>3600000){
  EEPROM.put(0,Energia);
  }
};

/////// FUNSION PARA ACTIVAR LOS DATOS////////////////////////
      boolean activardatos()
  {
  Serial.println("Probando Modulo");
  digitalWrite(21,LOW);
  delay(1200);
  digitalWrite(21,HIGH);
  delay(5000);
  //wdt_reset();
  delay(5000);
  //wdt_disable();
   int i=0,c=0;
   for (int b=0;b<3;b++){
  if (gsm.begin(9600)){                         //Start configuration of shield with baudrate.
    Serial.println("\nModulo OK");
    started=true;  
    break;
  }
  else {Serial.println("\nModulo no reconocido");
  started=false;
  i++;}}
  EEPROM.get(58,c);
  for (int b=0;b<3;b++){
  switch (c) {
  case 1:
      if (inet.attachGPRS("lte.avantel.com.co","","")){
      Serial.println("\n APN RECIBIDO Avantel");
      b=3;
      }
      else {Serial.println("APN ERROR Avantel");
      }
    break;
  case 2:
      if (inet.attachGPRS("internet.comcel.com.co","comcel","comcel")){
      Serial.println("\n APN RECIBIDO Claro");
      b=3;
      }
      else {Serial.println("APN ERROR Claro");
     }
    break;
  case 3:
      if (inet.attachGPRS("internet.movistar.com.co","movistar","movistar")){
      Serial.println("\n APN RECIBIDO Movistar");
      b=3;
      }
      else {Serial.println("APN ERROR Movistar");
     }
    break;
  case 4:
      if (inet.attachGPRS("web.colombiamovil.com.co","","")){
      Serial.println("\n APN RECIBIDO Tigo");
      b=3;
      }
      else {Serial.println("APN ERROR Tigo");
     }
     break;
 case 5:
      if (inet.attachGPRS("web.vmc.net.co","","")){
      Serial.println("\n APN RECIBIDO Virgin");
      b=3;
      }
      else {Serial.println("APN ERROR Virgin");
     }
    break;    
  default:
Serial.println("\n Operador no configurado");
    break;
} 
    delay(500);
    }
    
//wdt_enable(WDTO_8S);
//wdt_reset();
  delay(3000);
//wdt_reset();  resetea toda la arduino si se han apagado los datos 20 veces
if(reset>=20){
wdt_enable(WDTO_8S);
}
reset++;
}
    
//////////////// FUNCION PARA ENVIAR LOS DATOS //////////////////////////////////
   bool enviardata(){
    int decimal(float), dec, fases, tipo, auxx;
    float Field1=Volf1,Field2=Volf2,Field3=Volf3,Field4=Corf1,Field5=Corf2,Field6=Corf3,Field7=VolRec;// default
    EEPROM.get(65,fases);
    EEPROM.get(70,tipo);
    EEPROM.get(110,auxx);
    if ((fases==2)&&(tipo ==2)){
      Field7=GenOnLoad; // MG on Load
      }else if((fases==1)&&(tipo==1)){
        Field3=AcFail; // AC Fail
        Field6=SiteOnBatery; // Site on Batery
        }else if((fases==1)&&(tipo==2)){
          Field3=AcFail; // AC Fail
          Field6=GenOn;     // entrada 6 MG Running
          Field7=GenOnLoad; // MG on Load
          }
      boolean env=0;
     int x=(String(String(Field1)+String(Field2)+String(Field3)+String(Field4)+String(Field5)+String(Field6)+String(Field7)+String(Discret))).length();
     x=x+(String(String(Level)+String(VolBat)+String(Frecuencia)+String(Energia)+String(FpA)+String(FpB)+String(FpC))).length();
    Serial.println(x);
     // wdt_disable();
    if (inet.connectTCP("api.thingspeak.com", 80)==1){ 
     Serial.println("conectado al servidor \n\r subiendo data... www");
     gsm.SimpleWrite("POST /update HTTP/1.1\r\n");
     gsm.SimpleWrite("Host: api.thingspeak.com\r\n");
     gsm.SimpleWrite("Connection: close\r\n");
     gsm.SimpleWrite("X-THINGSPEAKAPIKEY:");
     gsm.SimpleWrite("CDX9Z4SNJXW14CDW\r\n"); //APIKEY
     gsm.SimpleWrite("Content-Type: application/x-www-form-urlencoded\r\n");
     gsm.SimpleWrite("Content-Length:");
     gsm.SimpleWrite(x+77);
     gsm.SimpleWrite("\r\n\r\nfield1=");   
     gsm.SimpleWrite(int(Field1));
     gsm.SimpleWrite(".");
     dec=decimal(Field1);
     if (dec<10){ gsm.SimpleWrite("0");}
     gsm.SimpleWrite(dec);
     gsm.SimpleWrite("&field2=");
     gsm.SimpleWrite(int(Field2));
     gsm.SimpleWrite(".");
     dec=decimal(Field2);
     if (dec<10){ gsm.SimpleWrite("0");}
     gsm.SimpleWrite(dec);
     gsm.SimpleWrite("&field3=");
     gsm.SimpleWrite(int(Field3));
     gsm.SimpleWrite(".");
     dec=decimal(Field3);
     if (dec<10){ gsm.SimpleWrite("0");}
     gsm.SimpleWrite(dec);
     gsm.SimpleWrite("&field4=");
     gsm.SimpleWrite(int(Field4));
     gsm.SimpleWrite(".");
     dec=decimal(Field4);
     if (dec<10){ gsm.SimpleWrite("0");}
     gsm.SimpleWrite(dec);
     gsm.SimpleWrite("&field5=");
     gsm.SimpleWrite(int(Field5));
     gsm.SimpleWrite(".");
     dec=decimal(Field5);
     if (dec<10){ gsm.SimpleWrite("0");}
     gsm.SimpleWrite(dec);
     gsm.SimpleWrite("&field6=");
     gsm.SimpleWrite(int(Field6));
     gsm.SimpleWrite(".");
     dec=decimal(Field6);
     if (dec<10){ gsm.SimpleWrite("0");}
     gsm.SimpleWrite(dec);
     gsm.SimpleWrite("&field7=");
     gsm.SimpleWrite(int(Field7));
     gsm.SimpleWrite(".");
     dec=decimal(Field7);
     if (dec<10){ gsm.SimpleWrite("0");}
     gsm.SimpleWrite(dec);
     gsm.SimpleWrite("&field8=");
     gsm.SimpleWrite(Discret);
//-----------------Status---------------------------------------------
 gsm.SimpleWrite("&status=");   
     gsm.SimpleWrite(int(Level));
     gsm.SimpleWrite(".");
     dec=decimal(Level);
     if (dec<10){ gsm.SimpleWrite("0");}
     gsm.SimpleWrite(dec);
     gsm.SimpleWrite(":");
     gsm.SimpleWrite(int(VolBat));
     gsm.SimpleWrite(".");
     dec=decimal(VolBat);
     if (dec<10){ gsm.SimpleWrite("0");}
     gsm.SimpleWrite(dec);
     gsm.SimpleWrite(":");
     gsm.SimpleWrite(int(Frecuencia));
     gsm.SimpleWrite(".");
     dec=decimal(Frecuencia);
     if (dec<10){ gsm.SimpleWrite("0");}
     gsm.SimpleWrite(dec);
     gsm.SimpleWrite(":");
     gsm.SimpleWrite(int(Energia));
     gsm.SimpleWrite(".");
     dec=decimal(Energia);
     if (dec<10){ gsm.SimpleWrite("0");}
     gsm.SimpleWrite(dec);
     gsm.SimpleWrite(":");
     gsm.SimpleWrite(int(FpA));
     gsm.SimpleWrite(".");
     dec=decimal(FpA);
     if (dec<10){ gsm.SimpleWrite("0");}
     gsm.SimpleWrite(dec);
     gsm.SimpleWrite(":");
     gsm.SimpleWrite(int(FpB));
     gsm.SimpleWrite(".");
     dec=decimal(FpB);
     if (dec<10){ gsm.SimpleWrite("0");}
     gsm.SimpleWrite(dec);
     gsm.SimpleWrite(":");
     gsm.SimpleWrite(int(FpC));
     gsm.SimpleWrite(".");
     dec=decimal(FpC);
     if (dec<10){ gsm.SimpleWrite("0");}
     gsm.SimpleWrite(dec);
////////////////////////////////////////////////////////////
     
     gsm.SimpleWrite("\r\n\r\n");
     gsm.SimpleWrite(end_c);
     Serial.println("Data subida...");
     Serial.println("\nData recibida:"); 
    //wdt_reset();
    delay(3000);
    gsm.WhileSimpleRead();  //Read until serial buffer is empty.
    delay(3000);
    inet.disconnectTCP();
    
     return true;
    }else {
      Serial.println("sin conexion al servidor www"); 
      gsm.SimpleWriteln("AT+CIPSHUT"); 
      gsm.SimpleWriteln(end_c); 
      inet.disconnectTCP();
       gsm.WhileSimpleRead();
       started=false;
     Serial.println("\nData recibida:"); 
    //wdt_reset();
    delay(3000);
    gsm.WhileSimpleRead();  //Read until serial buffer is empty.
    delay(3000);
    inet.disconnectTCP();
      return false;
      } 
    }

//////////////// FUNCION PARA LEER LOS SENSORES //////////////////////////////////

void LeerSensores() 
{
 int i,c,contador=1;
 long aux=0;
 float ccal1=0, ccal2=0, ccal3=0, caux2=0, caux=0, voaux=0, voaux2=0;

 //--------------- Voltajes CA---------------

  for (i=0;i<400;i++){
    voaux2=analogRead(Vf1);
    voaux=voaux+voaux2;
    if(voaux2>0){contador++;}
  }
  voaux2=voaux/contador;
  Volf1=voaux2*0.265;

   delay(400); voaux=0; voaux2=0; contador=1;

  for (i=0;i<400;i++){
    voaux2=analogRead(Vf2);
    voaux=voaux+voaux2;
    if(voaux2>0){contador++;}
  }
  voaux2=voaux/contador;
  Volf2=voaux2*0.265;

   delay(400); voaux=0; voaux2=0; contador=1;

  for (i=0;i<400;i++){
    voaux2=analogRead(Vf3);
    voaux=voaux+voaux2;
    if(voaux2>0){contador++;}
  }
  voaux2=voaux/contador;
  Volf3=voaux2*0.265;
  
  delay(400); voaux=0; voaux2=0; contador=1;

//---------------- Corrientes CA -------------------------
EEPROM.get(600,ccal1);
EEPROM.get(700,ccal2);
EEPROM.get(800,ccal3);
caux=0; caux2=0;
for (i=0;i<400;i++){
         caux2=analogRead(Cf1);
         if(caux2>=ccal1){
         caux=caux+pow((caux2-ccal1),2); 
         }else{
          caux=caux+pow((ccal1-caux2),2);
              }   
                  }
        caux2=sqrt(caux/400);
        Serial.println(caux2);
        Corf1=(caux2*0.142); // Corf1=(caux2*0.3364)-172.36; Con recistencia burden de 34ohm// 100 Ohm ->Corf1=(caux2*0.1091)-55.75;
        caux=0; caux2=0;
for (i=0;i<400;i++){
         caux2=analogRead(Cf2);
         if(caux2>=ccal2){
         caux=caux+pow((caux2-ccal2),2); 
         }else{
          caux=caux+pow((ccal2-caux2),2);
              }   
                  }
        caux2=sqrt(caux/400);
        Serial.println(caux2);
        Corf2=(caux2*0.142);  // Corf2=(caux2*0.3364)-172.36; Con recistencia burden de 34ohm// 100 Ohm ->Corf1=(caux2*0.1091)-55.75;
        caux=0; caux2=0;
for (i=0;i<400;i++){
         caux2=analogRead(Cf3);
         if(caux2>=ccal3){
         caux=caux+pow((caux2-ccal3),2); 
         }else{
          caux=caux+pow((ccal3-caux2),2);
              }   
                  }
        caux2=sqrt(caux/400);
        Serial.println(caux2);
        Corf3=(caux2*0.142);   // Corf3=(caux2*0.3364)-172.36; Con recistencia burden de 34ohm// 100 Ohm ->Corf1=(caux2*0.1091)-55.75;
        caux=0; caux2=0; 
               
   if (Corf1<0.3){Corf1=0;}
   if (Corf2<0.3){Corf2=0;}
   if (Corf3<0.3){Corf3=0;}


//------------------ Voltaje Rectificador --------------------
  float Vcal;
  EEPROM.get(900,Vcal);
  VolRec=(analogRead(Vdc)*0.076-72.7)-Vcal;

  // ----------------- Nivel de ACPM  -------------
  float alt, levaux;;
  int p;
  
  alt=analogRead(Lev)*0.234-42.2; // 0  -  169cm LECTURA DEL SENSOR 220OHM
  EEPROM.get(75,p); // lee el perimetro
  levaux=p*0.005;
  Level=levaux*alt;
  if(Level<=0){Level=0;}
  // ---------- Bateria Gestión --------------------

  VolBat=(analogRead(Vbat)*0.021);  //((33/10)*5)/1024

  //---------------------Temperatura ---------------

  //------------------- Factor de potencia ---------------
  unsigned long Periodo, dt1=0, dt2=0;
  int aux1=0, aux2=0, aux3=0, aux4=0;
  Periodo=micros();
  while(abs(micros()-Periodo)<16666){ // un periodo de la RC
    aux3=analogRead(Vf1);
   if (aux3>aux1){
    aux1=aux3;
    dt1=micros();
    }
    aux4=analogRead(Cf1);
   if (aux4>aux2){
    aux2=aux4;
    dt2=micros();
    }
   }
   FpA=abs(dt2-dt1);// diferencia de tiempo en microsegundos
   //Serial.println(FpA);
   if(FpA<-4166||FpA>4166){FpA=0;} //si supera los 90 grados tiempo cero 
   FpA=FpA*0.000377; // microsegundos a radianes phi
   FpA=cos(FpA); // factor de potencia Coseno de phi
   aux1=0, aux2=0, aux3=0, aux4=0, dt1=0, dt2=0;
     Periodo=micros();
  while(abs(micros()-Periodo)<16666){ // un periodo de la RC
    aux3=analogRead(Vf2);
   if (aux3>aux1){
    aux1=aux3;
    dt1=micros();
    }
    aux4=analogRead(Cf2);
   if (aux4>aux2){
    aux2=aux4;
    dt2=micros();
    }
   }
   FpB=abs(dt2-dt1);// diferencia de tiempo en microsegundos
   //Serial.println(FpB);
   if(FpB<-4166||FpB>4166){FpB=0;} //si supera los 90 grados tiempo cero 
   FpB=FpB*0.000377; // microsegundos a radianes phi
   FpB=cos(FpB); // factor de potencia Coseno de phi
   aux1=0, aux2=0, aux3=0, aux4=0, dt1=0, dt2=0;
     Periodo=micros();
  while(abs(micros()-Periodo)<16666){ // un periodo de la RC
    aux3=analogRead(Vf3);
   if (aux3>aux1){
    aux1=aux3;
    dt1=micros();
    }
    aux4=analogRead(Cf3);
   if (aux4>aux1){
    aux2=aux4;
    dt2=micros();
    }
   }
   FpC=abs(dt2-dt1);// diferencia de tiempo en microsegundos
   //Serial.println(FpC);
   if(FpC<-4166||FpC>4166){FpC=0;} //si supera los 90 grados tiempo cero 
   FpC=FpC*0.000377; // microsegundos a radianes phi
   FpC=cos(FpC); // factor de potencia Coseno de phi

   if (Volf1==0){FpA=1;}
   if (Volf2==0){FpB=1;}
   if (Volf3==0){FpC=1;}


   
  //----------------Potencia por hora consumida KW/h--------------

float Pot1, Pot2, Pot3, h=0.0000;
Pot1=(Volf1*Corf1)/1000; //Kw/h Fase 1
Pot2=(Volf2*Corf2)/1000;
Pot3=(Volf3*Corf3)/1000;
h=millis()-Tiempo;
h=h/3600000; // Fraccion de hora para la suma
Energia=Energia+(Pot1+Pot2+Pot3)*h; //KW/h

//---------------Frecuencia--------------------------

attachInterrupt(3, cero, FALLING); // activar la interrupcion de la frecuencia
Tiempo=millis();
Contador=0;
while(abs(millis()-Tiempo)<1999){
  delay(1);}
detachInterrupt(3);// desactiva la interrupcion de la frecuencia 
Frecuencia = Contador/4;


//---------------entradas discretas -------------------
DetectarEvento(1);

//---------------musta los valores en pantalla------------------------
  Serial.println("VF1");
  Serial.println(Volf1);
  Serial.println("VF2");
  Serial.println(Volf2);
  Serial.println("VF3");
  Serial.println(Volf3);
  Serial.println("CF1");
  Serial.println(Corf1);
  Serial.println("CF2");
  Serial.println(Corf2);
  Serial.println("CF3");
  Serial.println(Corf3);
  Serial.println("VolRec");
  Serial.println(VolRec);
  Serial.println("NivelACPM");
  Serial.println(Level);
  Serial.println("VoltajeBateria");
  Serial.println(VolBat);
  Serial.println("Discreto");
  Serial.println(Discret, BIN);  
  Serial.println(Discret);
  Serial.println("Frecuencia");
  Serial.println(Frecuencia); 
  Serial.println("Tiempo");
  Serial.println(Tiempo); 
  Serial.println("Fraccion hora"); 
  Serial.println(h,4);
  Serial.println("Energia");
  Serial.println(Energia,4);
  Serial.println("Factor de potencia");
  Serial.println(FpA); 
  Serial.println(FpB);
  Serial.println(FpC);

}
//-------------------Funcion para leer entradas discrtas y detectare eventos-------------

bool DetectarEvento(int n){
  
  int auxx, DiscreteAnt;
  bool Evento=false;
  
  DiscreteAnt=Discret;
  Serial.println("En espera de evento ...");
  for(int i=1;i<=n;i++){
  
  EEPROM.get(85,auxx);
  PtaPrin=digitalRead(17);
  if(auxx==0){PtaPrin=not(digitalRead(17));}  // entrada 1 Pta princi
  EEPROM.get(90,auxx);
  PtaGenDC=digitalRead(16);
  if(auxx==0){PtaGenDC=not(digitalRead(16));} // entrada 2 Pta Gen DC
  EEPROM.get(95,auxx);
  PtaChelf=digitalRead(15);
  if(auxx==0){PtaChelf=not(digitalRead(15));}; // entrada 3 Pta chelter  
  EEPROM.get(100,auxx);
  Mov=digitalRead(14);
  if(auxx==0){Mov=not(digitalRead(14));}      // entrada 4 movimento
  EEPROM.get(105,auxx);
  Vibra=digitalRead(3);
  if(auxx==0){Vibra=not(digitalRead(3));}     // entrada 5 sensor de vibracion
  EEPROM.get(110,auxx);
  GenOn=digitalRead(4);
  if(auxx==0){GenOn=not(digitalRead(4));}     // entrada 6 Running
  EEPROM.get(115,auxx);
  GenOnLoad=digitalRead(5);
  if(auxx==0){GenOnLoad=not(digitalRead(5));} // entrada 7 MG on Load
  EEPROM.get(120,auxx);
  FaultGen=digitalRead(6);
  if(auxx==0){FaultGen=not(digitalRead(6));}  // entrada 8 Falla del MG
  EEPROM.get(125,auxx);
  AcFail=digitalRead(7);
  if(auxx==0){AcFail=not(digitalRead(7));}  // entrada 9 Falla de red comercial
  EEPROM.get(130,auxx);
  SiteOnBatery=digitalRead(8);
  if(auxx==0){SiteOnBatery=not(digitalRead(8));}  // entrada 10 Sitio en baterías del rectificador
  EEPROM.get(135,auxx);
  LuzObs=digitalRead(9);
  if(auxx==0){LuzObs=not(digitalRead(9));}// Luces de obtruccion opcional 11
  EEPROM.get(140,auxx);
  Otra=digitalRead(10);
  if(auxx==0){Otra=not(digitalRead(10));} // Entrada 12 obcional

  Discret=1<<12;
  Discret=Discret|PtaPrin<<11;
  Discret=Discret|PtaGenDC<<10;
  Discret=Discret|PtaChelf<<9;
  Discret=Discret|Mov<<8;
  Discret=Discret|Vibra<<7;
  Discret=Discret|GenOn<<6;
  Discret=Discret|GenOnLoad<<5;
  Discret=Discret|FaultGen<<4;
  Discret=Discret|AcFail<<3;
  Discret=Discret|SiteOnBatery<<2;
  Discret=Discret|LuzObs<<1;
  Discret=Discret|Otra;

  
  if(Discret!=DiscreteAnt){
    Serial.println("Detecto evento");
    i=n+1;
    return true;
    }
    delay(500);
  }//end for
  return false;
  }

//---------------Interreupciones para sensor frecuencia--------------------
void cero() {
Contador++;  
}

//---------------sacar la parte decimal de los datos--------------------
int decimal(float x){
  int entero=int(x);
  int decemal=abs(int((x-entero)*100));
  return decemal;
  }
//---------------Interreupciones para sensor de movieminto y vibracion--------------------
  
void Alarma (){
AlarmaReal=true;
}
//---------------Alerta de las puertas y de los sensores de presencia--------------------


 
  /////////////////////////////////////////////Configurar///////////////////////////

  void configurar (){
   startconfig:;
     String pass="", aux="";
     char Cel1[11], site[20];
     float caux=0, caux2=0, Corf1=0, Corf2=0, Corf3=0;
     int c, i;
     unsigned long time1=0, time2=0;
     Serial.println("Password..."); 
     pass=escribir();
    if (pass!="ATCC1234"){
      Serial.println("\nPassword error");
      goto startconfig;
      }else{
        menu:;
Serial.println("\nQue Desea Configurar?\n");
Serial.println("\n1. ID y Nombre Sitio");
Serial.println("\n2. Numeros de Telefono");
Serial.println("\n3. Selec Operador");
Serial.println("\n4. Numero de Fases");
Serial.println("\n5. Tipo de Solucion");
Serial.println("\n6. Medidas Tanque");
Serial.println("\n7. Entradas Discretas");
Serial.println("\n8. Calibracion CTs");
Serial.println("\n9. Calibracion -48V DC");
Serial.println("\n10. Ver config Actual\n");
        aux=escribir();
        if((aux.toInt()<=0)or(aux.toInt()>10)){
          Serial.println("\nEscoja una Opcion Valida");
          goto menu;}
          
          switch(aux.toInt()){
case 1:
goto marca1;
break;
case 2:
goto marca2;
break;
case 3:
goto marca3;
break;
case 4:
goto marca4;
break;
case 5:
goto marca5;
break;
case 6:
goto marca6;
break;
case 7:
goto marca7;
break;
case 8:
goto marca8;
break;
case 9:
goto marca9;
break;
case 10:
goto marca10;
break;
          }

  marca1:;
         Serial.println("ID y Nombre del Sitio"); // para almacenar el nombre del sitio
          aux=escribir();
          aux.toCharArray(site, 20);
          EEPROM.put(500,site);
          EEPROM.get(500,site);
          Serial.println(site);
          goto menu;
          
 marca2:; 
        Serial.println("Telefono 1");
        aux=escribir();
        if((aux.length()!=10)or(aux.toInt()==0)){
          Serial.println("\nNo valido");
          goto marca2;}
          aux.toCharArray(Cel1, 11);
          EEPROM.put(300,Cel1);
          EEPROM.get(300,Cel1);
          Serial.println(Cel1);
          marca12:;
        Serial.println("Telefono 2");
        aux=escribir();
        if((aux.length()!=10)or(aux.toInt()==0)){
          Serial.println("\nNo valido");
          goto marca12;}
          aux.toCharArray(Cel1, 11);
          EEPROM.put(400,Cel1);
          EEPROM.get(400,Cel1);
          Serial.println(Cel1);
          goto menu;
  
  marca3:;
          Serial.println("\nSelec Operador...");
          Serial.println(" ");
          Serial.println("1. Avantel");
          Serial.println("2. Claro");
          Serial.println("3. Movistar");
          Serial.println("4. Tigo");
          Serial.println("5. Virgin");
          aux=escribir();
        if((aux.toInt()<=0)or(aux.toInt()>5)){
          Serial.println("\nNo valido");
          goto marca3;}
          EEPROM.put(58,aux.toInt());
          goto menu;
          
  marca4:;
          Serial.println("\nSelec Numero de fases...");
          Serial.println(" ");
          Serial.println("1. Bifasico");
          Serial.println("2. Trifasico");          
          aux=escribir();
        if((aux.toInt()<=0)or(aux.toInt()>2)){
          Serial.println("\nNo valido");
          goto marca4;}
          EEPROM.put(65,aux.toInt());
          goto menu;
          
  marca5:;
          Serial.println("\nSelec Tipo Solucion...");
          Serial.println(" ");
          Serial.println("1. DC");
          Serial.println("2. MG");
          Serial.println("3. N/A");          
          aux=escribir();
        if((aux.toInt()<=0)or(aux.toInt()>3)){
          Serial.println("\nNo valido");
          goto marca5;}
          EEPROM.put(70,aux.toInt());
          goto menu;
          
   marca6:;
          Serial.println("\nMedidas del Tanque...");
          Serial.println("\nPerimetro (cm)");
          aux=escribir();
          if((aux.toInt()<=0)or(aux.toInt()>450)){
          Serial.println("\nNo valido");
          goto marca6;}
          EEPROM.put(75,aux.toInt());
          marca62:;
          Serial.println("\nAltura (cm)");
          aux=escribir();
          if((aux.toInt()<=0)or(aux.toInt()>300)){
          Serial.println("\nNo valido");
          goto marca62;}
          EEPROM.put(80,aux.toInt());
          goto menu;
            
                    //---------------CONFIG DE ENTRADAS DISCRETAS-------------
   marca7:;      
          Serial.println("\nCONFIGUACION DE ENTRADAS DISCRETAS");
          for (int d=1; d<=12; d++){
           marca72:;
          Serial.print("Entrada: ");
          Serial.println(d); 
          Serial.println("1. N/C");
          Serial.println("0. N/A"); 
          aux=escribir();
          if((aux!="0")and(aux.toInt()!=1)){
          Serial.println("\nNo valido");
          goto marca72;}
          EEPROM.put(80+(5*d),aux.toInt()); // Aumente la posicion de la memoria en 5
          }
          goto menu;
          //------------------------Calibracion de CT´s-------------------
    marca8:;
          Serial.println("\nDesea calibrar lo CT´s?");
          Serial.println("\nPara calibrar desconecte los CT´s");
          Serial.println("1. SI");
          Serial.println("0. NO");
          aux=escribir();
          if((aux!="0")and(aux.toInt()!=1)){
          Serial.println("\nNo valido");
          goto marca8;}
          if(aux.toInt()==1){
          Serial.println("\nPor favor espere ...");
          for (i=0;i<400;i++){
          caux2=analogRead(Cf1);
          caux=caux+caux2;
                   }
          caux2=caux/400;
          EEPROM.put(600,caux2);
          EEPROM.get(600,caux2);
          Serial.println(caux2);
          delay(500); caux2=0; caux=0;         
//--------------fase 2-------------------------
          for (i=0;i<400;i++){
          caux2=analogRead(Cf2);
          caux=caux+caux2;
                   }
          caux2=caux/400;
          EEPROM.put(700,caux2);
          EEPROM.get(700,caux2);
          Serial.println(caux2);
          delay(500); caux2=0; caux=0;
////-------------------fase 3------------------
          for (i=0;i<400;i++){
          caux2=analogRead(Cf3);
          caux=caux+caux2;
                   }
          caux2=caux/400;
          EEPROM.put(800,caux2);
          EEPROM.get(800,caux2);
          Serial.println(caux2);
          delay(500); caux2=0; caux=0;
           }
          goto menu;

 //-------------------------CALIBRACION DE -48VDC-----------------------
marca9:;
          Serial.println("\nDesea calibrar -48V DC?");
          Serial.println("1. SI");
          Serial.println("0. NO");
          aux=escribir();
          if((aux!="0")and(aux.toInt()!=1)){
          Serial.println("\nNo valido");
          goto marca9;}
          if(aux.toInt()==1){
          Serial.println("\nPara calibrar tome la lectura del voltaje REC e ingresela");
          Serial.println("\nincluyendo el signo");
          aux=escribir();
          if((aux.toInt()>=0) or (aux.toInt()<-56)){
          Serial.println("\nNo valido");
          goto marca9;} 
          VolRec=(analogRead(Vdc)*0.076-72.7)-aux.toFloat();
          EEPROM.put(900,VolRec);
          delay(500);
          EEPROM.get(900,VolRec);
          Serial.println(VolRec);        
          }
          goto menu;
//------------------------------- VER CONFIGURACION ACTUAL---------------------------          
 marca10:;
          Serial.println("\nID NAME: ");
          EEPROM.get(500,site);
          Serial.println(site);
          Serial.println("\nTelefono1: ");
          EEPROM.get(300,Cel1);
          Serial.println(Cel1);
          Serial.println("\nTelefono2: ");
          EEPROM.get(400,Cel1);
          Serial.println(Cel1);
          Serial.print("\nOperador: ");
          EEPROM.get(58,c);
          switch(c){
          case 1:   
          Serial.println("Avantel");
          break;
          case 2: 
          Serial.println("Claro");
          break;
          case 3:
          Serial.println("Movistar");
          break;
          case 4:
          Serial.println("Tigo");
          break;
          case 5:
          Serial.println("Virgin");
          break;
          }
          EEPROM.get(65,c);
          Serial.print("\n# Fases: ");
          switch(c){
          case 1:   
          Serial.println("2");
          break;
          case 2: 
          Serial.println("3");
          break; }
          
          EEPROM.get(70,c);
          Serial.print("\nSolucion: ");
          switch(c){
          case 1:   
          Serial.println("DC");
          break;
          case 2: 
          Serial.println("MG");
          break;
          case 3:
          Serial.println("N/A"); 
          break; }     
          EEPROM.get(75,c);
          Serial.print("\nPeri Tanque: ");
          Serial.println(c); 
          EEPROM.get(80,c);
          Serial.print("\nAlto: ");
          Serial.println(c); 

          Serial.println("\nENTRADAS DISCRETAS");
          for (int d=1; d<=12; d++){
          Serial.print("Entrada: ");
          Serial.println(d); 
          EEPROM.get(80+(5*d),c);
          switch(c){
          case 1:   
          Serial.println("NC");
          break;
          case 0: 
          Serial.println("NO");
          break;
          }
          }
          Serial.println("\nPara empezar poner en modo Run y presione reset");
          goto menu;

        }
  }

    /////////////////////////leer Strinng//////////////////////////
    String escribir(){
    delay(20);
    //Se crea una variable que servirá como buffer
    String bufferString = "";
    char p="";
    while (1>0) {
      if (Serial.available() > 0){
        p=Serial.read();
        if (p == 10){break;}
      bufferString += p;
    }
      }
      return bufferString;
    }
  


#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <MCUFRIEND_kbv.h>   // Hardware-specific library
MCUFRIEND_kbv tft;

File myFile;


int IntensityNoon = 0; //globalni prommenne
int IntensityNow = 0;
int IntensityBefore = 0;
int Intensity = 0;
int IntensityMax = 0;
int cas = 0;
int casPoledne = 0;
int soucasnycas = 0;
int den = 0; //mysleno den jako svetlo nebo noc ne jako pondeli utery
int denBefore = 0; //toto uz vypisuje
int radek = 0;
int timestampNoon = 0;
int minutaNoon = 0;
int hodinaNoon = 0;
int hodnota = 0;
int casMax = 0;

String message;

const int C_VERB_DEBUG = 2;
const int C_VERB_NORM = 1;
const int C_VERB_LOW = 0;

const int C_VERBOSITY = C_VERB_DEBUG;

//----------------------------------------------------------------------------------

void Tisk (String message, int verbose) {
  if (verbose <= C_VERBOSITY) {
    Serial.println(message);    
  }
}

void setup() {
  

  Serial.begin(9600);
  
//-----------KOMUNIKACE-S-SD-KARTOU--------------  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }



  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done."); 
  myFile = SD.open("test.txt");
  

//-----------OTEVRENI SOUBORU--------------

   if (myFile) {   
    while (myFile.available()) {  
      String list  = myFile.readStringUntil('\n');      //ted to asi funguje

//-------------INTENSITA-----------------------      
      Intensity = list.toInt();  //list je string ale  
      
      message = "Intensita světla - ";
      message = message + Intensity;
      Tisk(message, C_VERB_NORM);    
      
      
      IntensityBefore = IntensityNow; //Hodnota intenzity svetla ted(predchozi mereni) se ulozi do Before
      IntensityNow = Intensity; //a IntensityNow se updatuje novou intensitou
//----------------TIMESTAMP-A-RADEK--------------------------------      
      cas = cas +5; //hodiny se posunou o 5 minut tzv jedno mereni   
      radek++;
      //Serial.println(casPoledne);
      message = "Radek ";
      message = message + radek;
      Tisk(message, C_VERB_DEBUG);      


      message = "timestamp - ";
      message = message + cas + " min";
      Tisk(message, C_VERB_NORM);      
//-------------FUNKCE--------------------------      
      DayOrNight(); //volani funkci

      if (den == 1){
        NoonDetection();
        ZobrazCas();
        TimePredict();

          NightPredict(); 
        

      }

      //Serial.println("Funkce called"); 
           
      Serial.println("-----------------------");  
 
//------------DELAY----------------------------      
      delay(500);//5 minut v ms


    }
    // close the file:
    myFile.close();
   } 
   else {
      // if the file didn't open, print an error:
      Serial.println("error opening test.txt");
   } 

}


//-----------------------------LOOP------------------------------------- F
void loop() {


  //Intensity = analogRead(A0); //z fotodiody si precte intenzitu a ulozi do Intensity
  //Serial.println("svetlo"); // vypise do serialu
  //Serial.println(Intensity); // vypise do serialu
  //Intensity = random(60, 1000);
  //Intensity == Intensity++; //testovani, FUNGUJE
  
}

//----------------------DAY-OR-NIGHT------------------------------------------ F
void DayOrNight(){ 
  denBefore = den;  //mysleno den jako svetlo nebo noc ne jako pondeli utery
  if (IntensityNow > 100){
    den = 1;
    message = "DEN";
    Tisk(message, C_VERB_NORM);  
  }
  
  else {
    den = 0;
    message = "NOC";
    Tisk(message, C_VERB_NORM);   
  }
  
  if(denBefore < den){
    cas = 0;
    casPoledne = 0; //kdyz vyjde slunce vynuluje cas poledne a cas aby se v dalsich dnech nematly hodiny
  }
  
}

//-------------------------------NOON-DETECTION------------------------------- FFF
void NoonDetection(){     
  if(IntensityMax > IntensityNow){
    hodnota++;                             
  }
  else{
    hodnota = 0;
  }
  if (hodnota == 5){ 
        IntensityNoon = IntensityMax;
        casPoledne = casMax;
        Serial.println("Hodnota = 5 ");         
        Serial.print("timestampNoon - ");
        Serial.println(timestampNoon);
        message = "Pred 25 min bylo Poledne";                                                                   //TOHLE UŽ FUNGUJE!!!
        Tisk(message, C_VERB_DEBUG);
        timestampNoon = casPoledne;  
  }     
  if(IntensityNow > IntensityBefore){ 
    if(IntensityNow > IntensityMax){
      IntensityMax = IntensityNow;
      casMax = cas;  
      message = "Nejvyšší naměřená hodnota so far";
      Tisk(message, C_VERB_DEBUG);            
   }    
    else{
      message = "Není Poledne";
      Tisk(message, C_VERB_DEBUG); 
   }          
 }  
  else{
      message = "Není Poledne";
      Tisk(message, C_VERB_NORM); 
       
  }        
}


//----------------------------ZOBRAZ-CAS--------------------------------F
void ZobrazCas(){ //zobrazuje cas, funguje pouze po poledni
  int hodina = 0;                                                                     //Jak to udelat aby jelo jen po poledni?
  int minuta = 0;

//----------------------------------------

      soucasnycas = cas - casPoledne; //pocita kolik je prave hodin od poledne
      hodina = soucasnycas / 60 + 12; //vypocet hodin
      minuta = soucasnycas % 60; //vypocet minut

      message = "Aktualni cas";
      message = message + hodina + ":" + minuta;
      Tisk(message, C_VERB_NORM);           

}

//------------------------------NIGHT-PREDICT------------------------------ F
void NightPredict(){ //predpovida zapad slunce
  int casNight = 0; //promenne
  int hodinaNight = 0;
  int minutaNight = 0;                                                                  
  casNight = casPoledne;
  hodinaNight = casNight / 60 + 12;
  minutaNight = casNight % 60;  

//------------------------------------

  message = "Predpovezeny cas vecera";
  message = message + hodinaNight + ":" + minutaNight;
  Tisk(message, C_VERB_NORM);          
  
}

//----------------------------------------------------------------------------------------
void TimePredict(){
   hodinaNoon =(720 - (timestampNoon - cas))/60; 
   minutaNoon =((720 - (timestampNoon - cas))%60);
  
  message = "Predpovezeny Cas";
  message = message + hodinaNoon + ":" + minutaNoon;
  Tisk(message, C_VERB_NORM);          
}  

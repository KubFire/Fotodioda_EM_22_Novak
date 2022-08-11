//-----------------------KNIHOVNY---------------------------------------
#include <SPI.h> 
#include <SD.h>

File myFile;

//-----------------------PROMENNE-------------------------------
int IntensityNoon = 0; 
int IntensityNow = 0;
int IntensityBefore = 0;
int Intensity = 0;
int IntensityMax = 0;
int cas = 0;
int casPoledne = 0;
int soucasnycas = 0;
int den = 0;                           //mysleno den jako svetlo nebo noc ne jako pondeli utery
int denBefore = 0; 
int radek = 0;
int timestampNoon = 0;
int minutaNoon = 0;
int hodinaNoon = 0;
int hodnota = 0;
int casMax = 0;

String message;
const int C_VERB_DEBUG = 2; //(Debugovací)
const int C_VERB_NORM = 1; //(Normální)
const int C_VERB_LOW = 0;  //(pro Graf v Excelu)

const int C_VERBOSITY = C_VERB_DEBUG;

//-------------------------VERBOSITY-----------------------------------

void Tisk (String message, int verbose) { //Slozitejsi nahrada Serial.print
  if (verbose <= C_VERBOSITY) {
    Serial.println(message);    
  }
}
//---------------------------SETUP------------------------------------ F
void setup() {
  

  Serial.begin(9600);                   //Komunikace
  
//-----------KOMUNIKACE-S-SD-KARTOU--------------  
  while (!Serial) {
    ; 
  }
  Serial.print("Initializace SD karty..."); 

  if (!SD.begin(4)) {
    Serial.println("initializace neúspěšná!");
    while (1);
  }
  Serial.println("initializace úspěšná"); 
  myFile = SD.open("data.txt"); 

//-----------OTEVRENI SOUBORU-------------- F 

   if (myFile) {   
    while (myFile.available()) {  
      String list  = myFile.readStringUntil('\n');      

//-------------INTENSITA----------------------- F     
      Intensity = list.toInt();           //prevod ze stringu na int //jen testovaci
      
      message = "Intensita světla - ";    //VERBOSITY
      message = message + Intensity;
      Tisk(message, C_VERB_NORM);    
     
      
      IntensityBefore = IntensityNow;     //Hodnota intenzity svetla ted(predchozi mereni) se ulozi do Before
      IntensityNow = Intensity;           //a IntensityNow se updatuje novou intensitou
//----------------TIMESTAMP-A-RADEK-------------------------------- F     
      cas = cas +5; //hodiny se posunou o 5 minut tzv jedno mereni   
      radek++;
      //Serial.println(casPoledne);
      message = "Radek ";               //VERBOSITY
      message = message + radek;
      Tisk(message, C_VERB_DEBUG);      


      message = "timestamp - ";         //VERBOSITY
      message = message + cas + " min";
      Tisk(message, C_VERB_NORM);      
//-------------FUNKCE-------------------------- F      
      DayOrNight(); //volani funkci

      if (den == 1){ //Vola Funkce co funguji jen ve dnee
        NoonDetection(); //detekuje poledne
        ZobrazCas();   //ukazuje    cas odpoledne
        TimePredict(); //predpovida cas dopoledne
        NightPredict(); //predpovida cas zapadu slunce
      }         
//------------DELAY---------------------------- F     
      delay(50);
//----------------------------------------------
      message = "celkove mereni - "; //mereni do excelu
      message = message + radek + "; " + Intensity + "; " + cas + "; " + den + "; " + timestampNoon + "; ";
      Tisk(message, C_VERB_LOW);   
      
      Serial.println("-----------------------");       
    }
    myFile.close();
   } 
   else {
      Serial.println("error při otevírání data.txt");
   } 
}


//-----------------------------LOOP------------------------------------- F
void loop() {
  
//-------------INTENSITA-----------------------      
  Intensity = analogRead(A0);       //z fotodiody si precte intenzitu a ulozi do Intensity
  message = "Intensita světla - ";    //VERBOSITY
  message = message + Intensity;
  Tisk(message, C_VERB_NORM);    
  
  
  IntensityBefore = IntensityNow;     //Hodnota intenzity svetla ted(predchozi mereni) se ulozi do Before
  IntensityNow = Intensity;           //a IntensityNow se updatuje novou intensitou
//----------------TIMESTAMP-A-RADEK--------------------------------      
  cas = cas +5; //hodiny se posunou o 5 minut tzv jedno mereni   
  radek++;
  //Serial.println(casPoledne);
  message = "Radek ";               //VERBOSITY
  message = message + radek;
  Tisk(message, C_VERB_DEBUG);      
  
  
  message = "timestamp - ";         //VERBOSITY
  message = message + cas + " min";
  Tisk(message, C_VERB_NORM);      
//-------------FUNKCE--------------------------      
  DayOrNight(); //volani funkci
  
  if (den == 1){ //Vola Funkce co funguji jen ve dnee
    NoonDetection(); //detekuje poledne
    ZobrazCas();   //ukazuje    cas odpoledne
    TimePredict(); //predpovida cas dopoledne
    NightPredict(); //predpovida cas zapadu slunce
  }         
  

//----------------------------------------------
  message = "celkove mereni - "; //mereni do excelu
  message = message + radek + "; " + Intensity + "; " + cas + "; " + den + "; " + timestampNoon + "; ";
  Tisk(message, C_VERB_NORM);   
  
  Serial.println("-----------------------");  
//------------DELAY----------------------------      
  delay(300000);//5 min v milisec   
}

//----------------------DAY-OR-NIGHT------------------------------------------ F
void DayOrNight(){ 
  denBefore = den;                  //mysleno den jako svetlo nebo noc ne jako pondeli utery
  if (IntensityNow > 100){          //pokud intenzita 100+ = den a vypise do VERBOSITY
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
    casPoledne = 0;                   //kdyz vyjde slunce vynuluje cas(timestamp) poledne a cas aby se v dalsich dnech nematly hodiny
  }
  
}

//-------------------------------NOON-DETECTION------------------------------- FFF
void NoonDetection(){                 //zjisti kdy je poledne
  if(IntensityMax > IntensityNow){    //s kazdym merenim kdy je intensity nizsi nez max se pricte 1
    hodnota++;                             
  }
  else{
    hodnota = 0;                      //pokud je intensity vyssi hodnota se vynuluje
  }
  if (hodnota == 5){                  //pokud se pricetlo 5krat znamena to ze je poledne. Prepisou se hodnoty a vypise do VERBOSITY
        IntensityNoon = IntensityMax;
        casPoledne = casMax;
        Serial.println("Hodnota = 5 ");         
        Serial.print("timestampNoon - ");
        Serial.println(timestampNoon);
        message = "Pred 25 min bylo Poledne";                                                                  
        Tisk(message, C_VERB_DEBUG);
        timestampNoon = casPoledne;  
  }     
  if(IntensityNow > IntensityBefore){ //Pokud je intenzita ted vetsi jak minule mereni 
    if(IntensityNow > IntensityMax){  //Pokud je intenzita ted vetsi jak nejvyssi mereni
      IntensityMax = IntensityNow;    //Do Nejvyssi intenzity se ulozi toto mereni
      casMax = cas;                   //a do timestamp max mereni se ulozi nynejsi timestamp
      message = "Nejvyšší naměřená hodnota so far"; //Vypise VERBOSITY
      Tisk(message, C_VERB_DEBUG);            
   }    
    else{
      message = "Není Poledne";     //pokud intensity ted neni vyssi jak nejvyssi namerena
      Tisk(message, C_VERB_DEBUG);  //VERBOSITY - neni poledne
   }          
 }  
  else{
      message = "Není Poledne";   //pokud intensity ted neni vyssi jak predchozi namerena
      Tisk(message, C_VERB_NORM); //VERBOSITY - neni poledne
       
  }        
}


//----------------------------ZOBRAZ-CAS--------------------------------F
void ZobrazCas(){ //zobrazuje cas, funguje pouze po poledni
  int hodina = 0;       
  int minuta = 0;

//----------------------------------------

      soucasnycas = cas - casPoledne; //pocita kolik je prave hodin od poledne
      hodina = soucasnycas / 60 + 12; //vypocet hodin
      minuta = soucasnycas % 60; //vypocet minut

      message = "Aktualni cas(Odpoledne)";
      message = message + hodina + ":" + minuta;
      Tisk(message, C_VERB_NORM);           

}

//------------------------------NIGHT-PREDICT------------------------------ F
void NightPredict(){ //predpovida zapad slunce
  int casNight = 0; //promenne
  int hodinaNight = 0;
  int minutaNight = 0;                                                                  
  casNight = casPoledne;  //vypocet
  hodinaNight = casNight / 60 + 12; //vypocet hodiny
  minutaNight = casNight % 60;  //vypocet minuty

//------------------------------------

  message = "Predpovezeny cas vecera"; //VERBOSITY casu
  message = message + hodinaNight + ":" + minutaNight;
  Tisk(message, C_VERB_NORM);          
  
}

//----------------------------------------------------------------------------------------
void TimePredict(){ //Predpovida cas dopoledne, pomoci timestamp poledne minuly den
   hodinaNoon =(720 - (timestampNoon - cas))/60;  //Vypocet 
   minutaNoon =((720 - (timestampNoon - cas))%60);
  
  message = "Predpovezeny Cas(Dopoledne)"; //VERBOSITY
  message = message + hodinaNoon + ":" + minutaNoon;
  Tisk(message, C_VERB_NORM);          
}  

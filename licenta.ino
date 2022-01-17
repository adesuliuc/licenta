#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <dht.h>

// se începe prin introducerea bibliotecilor necesare pentru utilizarea aplicației Blynk respectiv utilizarea microcontrolerului NodeMCU

char auth[] = "Vcuh7u2vB3wM4736bKvVsi35DxhWHdDj"; //token  
char ssid[] = "DESKTOP-MP3D0PI 4393"; //numele laptopului
char pass[] = "14157tV!";

WidgetLED astLed(V4);
WidgetLED functLed(V5);

//se inițializează ledurile din interfața aplicației: ledul de așteptare și cel de stropire.

//definim constantele de care avem nevoie
#define mpp1 14 //D5
#define mpp2 12 //D6
#define mpp3 13 //D7
#define mpp4 15 //D8
//pinii pentru motorașul pas cu pas
#define DC 5 //D9
//pinul la care este legat motorul de curent continuu
#define microS 4 //D2
//terminalul unde este legat micro comutatorul
#define stu 0 //D3
//pinul la care este legat senzorul de temperatura și umiditate
dht stu;

int pasi = 0;
int poz = 0;
int x = 0;
int nr_pasi = 0;
//declarăm variabile pentru pași, poziție, numărul total de pași și număr de pași pe care îl realizează MPP-ul.

int timer = 0;
int test = 0;
int timer1 = 0;
int timer2 = 0;
int debit = 0;
int distanta = 0;
int stropire = 0;
int l = 0;
//se inițializează parametrii virtuali ai aplicației Blynk


void setup()
{
//această funcție se apelează o singură dată la începutul rulării, unde se configurează pinii, se pornesc comunicațiile seriale, se realizează setările inițiale
    Serial.begin(9600);
     // se realizează conexiunea serială
    delay(500);
    Serial.println("STU\n\n");
    delay(1000);
    Blynk.begin(auth, ssid, pass);
     //și cea cu platforma Blynk
    pinMode(mpp1, OUTPUT);
    pinMode(mpp2, OUTPUT);
    pinMode(mpp3, OUTPUT);
    pinMode(mpp4, OUTPUT);
     //prin această funcție pinii MPP-ului se predefinesc ca ieșiri(OUTPUT-uri)
    pinMode(DC, OUTPUT);
     //se predefinește ca ieșire MCC-ul
    pinMode(microS, INPUT);
    //aici se micro comutatorul se predefinește ca intrare a sistemului
    pinMode(stu, INPUT);
    //senzotul de temperatura și umiditate se predefinește ca intrare(INPUT) 
    stop_mpp();
    //se apelează funcția de oprire a MPP-ului
}

void loop()
{
//această funcție se apelează la fiecare pas de eșantionare
    DHT.read11(stu);
    Serial.print("umiditatea curenta = ");
    Serial.print(DHT.humidity);
    Serial.print("%  ");
    Serial.print("temperatura = ");
    Serial.print(DHT.temperature); 
    Serial.println("C  ");
    delay(5000); //se așteaptă 5s înainte de a accesa din nou senzorul
    //aici se realizează afișarea în Serial Monitor a umidității și a temperaturii
    Blynk.run();
    //se începe rularea aplicației Blynk
    nr_pasi_c(distanta);
    if (timer == 1) 
    { 
     //se pornește timer-ul
        if (timer1 == 1 || timer2 == 1) 
        { 
         //odată setat timpul, se va realiza stropirea la momentul programat, cu ajutorul timerului, se stinge ledul de așteptare, se pornește cel de stropire, se ia în calcul debitul , iar în momentul în care actuatorul ajunge în home position se oprește ciclul, se stinge ledul de stropire și se aprinde cel de așteptare
            astLed.off();
            functLed.on();
            analogWrite(DC, debit);
//funcția analogWrite conține 3 factori, numele pinului digital și factorul de umplere
            funct_prog(x);
            analogWrite(DC, 0);
            homePosition();
            astLed.on();
            functLed.off();

        }
    }

    if (stropire == 1) 
    { 
     //când se apasă butonul de test complet se stinge ledul de așteptare, se aprinde cel de stropire se ține cont de volumul apei, de distanța care a fost programata prin glisor ca să o parcurgă actuatorul, iar în momentul în care s-au atins numărul de pași pe care MPP-ul a trebuit să îi producă ajunge în home position, se stinge ledul de stropire/funcționare și se aprinde cel de așteptare
        astLed.off();
        functLed.on();
        analogWrite(DC, debit);
        funct_prog(x);
        analogWrite(DC, 0);
        homePosition();
        astLed.on();
        functLed.off();
    }

    if (test == 1) 
    { 
     //în această condiție se impune cazul în care este apăsat testul poziției, inițial oprindu-se ledul de așteptare, pornind cel de funcționare, actuatorul se va deplasa pe poziția programată, iar odată ajuns înapoi în home position se va stinge ledul de stropire, urmând imediat să se aprindă cel de așteptare
        astLed.off();
        functLed.on();
        funct_prog(x);
        homePosition();
        astLed.on();
        functLed.off();
    }
}

BLYNK_WRITE(V0) 
{
    timer = param.asInt();
}
BLYNK_WRITE(V1) 
{
    test = param.asInt();
}
BLYNK_WRITE(V2) 
{
    timer1 = param.asInt();
}
BLYNK_WRITE(V3) 
{
    timer2 = param.asInt();
}
BLYNK_WRITE(V6) 
{
    debit = param.asInt();
}
BLYNK_WRITE(V7) 
{
    distanta = param.asInt();
}
BLYNK_WRITE(V8) 
{
    stropire = param.asInt();
}

//în liniile de cod de mai sus sunt sincronizați parametrii virtuali cu numerele butoanelor utilizate în realizarea interfeței aplicației, astfel aceasta este subrutina care verifică valorile parametrilor virtuali care vin de la aplicația Blynk, după care codul rulează în continuare, iar în funcție de alegerea utilizatorului, urmează să se apeleze una dintre subrutinele de mai jos. 

void nr_pasi_c(int distantaa) 
{
    x = (distantaa * (4076 / 4));
}

//funcția pentru calcularea numărului de pași, distanța pe care actuatorul este programat să o parcurgă

void homePosition() 
{
    l = digitalRead(microS);
    while (l == 0) 
    {
        m_pp(0);
        delay(1);
        l = digitalRead(microS);
    }
    stop_mpp();
    l = digitalRead(microS);
}

//funcția prin care actuatorul știe că a ajuns în home position, acest aspect fiind posibil cu ajutorul micro comutatorul care odată acționat de către actuator acesta dă semnal ca motorului pas cu pas să știe că a terminat ciclul și a ajuns în home position, ceea ce face oprirea motorului de tip stepper (pas cu pas)

void funct_prog(int total_p)
{
    while (nr_pasi < total_p) 
    {
        m_pp(1);
        delay(1);
        nr_pasi++;
    }
    stop_mpp();
    nr_pasi = 0;
}

//funcția care face MPP-ul să funcționeze, atâta timp cât numărul de pași pe care îî mai are de realizat MPP-ul este mai mic decât numărul total de pași pe care îi are de realizat, motorașul se mai mișcă cu câte un pas, verificând de fiecare dată condiția, în momentul în care numărul de pași realizați este egal cu numărul de pași programat să îi facă atunci MPP-ul se apelează funcția de oprire MPP-ului, își schimbă sensul, se întoarce în home position și se oprește

void m_pp(int directie) 
{
    for (int x = 0; x < 1; x++) 
    {
        Blynk.run();
        switch (pasi) 
        {
        case 0:
            digitalWrite(mpp1, LOW);
            digitalWrite(mpp2, LOW);
            digitalWrite(mpp3, LOW);
            digitalWrite(mpp4, HIGH);
            break;
        case 1:
            digitalWrite(mpp1, LOW);
            digitalWrite(mpp2, LOW);
            digitalWrite(mpp3, HIGH);
            digitalWrite(mpp4, HIGH);
            break;
        case 2:
            digitalWrite(mpp1, LOW);
            digitalWrite(mpp2, LOW);
            digitalWrite(mpp3, HIGH);
            digitalWrite(mpp4, LOW);
            break;
        case 3:
            digitalWrite(mpp1, LOW);
            digitalWrite(mpp2, HIGH);
            digitalWrite(mpp3, HIGH);
            digitalWrite(mpp4, LOW);
            break;
        case 4:
            digitalWrite(mpp1, LOW);
            digitalWrite(mpp2, HIGH);
            digitalWrite(mpp3, LOW);
            digitalWrite(mpp4, LOW);
            break;
        case 5:
            digitalWrite(mpp1, HIGH);
            digitalWrite(mpp2, HIGH);
            digitalWrite(mpp3, LOW);
            digitalWrite(mpp4, LOW);
            break;
        case 6:
            digitalWrite(mpp1, HIGH);
            digitalWrite(mpp2, LOW);
            digitalWrite(mpp3, LOW);
            digitalWrite(mpp4, LOW);
            break;
        case 7:
            digitalWrite(mpp1, HIGH);
            digitalWrite(mpp2, LOW);
            digitalWrite(mpp3, LOW);
            digitalWrite(mpp4, HIGH);
            break;
        default:
            digitalWrite(mpp1, LOW);
            digitalWrite(mpp2, LOW);
            digitalWrite(mpp3, LOW);
            digitalWrite(mpp4, LOW);
            break;
        }
        if (directie == 0) 
        {
            pasi++;
        }
        if (directie == 1) 
        {
            pasi--;
        }
        if (pasi > 7)
        {
            pasi = 0;
        }
        if (pasi < 0)
        {
            pasi = 7;
        }
    }
}

//în incipitul funcției se apelează funcția de mișcare a MPP-ulu, totodată funcție este pentru a realiza direcția și modul de mișcare a actuatorului, astfel există 8 cazuri schimbând în mod repetat pinurile care sunt pe HIGH, respectiv LOW, făcând 8 situații distincte + default unde motorul nu mai funcționează, iar mai apoi există o condiție if, care impune schimbarea de direcție
 
void stop_mpp() 
{
    digitalWrite(mpp1, LOW);
    digitalWrite(mpp2, LOW);
    digitalWrite(mpp3, LOW);
    digitalWrite(mpp4, LOW);
}
//funcția de oprire a MPP-ului unde toți pini care sunt legați de la motoraș la driver mai apoi la microcontroler sunt puși pe LOW

#include <EEPROM.h>
#include <SoftwareSerial.h> //Serve para "copiar" a funçao do RX e TX para outras portas a selecionar
#include <LiquidCrystal.h>
#include <TimerOne.h>
 
SoftwareSerial Bluetooth (A3, A4); //As portas 6 e 7 substituem as portas RX e TX respectivamente.
                                 //Lembrar que RX conecta ao TX do blue e TX conecta ao RX do bluetooth
char data, tarifa;

float Potencia_escolhida, Termo_fixo, Energia_hora, Vazio, Cheias, Sensor;
float Fixo_edp, Fixo_fenosa, Fixo_golden, Fixo_boa, Fixo_iberdrola, Fixo_ylce, Fixo_audax, Fixo_galp, Fixo_endesa;
float Hora_edp, Hora_fenosa, Hora_golden, Hora_boa, Hora_iberdrola, Hora_ylce, Hora_audax, Hora_galp, Hora_endesa;
float Vazio_edp, Vazio_fenosa, Vazio_golden, Vazio_boa, Vazio_iberdrola, Vazio_ylce, Vazio_audax, Vazio_galp, Vazio_endesa;
char calendario[13];
String comando, melhor; 
boolean alterar=false; 
volatile float Potencia, Corrente;
volatile int segundos, minuto, hora, dia=1, mes=1, ano, corrente_max, corrente_min, dias=2;
int i, d, p=1, vez=0, melhor2, meses=1;
int Gastos_dia[13][34], poupanca, Gastos_diario, Gastos_mensal, Gastos_hora;
volatile float Consumo_diario, Consumo_mensal, Consumo_mensal_enviar, Consumo_hora, edp, fenosa, golden, boa, iberdrola, ylce, audax, galp, endesa;

void EEPROMWriteInt(int address, int value); //2 Bytes
int  EEPROMReadInt(int address);

void EEPROMWriteLong(int address, long value); //4 Bytes
long EEPROMReadLong(int address);

void EEPROMWriteStr(int address, String value); //n Bytes
void EEPROMConcatStr(int address, String value);
String EEPROMReadStr(int address);

LiquidCrystal lcd(12,11,5,4,3,2);

void ContaTempo();

void ContaTempo(){     //timer de 1 seg para contagem do tempo
    segundos++;
  }

void setup() {
  lcd.begin (16,2);
  Serial.begin(9600);
  Bluetooth.begin(9600); //utilizar este serial porque houve mudanlas nas portas do RX e TX

  Timer1.initialize(1000000);
  Timer1.attachInterrupt(ContaTempo);
  lcd.clear();
  lcd.print ("A instalar");
  while (mes <12){
    Gastos_dia[mes][dia] = EEPROMReadInt(i);
    i=i+2;
    dia++;
    if (dia=33){
      mes++;
      dia=0;
    }
  }
}

void loop() { 
  corrente_max = 0;
  corrente_min = 1000;
  for (i=0; i<=200; i++){
    Sensor= analogRead (A0);
    if (Sensor >= corrente_max){
      corrente_max=Sensor;
    } else if (Sensor <= corrente_min){
      corrente_min= Sensor;
    }
    if (corrente_max <=512) corrente_max= 512;  
  }

  Corrente = (corrente_max - 512)*0.707 /3; //11.833 é um valor de ajuste //CorrenteRMS
  
  Potencia = 220*Corrente; //PotenciaRMS
  Consumo_hora= Consumo_hora + (Potencia *(0.5/60/60/1000)); //demora 0.2 a fazer o ciclo

  delay(200);
  
  lcd.clear();
  lcd.print ("Potencia: ");
  lcd.print (Potencia);
  lcd.print (" W");
  lcd.setCursor (0,1);
  lcd.print ("Corrente: ");
  lcd.print (Corrente);
  lcd.print (" Amp");

  if (segundos>59){
    segundos=0;
    minuto++;
    if (minuto>59){
      hora++;
      minuto=0;
      if (tarifa == 'a'){
        edp = edp + Consumo_hora * Hora_edp;
        fenosa= fenosa + Consumo_hora * Hora_fenosa;
        golden= golden + Consumo_hora * Hora_golden;
        boa= boa + Consumo_hora * Hora_boa;
        galp= galp + Consumo_hora * Hora_galp;
        iberdrola= iberdrola + Consumo_hora * Hora_iberdrola;
        endesa= endesa + Consumo_hora * Hora_endesa;
        audax= fenosa + Consumo_hora * Hora_audax;
        ylce= ylce + Consumo_hora * Hora_ylce;
        Gastos_hora = Consumo_hora * Energia_hora;
        Gastos_diario = Gastos_diario + Gastos_hora;
        
        Consumo_diario= Consumo_diario + Consumo_hora;
        Consumo_hora = 0;
        Gastos_hora = 0;
      }
      if (tarifa == 'b'){
        if ( 8 <= hora < 22){
          edp = edp + Consumo_hora * Hora_edp;
          fenosa= fenosa + Consumo_hora * Hora_fenosa;
          golden= golden + Consumo_hora * Hora_golden;
          boa= boa + Consumo_hora * Hora_boa;
          galp= galp + Consumo_hora * Hora_galp;
          iberdrola= iberdrola + Consumo_hora * Hora_iberdrola;
          endesa= endesa + Consumo_hora * Hora_endesa;
          audax= fenosa + Consumo_hora * Hora_audax;
          ylce= ylce + Consumo_hora * Hora_ylce;
          Gastos_hora = Consumo_hora * Energia_hora;
          Gastos_diario = Gastos_diario + Gastos_hora;
          
          Consumo_diario= Consumo_diario + Consumo_hora;
          Consumo_hora = 0;
          Gastos_hora = 0;
        } else {
          edp = edp + Consumo_hora * Vazio_edp;
          fenosa= fenosa + Consumo_hora * Vazio_fenosa;
          golden= golden + Consumo_hora * Vazio_golden;
          boa= boa + Consumo_hora * Vazio_boa;
          galp= galp + Consumo_hora * Vazio_galp;
          iberdrola= iberdrola + Consumo_hora * Vazio_iberdrola;
          endesa= endesa + Consumo_hora * Vazio_endesa;
          audax= fenosa + Consumo_hora * Vazio_audax;
          ylce= ylce + Consumo_hora * Vazio_ylce;
          Gastos_hora = Consumo_hora * Vazio;
          Gastos_diario = Gastos_diario + Gastos_hora;

          Consumo_diario= Consumo_diario + Consumo_hora;
          Consumo_hora = 0;
          Gastos_hora = 0;
        }
      }
      if (tarifa == 'c'){
        
      }
       if (hora>23){
          Gastos_dia[mes][dia]=Consumo_diario;
          EEPROMWriteInt(mes*dia ,Gastos_dia[mes][dia]);
          Gastos_mensal = Gastos_mensal + Gastos_diario;
          Consumo_mensal = Consumo_mensal + Consumo_diario;
          Consumo_diario=0;
          Gastos_diario = 0;
          dia++;
          hora=0;
          edp= edp + Fixo_edp; fenosa= fenosa + Fixo_fenosa;
          galp= galp + Fixo_galp; iberdrola=iberdrola + Fixo_iberdrola;
          audax=audax + Fixo_audax; ylce= ylce + Fixo_ylce;
          endesa=endesa + Fixo_endesa; golden=golden + Fixo_golden;
          boa=boa + Fixo_boa;
          if (Consumo_mensal > edp) {
            poupanca = Gastos_mensal - edp;
            melhor2 = edp;
            melhor = "EDP";
          } else if (melhor2 > fenosa) {
            poupanca = Gastos_mensal - fenosa;
            melhor2 = fenosa;
            melhor = "FENOSA";
          } else if (melhor2 > galp) {
            poupanca = Gastos_mensal - galp;
            melhor = "GALP";
            melhor2= galp;
          } else if (melhor2 > iberdrola) {
            poupanca = Gastos_mensal - iberdrola;
            melhor = "IBERDROLA";
            melhor2 = iberdrola;
          } else if (melhor2 > audax) {
            poupanca = Gastos_mensal - audax;
            melhor = "AUDAX";
            melhor2 = audax;
          } else if (melhor2 > ylce) {
            poupanca = Gastos_mensal - ylce;
            melhor = "YLCE";
            melhor2= ylce;
          } else if (melhor2 > endesa) {
            poupanca = Gastos_mensal - endesa;
            melhor = "ENDESA";
            melhor2 = endesa;
          } else if (melhor2 > golden) {
            poupanca = Gastos_mensal - golden;
            melhor = "GOLDEN";
            melhor2=golden;
          } else if (melhor2 > boa) {
            poupanca = Gastos_mensal - boa;
            melhor = "BOA";
            melhor2 = boa;
          } 
          if (mes==1||mes==3||mes==5||mes==7||mes==8||mes==10||mes==12){
            if (dia>31){
              dia=1;
              Gastos_dia[mes][32]=Consumo_mensal;
              Gastos_dia[mes][33]=Gastos_mensal;
              Consumo_mensal_enviar=Consumo_mensal;
              Consumo_mensal=0;
              mes++;
              if (mes>12){
                ano++;
                mes=1;
              }
            }
          }
          else 
          if (dia>30){
            Gastos_dia[mes][32]=Consumo_mensal;
            Gastos_dia[mes][31]=0;
            Gastos_dia[mes][33]=Gastos_mensal;
            Consumo_mensal_enviar=Consumo_mensal;
            Consumo_mensal=0;
            Gastos_mensal=0;
            dia=1;
            mes++;
          }
          }
    }
  } 
  if (Bluetooth.available())
  {         
        char data = Bluetooth.read();
        comando+= data;
        calendario[d]=data;
        d++;
        delay(10);
        if (d>11){
          if(vez==0)
          {
          dia=(calendario[0]-'0')*10+(calendario[1]-'0'); //converte char para int 
          Serial.println(dia);
          mes=(calendario[3]-'0')*10+(calendario[4]-'0');
          Serial.println(mes);
          ano=(calendario[6]-'0')*1000+(calendario[7]-'0')*100+(calendario[8]-'0')*10+(calendario[9]-'0');
          Serial.println(ano);
          d=0;
          vez++;
          }
        }
        if (d>12){
          if (vez==1)
          {
          hora=(calendario[0]-'0')*10+(calendario[1]-'0');
          Serial.println(hora);
          minuto=(calendario[3]-'0')*10+(calendario[4]-'0');
          Serial.println(minuto);
          vez++;
          comando ="";
          }
        }
        delay (10);

        if (comando.indexOf("sim") >=0) {
          comando="";
          p = 1;
          alterar=true; 
        }
        if (p == 1 && alterar==true){
          if (comando.indexOf( ",") >=0){
              Termo_fixo = comando.toFloat();
              //lcd.clear();
              //lcd.print("Termo fixo:");
              //lcd.setCursor(0, 1);
              //lcd.print(Termo_fixo);
              comando="";
              p=2;
          } 
        }
        if (p==2 && alterar == true){
          if (comando.indexOf( ",") >=0){
              Energia_hora = comando.toFloat();
              //lcd.clear();
              //lcd.print("Termo de consumo:");
              //lcd.setCursor(0, 1);
              //lcd.print(Energia_hora);
              comando="";
              p=3;
          }
        }
        if (p==3 && alterar == true){
          if (comando.indexOf( ",") >=0){
              Vazio = comando.toFloat();
              //lcd.clear();
              //lcd.print("Termo de vazio:");
              //lcd.setCursor(0, 1);
              //lcd.print(Vazio);
              comando="";
              p=4;
          }
        }
        if (p==4 && alterar == true){
          if (comando.indexOf( ",") >=0){
              Cheias = comando.toFloat();
              comando="";
              p=5;
          }
        }
               
        if (comando.indexOf("Consumo") >=0)
        {   
        lcd.clear();
        lcd.print("A enviar dados");
        Gastos_dia[meses][1]=11; Gastos_dia[meses][2]=12; Gastos_dia[meses][32]=8;
        delay(20);
        Bluetooth.println("{");
        Bluetooth.println(Gastos_dia[meses][1]);
        Bluetooth.println("}");
        delay(20);
        comando="";
        while (meses<=12){
           while (dias<=33){
             while (comando.indexOf("O") <=0){
                char data = Bluetooth.read();
                comando+= data;
                Serial.println(comando);
                delay(10);
             }
             Bluetooth.println("{");
             Bluetooth.println(Gastos_dia[meses][dias]);
             Bluetooth.println("}");
             Serial.println(dias);       
             dias++;
             comando="";
             
          }
          comando="";
          dias=1;
          meses++;
        }
        meses=1;
        comando="";
        i=0;

        while (i<=12){
           while (comando.indexOf ("O") <=0){
                char data = Bluetooth.read();
                comando+= data;
                Serial.println(comando);
                delay(20);
           }
          Bluetooth.print("{");
          /*if (i==0) Bluetooth.print(Consumo_mensal_enviar);
          if (i==1) Bluetooth.print(edp);
          if (i==2) Bluetooth.print(galp);
          if (i==3) Bluetooth.print(endesa);
          if (i==4) Bluetooth.print(fenosa);
          if (i==5) Bluetooth.print(golden);
          if (i==6) Bluetooth.print(boa);
          if (i==7) Bluetooth.print(iberdrola);
          if (i==8) Bluetooth.print(ylce);
          if (i==9) Bluetooth.print(audax);
          if (i==10) Bluetooth.print(melhor);
          if (i==11) Bluetooth.print(poupanca);
          if (i==12) Bluetooth.print(poupanca*12);*/
          if (i<10){
          Bluetooth.print (i);
          Bluetooth.println("}");
          } else {
          Bluetooth.print ("edpp");
          Bluetooth.println("}");
          }
          comando="";
          i++;
        }
     }
     
//////////////////////escolha do tarifário/////////////////////////////////////////
  if (comando.indexOf("Simples") >=0) { //se o recebido (comando) for igual a Simples
      tarifa = 'a';                       // a=simples, b=bihorario, c=trihorario
      if (Potencia_escolhida==1.15){
        Fixo_edp=0.1232; Fixo_fenosa=0.0842; Fixo_golden=0; Fixo_boa=0.1148; Fixo_iberdrola=0.111; Fixo_ylce=0.0885; Fixo_audax=0.1147; Fixo_galp=0.1429; Fixo_endesa=0;
        Hora_edp=0.1529 ; Hora_fenosa=0.1809 ; Hora_golden=0 ; Hora_boa=0.1619 ; Hora_iberdrola=0.149 ; Hora_ylce=0.1549 ; Hora_audax=0.1496 ; Hora_galp=0.1598 ; Hora_endesa=0 ;
      }
      if (Potencia_escolhida==2.3){
        Fixo_edp=0.1758; Fixo_fenosa=0.1331; Fixo_golden=0; Fixo_boa=0.1637; Fixo_iberdrola=0.158; Fixo_ylce=0.1549; Fixo_audax=0.1637; Fixo_galp=0.1885; Fixo_endesa=0;
        Hora_edp=0.1548 ; Hora_fenosa=0.1741 ; Hora_golden=0 ; Hora_boa=0.1619 ; Hora_iberdrola=0.149 ; Hora_ylce=0.1549 ; Hora_audax=0.1496 ; Hora_galp=0.1598 ; Hora_endesa=0 ;
      }
      if (Potencia_escolhida==3.45){
        Fixo_edp=0.2187; Fixo_fenosa=0.1821; Fixo_golden=0.1523; Fixo_boa=0.2127; Fixo_iberdrola=0.1659; Fixo_ylce=0.1599; Fixo_audax=0.2127; Fixo_galp=0.2027; Fixo_endesa=0.1983;
        Hora_edp=0.1493 ; Hora_fenosa=0.1672 ; Hora_golden=0.1557 ; Hora_boa=0.1619 ; Hora_iberdrola=0.1509 ; Hora_ylce=0.1549 ; Hora_audax=0.1496 ; Hora_galp=0.1598 ; Hora_endesa=0.1699 ;
      }
      if (Potencia_escolhida==4.6){
        Fixo_edp=0.2751; Fixo_fenosa=0.231; Fixo_golden= 0.1979; Fixo_boa=0.2616; Fixo_iberdrola=0.2156; Fixo_ylce=0.2129; Fixo_audax=0.2617; Fixo_galp=0.2603; Fixo_endesa=0.2594;
        Hora_edp=0.1534 ; Hora_fenosa=0.1668 ; Hora_golden=0.1557; Hora_boa=0.1619 ; Hora_iberdrola=0.1509 ; Hora_ylce=0.1549 ; Hora_audax=0.1496 ; Hora_galp=0.1598 ; Hora_endesa=0.1699 ;
      }
      if (Potencia_escolhida==5.75){
        Fixo_edp=0.3283; Fixo_fenosa=0.28; Fixo_golden=0.2633; Fixo_boa=0.3106; Fixo_iberdrola=0.265; Fixo_ylce=0.2619; Fixo_audax=0.3107; Fixo_galp=0.3206; Fixo_endesa=0.3195;
        Hora_edp=0.1546 ; Hora_fenosa=0.1665 ; Hora_golden=0.1557 ; Hora_boa=0.1619 ; Hora_iberdrola=0.1509 ; Hora_ylce=0.1549 ; Hora_audax=0.1496 ; Hora_galp=0.1598 ; Hora_endesa=0.1699 ;
      }
      if (Potencia_escolhida==6.9){
        Fixo_edp=0.3791; Fixo_fenosa=0.3269; Fixo_golden=0.3191; Fixo_boa=0.3596; Fixo_iberdrola=0.3145; Fixo_ylce=0.3099; Fixo_audax=0.3597; Fixo_galp=0.3888; Fixo_endesa=0.3773;
        Hora_edp=0.1544 ; Hora_fenosa=0.1659 ; Hora_golden=0.1559 ; Hora_boa=0.1619 ; Hora_iberdrola=0.1509 ; Hora_ylce=0.1549 ; Hora_audax=0.1496 ; Hora_galp=0.1598 ; Hora_endesa=0.1699 ;
      }
      if (Potencia_escolhida==10.35){
        Fixo_edp=0.5320; Fixo_fenosa=0.4738; Fixo_golden=0.4778; Fixo_boa=0.5065; Fixo_iberdrola=0.4629; Fixo_ylce=0.4631; Fixo_audax=0.5063; Fixo_galp=0.5874; Fixo_endesa=0.5545;
        Hora_edp=0.1544 ; Hora_fenosa=0.1655 ; Hora_golden=0.1564 ; Hora_boa=0.1619 ; Hora_iberdrola=0.1511 ; Hora_ylce=0.1549 ; Hora_audax=0.1496 ; Hora_galp=0.1598 ; Hora_endesa=0.1699 ;
      }
      if (Potencia_escolhida==13.8){
        Fixo_edp=0.6917; Fixo_fenosa=0.6206; Fixo_golden=0.6200; Fixo_boa=0.6533; Fixo_iberdrola=0.6113; Fixo_ylce=0.6138; Fixo_audax=0.6533; Fixo_galp=0.7722; Fixo_endesa=0.7402;
        Hora_edp=0.1559 ; Hora_fenosa=0.1654 ; Hora_golden=0.1564 ; Hora_boa=0.1619 ; Hora_iberdrola=0.1511 ; Hora_ylce=0.1549 ; Hora_audax=0.1496 ; Hora_galp=0.1598 ; Hora_endesa=0.1699 ;
      }
      if (Potencia_escolhida==17.25){
        Fixo_edp=0.8483; Fixo_fenosa=0.7669; Fixo_golden=0.7642; Fixo_boa=0.8002; Fixo_iberdrola=0.7596; Fixo_ylce=0.7648; Fixo_audax=0.8002; Fixo_galp=0.9479; Fixo_endesa=0.9298;
        Hora_edp=0.1571 ; Hora_fenosa=0.1649 ; Hora_golden=0.1564 ; Hora_boa=0.1619 ; Hora_iberdrola=0.1511 ; Hora_ylce=0.1549 ; Hora_audax=0.1496 ; Hora_galp=0.1598 ; Hora_endesa=0.1699 ;
      }
      if (Potencia_escolhida==20.7){
        Fixo_edp=1.0101; Fixo_fenosa=0.9138; Fixo_golden=0.8631; Fixo_boa=0.9471; Fixo_iberdrola=0.9079; Fixo_ylce=0.9158; Fixo_audax=0.9472; Fixo_galp=1.1339; Fixo_endesa=1.1147;
        Hora_edp=0.1580 ; Hora_fenosa=0.1646 ; Hora_golden=0.1564 ; Hora_boa=0.1619 ; Hora_iberdrola=0.1511 ; Hora_ylce=0.1549 ; Hora_audax=0.1496 ; Hora_galp=0.1598 ; Hora_endesa=0.1699 ;
      }
  }
    if (comando.indexOf("Bihorario") >=0) { //se o recebido (comando) for igual a Simples
      tarifa = 'b';                       // a=simples, b=bihorario, c=trihorario
      if (Potencia_escolhida==1.15){
        Fixo_edp=0; Fixo_fenosa=0; Fixo_golden=0; Fixo_boa=0; Fixo_iberdrola=0; Fixo_ylce=0; Fixo_audax=0; Fixo_galp=0.1429; Fixo_endesa=0;
        Hora_edp=0; Hora_fenosa=0; Hora_golden=0; Hora_boa=0; Hora_iberdrola=0; Hora_ylce=0; Hora_audax=0; Hora_galp=0.1996; Hora_endesa=0 ;
        Vazio_edp=0; Vazio_fenosa=0; Vazio_golden=0; Vazio_boa=0; Vazio_iberdrola=0; Vazio_ylce=0; Vazio_audax=0; Vazio_galp=0.0929; Vazio_endesa=0; 
      }
      if (Potencia_escolhida==2.3){
        Fixo_edp=0; Fixo_fenosa=0; Fixo_golden=0; Fixo_boa=0; Fixo_iberdrola=0; Fixo_ylce=0; Fixo_audax=0; Fixo_galp=0.1885; Fixo_endesa=0;
        Hora_edp=0; Hora_fenosa=0; Hora_golden=0; Hora_boa=0; Hora_iberdrola=0; Hora_ylce=0; Hora_audax=0; Hora_galp=0.1996; Hora_endesa=0;
        Vazio_edp=0; Vazio_fenosa=0; Vazio_golden=0; Vazio_boa=0; Vazio_iberdrola=0; Vazio_ylce=0; Vazio_audax=0; Vazio_galp=0.0929; Vazio_endesa=0; 

      }
      if (Potencia_escolhida==3.45){
        Fixo_edp=0.2282; Fixo_fenosa=0.1821; Fixo_golden=0.1673; Fixo_boa=0.2127; Fixo_iberdrola=0.1659; Fixo_ylce=0.1599; Fixo_audax=0.2127; Fixo_galp=0.2027; Fixo_endesa=0.1983;
        Hora_edp=0.1867 ; Hora_fenosa=0.1991 ; Hora_golden=0.1892 ; Hora_boa=0.201 ; Hora_iberdrola=0.1816 ; Hora_ylce=0.1869; Hora_audax=0.1792; Hora_galp=0.1996; Hora_endesa=0.2048;
        Vazio_edp=0.1098; Vazio_fenosa=0.1169; Vazio_golden=0.0867; Vazio_boa=0.103; Vazio_iberdrola=0.0992; Vazio_ylce=0.0997; Vazio_audax=0.1031; Vazio_galp=0.0929; Vazio_endesa=0.1267; 
      }
      if (Potencia_escolhida==4.6){
        Fixo_edp=0.28; Fixo_fenosa=0.231; Fixo_golden= 0.222; Fixo_boa=0.2616; Fixo_iberdrola=0.2156; Fixo_ylce=0.2129; Fixo_audax=0.2617; Fixo_galp=0.2603; Fixo_endesa=0.2594;
        Hora_edp=0.187 ; Hora_fenosa=0.1984 ; Hora_golden=0.1892; Hora_boa=0.201 ; Hora_iberdrola=0.1816 ; Hora_ylce=0.1869; Hora_audax=0.1792; Hora_galp=0.1996; Hora_endesa=0.2048;
        Vazio_edp=0.11; Vazio_fenosa=0.1163; Vazio_golden=0.0868; Vazio_boa=0.103; Vazio_iberdrola=0.0992; Vazio_ylce=0.0997; Vazio_audax=0.1031; Vazio_galp=0.0929; Vazio_endesa=0.1267; 
      }
      if (Potencia_escolhida==5.75){
        Fixo_edp=0.3317; Fixo_fenosa=0.28; Fixo_golden=0.2753; Fixo_boa=0.3106; Fixo_iberdrola=0.265; Fixo_ylce=0.2619; Fixo_audax=0.3107; Fixo_galp=0.3206; Fixo_endesa=0.3195;
        Hora_edp=0.1870 ; Hora_fenosa=0.1986 ; Hora_golden=0.1892; Hora_boa=0.201 ; Hora_iberdrola=0.1816 ; Hora_ylce=0.1869; Hora_audax=0.1792; Hora_galp=0.1996; Hora_endesa=0.2048;
        Vazio_edp=0.11; Vazio_fenosa=0.1165; Vazio_golden=0.0868; Vazio_boa=0.103; Vazio_iberdrola=0.0992; Vazio_ylce=0.0997; Vazio_audax=0.1031; Vazio_galp=0.0929; Vazio_endesa=0.1267; 
      }
      if (Potencia_escolhida==6.9){
        Fixo_edp=0.3832; Fixo_fenosa=0.3269; Fixo_golden=0.3292; Fixo_boa=0.3596; Fixo_iberdrola=0.3145; Fixo_ylce=0.3099; Fixo_audax=0.3597; Fixo_galp=0.3888; Fixo_endesa=0.3773;
        Hora_edp=0.187 ; Hora_fenosa=0.1975 ; Hora_golden=0.1899 ; Hora_boa=0.201 ; Hora_iberdrola=0.1816 ; Hora_ylce=0.1869; Hora_audax=0.1792; Hora_galp=0.1996; Hora_endesa=0.2048;
        Vazio_edp=0.11; Vazio_fenosa=0.1154; Vazio_golden=0.0868; Vazio_boa=0.103; Vazio_iberdrola=0.0992; Vazio_ylce=0.0997; Vazio_audax=0.103; Vazio_galp=0.0929; Vazio_endesa=0.1267; 
      }
      if (Potencia_escolhida==10.35){
        Fixo_edp=0.537; Fixo_fenosa=0.4738; Fixo_golden=0.4448; Fixo_boa=0.5065; Fixo_iberdrola=0.4629; Fixo_ylce=0.4631; Fixo_audax=0.5063; Fixo_galp=0.5874; Fixo_endesa=0.5545;
        Hora_edp=0.1866 ; Hora_fenosa=0.1972 ; Hora_golden=0.1892 ; Hora_boa=0.201 ; Hora_iberdrola=0.1833 ; Hora_ylce=0.1869; Hora_audax=0.1792; Hora_galp=0.1996; Hora_endesa=0.2048;
        Vazio_edp=0.1098; Vazio_fenosa=0.115; Vazio_golden=0.0964; Vazio_boa=0.103; Vazio_iberdrola=0.0993; Vazio_ylce=0.0997; Vazio_audax=0.103; Vazio_galp=0.0929; Vazio_endesa=0.1267; 
      }
      if (Potencia_escolhida==13.8){
        Fixo_edp=0.693; Fixo_fenosa=0.6206; Fixo_golden=0.5809; Fixo_boa=0.6533; Fixo_iberdrola=0.6113; Fixo_ylce=0.6138; Fixo_audax=0.6533; Fixo_galp=0.7722; Fixo_endesa=0.7402;
        Hora_edp=0.1871 ; Hora_fenosa=0.1967 ; Hora_golden=0.1892 ; Hora_boa=0.201 ; Hora_iberdrola=0.1833 ; Hora_ylce=0.1869; Hora_audax=0.1792; Hora_galp=0.1996; Hora_endesa=0.2048;
        Vazio_edp=0.11; Vazio_fenosa=0.1146; Vazio_golden=0.0964; Vazio_boa=0.103; Vazio_iberdrola=0.0993; Vazio_ylce=0.0997; Vazio_audax=0.103; Vazio_galp=0.0929; Vazio_endesa=0.1267; 
      }
      if (Potencia_escolhida==17.25){
        Fixo_edp=0.8483; Fixo_fenosa=0.7669; Fixo_golden=0.7018; Fixo_boa=0.8002; Fixo_iberdrola=0.7596; Fixo_ylce=0.7648; Fixo_audax=0.8002; Fixo_galp=0.9479; Fixo_endesa=0.9298;
        Hora_edp=0.1877 ; Hora_fenosa=0.1964 ; Hora_golden=0.1892 ; Hora_boa=0.201; Hora_iberdrola=0.1833; Hora_ylce=0.1869; Hora_audax=0.1792; Hora_galp=0.1996; Hora_endesa=0.2048;
        Vazio_edp=0.1107; Vazio_fenosa=0.1143; Vazio_golden=0.0964; Vazio_boa=0.103; Vazio_iberdrola=0.0993; Vazio_ylce=0.0997; Vazio_audax=0.103; Vazio_galp=0.0929; Vazio_endesa=0.1267; 
      }
      if (Potencia_escolhida==20.7){
        Fixo_edp=1.0039; Fixo_fenosa=0.9138; Fixo_golden=0.8431; Fixo_boa=0.9471; Fixo_iberdrola=0.9079; Fixo_ylce=0.9158; Fixo_audax=0.9472; Fixo_galp=1.1339; Fixo_endesa=1.1147;
        Hora_edp=0.188 ; Hora_fenosa=0.196 ; Hora_golden=0.1892 ; Hora_boa=0.201 ; Hora_iberdrola=0.1833; Hora_ylce=0.1869; Hora_audax=0.1792; Hora_galp=0.1996; Hora_endesa=0.2048;
        Vazio_edp=0.1108; Vazio_fenosa=0.1138; Vazio_golden=0.0964; Vazio_boa=0.103; Vazio_iberdrola=0.0993; Vazio_ylce=0.0997; Vazio_audax=0.103; Vazio_galp=0.0929; Vazio_endesa=0.1267; 
      }
  }
    if (comando.indexOf("Trihorario") >=0) { 
      tarifa = 'c';
    //Calcular valores
  }

///////////////////////escolha da operadora//////////////////////////////////////

////////////////EDP////////////////////

    if (comando.indexOf("Edp") >= 0) {
      if (Potencia_escolhida==1.15){
        if (tarifa == 'a'){
        Termo_fixo=0.1232;
        Energia_hora=0.1529;
        }
      } else if (Potencia_escolhida==2.3){
          if (tarifa == 'a'){
          Termo_fixo=0.1758;
          Energia_hora=0.1548;
          }
      } else if (Potencia_escolhida==3.45){
          if (tarifa == 'a'){
          Termo_fixo=0.2187;
          Energia_hora=0.1493;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.2282;
          Energia_hora=0.1867;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1098;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.2225;
          Energia_hora=0.2735;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.1571;
          Vazio=0.1037;
          }
      } else if (Potencia_escolhida==4.6){
          if (tarifa == 'a'){
          Termo_fixo=0.2751;
          Energia_hora=0.1534;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.2800;
          Energia_hora=0.1870;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1100;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.2727;
          Energia_hora=0.2738;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.1573;
          Vazio=0.1039;
          }
      } else if (Potencia_escolhida==5.75){
          if (tarifa == 'a'){
          Termo_fixo=0.3283;
          Energia_hora=0.1546;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.3317;
          Energia_hora=0.1870;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1100;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.3230;
          Energia_hora=0.2737;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.1572;
          Vazio=0.1038;
          }
      } else if ( Potencia_escolhida==6.9){
          if (tarifa == 'a'){
          Termo_fixo=0.3791;
          Energia_hora=0.1544;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.3832;
          Energia_hora=0.1870;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1100;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.3732;
          Energia_hora=0.2735;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.1571;
          Vazio=0.1038;
          }
      } else if (Potencia_escolhida == 10.35){
          if (tarifa == 'a'){
          Termo_fixo=0.5320;
          Energia_hora=0.1544;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.5370;
          Energia_hora=0.1866;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1098;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.5258;
          Energia_hora=0.2738;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.1572;
          Vazio=0.1038;
          }
      } else if ( Potencia_escolhida == 13.8){
          if (tarifa == 'a'){
          Termo_fixo=0.6917;
          Energia_hora=0.1559;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.6930;
          Energia_hora=0.1871;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1100;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.6779;
          Energia_hora=0.2738;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.1572;
          Vazio=0.1039;
          }
      } else if (Potencia_escolhida == 17.25){
          if (tarifa == 'a'){
          Termo_fixo=0.8493;
          Energia_hora=0.1571;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.8483;
          Energia_hora=0.1877;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1107;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.8273;
          Energia_hora=0.2727;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.1555;
          Vazio=0.1035;
          }
      } else if ( Potencia_escolhida == 20.7){
          if (tarifa == 'a'){
          Termo_fixo=1.0101;
          Energia_hora=0.1580;
          }
          if (tarifa == 'b'){
          Termo_fixo=1.0039;
          Energia_hora=0.1880;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1108;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.9754;
          Energia_hora=0.2730;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.1568;
          Vazio=0.1036;
          }
      }
    }

    /////////////////////ENDESA/////////////

    if (comando.indexOf("Endesa") >= 0) {
      if (Potencia_escolhida==1.15){
          
      } else if (Potencia_escolhida==2.3){
          
      } else if (Potencia_escolhida==3.45){
          if (tarifa == 'a'){
          Termo_fixo=0.1983;
          Energia_hora=0.1699;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.1983;
          Energia_hora=0.2048;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1267;
          }
      } else if (Potencia_escolhida==4.6){
          if (tarifa == 'a'){
          Termo_fixo=0.2594;
          Energia_hora=0.1699;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.2594;
          Energia_hora=0.2048;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1267;
          }
      } else if (Potencia_escolhida==5.75){
          if (tarifa == 'a'){
          Termo_fixo=0.3195;
          Energia_hora=0.1699;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.3195;
          Energia_hora=0.2048;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1267;
          }
      } else if ( Potencia_escolhida==6.9){
          if (tarifa == 'a'){
          Termo_fixo=0.3773;
          Energia_hora=0.1699;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.3773;
          Energia_hora=0.2048;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1267;
          }
      } else if (Potencia_escolhida == 10.35){
          if (tarifa == 'a'){
          Termo_fixo=0.5545;
          Energia_hora=0.1699;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.5545;
          Energia_hora=0.2048;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1267;
          }
      } else if ( Potencia_escolhida == 13.8){
          if (tarifa == 'a'){
          Termo_fixo=0.7402;
          Energia_hora=0.1699;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.7402;
          Energia_hora=0.2048;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1267;
          }
      } else if (Potencia_escolhida == 17.25){
          if (tarifa == 'a'){
          Termo_fixo=0.9298;
          Energia_hora=0.1699;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.9298;
          Energia_hora=0.2048;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1267;
          }
      } else if ( Potencia_escolhida == 20.7){
          if (tarifa == 'a'){
          Termo_fixo=1.1147;
          Energia_hora=0.1699;
          }
          if (tarifa == 'b'){
          Termo_fixo=1.1147;
          Energia_hora=0.2048;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1267;
          }
      }
    }

 /////////////////////GALP/////////////

   if (comando.indexOf("Galp") >= 0) {
      if (Potencia_escolhida==1.15){
          if (tarifa == 'a'){
            Termo_fixo=0.1429;
            Energia_hora=0.1598;
          } else if (tarifa == 'b'){
            Termo_fixo=0.1429;
            Energia_hora=0.1996;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.0929;
          }
      } else if (Potencia_escolhida==2.3){
          if (tarifa == 'a'){
            Termo_fixo=0.1885;
            Energia_hora=0.1598;
          } else if (tarifa == 'b'){
            Termo_fixo=0.1885;
            Energia_hora=0.1996;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.0929;
          }
      } else if (Potencia_escolhida==3.45){
          if (tarifa == 'a'){
            Termo_fixo=0.2027;
            Energia_hora=0.1598;
           }
          if (tarifa == 'b'){
            Termo_fixo=0.2027;
            Energia_hora=0.1996;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.0929;
          }
      } else if (Potencia_escolhida==4.6){
          if (tarifa == 'a'){
          Termo_fixo=0.2603;
          Energia_hora=0.1598;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.2603;
          Energia_hora=0.1996;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.0929;
          }
      } else if (Potencia_escolhida==5.75){
          if (tarifa == 'a'){
          Termo_fixo=0.3206;
          Energia_hora=0.1598;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.3206;
          Energia_hora=0.1996;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.0929;
          }
      } else if ( Potencia_escolhida==6.9){
          if (tarifa == 'a'){
          Termo_fixo=0.3888;
          Energia_hora=0.1598;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.3888;
          Energia_hora=0.1996;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.0929;
          }
      } else if (Potencia_escolhida == 10.35){
          if (tarifa == 'a'){
          Termo_fixo=0.5874;
          Energia_hora=0.1598;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.5874;
          Energia_hora=0.1996;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.0929;
          }
      } else if ( Potencia_escolhida == 13.8){
          if (tarifa == 'a'){
          Termo_fixo=0.7722;
          Energia_hora=0.1598;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.7722;
          Energia_hora=0.1996;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.0929;
          }
      } else if (Potencia_escolhida == 17.25){
          if (tarifa == 'a'){
          Termo_fixo=0.9479;
          Energia_hora=0.1598;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.9479;
          Energia_hora=0.1996;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.0929;
          }
      } else if ( Potencia_escolhida == 20.7){
          if (tarifa == 'a'){
          Termo_fixo=1.1339;
          Energia_hora=0.1598;
          }
          if (tarifa == 'b'){
          Termo_fixo=1.1339;
          Energia_hora=0.1996;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.0929;
          }
      }
    }

///////////////////FENOSA///////////////

if (comando.indexOf("Fenosa") >= 0) {
      if (Potencia_escolhida==1.15){
        if (tarifa == 'a'){
        Termo_fixo=0.0842;
        Energia_hora=0.1809;
        }
      } else if (Potencia_escolhida==2.3){
          if (tarifa == 'a'){
          Termo_fixo=0.1331;
          Energia_hora=0.1741;
          }
      } else if (Potencia_escolhida==3.45){
          if (tarifa == 'a'){
          Termo_fixo=0.1821;
          Energia_hora=0.1672;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.1821;
          Energia_hora=0.1991;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1169;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.1821;
          Energia_hora=0.2874;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.174;
          Vazio=0.1168;
          }
      } else if (Potencia_escolhida==4.6){
          if (tarifa == 'a'){
          Termo_fixo=0.231;
          Energia_hora=0.1668;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.231;
          Energia_hora=0.1984;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1163;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.231;
          Energia_hora=0.2862;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.1728;
          Vazio=0.1156;
          }
      } else if (Potencia_escolhida==5.75){
          if (tarifa == 'a'){
          Termo_fixo=0.2800;
          Energia_hora=0.1665;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.280;
          Energia_hora=0.1986;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1165;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.2800;
          Energia_hora=0.2860;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.1726;
          Vazio=0.1154;
          }
      } else if ( Potencia_escolhida==6.9){
          if (tarifa == 'a'){
          Termo_fixo=0.3269;
          Energia_hora=0.1659;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.3269;
          Energia_hora=0.1975;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1154;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.3269;
          Energia_hora=0.2856;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.1722;
          Vazio=0.1150;
          }
      } else if (Potencia_escolhida == 10.35){
          if (tarifa == 'a'){
          Termo_fixo=0.4738;
          Energia_hora=0.1655;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.4738;
          Energia_hora=0.1972;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.115;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.4738;
          Energia_hora=0.2853;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.1719;
          Vazio=0.1147;
          }
      } else if ( Potencia_escolhida == 13.8){
          if (tarifa == 'a'){
          Termo_fixo=0.6206;
          Energia_hora=0.1654;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.6206;
          Energia_hora=0.1967;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1146;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.6206;
          Energia_hora=0.2852;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.1718;
          Vazio=0.1146;
          }
      } else if (Potencia_escolhida == 17.25){
          if (tarifa == 'a'){
          Termo_fixo=0.7669;
          Energia_hora=0.1649;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.7669;
          Energia_hora=0.1964;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1143;
          }
      } else if ( Potencia_escolhida == 20.7){
          if (tarifa == 'a'){
          Termo_fixo=0.9138;
          Energia_hora=0.1646;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.9138;
          Energia_hora=0.196;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1138;
          }
      }
    }


 /////////////////////GOLDEN ENERGY/////////////

   if (comando.indexOf("Golden") >= 0) {
      if (Potencia_escolhida==1.15){
          
      } else if (Potencia_escolhida==2.3){

      } else if (Potencia_escolhida==3.45){
          if (tarifa == 'a'){
            Termo_fixo=0.1523;
            Energia_hora=0.1557;
           }
          if (tarifa == 'b'){
            Termo_fixo=0.1673;
            Energia_hora=0.1892;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.0867;
          }
      } else if (Potencia_escolhida==4.6){
          if (tarifa == 'a'){
          Termo_fixo=0.1979;
          Energia_hora=0.1557;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.2220;
          Energia_hora=0.1892;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.0868;
          }
      } else if (Potencia_escolhida==5.75){
          if (tarifa == 'a'){
          Termo_fixo=0.2633;
          Energia_hora=0.1557;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.2753;
          Energia_hora=0.1892;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.0868;
          }
      } else if ( Potencia_escolhida==6.9){
          if (tarifa == 'a'){
          Termo_fixo=0.3191;
          Energia_hora=0.1559;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.3292;
          Energia_hora=0.1899;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.0868;
          }
      } else if (Potencia_escolhida == 10.35){
          if (tarifa == 'a'){
          Termo_fixo=0.4778;
          Energia_hora=0.1564;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.4448;
          Energia_hora=0.1892;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.0964;
          }
      } else if ( Potencia_escolhida == 13.8){
          if (tarifa == 'a'){
          Termo_fixo=0.6200;
          Energia_hora=0.1564;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.5809;
          Energia_hora=0.1892;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.0964;
          }
      } else if (Potencia_escolhida == 17.25){
          if (tarifa == 'a'){
          Termo_fixo=0.7642;
          Energia_hora=0.1564;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.7018;
          Energia_hora=0.1892;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.0964;
          }
      } else if ( Potencia_escolhida == 20.7){
          if (tarifa == 'a'){
          Termo_fixo=0.8631;
          Energia_hora=0.1564;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.8431;
          Energia_hora=0.1892;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.0964;
          }
      }
    }

 /////////////////////LUZ BOA/////////////

   if (comando.indexOf("Boa") >= 0) {
      if (Potencia_escolhida==1.15){
          if (tarifa == 'a'){
            Termo_fixo=0.1148;
            Energia_hora=0.1619;
           }
      } else if (Potencia_escolhida==2.3){
           if (tarifa == 'a'){
            Termo_fixo=0.1637;
            Energia_hora=0.1619;
           }
      } else if (Potencia_escolhida==3.45){
          if (tarifa == 'a'){
            Termo_fixo=0.2127;
            Energia_hora=0.1619;
           }
          if (tarifa == 'b'){
            Termo_fixo=0.2127;
            Energia_hora=0.201;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.103;
          }
      } else if (Potencia_escolhida==4.6){
          if (tarifa == 'a'){
          Termo_fixo=0.2616;
          Energia_hora=0.1619;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.2616;
          Energia_hora=0.201;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.103;
          }
      } else if (Potencia_escolhida==5.75){
          if (tarifa == 'a'){
          Termo_fixo=0.3106;
          Energia_hora=0.1619;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.3106;
          Energia_hora=0.201;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.103;
          }
      } else if ( Potencia_escolhida==6.9){
          if (tarifa == 'a'){
          Termo_fixo=0.3596;
          Energia_hora=0.1619;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.3596;
          Energia_hora=0.201;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.103;
          }
      } else if (Potencia_escolhida == 10.35){
          if (tarifa == 'a'){
          Termo_fixo=0.5065;
          Energia_hora=0.1619;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.5065;
          Energia_hora=0.201;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.103;
          }
      } else if ( Potencia_escolhida == 13.8){
          if (tarifa == 'a'){
          Termo_fixo=0.6533;
          Energia_hora=0.1619;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.6533;
          Energia_hora=0.201;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.103;
          }
      } else if (Potencia_escolhida == 17.25){
          if (tarifa == 'a'){
          Termo_fixo=0.8002;
          Energia_hora=0.1619;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.8002;
          Energia_hora=0.201;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.103;
          }
      } else if ( Potencia_escolhida == 20.7){
          if (tarifa == 'a'){
          Termo_fixo=0.9471;
          Energia_hora=0.1619;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.9471;
          Energia_hora=0.201;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.103;
          }
      }
    }

////////////////AUDAX////////////////////

    if (comando.indexOf("Audax") >= 0) {
      if (Potencia_escolhida==1.15){
        if (tarifa == 'a'){
        Termo_fixo=0.1147;
        Energia_hora=0.1496;
        }
      } else if (Potencia_escolhida==2.3){
          if (tarifa == 'a'){
          Termo_fixo=0.1637;
          Energia_hora=0.1496;
          }
      } else if (Potencia_escolhida==3.45){
          if (tarifa == 'a'){
          Termo_fixo=0.2127;
          Energia_hora=0.1496;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.2127;
          Energia_hora=0.1792;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1031;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.2127;
          Energia_hora=0.2687;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.1532;
          Vazio=0.103;
          }
      } else if (Potencia_escolhida==4.6){
          if (tarifa == 'a'){
          Termo_fixo=0.2617;
          Energia_hora=0.1496;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.2617;
          Energia_hora=0.1792;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.1031;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.2617;
          Energia_hora=0.2687;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.1532;
          Vazio=0.103;
          }
      } else if (Potencia_escolhida==5.75){
          if (tarifa == 'a'){
          Termo_fixo=0.3107;
          Energia_hora=0.1496;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.3107;
          Energia_hora=0.1792;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.103;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.3107;
          Energia_hora=0.2687;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.1532;
          Vazio=0.103;
          }
      } else if ( Potencia_escolhida==6.9){
          if (tarifa == 'a'){
          Termo_fixo=0.3597;
          Energia_hora=0.1496;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.3597;
          Energia_hora=0.1792;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.103;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.3597;
          Energia_hora=0.2687;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.1532;
          Vazio=0.103;
          }
      } else if (Potencia_escolhida == 10.35){
          if (tarifa == 'a'){
          Termo_fixo=0.5063;
          Energia_hora=0.1496;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.5063;
          Energia_hora=0.1792;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.103;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.5063;
          Energia_hora=0.2687;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.1532;
          Vazio=0.103;
          }
      } else if ( Potencia_escolhida == 13.8){
          if (tarifa == 'a'){
          Termo_fixo=0.6533;
          Energia_hora=0.1496;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.6533;
          Energia_hora=0.1792;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.103;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.6533;
          Energia_hora=0.2687;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.1532;
          Vazio=0.103;
          }
      } else if (Potencia_escolhida == 17.25){
          if (tarifa == 'a'){
          Termo_fixo=0.8002;
          Energia_hora=0.1496;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.8002;
          Energia_hora=0.1792;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.103;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.8002;
          Energia_hora=0.2687;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.1532;
          Vazio=0.103;
          }
      } else if ( Potencia_escolhida == 20.7){
          if (tarifa == 'a'){
          Termo_fixo=0.9472;
          Energia_hora=0.1496;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.9472;
          Energia_hora=0.1792;  //Energia_hora em bihorario corresponde á energia fora de Vazio
          Vazio=0.103;
          }
          if (tarifa == 'c'){
          Termo_fixo=0.9472;
          Energia_hora=0.2687;  //Energia_hora em trihorario corresponder á energia em horas de ponta
          Cheias=0.1532;
          Vazio=0.103;
          }
      }
    }

/////////////////////IBERDROLA/////////////

   if (comando.indexOf("Iberdrola") >= 0) {
      if (Potencia_escolhida==1.15){
          if (tarifa == 'a'){
            Termo_fixo=0.111;
            Energia_hora=0.149;
           }
      } else if (Potencia_escolhida==2.3){
           if (tarifa == 'a'){
            Termo_fixo=0.158;
            Energia_hora=0.149;
           }
      } else if (Potencia_escolhida==3.45){
          if (tarifa == 'a'){
            Termo_fixo=0.1659;
            Energia_hora=0.1509;
           }
          if (tarifa == 'b'){
            Termo_fixo=0.1659;
            Energia_hora=0.1816;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.0992;
          }
      } else if (Potencia_escolhida==4.6){
          if (tarifa == 'a'){
          Termo_fixo=0.2156;
          Energia_hora=0.1509;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.2156;
          Energia_hora=0.1816;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.0992;
          }
      } else if (Potencia_escolhida==5.75){
          if (tarifa == 'a'){
          Termo_fixo=0.265;
          Energia_hora=0.1509;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.265;
          Energia_hora=0.1816;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.0992;
          }
      } else if ( Potencia_escolhida==6.9){
          if (tarifa == 'a'){
          Termo_fixo=0.3145;
          Energia_hora=0.1509;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.3145;
          Energia_hora=0.1816;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.0992;
          }
      } else if (Potencia_escolhida == 10.35){
          if (tarifa == 'a'){
          Termo_fixo=0.4629;
          Energia_hora=0.1511;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.4629;
          Energia_hora=0.1833;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.0993;
          }
      } else if ( Potencia_escolhida == 13.8){
          if (tarifa == 'a'){
          Termo_fixo=0.6113;
          Energia_hora=0.1511;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.6113;
          Energia_hora=0.1833;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.0993;
          }
      } else if (Potencia_escolhida == 17.25){
          if (tarifa == 'a'){
          Termo_fixo=0.7596;
          Energia_hora=0.1511;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.7596;
          Energia_hora=0.1833;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.0993;
          }
      } else if ( Potencia_escolhida == 20.7){
          if (tarifa == 'a'){
          Termo_fixo=0.9079;
          Energia_hora=0.1511;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.9079;
          Energia_hora=0.1833;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.0993;
          }
      }
    }

/////////////////////YLCE/////////////

   if (comando.indexOf("YLCE") >= 0) {
      if (Potencia_escolhida==1.15){
          if (tarifa == 'a'){
            Termo_fixo=0.0885;
            Energia_hora=0.1549;
           }
      } else if (Potencia_escolhida==2.3){
           if (tarifa == 'a'){
            Termo_fixo=0.1549;
            Energia_hora=0.1549;
           }
      } else if (Potencia_escolhida==3.45){
          if (tarifa == 'a'){
            Termo_fixo=0.1599;
            Energia_hora=0.1549;
           }
          if (tarifa == 'b'){
            Termo_fixo=0.1599;
            Energia_hora=0.1869;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.0997;
          }
      } else if (Potencia_escolhida==4.6){
          if (tarifa == 'a'){
          Termo_fixo=0.2129;
          Energia_hora=0.1549;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.2129;
          Energia_hora=0.1869;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.0997;
          }
      } else if (Potencia_escolhida==5.75){
          if (tarifa == 'a'){
          Termo_fixo=0.2619;
          Energia_hora=0.1549;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.2619;
          Energia_hora=0.1869;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.0997;
          }
      } else if ( Potencia_escolhida==6.9){
          if (tarifa == 'a'){
          Termo_fixo=0.3099;
          Energia_hora=0.1549;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.3099;
          Energia_hora=0.1869;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.0997;
          }
      } else if (Potencia_escolhida == 10.35){
          if (tarifa == 'a'){
          Termo_fixo=0.4631;
          Energia_hora=0.1549;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.4631;
          Energia_hora=0.1869;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.0997;
          }
      } else if ( Potencia_escolhida == 13.8){
          if (tarifa == 'a'){
          Termo_fixo=0.6138;
          Energia_hora=0.1549;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.6138;
          Energia_hora=0.1869;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.0997;
          }
      } else if (Potencia_escolhida == 17.25){
          if (tarifa == 'a'){
          Termo_fixo=0.7648;
          Energia_hora=0.1549;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.7648;
          Energia_hora=0.1869;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.0997;
          }
      } else if ( Potencia_escolhida == 20.7){
          if (tarifa == 'a'){
          Termo_fixo=0.9158;
          Energia_hora=0.1549;
          }
          if (tarifa == 'b'){
          Termo_fixo=0.9158;
          Energia_hora=0.1869;  //Energia_hora em bihorario corresponde á energia fora de Vazio
            Vazio=0.0997;
          }
      }
    }


    

///////////////////////escolha da potencia////////////////////////////////////////
    if (comando.indexOf("1.15") >=0) { 
    Potencia_escolhida=1.15;
    comando = "";
    }
    if (comando.indexOf("2.3") >=0) { 
    Potencia_escolhida=2.3;
    comando = "";
    }
    if (comando.indexOf("3.45") >=0) {
    Potencia_escolhida=3.45;
    comando = "";
    }
    if (comando.indexOf("4.6") >=0) { 
    Potencia_escolhida=4.6;
    comando = "";
    }
    if (comando.indexOf("5.75") >=0) { 
    Potencia_escolhida=5.75;
    comando = "";
    }
    if (comando.indexOf("6.9") >=0) { 
    Potencia_escolhida=6.9;
    comando = "";
    }
    if (comando.indexOf("10.35") >=0) { 
    Potencia_escolhida=10.35;
    comando = "";
    }
    if (comando.indexOf("13.8") >=0) { 
    Potencia_escolhida=13.8;
    comando = "";
    }
    if (comando.indexOf("17.25") >=0) { 
    Potencia_escolhida=17.25;
    comando = "";
    }
    if (comando.indexOf("20.7") >=0) { 
    Potencia_escolhida=20.7;
    comando = "";
    }
  }
}

void EEPROMWriteInt(int address, int value) {
   byte hiByte = highByte(value);
   byte loByte = lowByte(value);

   EEPROM.write(address, hiByte);
   EEPROM.write(address + 1, loByte);   
}

int EEPROMReadInt(int address) {
   byte hiByte = EEPROM.read(address);
   byte loByte = EEPROM.read(address + 1);
   
   return word(hiByte, loByte); 
}

void EEPROMWriteLong(int address, long value) {
   byte four = (value & 0xFF);
   byte three = ((value >> 8) & 0xFF);
   byte two = ((value >> 16) & 0xFF);
   byte one = ((value >> 24) & 0xFF);

   EEPROM.write(address, four);
   EEPROM.write(address + 1, three);
   EEPROM.write(address + 2, two);
   EEPROM.write(address + 3, one);
}

long EEPROMReadLong(int address) {
   long four = EEPROM.read(address);
   long three = EEPROM.read(address + 1);
   long two = EEPROM.read(address + 2);
   long one = EEPROM.read(address + 3);

   return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void EEPROMWriteStr(int address, String value) {
int nL;

   for (nL=0; nL < value.length(); nL++)  {
      EEPROM.write(address + nL, value.charAt(nL) );

      if (nL >= 1000) {
         break;
      }
   }

   if (nL < 1000) {
      EEPROM.write(address + nL, (char)0 );
   }
}

void EEPROMConcatStr(int address, String value) {
int nL;
char readByte;
bool nullFound = false;
int strPos = 0;

   for (nL=0; nL < 1000; nL++)  {

      if (!nullFound) {
         readByte = EEPROM.read(address + nL);

         if (readByte == (char)0) {
            nullFound = true;
         }
      } 
      
      if (nullFound) {
         EEPROM.write(address + nL, value.charAt(strPos) );          
         strPos++;

         if ((strPos + 1) > value.length()) {
            EEPROM.write(address + nL + 1, (char)0 );
            break;
         }
      }
   }     
}

String EEPROMReadStr(int address) {
String readStr = "";
char readByte;
int readAddress = address;

   do {
      readByte = EEPROM.read(readAddress);
      readStr += readByte;
      readAddress++;
   } while ( (readByte != (char)0) && (readAddress < (address + 1000)) );
   
   return readStr; 
}

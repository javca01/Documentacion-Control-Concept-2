//************************************************

#include "VL53L0X.h" // Librería del sensor de distancia.
#include "Wire.h"

// Variables para el control de tiempo.
//unsigned long initTime = 0;

VL53L0X sensor;
int medida=0; //Variable que toma el valor generado por el sensor de posición.

//Datos Encoder:
const int channelPinA = 18; //pin 18 ESP32.
const int channelPinB = 19; //pin 19 ESP32.
const int timeThreshold = 1;//Umbral de tiempo para reconocimiento.
long timeCounter = 0;
//const int maxSteps = 360; //360 pulsos por revolución.
volatile int ISRCounter = 0;
int counter = 0;
int counter_old = 0;
float resolution = 360.0;
float rpm = 0.0;
//float acel = 0.0;
unsigned long timeold;
float dt=0.01;
bool IsCW = true;
//************************************************

const int  RPWM_Output = 2;
const int  LPWM_Output = 4;
const int in1_negro=15; // Pin 15 --> Final de carrea del extremo de la máquina cercano al panel.
const int in2_verde=23; // Pin 23 --> Final de carrea del extremo de la máquina lejano al panel.

//************************************************

// Valores de configuración de la señal PWM emitida desde la ESP32
const int frecuencia = 15000;
//const int canal = 1;
const int resolucion = 8;
//************************************************

// Vector que contiene los ciclos útiles para la señal PWM del perfil de velocidad de la máquina.
int Velocidad[] = {0,0,0,1,1,2,3,3,4,5,6,7,9,10,11,13,14,16,18,20,21,23,25,27,29,31,34,36,38,40,43,45,47,50,52,54,57,59,61,63,66,68,70,72,74,76,78,80,82,84,86,88,89,91,92,94,95,96,98,99,100,100,101,102,102,103,103,104,104,104,104,104,103,103,102,102,101,100,100,99,98,96,95,94,92,91,89,88,86,84,82,80,78,76,74,72,70,68,66,63,61,59,57,54,52,50,47,45,43,40,38,36,34,31,29,27,25,23,21,20,18,16,14,13,11,10,9,7,6,5,4,3,3,2,1,1,0,0,0,0,0,0,-1,-1,-2,-3,-3,-4,-5,-6,-7,-9,-10,-11,-13,-14,-16,-18,-20,-21,-23,-25,-27,-29,-31,-34,-36,-38,-40,-43,-45,-47,-50,-52,-54,-57,-59,-61,-63,-66,-68,-70,-72,-74,-76,-78,-80,-82,-84,-86,-88,-89,-91,-92,-94,-95,-96,-98,-99,-100,-100,-101,-102,-102,-103,-103,-104,-104,-104,-104,-104,-103,-103,-102,-102,-101,-100,-100,-99,-98,-96,-95,-94,-92,-91,-89,-88,-86,-84,-82,-80,-78,-76,-74,-72,-70,-68,-66,-63,-61,-59,-57,-54,-52,-50,-47,-45,-43,-40,-38,-36,-34,-31,-29,-27,-25,-23,-21,-20,-18,-16,-14,-13,-11,-10,-9,-7,-6,-5,-4,-3,-3,-2,-1,-1,0,0,0
};// 278 elementos

//Inicio del ciclo negativo en el dato 139 del vector Velocidad.
//************************************************

// Variables de la aplicación.
//float currTime;
bool start = false;
bool calibrated = false;
//************************************************

// Banderas para el control lógico del programa.
//bool pulsacion1 = false;
//bool pulsacion2 = false;
bool flag_detect_interrupt = false;
bool flag_ccw = false;
bool flag_cw = false;
bool flag_exit_ccw = false;
bool flag_exit_cw = false;
bool flag_calibracion = false;
bool flag_begin_move = false;
bool flag_home_negro = false;
//bool start_calib = false;
bool flag_stop =  false;
bool flag_detect_now =false;
//bool flag_start = false;

//Banderas para la identificación de la activación de los switches sobre la máquina.
bool flag_sw_green = false;
bool flag_sw_black = true; 

bool flag_home_done = false;
//************************************************

//Permiten medir el dt necesario para el cálculo del perfil de aceleración.
double t_actual=0;
double t_anterior=0;

String msg;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin();
  sensor.init();
  sensor.setTimeout(50);//Estaba inicialmente en 500.
  //Estaba en 5 para lectura rápida

  pinMode(RPWM_Output,OUTPUT);
  pinMode(LPWM_Output,OUTPUT);
  pinMode(in1_negro, INPUT);    
  pinMode(in2_verde, INPUT);
  
  ledcSetup(0, frecuencia, resolucion);
  ledcAttachPin( RPWM_Output,0);
 
  ledcSetup(1,frecuencia, resolucion);
  ledcAttachPin(LPWM_Output,1);
  //Configuración Encoder
  pinMode(channelPinA, INPUT_PULLUP); //"PULLUP" Hace que la interrupción se habilite con tierra.
  pinMode(channelPinB, INPUT_PULLUP); //"PULLUP" Hace que la interrupción se habilite con tierra.
  ///////////////
   
  //Interrupciones asociadas al encoder:
  attachInterrupt(digitalPinToInterrupt(channelPinA), doEncodeA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(channelPinB), doEncodeB, CHANGE);
  //////////////////

  //calibracion(); //Realizamos el proceso de calibración.

  // Las interrupciones de los finales de carrera se definen en este punto para que no interfieran con 
  // el proceso de calibración.

  
  //Uso de interrupciones para detectar los finales de carrera.
  attachInterrupt(digitalPinToInterrupt(in1_negro), rutina_movimiento_cw, CHANGE);
  attachInterrupt(digitalPinToInterrupt(in2_verde), rutina_movimiento_ccw, CHANGE);
  ///////////////////
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()>0){
    msg=Serial.read();
    //Serial.println("recibido: "+msg); 
    //RUTINA DE CALIBRACIÓN***********************************
    if (msg=="97" && calibrated == false){//Corresponde al envío de la "a" -->Emitida por el botón de calibración
      //Rutina de prueba************************************
      //Serial.println("Estamos en caracter a");
      //Serial.println("Se recibió el caracter "+msg); 
      //Rutina principal************************************
      detachInterrupt(digitalPinToInterrupt(in2_verde));
      detachInterrupt(digitalPinToInterrupt(in1_negro));
      //start_calib = false;
      //Serial.println("Comenzando calibración desde ESP32");
      delay(5000);
      //Serial.println("Iniciando Proceso de calibración...., espere.");
      delay(2000);
      //Serial.println("Iniciamos.");
  
      while(digitalRead(in2_verde) == LOW){
        //Calibrar posición del motor
         //Rutina para la detección temprana del botón de parada
        msg=Serial.read();
        if (msg=="99" || flag_detect_now == true){
          flag_detect_now = true;
          detener();
          break;
        }
        //***************************************************
        ledcWrite(0, 60);
        ledcWrite(1, 0); 
       // Serial.println("60");
        medida = lectura_distancia();
        rpm = lectura_encoder();
      }
    
      //parar motor
      ledcWrite(0, 0);
      ledcWrite(1, 0);
      //Serial.println("Detenido en extremo izquierdo..., espere.");
      delay(3000);
      //Serial.println("Desplazando al extremo derecho.");
      //Ir a posición intermedia
      //Serial.println("Midiendo distancia.");
      //Hacer rutina para medir la distancia entre ambos extremos.
      while(digitalRead(in1_negro) == LOW){
        //Serial.println("Ingreso a negro");
         //Rutina para la detección temprana del botón de parada
        msg=Serial.read();
        if (msg=="99" || flag_detect_now == true){
          detener();
          break;
        }
        //***************************************************
        ledcWrite(0, 0);
        ledcWrite(1, 60);
        //Serial.println("60");
        medida = lectura_distancia();
        rpm = lectura_encoder();
      }
      //parar motor
      ledcWrite(0, 0);
      ledcWrite(1, 0); 
      //Serial.println("Detenido en extremo derecho..., espere.");
      delay(2000);

      //Serial.println("Iniciando retroceso para liberar final de carrera.[Calibración]");
      
      while(digitalRead(in1_negro) == HIGH){
        ledcWrite(0, 60);
        ledcWrite(1, 0);
      }
      //Detenemos el motor.
      ledcWrite(0, 0);
      ledcWrite(1, 0); 

      //Serial.println("Posición final de calibración alcanzada. Sin oprimir final de carrera.");
      calibrated = true;
      flag_calibracion = true;
      //Serial.println("Fin del proceso de calibración");
      if(calibrated){
        start = true;
        flag_stop = false;
        flag_begin_move= false;
        flag_detect_interrupt = false;
        flag_exit_ccw = false;
        flag_exit_cw = false;
        flag_home_negro = false;
        //initTime = millis();
        }
      delay(3000);
      //Fin de rutina principal********************************
      //FIN DE RUTINA DE CALIBRACIÓN      
    }else if(msg=="98"){//Corresponde con el botón Iniciar
      //Serial.println("Estamos en caracter b,acción iniciar");
      //Serial.println("Se recibió el caracter "+msg); 
      if (calibrated){
        start= true;
        flag_detect_interrupt= false;
        flag_calibracion = true;
        flag_exit_ccw = false; 
        flag_exit_cw = false;
        flag_cw = true;
        flag_ccw = true; 
        flag_home_negro = false;
      }
    if (start){
      if (flag_detect_interrupt == false && flag_calibracion == true){
      //rutina_movimiento();
        attachInterrupt(digitalPinToInterrupt(in1_negro), rutina_movimiento_cw, CHANGE);
        attachInterrupt(digitalPinToInterrupt(in2_verde), rutina_movimiento_ccw, CHANGE);
        
      if (flag_begin_move == false){
        flag_begin_move = true;
        //Serial.println("Iniciando rutina de movimiento..., espere");
        delay(2000);
        //Serial.println("Comenzamos");
        delay(1000);  
        medida = lectura_distancia();
        rpm = lectura_encoder();
      }
      int Vpwm =0;
      int i=0;
      
      //double dt=0;
      //for (int i=0;i<278;i++){
      while(start){
        //Rutina para la detección temprana del botón de parada
        msg=Serial.read();
        if (msg=="99"){
          detener();
          break;
        }
        //***************************************************
      if (flag_sw_black == true){
        //Serial.println("Movimiento derecha a izquierda de la máquina.");
        //Serial.println("Detención preventiva en extremo por CW");
        ledcWrite(0, 0);
        ledcWrite(1, 0);
        delay(10);
        for (int i=0;i<=139;i=i+3){
          Vpwm = Velocidad[i];      
          ledcWrite(0, Vpwm);
          ledcWrite(1, 0);
          medida = lectura_distancia();
          rpm = lectura_encoder();
          dt=abs(t_actual-t_anterior);  
          //Serial.println(Velocidad[i]);
          delay(20); //Se llevó a 20 para que el piñón alcance a dar las tres vueltas.  //Se deja en 10 para acortar la rutina de movimiento
          if (flag_sw_black == false){ //abandona el ciclo al salir de la interrupción.
            break;
          }
        }   
      }else if(flag_sw_green == true){
        //Serial.println("Movimiento izquierda a derecha de la máquina.");
        //Serial.println("Detención preventiva en extremo por CCW");
        ledcWrite(0, 0);
        ledcWrite(1, 0);
        delay(10);
        for (int i=139;i<=277;i=i+3){
          Vpwm = -Velocidad[i];      
          ledcWrite(0, 0);
          ledcWrite(1, Vpwm);
          medida = lectura_distancia();
          rpm = lectura_encoder();
          dt=abs(t_actual-t_anterior);  
          //Serial.println(Velocidad[i]);
          delay(20); //Se llevó a 20 para que el piñón alcance a dar las tres vueltas.  //Se deja en 10 para acortar la rutina de movimiento
          if (flag_sw_green == false){ //abandona el ciclo al salir de la interrupción.
            break;
          }
        }   
      }
        
      
     /* if (flag_exit_ccw == true || flag_exit_cw == true) { //impide la reanudación de la rutina de movimiento una vez finalizan las interrupciones.
        break;
      }*/   
      
      }
    } 
    //Serial.println("Hicimos a start=false");
    start = false; 
  }
  
  /*if(flag_ccw == true || flag_cw == true ){
    rutina_home();
  }*/

  if (flag_stop == false)
  {
    medida = lectura_distancia();
    rpm = lectura_encoder();
  }
             
    }else if(msg=="99"){ //Corresponde con el botón detener.
      //Serial.println("Estamos en caracter c, botón detener");
      //Serial.println("Se recibió el caracter "+msg); 
      start = false;
      flag_stop = true;
      flag_detect_interrupt = false;
      detener();
      }
    }
}
//msg=97 -->Corresponde a recibir el caracter "a"
//msg=98 -->Corresponde a recibir el caracter "b"
//msg=99 -->Corresponde a recibir el caracter "a"
//Declarandolo com int a msg no funciona.

void rutina_movimiento_cw(){
  //detachInterrupt(digitalPinToInterrupt(in1_negro));//Desactivamos la detección de interrupciones por el final de carrera cw.
  // Realizamos detención preventiva.
  int Vpwm =0;
  flag_detect_interrupt = true;
  flag_cw = true;
  flag_exit_cw = true;
  //start = false;
  //ledcWrite(0, 0);
  //ledcWrite(1, 0); 
  //Serial.println("Fin de rutina de parada_cw");
  //Serial.println("Interrupción CW , blc=T, gre=F");
  flag_sw_black = true;
  flag_sw_green = false;
}

void rutina_movimiento_ccw(){
  //detachInterrupt(digitalPinToInterrupt(in2_verde));//Desactivamos la detección de interrupciones por el final de carrera cw.
  int Vpwm =0;
  flag_detect_interrupt = true;
  flag_ccw = true;
  //Serial.println("Inicio de rutina de detención extremo izquierdo.");  
  //ledcWrite(0, 0);     
  //ledcWrite(1, 0);
  //Serial.println("Fin de rutina de parada_ccw");
  //Serial.println("Interrupción CCW , blc=F, gre=T");
  flag_exit_ccw = true;
  //start = false;
  flag_sw_green = true; 
  flag_sw_black = false;  
  }  


void doEncodeA(){
   if (millis() > timeCounter + timeThreshold){
      if (digitalRead(channelPinA) == digitalRead(channelPinB)){
         IsCW = true;
         ISRCounter++;
      }
      else{
         IsCW = false;
         ISRCounter--;
      }
      timeCounter = millis();
   }
}

void doEncodeB(){
   if (millis() > timeCounter + timeThreshold){
      if (digitalRead(channelPinA) != digitalRead(channelPinB)){
        ISRCounter++;
      }
      else{
         IsCW = false;
         ISRCounter--;
      }
      timeCounter = millis();
   }
}
int lectura_inicial = 0.0;
int lectura_final = 0.0;

int lectura_distancia(){
  int lect=0;
  
  lectura_final = sensor.readRangeSingleMillimeters()/10.0;  
  //if (sensor.timeoutOccurred()) { Serial.print("-1"); } //imprime -1 si no hay lectura.
  if (lectura_final>110.0){ //Evita sobrepicos por perdida de lectura
    lectura_final=lectura_inicial;
  }
  lect = (int)lectura_final;
  lectura_inicial = lectura_final;
  
  //Serial.print("dist: ");
  Serial.println(lect); //Enviamos a la interfaz el valor del sensor de distancia
  return lect;
}

float lectura_encoder(){
  if (counter != ISRCounter){
      counter_old=counter;
      counter = ISRCounter;
      dt=float(millis()-timeold);
      rpm = 3.4*float((abs(counter-counter_old)/dt)*(60000.0/resolution)); //RPM
    }
  //Serial.print("rpm: ");
  Serial.println(rpm); //Enviamos a la interfaz el valor del encoder.
  timeold = millis();
  return rpm;
}

void detener(){
  //Serial.println("Ejecutando detención de la máquina");
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  delay(1000);
  rutina_home();
}

void rutina_home(){
  detachInterrupt(digitalPinToInterrupt(in1_negro)); //Desactivamos la detección de interrupciones por el final de carrera negro.
  detachInterrupt(digitalPinToInterrupt(in2_verde)); //Desactivamos la detección de interrupciones por el final de carrera verde.
  //Serial.println("Sistema detenido por el usuario. LLevando a home.");

  if (digitalRead(in1_negro)== LOW){
    //Serial.println("Desplazamiento a Home de izquierda a derecha");
    while(digitalRead(in1_negro)== LOW){
      ledcWrite(0, 0);
      ledcWrite(1, 60);
      //Serial.println("60");
    }
    //Serial.println("Sistema detenido en Home con interruptor oprimido.");
      ledcWrite(0, 0);
      ledcWrite(1, 0);  
      delay(100);
    //Serial.println("Posición de home alcanzada, liberando el interruptor.");  
    while(digitalRead(in1_negro)== HIGH){
      ledcWrite(0, 60);
      ledcWrite(1, 0);
      //Serial.println("60");
    }  
    //Serial.println("Sistema detenido en Home con interruptor liberado.");
      ledcWrite(0, 0);
      ledcWrite(1, 0);  
  }else{
    //Serial.println("Sistema en Home, liberando el interruptor.");
    while(digitalRead(in1_negro)== HIGH){
      ledcWrite(0, 60);
      ledcWrite(1, 0);
      //Serial.println("60");
    }
    //Serial.println("Sistema detenido en Home con interruptor liberado.");
      ledcWrite(0, 0);
      ledcWrite(1, 0);  
  }
  flag_home_done = true;
}


//Lleva la silla hasta el extremo derecho, el cual es el punto de partida para todos los casos.
/*void rutina_home(){
  detachInterrupt(digitalPinToInterrupt(in1_negro)); //Desactivamos la detección de interrupciones por el final de carrera negro.
  detachInterrupt(digitalPinToInterrupt(in2_verde)); //Desactivamos la detección de interrupciones por el final de carrera verde.
  int Vpwm =0;
  Serial.println("Llevando la Silla a Home");
  // desde extremo izquierdo.

  if (flag_ccw == true){  //Rutina para ir a home desde el extremo izquierdo.
    Serial.println("Rutina para ir a home desde el extremo izquierdo."); 
    
    // Se lleva la silla a Home a velocidad constante.
    
    //Detenemos el motor.
    ledcWrite(0, 0);
    ledcWrite(1, 0); 
    delay(2000);
    
    while(digitalRead(in1_negro) == LOW && flag_home_negro == false){
      ledcWrite(0, 0);
      ledcWrite(1, 60); //Corregido rutina de Home según experiencia.
      Serial.println("60");
       //Rutina para la detección temprana del botón de parada
        msg=Serial.read();
        if (msg=="99"){
          detener();
          break;
        }
        //***************************************************
      
      }
      Serial.println("Sistema en posición de home (Oprimiendo final de carrera)");  
      flag_home_negro = true;
      //Detenemos el motor.
      ledcWrite(0, 0);
      ledcWrite(1, 0); 
      Serial.println("Iniciando retroceso para liberar final de carrera.");
      
     while(digitalRead(in1_negro) == HIGH){
      ledcWrite(0, 60);
      ledcWrite(1, 0);
       //Rutina para la detección temprana del botón de parada
        msg=Serial.read();
        if (msg=="99"){
          detener();
          break;
        }
        //***************************************************
     }
      //Detenemos el motor.
        ledcWrite(0, 0);
        ledcWrite(1, 0); 
        
      Serial.println("Posición de home alcanzada. Detenido. (Sin oprimir final de carrera.)");
      delay(2000);
      flag_ccw = false;
  }else if (flag_cw == true){ //Rutina para ir a home desde el extremo derecho.
    Serial.println("Rutina para ir a home desde el extremo derecho"); 
    //Detenemos el motor.
    ledcWrite(0, 0);  
    ledcWrite(1, 0); 
    delay(2000);
    Serial.println("Sistema en posición de home (Oprimiendo final de carrera)");  
    Serial.println("Iniciando retroceso para liberar final de carrera.");
    
    while(digitalRead(in2_verde) == HIGH){
      ledcWrite(0, 0);
      ledcWrite(1, 60);
       //Rutina para la detección temprana del botón de parada
        msg=Serial.read();
        if (msg=="99"){
          detener();
          break;
        }
        //***************************************************
     }
    //Detenemos el motor. 
    ledcWrite(0, 0);
    ledcWrite(1, 0); 
    Serial.println("Posición de home alcanzada. Detenido. (Sin oprimir final de carrera.)");
    delay(2000);
    flag_cw = false;
  }
}*/

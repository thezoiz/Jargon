//Este codigo fue escrito originalmente por Pikibot (Daniel Murillo)
//Fiesta.

//motores
int motor1 = 22;
int pwm1 = 13;
int motor2 = 24;
int pwm2 = 12;

//sensores
int trig1 = 52; //1 = sensor izquierdo
int echo1 = 53;
int trig2 = 46; //2 = sensor derecho
int echo2 = 47;
int trig3 = 48; //3 = sensor superior
int echo3 = 49;

//variables globales
int tiempoParedInicial = 0;
int tiempoParedFinal = 0;
int velocidad = 0;
int estadoMotores = 0;
int tiempoActual = 0;
int tiempoInicio = 0;
double lecturaIzquierda; //distancia 5in = 12.7
double lecturaDerecha;
double lecturaPared;// ditancia 6.5in = 16.51
double diferenciaDistancia = 0.0;
boolean detectado = false, primeras = true, tresmil = false, inicio = true;
void setup() 
{
  Serial.begin(9600);
  pinMode(motor1, OUTPUT);
  pinMode(pwm1, OUTPUT);
  pinMode(motor2, OUTPUT);
  pinMode(pwm2, OUTPUT);
  
  pinMode(echo1, INPUT);
  pinMode(trig1, OUTPUT);
  pinMode(echo2, INPUT);
  pinMode(trig2, OUTPUT);
  pinMode(echo3, INPUT);
  pinMode(trig3, OUTPUT);
}

void loop()
{  
  if(inicio)
  {
    moverEnfrente(255);
    delay(2000);
    inicio = false;
  }
  buscarPared();
  lecturaIzquierda = sensorIzquierda();
  lecturaDerecha = sensorDerecha();
  diferenciaDistancia = abs(lecturaIzquierda - lecturaDerecha);
  Serial.print(lecturaIzquierda);
  Serial.print("        ");
  Serial.print(lecturaDerecha);
  Serial.print("        ");
  Serial.println(sensorPared());
  if(!detectado)//se acomoda si alguno de los sensores detecta algo, sino girara hacia la derecha
  {
    if((lecturaIzquierda<80 && diferenciaDistancia<10)||(lecturaIzquierda > 1000 && lecturaDerecha<255))
    {//gira izquierda si mira algo por la izquierda
      girarIzquierda();
      tiempoInicio = millis();
    }
    else if(((lecturaIzquierda<80 && diferenciaDistancia<10))||(lecturaDerecha < 1000 && lecturaIzquierda < 255))
    {//gira derecha si mira algo por la derecha
      girarDerecha();
      tiempoInicio = millis();
    }
    else 
    { //si lleva mas de 5 segundos sin encontrar nada hara reversa por .75s para acomodarse
       // y buscar denuevo   
      if((tiempoActual - tiempoInicio) >= 5000)
      { 
         moverEnfrente(255);
         delay(700);
         tiempoInicio = millis();
      } 
      else
      {
        girarDerecha();
      }
      tiempoActual = millis();
    }
    if(((lecturaIzquierda > 1000) && (lecturaDerecha > 1000)) || ((lecturaIzquierda < 80) && (lecturaDerecha < 80)))
    {      
      detectado = true;
      primeras = true;
      if((lecturaIzquierda > 1000) && (lecturaDerecha > 1000))
      {
        tresmil = true;
      }
    }
  }
  if(detectado) //al detectar algo por primera vez se ira contra el objetivo por 1.5 segundos
  {    // despues de eso si todavia hay algo enfrente de el seguira moviendose hasta que ya no lo vea                                                       
    if(primeras)
    {
      
       moverEnfrente(255);
       delay(1000);
       //moverEnfrente(250);
       primeras = false;
    }   
    else
    {
       moverEnfrente(250);
    }
    if(tresmil)
    {
       moverEnfrente(255);
       delay(1000);
       moverReversa(200);
       delay(650);
       girarDerecha();
       delay(300);
      tresmil = false; 
    }
   if(((lecturaIzquierda < 1000) || (lecturaDerecha < 1000)) && ((lecturaIzquierda > 80) || (lecturaDerecha > 80)))
    {     
      detectado = false;
      tiempoInicio = millis();
    }    
  }
}
double sensorIzquierda()
{
  return distancia(true, trig1, echo1);
}

double sensorDerecha()
{
  return distancia(true, trig2, echo2);
}

double sensorPared()
{
  return distancia(true, trig3, echo3);
}

double tiempo(int trig, int echo)
{
  double duracion;
  digitalWrite(trig, LOW);
  delayMicroseconds(2);   //para asegurarnos que esta en LOW
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,LOW);
  duracion = pulseIn(echo, HIGH);
  return duracion;
}

double distancia(boolean cent, int trig, int echo)
{
  double duracion, centimetros, pulgadas;
  duracion = tiempo(trig, echo);
  centimetros = duracion / 58;
  pulgadas = duracion = duracion / 148;
  if(cent)
  {
    return centimetros;
  }
  else
  {
   return pulgadas;
  }
}

void moverEnfrente(int x)  //mover hacia enfrente representa 1
{
  if(estadoMotores != 1)
  {
    Frenar(); 
  }
  estadoMotores = 1;
  digitalWrite(motor1, HIGH);
  digitalWrite(motor2, LOW);
  Acelerar(x);   //acelera hasta maxima velocidad
}

void moverReversa(int x)  //reversa representa estado motores 2
{
  if(estadoMotores != 2)
  {
    Frenar(); 
  }
  estadoMotores = 2;
  digitalWrite(motor1, LOW);
  digitalWrite(motor2, HIGH);
  Acelerar(x);
}

void girarDerecha()  //girarDerecha representa estado de motores = 3
{
  if(estadoMotores != 3)
  {
    Frenar(); 
  }
  estadoMotores = 3;
  digitalWrite(motor1, HIGH);  
  digitalWrite(motor2, HIGH);
  Acelerar(200); 
}

void girarIzquierda()  //girarIzquierda representa estado de motores = 4
{
  if(estadoMotores != 4)
  {
    Frenar(); 
  }
  estadoMotores = 4;
  digitalWrite(motor1, LOW);  
  digitalWrite(motor2, LOW);
  Acelerar(200); //acelerar hasta 100 (pwm)
}

void Frenar()
{
  while(velocidad !=0)
  {
   velocidad -= 10;
   if(velocidad < 0 )
   {
     velocidad = 0;
   }
   analogWrite(pwm1, velocidad);
   analogWrite(pwm2, velocidad);
   delay(5);
  } 
}

void Acelerar(int velocidadFinal)
{
  if(velocidad < velocidadFinal)
  {
    velocidad=50;
  }
    
  while(velocidad < velocidadFinal)
  {
    velocidad += 5;
    if(velocidad > 255)
    {
      velocidad = 255;
    }
    analogWrite(pwm1, velocidad);
    analogWrite(pwm2, velocidad);
    delay(3);
  }
}

void buscarPared() //metodo para saber si esta enfrente de una pared, si lo esta mover hacia atras y girar
{
  lecturaPared = sensorPared();
  lecturaIzquierda = sensorIzquierda();
  lecturaDerecha = sensorDerecha();
  if(lecturaPared <= 5)
  {
    lecturaPared == 15;
  }
  if( lecturaIzquierda <=5)
  {
    lecturaIzquierda = 12;
  }
  if(lecturaDerecha <=5)
  {
    lecturaDerecha = 12;
  }
  
  if((lecturaPared <= 26 && lecturaPared >= 12 || lecturaPared > 1000)  && (lecturaIzquierda <= 21 && lecturaIzquierda >= 10) && (lecturaDerecha <= 21  && lecturaDerecha >= 10))
 {
   
   tiempoParedFinal = millis();
   if((tiempoParedFinal - tiempoParedInicial) >= 250)
   {
     moverReversa(255);
     delay(750);
     girarDerecha();
     delay(850);
     Serial.println("Pa atras");
     tiempoParedInicial = millis();
   }
   detectado = false;
 }
 else
 {
   tiempoParedInicial = millis();
 }
}

boolean detectarObjetivo()
{
  lecturaIzquierda = sensorIzquierda();
  lecturaDerecha = sensorDerecha();
  if((abs(lecturaDerecha - lecturaIzquierda)) < 5)
  {
    return true;
  }
  else
  {
    return false;
  }
}

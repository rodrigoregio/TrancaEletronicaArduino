/*
 * --------------------------------------------------------------------------------
 * Comandos e pinagens necessárias ao sensor UltraSonico
 * ---------------------------------------------------------------------------------
 */
#include <hcsr04.h>
const int pinoEcho=12;
const int pinoTrigger=13;
HCSR04 ultras(pinoTrigger,pinoEcho,20,4000);
/*
 * ---------------------------------------------------------------------------------
 * Comandos e pinagens do BlueToofh
 * ---------------------------------------------------------------------------------
 */
#include <SoftwareSerial.h>
SoftwareSerial BLE(11,10);//TX,RX
String com="";
/*
 * ---------------------------------------------------------------------------------
 * Comandos e pinagens necessários ao Servo
 * ---------------------------------------------------------------------------------
 */
#include <Servo.h>
const int servoPin=3;
Servo SERVO;
const int servo2Pin=9;
Servo SERVO2;
/*
 * ---------------------------------------------------------------------------------
 * Pinos referentes ao led RGB
 * ---------------------------------------------------------------------------------
 */
const int ledR=5;
const int ledG=6;
const int ledB=7;
/*
 * ---------------------------------------------------------------------------------
 * Pino necessário ao alto falante
 * ---------------------------------------------------------------------------------
 */
const int som=2;
/*
 * ---------------------------------------------------------------------------------
 * Pino referente ao sensor de temperatura
 * ---------------------------------------------------------------------------------
 */
const int sensorT=A4;
/*
 * Método das configurações iniciais do programa
 * Iniciando comunicação serial e com o bluetooth
 * configurando angulo do servo
 * Configurando os pinos do led RGB no sistema
 * Deixando o led azul por 2 segundos
 */
void setup() {
  Serial.begin(9600);
  BLE.begin(9600);
  SERVO.attach(servoPin);
  SERVO.write(180);
  SERVO2.attach(servo2Pin);
  SERVO2.write(100);
  pinMode(ledR,OUTPUT);
  pinMode(ledG,OUTPUT);
  pinMode(ledB,OUTPUT);
  digitalWrite(ledB,HIGH);
  delay(2000);
  digitalWrite(ledB,LOW);
}
/*
 * Metodo responsável por medir a distancia
 * pegando a distância em milimetros
 * Se a distância for maior que 150 milimetros (que dá os 15 centimetros
 * Mostre por bluetooth que a tampa está aberta
 * Senão altere a posição dos servos
 */
void medeDist(){
  float milimeter=ultras.distanceInMillimeters();
  if(milimeter > 150){
    BLE.println("Tampa aberta!");
  }else{
    SERVO.write(180);
    SERVO2.write(100);
  }
}
/*
 * Método responsável por medir a temperatura
 * armazena a temperatura lida do sensor e já realiza o calculo de conversão
 * retorna a temperatura
 */
float medeTemp(){
  float temperatura=(float(analogRead(sensorT))*5/(1023))/0.01;
  return temperatura;
}
/*
 * Este servo não conseguia levantar a tampa (mas foi implementado o codigo o servo mudava de posição mas não aguentava)
 * Método responsável por mudar a posição do segundo servo
 * ele levantaria a tampa e espera 5 segundos e desceria a tampa
 */
void mudarPos2(){
  SERVO2.write(180);
  delay(5000);
  SERVO2.write(100);
}
/*
 * Método responsável por mudar a posição do servo que tranca a porta
 * muda a posição
 * chama o método para levantar a tampa um pouco (do segundo servo, método acima)
 * espera 10 segundos
 * e mede a distância
 */
void mudarPos(){
  SERVO.write(135);
  mudarPos2();
  delay(10000);
  medeDist();
}
/*
 * Método loop (que executa a todo instante
 * Verifica se o bluetooth está disponível
 * espera 10 milisegundos
 * e enquanto a comunicação bluetooth está disponível
 * espera 100 milisegundos
 * caractere variavel d recebe o comando que veio por bluetooth
 *
 * se a variavel acima for igual a 'o' ou 'O'
 *
 * imprima por bluetooth que a tampa pode ser aberta
 * apague a cor vermelha do led
 * e acenda a verde
 * emita um som no autofalante
 * chame o método que muda a posição dos servos
 * pare de emitir som
 * apague a luz verde e acenda a vermelha
 *
 * se a variavel não for 'o' ou 'O' e for 't' ou 'T'
 *
 * imprima por bluetooth "Temperatura atual em celsius: "
 * e imprima o valor medido no metodo medeTemp()
 */
void loop() {
  if(BLE.available()){
  delay(10);
  while(BLE.available()){
    delay(100);
    char d=BLE.read();
    if(d == 'o' || d == 'O'){
      BLE.println("Aberto");
      digitalWrite(ledR,LOW);
      digitalWrite(ledG,HIGH);
      tone(som,1500);
      mudarPos();
      noTone(som);
      digitalWrite(ledG,LOW);
      digitalWrite(ledR,HIGH);
    }else if(d=='t' || d == 'T'){
      BLE.println("Temperatura atual em celsius:");
      BLE.println(medeTemp());
    }
    com+=d;
  }
  Serial.println(com);
  com="";
}

}

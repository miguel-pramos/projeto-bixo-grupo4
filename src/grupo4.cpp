#include <Arduino.h>

// ==========================================
// 1. DEFINICAO DE PINOS (O grupo deve adaptar para o seu Hardware)
// ==========================================
// Pinos dos Encoders (Escolher pinos que suportam interrupcao de hardware)
#define ENC_IN_ESQ_A 0
#define ENC_IN_ESQ_B 1
#define ENC_IN_DIR_A 2
#define ENC_IN_DIR_B 3

// Pinos da Ponte H 
#define MOT_ESQ_PWM 4
#define MOT_ESQ_IN1 5
#define MOT_ESQ_IN2 6
#define MOT_DIR_PWM 7
#define MOT_DIR_IN1 8
#define MOT_DIR_IN2 9

// Extras
int num_dentes = 10;
float dist_rodas = 11.3; // Entre os eixos
float diametro_rodas = 6.5; // Centímetros
float raio = 3.25; // Centímetros
float pi = 3.1415; 
float Velocidades[2];

// Variáveis de Tempo e Velocidade
unsigned long tempoAtual;
unsigned long deltaTempo;
unsigned long tempoAnterior = 0;
double velocidadeAtual = 0;
double velocidadeDesejada = 100.0; // Velocidade alvo em pulsos por segundo

// Constantes PID (Esses valores devem ser ajustados/tunados para cada caso)
double kp = 2.0;
double ki = 0.5;
double kd = 1.0;

// Variáveis PID
double erroIntegral = 0;
double erroAnterior = 0;

// ==========================================
// 2. VARIAVEIS GLOBAIS DE SISTEMA
// ==========================================
// O termo 'volatile' informa ao compilador que a variavel pode mudar a qualquer momento 
// fora do fluxo normal do codigo (ou seja, dentro das interrupcoes).
volatile long ticks_esq = 0;
volatile long ticks_dir = 0;

// Variaveis para garantir que o loop principal rode em frequencia fixa (Sem delay!)
unsigned long tempo_anterior = 0;
const int INTERVALO_AMOSTRAGEM_MS = 50; // Roda o controle a 20Hz

// ==========================================
// 3. CABECALHOS PARA O MICRO-ROS (PARA A AULA 5)
// ==========================================
/* 
 * ATENCAO ALUNOS: Deixem esta secao comentada durante as Aulas 1 a 4.
 * Voces a ativarao na Aula 5 para transformar o microcontrolador em um No ROS 2.
 */
// #include <micro_ros_arduino.h>
// #include <rcl/rcl.h>
// #include <rclc/rclc.h>
// #include <rclc/executor.h>
// #include <geometry_msgs/msg/twist.h>
// #include <nav_msgs/msg/odometry.h>
// rcl_publisher_t odom_publisher;
// rcl_subscription_t cmd_vel_subscriber;

// ==========================================
// 4. INTERRUPCOES DE HARDWARE (ISRs)
// ==========================================
// IRAM_ATTR aloca a funcao na memoria RAM do microcontrolador, garantindo execucao extremamente rapida.
void IRAM_ATTR isr_encoder_esq() {
  int estado_B = digitalRead(ENC_IN_ESQ_B);
  if(ENC_IN_ESQ_B==HIGH){
    ticks_esq++;
  } 
  else{
    ticks_esq--;
  }
}

void IRAM_ATTR isr_encoder_dir() {
  int estado_B = digitalRead(ENC_IN_DIR_B);
  if(ENC_IN_DIR_B==HIGH){
    ticks_dir++;
  } 
  else{
    ticks_dir--;
  }
}

// ==========================================
// 5. FUNcOES DE CALCULO E CONTROLE (AULAS 3 E 4)
// ==========================================
void calcula_odometria(float Velocidades[2]) {
  // O resgate de variaveis volatile precisa ser rapido. 
  // Desligamos as interrupcoes por um microssegundo para copiar os valores e nao corromper os dados.
  noInterrupts();
  long ticks_atuais_esq = ticks_esq;
  long ticks_atuais_dir = ticks_dir;
  ticks_esq = 0;
  ticks_dir = 0;
  interrupts();

  int tempo;
  float distancia_esq, distancia_dir;
  float v_angular_direita, v_angular_robo, v_angular_esquerda;
  float velocidade_linear_robo, velocidade_linear_esq, velocidade_linear_dir;
  distancia_dir = 2*pi*raio*(ticks_atuais_dir/num_dentes);
  distancia_esq = 2*pi*raio*(ticks_atuais_esq/num_dentes); 
  tempo = INTERVALO_AMOSTRAGEM_MS / 1000; // Tempo em segundos;
  
  v_angular_direita = distancia_dir/(raio*tempo);
  v_angular_esquerda = distancia_esq/(raio*tempo);
  velocidade_linear_robo = (distancia_dir + distancia_esq)/(2 * tempo);
  v_angular_robo = (ticks_atuais_dir - ticks_atuais_esq) / (tempo * dist_rodas);

  velocidade_linear_dir = distancia_dir / tempo;
  velocidade_linear_esq = distancia_esq / tempo;

  Serial.println(v_angular_direita);
  Serial.println(v_angular_esquerda);
  Serial.println(velocidade_linear_robo);
  Serial.println(v_angular_robo);

  Velocidades[0] = velocidade_linear_dir;
  Velocidades[1] = velocidade_linear_esq;
}


void logicaPIDdireita(float velocidadeAtual)
{
  // 2. Algoritmo PID

  // Proporcional
  double erro = velocidadeDesejada - velocidadeAtual;

  // Integral
  erroIntegral += erro * (deltaTempo / 1000.0);

  // Derivativo
  double erroDerivativo = (erro - erroAnterior) / (deltaTempo / 1000.0);

  // Fórmula final do OUTPUT (Esforço de Controle)
  double sinalSaida = (kp * erro) + (ki * erroIntegral) + (kd * erroDerivativo);


  // 3. Converter o OUTPUT (Esforço de Controle) para PWM (0-255 com direção)

  // Lidar com a direção
  if (sinalSaida > 0)
  {
    // Frente
    digitalWrite(MOT_DIR_IN1, HIGH);
    digitalWrite(MOT_DIR_IN2, LOW);
  }
  else
  {
    // Ré / Freio
    digitalWrite(MOT_DIR_IN1, LOW);
    digitalWrite(MOT_DIR_IN2, HIGH);
  }

  // Valor absoluto (módulo)
  int pwm = abs(sinalSaida);

  // IMPORTANTE: limitar a saída para o intervalo possível
  if (pwm > 255)
  {
    pwm = 255;
  }

  // 4. Enviar saída para o motor (PWM)
  analogWrite(MOT_DIR_PWM, pwm);

  // 5. Atualizar variáveis para o próximo ciclo
  tempoAnterior = tempoAtual;
  // Alguma lógica associada ao cálculo da velocidade pode vir aqui

  // Monitor Serial
  Serial.print("Desejada:");
  Serial.print(velocidadeDesejada);
  Serial.print("Atual:");
  Serial.println(velocidadeAtual);
}

void logicaPIDesquerda(float velocidadeAtual)
{
  // 1. Obter a velocidade atual do motor, isso pode vir de uma lógia externa à esta função
  // neste caso, está sendo feito no loop, atualizada na variável 'velocidadeAtual'


  // 2. Algoritmo PID

  // Proporcional
  double erro = velocidadeDesejada - velocidadeAtual;

  // Integral
  erroIntegral += erro * (deltaTempo / 1000.0);

  // Derivativo
  double erroDerivativo = (erro - erroAnterior) / (deltaTempo / 1000.0);

  // Fórmula final do OUTPUT (Esforço de Controle)
  double sinalSaida = (kp * erro) + (ki * erroIntegral) + (kd * erroDerivativo);


  // 3. Converter o OUTPUT (Esforço de Controle) para PWM (0-255 com direção)

  // Lidar com a direção
  if (sinalSaida > 0)
  {
    // Frente
    digitalWrite(MOT_ESQ_IN1, HIGH);
    digitalWrite(MOT_ESQ_IN2, LOW);
  }
  else
  {
    // Ré / Freio
    digitalWrite(MOT_ESQ_IN1, LOW);
    digitalWrite(MOT_ESQ_IN2, HIGH);
  }

  // Valor absoluto (módulo)
  int pwm = abs(sinalSaida);

  // IMPORTANTE: limitar a saída para o intervalo possível
  if (pwm > 255)
  {
    pwm = 255;
  }

  // 4. Enviar saída para o motor (PWM)
  analogWrite(MOT_ESQ_PWM, pwm);

  
  // 5. Atualizar variáveis para o próximo ciclo
  tempoAnterior = tempoAtual;
  // Alguma lógica associada ao cálculo da velocidade pode vir aqui

  // Monitor Serial
  Serial.print("Desejada:");
  Serial.print(velocidadeDesejada);
  Serial.print("Atual:");
  Serial.println(velocidadeAtual);
}

// ==========================================
// SETUP INICIAL
// ==========================================
void setup() {
  Serial.begin(115200);
  
  // Configuracao dos pinos dos encoders (INPUT_PULLUP previne ruidos caso o encoder seja do tipo open-collector)
  pinMode(ENC_IN_ESQ_A, INPUT_PULLUP);
  pinMode(ENC_IN_ESQ_B, INPUT_PULLUP);
  pinMode(ENC_IN_DIR_A, INPUT_PULLUP);
  pinMode(ENC_IN_DIR_B, INPUT_PULLUP);
  
  // Acoplando as interrupcoes aos pinos A de cada motor.
  // "RISING" significa que a interrupcao dispara quando o sinal sobe de 0V para 3.3V/5V.
  attachInterrupt(digitalPinToInterrupt(ENC_IN_ESQ_A), isr_encoder_esq, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_IN_DIR_A), isr_encoder_dir, RISING);

  // TODO: Configurar os pinos da Ponte H como saidas (OUTPUT) e configurar os canais PWM.
  
  // ==========================================
  // SETUP DO MICRO-ROS (Comentado para Aulas 1-4)
  // ==========================================
  // set_microros_transports();
  // rclc_support_init(...);
  // rclc_node_init_default(...);
  // rclc_publisher_init_default(...);
  
  Serial.println("Sistema Iniciado. Aguardando inicio dos ciclos de controle...");
}

// ==========================================
// LOOP PRINCIPAL (Arquitetura Nao-Bloqueante)
// ==========================================
void loop() {
  unsigned long tempo_atual = millis();
  // Quanto tempo passou desde a última atualização
  unsigned long deltaTempo = tempoAtual - tempoAnterior;
  
  // Verifica se ja passou o tempo necessario (ex: 50ms) para rodar o controle novamente
  if (deltaTempo >= INTERVALO_AMOSTRAGEM_MS) {
    
    calcula_odometria(Velocidades);
    logicaPIDdireita(Velocidades[0]);
    logicaPIDesquerda(Velocidades[1]);

    // ==========================================
    // INTEGRAcaO ROS 2 (Apenas na Aula 5)
    // ==========================================
    // TODO: Popular a mensagem nav_msgs/Odometry com os dados calculados por voces e publicar:
    // rcl_publish(&odom_publisher, &odom_msg, NULL);
    // rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
    
    // (Uso para as Aulas 3 e 4) - Log Serial para os Engenheiros de Dados plotarem graficos!
    Serial.print("Ticks_Esq:"); 
    Serial.print(ticks_esq);
    Serial.print("\tTicks_Dir:"); 
    Serial.println(ticks_dir);
    
    // Atualiza o relogio para a proxima execucao
    tempo_anterior = tempo_atual;
  }

  // Atualiza a cada 100ms
}


#include <Arduino.h>
#include <WiFi.h>
#include <micro_ros_platformio.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <geometry_msgs/msg/twist.h>
#include <geometry_msgs/msg/point.h>

#include "secrets.h"

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

// Limites de seguranca para os comandos de velocidade (AJUSTAR conforme o robo)
const float VEL_LINEAR_MAX_MS = 0.3f;   // m/s
const float VEL_ANGULAR_MAX_RADS = 2.0f; // rad/s

// Ganhos do controlador proporcional que leva o robo ate uma coordenada (ver secao 6)
const float KP_LINEAR = 0.6f;
const float KP_ANGULAR = 1.5f;

// ==========================================
// 2. VARIAVEIS GLOBAIS DE SISTEMA
// ==========================================
// O termo 'volatile' informa ao compilador que a variavel pode mudar a qualquer momento
// fora do fluxo normal do codigo (ou seja, dentro das interrupcoes).
volatile long ticks_esq = 0;
volatile long ticks_dir = 0;

// Velocidade angular de cada roda (rad/s) que o controle_pid() deve perseguir.
// Atualizadas pelos callbacks do micro-ROS (cmd_vel ou bixo/goal), ver secoes 5 e 6.
volatile float setpoint_omega_esq = 0.0f;
volatile float setpoint_omega_dir = 0.0f;

// Variaveis para garantir que o loop principal rode em frequencia fixa (Sem delay!)
unsigned long tempo_anterior = 0;
const int INTERVALO_AMOSTRAGEM_MS = 50; // Roda o controle a 20Hz

// ==========================================
// 3. MICRO-ROS
// ==========================================
// Handles do rcl/rclc. O transporte usado e WiFi/UDP (ver README e ./bixo wifi),
// falando com o micro-ROS agent (docker) configurado em docker-compose.yml.
rcl_subscription_t cmd_vel_subscriber;
rcl_subscription_t goal_subscriber;
geometry_msgs__msg__Twist cmd_vel_msg;
geometry_msgs__msg__Point goal_msg;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

#define RCCHECK(fn)                     \
  {                                     \
    rcl_ret_t rc = fn;                  \
    if (rc != RCL_RET_OK) error_loop(); \
  }
#define RCSOFTCHECK(fn) \
  {                     \
    rcl_ret_t rc = fn;  \
    (void)rc;           \
  }

void error_loop() {
  while (true) {
    Serial.println("error_loop: rcl init falhou");
    delay(500);
  }
}

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
// 6. CINEMATICA DIFERENCIAL + INTEGRACAO ROS 2
// ==========================================
// Converte uma velocidade linear (v, m/s) e angular (w, rad/s) do centro do robo
// nas velocidades angulares (rad/s) de cada roda e atualiza os setpoints do controle_pid().
void aplica_velocidade(float v, float w) {
  v = constrain(v, -VEL_LINEAR_MAX_MS, VEL_LINEAR_MAX_MS);
  w = constrain(w, -VEL_ANGULAR_MAX_RADS, VEL_ANGULAR_MAX_RADS);

  float vel_linear_esq = v - (w * dist_rodas / 2.0f);
  float vel_linear_dir = v + (w * dist_rodas / 2.0f);

  setpoint_omega_esq = vel_linear_esq / raio;
  setpoint_omega_dir = vel_linear_dir / raio;
}

// Assina geometry_msgs/Twist em "cmd_vel": usa linear.x como v e angular.z como w.
void cmd_vel_callback(const void *msgin) {
  const geometry_msgs__msg__Twist *msg = (const geometry_msgs__msg__Twist *)msgin;
  aplica_velocidade((float)msg->linear.x, (float)msg->angular.z);
}

// Calcula (v, w) para levar o robo ate uma coordenada (x, y) no referencial do robo,
// simplificando o robo na origem (0, 0) virado para o eixo Y positivo (Y = frente, X = lateral).
// Controlador proporcional simples: gira em direcao ao alvo e anda proporcional a distancia.
// Nao substitui um planejador de trajetoria de verdade, e um ponto de partida.
void coordenada_para_velocidade(float x, float y, float *v, float *w) {
  float distancia = sqrtf(x * x + y * y);
  float angulo_para_alvo = atan2f(x, y); // 0 = alvo na frente (+Y), >0 = alvo a direita

  *w = KP_ANGULAR * angulo_para_alvo;
  *v = KP_LINEAR * distancia;

  // Gira no lugar primeiro se o alvo estiver muito fora do eixo de frente,
  // so anda pra frente depois de estar (quase) apontado pra ele.
  if (fabsf(angulo_para_alvo) > 0.3f) {
    *v = 0.0f;
  }
}

// Assina geometry_msgs/Point em "bixo/goal": trata (x, y) como coordenada-alvo (ver acima) e
// converte direto em setpoints de roda, reaproveitando aplica_velocidade().
void goal_callback(const void *msgin) {
  const geometry_msgs__msg__Point *msg = (const geometry_msgs__msg__Point *)msgin;
  float v, w;
  coordenada_para_velocidade((float)msg->x, (float)msg->y, &v, &w);
  aplica_velocidade(v, w);
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

  // TODO (Aula 2): Configurar os pinos da Ponte H como saidas (OUTPUT) e configurar os canais PWM.

  // ==========================================
  // SETUP DO MICRO-ROS
  // ==========================================
  // Credenciais de wifi/agent ficam fora do repo: gere firmware/src/secrets.h com `./bixo wifi ...`
  // (veja firmware/src/secrets.h.example e o README).
  IPAddress agent_ip;
  agent_ip.fromString(AGENT_IP);
  set_microros_wifi_transports((char *)WIFI_SSID, (char *)WIFI_PASS,
                                agent_ip, AGENT_PORT);
  WiFi.setSleep(false); // evita "could not send data: 12" (ENOMEM) por modem-sleep
  delay(2000);

  allocator = rcl_get_default_allocator();

  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
  RCCHECK(rclc_node_init_default(&node, "bixo_esp32c3_node", "", &support));

  RCCHECK(rclc_subscription_init_default(
      &cmd_vel_subscriber, &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist), "cmd_vel"));

  RCCHECK(rclc_subscription_init_default(
      &goal_subscriber, &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Point), "bixo/goal"));

  RCCHECK(rclc_executor_init(&executor, &support.context, 2, &allocator));
  RCCHECK(rclc_executor_add_subscription(&executor, &cmd_vel_subscriber, &cmd_vel_msg,
                                          &cmd_vel_callback, ON_NEW_DATA));
  RCCHECK(rclc_executor_add_subscription(&executor, &goal_subscriber, &goal_msg,
                                          &goal_callback, ON_NEW_DATA));

  // TODO (Aula 5): publicar odometria (nav_msgs/Odometry) com os dados de calcula_odometria(),
  // assim que ela estiver implementada, para o PC acompanhar a pose do robo.

  Serial.println("Sistema Iniciado. Aguardando inicio dos ciclos de controle...");
}

// ==========================================
// LOOP PRINCIPAL (Arquitetura Nao-Bloqueante)
// ==========================================
void loop() {
  unsigned long tempo_atual = millis();

  // Verifica se ja passou o tempo necessario (ex: 50ms) para rodar o controle novamente
  if (tempo_atual - tempo_anterior >= INTERVALO_AMOSTRAGEM_MS) {

    calcula_odometria(Velocidades);
    logicaPIDdireita(Velocidades[0]);
    logicaPIDesquerda(Velocidades[1]);

    // (Uso para as Aulas 3 e 4) - Log Serial para os Engenheiros de Dados plotarem graficos!
    Serial.print("Ticks_Esq:");
    Serial.print(ticks_esq);
    Serial.print("\tTicks_Dir:");
    Serial.print(ticks_dir);
    Serial.print("\tSetpoint_Esq(rad/s):");
    Serial.print(setpoint_omega_esq);
    Serial.print("\tSetpoint_Dir(rad/s):");
    Serial.println(setpoint_omega_dir);

    // Atualiza o relogio para a proxima execucao
    tempo_anterior = tempo_atual;
  }

  RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10)));
}

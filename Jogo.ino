/*Projeto final da disciplina de Lógica de Programação - IFSC
  Autores: Geovane Schmitz e Pamela Fialho
  Turma: 622
  Data: 08/09/2020
  Função: Jogo da memória utilizando botões e LEDs, onde o usuário deve repetir a sequência de LEDs acesos através da ativação dos botões correspondentes*/

//Adicionando as bibliotecas
#include <LiquidCrystal.h> //Biblioteca para utilização do Display LCD 16x2

//Definindo as variáveis
#define LIMIT 500       //Variável que limita a quantidade de níveis/fases dentro do jogo
#define OPTIONS_COUNT 4 //Variável que determina o número de LEDs, e consequentemente de botões, que o jogador irá utilizar para jogar
int level = 0;          //Variável que determina o nível/fase do jogo que a pessoa se encontra
int max_points = 0;     //Variável que conta o máximo de pontos conquistados desde a última vez que o jogo foi ligado
int is_record = 0;      //Variável que controla o alcance de recordes de pontos, em relação ao máximo de pontos contabilizados pelo jogo
short difficulty = 0;   //Variável que determina a dificuldade da rodada

//Definindo os pinos de conexão
#define LED_0 13        //Conexão do LED verde no pino digital 13 do arduino
#define LED_1 10        //Conexão do LED vermelho no pino digital 10 do arduino
#define LED_2 9         //Conexão do LED azul no pino digital 9 do arduino
#define LED_3 8         //Conexão do LED branco no pino digital 8 do arduino
#define BUTTON_0 A0     //Conexão do botão 1 correspondente ao LED verde no pino analógico 0 do arduino
#define BUTTON_1 A1     //Conexão do botão 2 correspondente ao LED vermelho no pino analógico 1 do arduino
#define BUTTON_2 A2     //Conexão do botão 3 correspondente ao LED azul no pino analógico 2 do arduino
#define BUTTON_3 A3     //Conexão do botão 4 correspondente ao LED branco no pino analógico 3 do arduino
#define BUTTON_RESET A4 //Conexão do botão responsável por fazer o reset do jogo e a confirmação da escolha de dificuldade no pino analógico 4 do arduino

//Definindo os pinos de interface do Display LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//Definindo os vetores
const int buttons[OPTIONS_COUNT] = {BUTTON_0, BUTTON_1, BUTTON_2, BUTTON_3}; //Vetor responsável por organizar os botões
const int leds[OPTIONS_COUNT] = {LED_0, LED_1, LED_2, LED_3};                //Vetor responsável por organizar os LEDs
short order[LIMIT] = {0};                                                    //Vetor responsável por organizar os LEDs

//Definindo as funções
void reset();             //Define a função que reseta as configurações dos jogos para o padrão inicial
void show_sequece();      //Define a função que mostra a sequência de LEDs que deverá ser repetida nos botões
void add_sequece();       //Define a função que adiciona mais uma sequência de LEDs, caso o jogador não esteja no último nível/fase
void check_if_win();      //Define a função que verifica se o jogador ganhou o jogo, se chegou no último nível/fase
void read_buttons();      //Define a função que realiza a leitura dos botões
void show_init_message(); //Define a função que coloca no Display LCD a mensagem inicial do jogo
void select_difficulty(); //Define a função que inicia o jogo com a seleção da dificuldade do nível que será jogado
void update_display();    //Define a função que faz a atualização dos dados no Display LCD

void setup() //Função inicial do sistema
{
  lcd.begin(16, 2);       //Define o número de colunas e linhas do Display LCD
  lcd.setCursor(0, 1);    //Define a posição inicial do cursor do Display LCD na primeira coluna e segunda linha
  pinMode(LED_0, OUTPUT); //Define o pino digital associado ao LED verde como saída
  pinMode(LED_1, OUTPUT); //Define o pino digital associado ao LED vermelho como saída
  pinMode(LED_2, OUTPUT); //Define o pino digital associado ao LED azul como saída
  pinMode(LED_3, OUTPUT); //Define o pino digital associado ao LED branco como saída

  pinMode(BUTTON_0, INPUT);     //Define o pino analógico associado ao primeiro botão, referente ao LED verde, como entrada
  pinMode(BUTTON_1, INPUT);     //Define o pino analógico associado ao segundo botão, referente ao LED vermelho, como entrada
  pinMode(BUTTON_2, INPUT);     //Define o pino analógico associado ao terceiro botão, referente ao LED azul, como entrada
  pinMode(BUTTON_3, INPUT);     //Define o pino analógico associado ao quarto botão, referente ao LED branco, como entrada
  pinMode(BUTTON_RESET, INPUT); //Define o pino analógico associado ao botão de reset como entrada

  pinMode(3, INPUT);   //Define o pino digital associado ao pino DB5 do Display LCD como entrada
  show_init_message(); //Chama a função que coloca no Display LCD a mensagem inicial do jogo
  reset();             //Chama a função que reseta o jogo e inicia os comandos do jogo
  delay(2000);         //Delei entre o final do jogo e o começo
}

void loop() //Função loop principal do sistema
{
  update_display(); //Chama a função que faz a atualização dos dados no Display LCD
  check_if_win();   //Chama a função que verifica se o jogador ganhou o jogo, se chegou no último nível/fase
  add_sequece();    //Chama a função que adiciona mais uma sequência de LEDs caso o jagador não esteja no último nível/fase
  show_sequece();   //Chama a função que mostra a sequência de LEDs que deverá ser repetida nos botões
  delay(500);       //Tempo entre a amostragem dos LEDs e da leitura dos botões
  read_buttons();   //Chama a função que realiza a leitura dos botões
}

void reset() //Define a função que reseta as confugurações dos jogos para o padrão inicial
{
  is_record = 0;                          //Zera a variável de controle de recorde alcançado
  level = 0;                              //Zera a contabilização do nível em que o jogador está
  for (int i = 0; i < OPTIONS_COUNT; i++) //Acende os LEDs conforme o número de LEDs determinados pela variável OPTIONS_COUNT
    digitalWrite(leds[i], HIGH);

  delay(500); //Tempo dos LEDs ficarem acesos e sinalizar que o jogo está sendo iniciado

  for (int i = 0; i < OPTIONS_COUNT; i++) //Desliga os LEDs conforme o número de LEDs determinados pela variável OPTIONS_COUNT
    digitalWrite(leds[i], LOW);
  select_difficulty();                    //Chama a função que inicia o jogo com a seleção da dificuldade do nível que será jogado
}

void show_sequece() //Define a função que mostra a sequência de LEDs que deverá ser repetida nos botões
{
  int interval = 2000 / (difficulty + 1); //Define uma variável para o tempo que o LED ficará aceso; Quanto maior a dificuldade, menor será o tempo
  for (int i = 0; i < level; i++)         //Executa os comandos conforme o nível/fase que o jogador está
  {
    int pin = leds[order[i]]; //Pega o pino do LED da sequência pelo nível/fase que o jogador se encontra
    digitalWrite(pin, HIGH);  //Acende o LED
    delay(interval);          //Tempo para o LED ficar aceso
    digitalWrite(pin, LOW);   //Apaga o LED
  }
}

void add_sequece() //Define a função que adiciona mais uma sequência de LEDs caso o jogador não esteja no último nível/fase
{
  if (level < LIMIT) //Se o jogador estiver em um nível/fase anterior ao limite de níveis determinado pela variável LIMIT
  {
    order[level] = (short)random(0, OPTIONS_COUNT);       //Escolhe um LED aleatoriamente, conforme o número de LEDs determinados pela variável OPTIONS_COUNT
    while (level > 0 && order[level] == order[level - 1]) //Caso o valor mostrado anteriormente pelo LED tenha sido escolhido novamente, um novo será gerado
    {
      order[level] = (short)random(0, OPTIONS_COUNT); //Escolhe um LED aleatoriamente, conforme o número de LEDs determinados pela variável OPTIONS_COUNT
    }
    level++; //Incremento na variável level para registrar o nível/fase em que o jogador se encontra
  }
}

void check_if_win() //Define a função que verifica se o jogador ganhou o jogo, se chegou no último nível/fase
{
  if (level >= LIMIT) //Se o nível/fase que o jogador estiver for maior ou igual que o limite de nível/fase escolhido
  {
    lcd.clear();               //Limpa o Display LCD
    lcd.setCursor(3, 0);       //Define a posição do cursor do Display LCD na quarta coluna e primeira linha
    lcd.print("Parabens");     //Coloca no Display LCD a mensagem de parabenização
    lcd.setCursor(2, 0);       //Define a posição do cursor do Display LCD na terceira coluna e primeira linha
    lcd.print("Voce ganhou!"); //Coloca no Display LCD a mensagem dizendo que o jogador ganhou o jogo
    delay(2000);               //Tempo antes de realizar o reset
    reset();                   //Chama a função que realizar o reset do jogo e o inicia
  }
}

void read_buttons() //Define a função que realiza a leitura dos botões
{
  int i = 0;        //Define uma variável para controle do loop
  while (i < level) //Enquanto a variável i de controle for menor que o nível/fase que o jogador está irá fazer a leitura dos botões
  {
    if (digitalRead(BUTTON_RESET)) //Se o botão de reset for ativado, o jogo será resetado
    {
      reset(); //Chama a função que reseta as confugurações dos jogos para o padrão inicial
    }
    int correct_answer = order[i];          //Define uma variável que armazena a resposta certa
    int is_lost = 0;                        //Define uma variável de controle para quando o jogador errar uma sequência
    for (int j = 0; j < OPTIONS_COUNT; j++) //Realiza a leitura dos botões conforme o número de botões determinados pela variável OPTIONS_COUNT
    {
      if (digitalRead(buttons[j])) //Se há inserção de sinais nos pinos ligados aos botões, o jogo confere se a sequência inserida está correta
      {
        if (correct_answer == j) //Se o jogador inserir corretamente a sequência de LEDs, esse fato é contabilizado na variável i de controle do loop
        {
          i += 1;                         //Incremento na variável de controle, fazendo com que o jogo faça o mesmo número de leituras dos botões em relação ao número de vezes que os LEDs foram acesos
          digitalWrite(leds[j], HIGH);    //Acende o LED que o jogador inseriu
          while (digitalRead(buttons[j])) //Enquanto estiver ocorrendo a leitura dos botões, não irá ocorrer nada
            ;
          digitalWrite(leds[j], LOW); //Apaga o LED que o jogador inseriu
        }
        else //Se o jogador não inserir corretamente a sequência de LEDs, esse fato é contabilizado na variável is_lost de controle
        {
          is_lost = 1; //Contabiliza que o jogador errou a sequência
          break;       //Termina o loop
        }
      }
    }
    if (is_lost) //Caso o jogador tenha errado a sequência de LEDs, uma mensagem o sinaliza sobre e o jogo é resetado
    {
      lcd.clear();                   //Limpa o Display LCD
      lcd.setCursor(0, 0);           //Define a posição do cursor do Display LCD na primeira coluna e primeira linha
      lcd.print("Tente novamente!"); //Coloca no Display LCD a mensagem avisando que o jogador errou e pode tentar novamente
      lcd.setCursor(7, 1);           //Define a posição do cursor do Display LCD na oitava coluna e primeira linha
      lcd.print(":(");               //Coloca no Display LCD a mensagem com rostinho triste pois o jogador perdeu o jogo
      delay(2000);                   //Tempo que a mensagem é mantida no Display LCD
      reset();                       //Chama a função que reseta o jogo e inicia os comandos do jogo
      break;                         //Termina o loop
    }
  }
}

void show_init_message() //Define a função que coloca no Display LCD a mensagem inicial do jogo
{
  lcd.setCursor(0, 0);          //Define a posição do cursor do Display LCD na primeira coluna e primeira linha
  lcd.clear();                  //Limpa o Display LCD
  lcd.print("Jogo da memoria"); //Coloca no Display LCD a mensagem com o nome do jogo
  delay(2000);                  //Tempo que a mensagem é mantida no Display LCD
}

void select_difficulty() //Define a função que inicia o jogo com a seleção da dificuldade do nível que será jogado
{
  lcd.clear();                          //Limpa o Display LCD
  lcd.setCursor(2, 0);                  //Define a posição do cursor do Display LCD na terceira coluna e primeira linha
  lcd.print("Dificuldade:");            //Coloca no Display LCD a mensagem falando para selecionar a dificuldade que será jogada
  lcd.setCursor(3, 1);                  //Define a posição do cursor do Display LCD na quarta coluna e segunda linha
  lcd.print("1  2  3  4");              //Coloca no Display LCD a mensagem com os números representando as dificuldades que poderão ser jogadas
  lcd.setCursor(3 + difficulty * 3, 1); //Define a posição do cursor do Display LCD na posição da dificuldade escolhida pelo jogador
  lcd.blink();                          //Display LCD ativa a visualização do cursor sobre o número escolhido
  while (!digitalRead(BUTTON_RESET))    //Enquanto o botão de reset não for pressionado, será feita a leitura dos botões
  {
    for (int i = 0; i < OPTIONS_COUNT; i++) //Leitura dos botões conforme o número de botões determinados pela variável OPTIONS_COUNT
    {
      if (digitalRead(buttons[i])) //Leitura da seleção dos botões para seleção da dificuldade
      {
        lcd.setCursor(3 + i * 3, 1); //Define a posição do cursor do Display LCD na posição da dificuldade escolhida pelo jogador
        difficulty = i;              //Variável difficulty é igualada a variável i de controle do loop
      }
    }
  }
  lcd.noBlink();          //Display LCD desativa a visualização do cursor
  lcd.clear();            //Limpa o Display LCD
  lcd.setCursor(3, 0);    //Define a posição do cursor do Display LCD na quarta coluna e primeira linha
  lcd.print("Bom jogo!"); //Coloca no Display LCD a mensagem desejando bom jogo
}

void update_display() //Define a função que faz a atualização dos dados no Display LCD
{
  lcd.clear();            //Limpa o Display LCD
  lcd.setCursor(0, 0);    //Define a posição do cursor do Display LCD na primeira coluna e primeira linha
  lcd.print("Pontos: ");  //Coloca no Display LCD a mensagem que antecede o número de pontos
  lcd.print(level);       //Coloca no Display LCD a variável level, que diz o número do nível/fase que o jogador está
  if (level > max_points) //Se o nível/fase atual do jogador ultrapassar o máximo de níveis/fases atingido, desde que o jogo foi ligado pela última vez, a variável de máximo de pontos será atualizada
  {
    max_points = level; //A variável que conta o máximo de níveis/fases atingido assume o valor do nível/fase atual do jogador
    if (!is_record)     //Se for a primeira vez que ocorre um recorde de pontos, em relação ao máximo de níveis/fases atingido desde que o jogo foi ligado pela última vez, haverá uma comemoração visual para o usuário
    {
      is_record = 1;   //Muda o valor da variável de controle de recorde para contabilizar que houve um
      lcd.noDisplay(); //Desliga o Display LCD
      delay(100);      //Tempo para piscar o Display LCD
      lcd.display();   //Liga o Display LCD
      delay(100);      //Tempo para piscar o Display LCD
      lcd.noDisplay(); //Desliga o Display LCD
      delay(100);      //Tempo para piscar o Display LCD
      lcd.display();   //Liga o Display LCD
    }
  }
  lcd.setCursor(0, 1);    //Define a posição do cursor do Display LCD na primeira coluna e segunda linha
  lcd.print("Recorde: "); //Coloca no Display LCD a mensagem que antecede o número de pontos recorde
  lcd.print(max_points);  //Coloca no Display LCD a variável max_points, que diz o máximo de níveis/fases atingido, desde que o jogo foi ligado pela última vez
}

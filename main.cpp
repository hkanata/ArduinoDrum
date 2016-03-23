//  *****************************************************************************************************************
//  *                                                                                                               *
//  *                                     opba.com.br                                                               *
//  *****************************************************************************************************************
//*******************************************************************************************************************
// Variáveis de Definição do Usuário.                      
//*******************************************************************************************************************

//0 chimbal
//1 caixa 
//2 TON 1
//3 TON 2
//4 PRATO CONDUCAO
//5 bumbo
//6 SURDO1 67
//7 SURDO2 65
//8 prato ataque 77
//9 prato direita 79
//10 splash master 81

unsigned char PadNote[11] = {49,38,71, 69,60,36, 67,65,77, 79,81}; // Notas MIDI - 0 a 127
int PadCutOff[11] = {600,450,300, 300,300,600, 300,300,300, 600,600}; // Valor Mínimo do Sensor para causar o som
int MaxPlayTime[11] = {10,20,30, 30,30,30, 30,30,30, 30,30}; // Ciclos a passar antes da Segunda Batida ser acionada.
#define  midichannel    0;                              // Canal Midi
boolean VelocityFlag  = true;                           // Se o som será de acordo com a intensidade da Batida.
 
//*******************************************************************************************************************
// Variáveis de uso Interno                   
//*******************************************************************************************************************
 
boolean activePad[11] = {0,0,0, 0,0,0, 0,0,0, 0,0};                   // Salva se os pads estao ativos ou nao.
int PinPlayTime[11] = {0,0,0, 0,0,0, 0,0,0, 0,0};                     // Contador dos ciclos desde que o pad foi acionado.
 
unsigned char status;
 
int pin = 0;    
int hitavg = 0;
 
//*******************************************************************************************************************
// Setup                       
//*******************************************************************************************************************

int butao = 13;
int caixa = 12;
int bumbo = 11;
int valor;

int chimbalPe = 13;
int retornoChimbal;
boolean chimbalApertado = false;

int valorCaixa;
int valorBumbo;

void setup()
{
  Serial.begin(115200);     
  pinMode(chimbalPe, INPUT);
                 
}
long previousMillis = 0;
long interval = 3000; 
//*******************************************************************************************************************
// Main Program                
//*******************************************************************************************************************

void loop()
{

	retornoChimbal = digitalRead(13);
	chimbalApertado = false;
	if( retornoChimbal == 1 ) {
		chimbalApertado = true;
    } 
  
  for(int pin=0; pin < 11; pin++) // Percorre os Pinos Analógicos
  {
    hitavg = analogRead(pin);  // Lê o Valor do Sensor                            
 
    if((hitavg > PadCutOff[pin]))  // Verifica se o valor pego pelo sensor é maior que o Valor minimo para causar o Som
    {
      if((activePad[pin] == false))  // Verifica se o Pad já está sendo executado.
      {
        if(VelocityFlag == true)  // Verifica se o som será de acordo com a Intensidade da Batida, para gerar o Sinal Midi.
        {
          //hitavg = 127 / ((1023 - PadCutOff[pin]) / (hitavg - PadCutOff[pin]));    // With full range (Too sensitive ?)

          hitavg = (hitavg / 8) -1 ;                                                 // Upper range
        }
        else
        {
          hitavg = 127;
        }

   
          MIDI_TX(144,PadNote[pin],hitavg);
       
        
        if(!chimbalApertado){
          if(PadNote[pin] == 49){
         
            MIDI_TX(144,56,hitavg);
         
          
         }
        }else{
        //MIDI_TX(144,PadNote[pin],hitavg); // Joga o SInal MIDI  
        }
 
        
        PinPlayTime[pin] = 0;  //Seta o Ciclo para '0'
        activePad[pin] = true;  // Altera o Pad para Ativo.
      }
      else
      {
        PinPlayTime[pin] = PinPlayTime[pin] + 1; // Caso o Pad ja esteja ativo, incrementa 1 Ciclo.
      }
    }
    else if((activePad[pin] == true)) // ESTA SEGUNDA PARTE É RESPONSÁVEL APENAS POR INCREMENTAR OS CICLOS E ATIVAR/DESATIVAR OS PADS.
    {
      PinPlayTime[pin] = PinPlayTime[pin] + 1;
     
      if(PinPlayTime[pin] > MaxPlayTime[pin])
      {
        activePad[pin] = false;
        

        MIDI_TX(128,PadNote[pin],127);
      }
    }
  }
}
 
 
//*******************************************************************************************************************
// Função que transmite o MIDI                       
//*******************************************************************************************************************
void MIDI_TX(unsigned char MESSAGE, unsigned char PITCH, unsigned char VELOCITY)
{
  status = MESSAGE + midichannel;
  Serial.write(status);
  Serial.write(PITCH);
  Serial.write(VELOCITY);
}

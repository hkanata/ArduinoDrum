/*
	*********************
	**** opba.com.br ****
	*********************
*/

// Variáveis dos sons
// - 0 chimbal
// - 1 caixa 
// - 2 TON 1
// - 3 TON 2
// - 4 PRATO CONDUCAO
// - 5 bumbo
// - 6 SURDO1 67
// - 7 SURDO2 65
// - 8 prato ataque 77
// - 9 prato direita 79
// - 10 splash master 81

//Notas MID
unsigned char PadNote[11] = {49,38,71, 69,60,36, 67,65,77, 79,81};

// Valor Mínimo do Sensor para causar o som
int PadCutOff[11] = {600,450,300, 300,300,600, 300,300,300, 600,600}; 

// Ciclos a passar antes da Segunda Batida ser acionada.
int MaxPlayTime[11] = {10,20,30, 30,30,30, 30,30,30, 30,30}; 

// Canal Midi
#define  midichannel    0;                             
 
// Se o som será de acordo com a intensidade da Batida.
boolean VelocityFlag  = true;                           

// Variáveis de uso Interno                    
// Salva se os pads estao ativos ou nao.
boolean activePad[11] = {0,0,0, 0,0,0, 0,0,0, 0,0}; 
                  
// Contador dos ciclos desde que o pad foi acionado.
int PinPlayTime[11] = {0,0,0, 0,0,0, 0,0,0, 0,0};                     
 
unsigned char status;
 
int pin = 0;    
int hitavg = 0;
int chimbal = 13;
int chimbalNote = 56;

int chimbalReturn;
boolean chimbalPressed = false;

long previousMillis = 0;
long interval = 3000; 

void setup()
{
	Serial.begin(115200);     
	pinMode(chimbal, INPUT);       
}


void loop()
{
	chimbalReturn = digitalRead(13);
	chimbalPressed = false;
	if( chimbalReturn == 1 ) {
		chimbalPressed = true;
    	} 
  
	for(int pin=0; pin < 11; pin++){
		
		hitavg = analogRead(pin);
		if((hitavg > PadCutOff[pin])){
			if((activePad[pin] == false)){
				if(VelocityFlag == true){
					hitavg = (hitavg / 8) -1;
				}else{
					hitavg = 127;
				}
				MIDI_TX(144,PadNote[pin],hitavg);
				if(!chimbalPressed){
					if(PadNote[pin] == 49){
						MIDI_TX(144,chimbalNote,hitavg);
					}
				}
				PinPlayTime[pin] = 0;  
				activePad[pin] = true;  
			}else{
				PinPlayTime[pin] = PinPlayTime[pin] + 1; 
			}
		}else if((activePad[pin] == true)){
			PinPlayTime[pin] = PinPlayTime[pin] + 1;
			if(PinPlayTime[pin] > MaxPlayTime[pin]){
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

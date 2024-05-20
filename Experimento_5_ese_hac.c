#include <REG51F.H>

char TxBuffer[16];
unsigned char TxIn = 0;
unsigned char TxOut = 0;
char RxBuffer[16];
unsigned char RxIn = 0;
unsigned char RxOut = 0;

unsigned char TxOcupado = 0;
unsigned char RecebeuString = 0;

void serial_inicializa(){
	
	TR1 = 0;
	TMOD = (TMOD & 0x0f) | 0x2f;
	ET1 = 0;
	TF1 = 1;
	TR1 = 1;
	// Mantendo os valores do Experimento 4
	SCON = 0x5c;
	TH1 = 0xe6;
	PCON =  PCON & 0x7f;
}

void InsertFromSerialToRxBuffer(char c){
	if ((RxIn + 1) % 16 == RxOut){
		return ;
	}
	RxBuffer[RxIn] = c;
	
	if(RxIn == 15){
		RxIn = 0;
	}else{
		RxIn++;
	}
	if(c == '$'){
		RecebeuString++;
	}
	return;
}
char TxBufferVazio(){
	if(TxIn == TxOut){
		return 1;
	}else{
		return 0;
	}
}
void interruption_function(void) interrupt 4{
	char TxVazio;
	if(RI == 1){
		RI = 0;
		InsertFromSerialToRxBuffer(SBUF);
		
	}
	if (TI == 1){
		TI = 0;
		TxVazio = TxBufferVazio();
		if(!TxVazio){
			SBUF = TxBuffer[TxOut];
			if(TxOut == 15){
				TxOut = 0;
			}else{
				TxOut++;
			}
		}else{
			TxOcupado = 0;
		}
		
	}
	
}
char SendChar(char c){
	if((TxIn + 1)% 16 == TxOut){
		return 0;
	}
	TxBuffer[TxIn] = c;
	if(TxIn == 15){
		TxIn = 0;
	}else{
		TxIn++;
	}
	if(!TxOcupado){
		TxOcupado = 1;
		TI = 1;
	}
	return 1;
}
void SendString(char *s){

	char result;
	
	while(*s != '$'){
		result = SendChar(*s);
		s++;
	}
	result = SendChar(*s);
	return;
}
char ReceiveChar(){
	
	char result;
	
	result = RxBuffer[RxOut];
	
	if(RxOut == 15){
		RxOut = 0;
	}else{
		RxOut++;
	}
	return result;
}

void ReceiveString(char *s){
	char nextChar;
	
	nextChar = ReceiveChar();
	while(nextChar != '$'){
		*s = nextChar;
		s++;
		nextChar = ReceiveChar();
	}
	*s = nextChar;
	RecebeuString--;
	return;
}
char RxBufferVazio(){
	if(RxIn == RxOut){
		return 1;
	}else{
		return 0;
	}
}
void main(){
	char temp[16];
	char RxVazio;
	serial_inicializa();
	ES = 1;
	EA = 1;
	
	
	while(1){
		if(RecebeuString > 0){
			RxVazio = RxBufferVazio();
			if(RxVazio == 1){
				continue;
			}
			ReceiveString(temp);
			SendString(temp);
		}
	} 
}
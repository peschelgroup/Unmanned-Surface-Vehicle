// 0:r<CR>
// 1:i<CR>
// 2:c<CR>
// 3:r<CR>

#include <SoftwareSerial.h>         //Include the software serial library  

SoftwareSerial sSerial(11, 10);     // RX, TX  - Name the software serial library sftSerial (this cannot be omitted)
                                    // assigned to pins 10 and 11 for maximum compatibility
int s0 = 7;                         // Tentacle uses pin 7 for multiplexer control S0
int s1 = 6;                         // Tentacle uses pin 6 for multiplexer control S1
int enable_1 = 5;              // Tentacle uses pin 5 to control pin E on shield 1
int enable_2 = 4;             // Tentacle uses pin 4 to control pin E on shield 2

char computerdata[20];              // A 20 byte character array to hold incoming data from a pc/mac/other
char sensordata[30];                // A 30 byte character array to hold incoming data from the sensors
byte computer_bytes_received = 0;   // We need to know how many characters bytes have been received
byte sensor_bytes_received = 0;     // We need to know how many characters bytes have been received

char *channel;                      // Char pointer used in string parsing
char *cmd;                          // Char pointer used in string parsing



void setup() {
  pinMode(s0, OUTPUT);             // set the digital output pins for the serial multiplexer
  pinMode(s1, OUTPUT);
  pinMode(enable_1, OUTPUT);
  pinMode(enable_2, OUTPUT);

  Serial.begin(9600);              // Set the hardware serial port to 9600
  sSerial.begin(9600);             // Set the soft serial port to 9600 (change if all your devices use another baudrate)
  intro();                         // display startup message
}


void serialEvent() {               //This interrupt will trigger when the data coming from the serial monitor(pc/mac/other) is received
  computer_bytes_received = Serial.readBytesUntil(13, computerdata, 20); //We read the data sent from the serial monitor(pc/mac/other) until we see a <CR>. We also count how many characters have been received
  computerdata[computer_bytes_received] = 0;      //We add a 0 to the spot in the array just after the last character we received.. This will stop us from transmitting incorrect data that may have been left in the buffer
}

void loop() {
  int x;
  int y;
  char beginning = 's';
  char ending = 'e';
  String out = "";
  for(x = 0; x < 4; x++){     
      if(x == 0){
        out+=beginning;
      }
      computerdata[0] = x + '0';
      computerdata[1] = ':';
      computerdata[2] = 'r';
      computerdata[3] = '\0';
     // Serial.println(computerdata);                                 // If computer_bytes_received does not equal zero
      channel = strtok(computerdata, ":");          // Let's parse the string at each colon
      cmd = strtok(NULL, ":");
      open_channel();                               // Call the function "open_channel" to open the correct data path
      if (cmd != 0) {                               // if no command has been sent, send nothing
        sSerial.print(cmd);                         // Send the command from the computer to the Atlas Scientific device using the softserial port
        sSerial.print("\r");                        // <CR> carriage return to terminate message
      }
      computer_bytes_received = 0;      
    delay(1000);
    if (sSerial.available() > 0) {                 // If data has been transmitted from an Atlas Scientific device
      sensor_bytes_received = sSerial.readBytesUntil(13, sensordata, 30); //we read the data sent from the Atlas Scientific device until we see a <CR>. We also count how many character have been received
      sensordata[sensor_bytes_received] = 0;       // we add a 0 to the spot in the array just after the last character we received. This will stop us from transmitting incorrect data that may have been left in the buffer
      for(y = 0; y < strlen(sensordata); y++){
        out+= sensordata[y];
      }
      if(x != 3){
        out+= ", ";
      }else{
        out+=ending;
        out+='\0'; 
       }
      sensor_bytes_received = sSerial.readBytesUntil(13, sensordata, 30); //we read the data sent from the Atlas Scientific device until we see a <CR>. We also count how many character have been received
      sensordata[sensor_bytes_received] = 0;       // we add a 0 to the spot in the array just after the last character we received. This will stop us from transmitting incorrect data that may have been left in the buffer
    }
    sSerial.flush();
    delay(1000);
  }
  Serial.println(out);
}

// Open a channel via the Tentacle serial multiplexer
void open_channel() {

  switch (*channel) {

    case '0':                                // if channel==0 then we open channel 0
      digitalWrite(enable_1, LOW);           // setting enable_1 to low activates primary channels: 0,1,2,3
      digitalWrite(enable_2, HIGH);          // setting enable_2 to high deactivates secondary channels: 4,5,6,7
      digitalWrite(s0, LOW);                 // S0 and S1 control what channel opens
      digitalWrite(s1, LOW);                 // S0 and S1 control what channel opens
      break;

    case '1':
      digitalWrite(enable_1, LOW);
      digitalWrite(enable_2, HIGH);
      digitalWrite(s0, HIGH);
      digitalWrite(s1, LOW);
      break;

    case '2':
      digitalWrite(enable_1, LOW);
      digitalWrite(enable_2, HIGH);
      digitalWrite(s0, LOW);
      digitalWrite(s1, HIGH);
      break;

    case '3':
      digitalWrite(enable_1, LOW);
      digitalWrite(enable_2, HIGH);
      digitalWrite(s0, HIGH);
      digitalWrite(s1, HIGH);
      break;

    case '4':
      digitalWrite(enable_1, HIGH);
      digitalWrite(enable_2, LOW);
      digitalWrite(s0, LOW);
      digitalWrite(s1, LOW);
      break;

    case '5':
      digitalWrite(enable_1, HIGH);
      digitalWrite(enable_2, LOW);
      digitalWrite(s0, HIGH);
      digitalWrite(s1, LOW);
      break;

    case '6':
      digitalWrite(enable_1, HIGH);
      digitalWrite(enable_2, LOW);
      digitalWrite(s0, LOW);
      digitalWrite(s1, HIGH);
      break;

    case '7':
      digitalWrite(enable_1, HIGH);
      digitalWrite(enable_2, LOW);
      digitalWrite(s0, HIGH);
      digitalWrite(s1, HIGH);
      break;
  }
}


// Print intro
void intro() {
  Serial.flush();
  Serial.println(" ");
//Serial.println("READY_");
}

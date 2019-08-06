//NAME: serialTest.ino
//AUTH: Ryan McCartney
//DATE: 11/06/2019
//NOTE: Serial Passthrough test for API

String inputString = "";
volatile bool stringComplete = false;
int loops = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Setup Complete");
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  
  delay(500);                      
  digitalWrite(LED_BUILTIN, LOW);    
  delay(500);                   
  Serial.print("INFO: ");
  Serial.print(loops);
  Serial.println(" loops have passed.");
   
  if(stringComplete == true){
    Serial.print("Data received is '");
    Serial.print(inputString);
    Serial.println("'.");
    stringComplete = false;
    }
  loops++;
}

void serialEvent() {

  inputString = "";
  while (!stringComplete){

    //Gets the next byte
    char inputChar = (char)Serial.read();
       
    if (inputChar == '\n'){
      stringComplete = true;
    }
    else{
      //Add latest char to string
      inputString += inputChar;
    }
  }
}

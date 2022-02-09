int brightness=0, fadeamt=5,count=0;
int bstate=0,lastate=0;
int button=9;
long lCnt = 0;
unsigned long lStart;
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(5,OUTPUT);
  Serial.begin(9600);
  pinMode(button,INPUT);
}
void LEDBLNK(){
    digitalWrite(3,HIGH);
    digitalWrite(11,HIGH);
  	digitalWrite(5,HIGH);
  	delay(100);
  	digitalWrite(3, LOW);
  	digitalWrite(5,LOW);
  	digitalWrite(11,LOW);
  	delay(100);
  }
void LEDFADE(){
  for (brightness = 0; brightness <= 255; brightness += 5) {
    analogWrite(3, brightness);
    analogWrite(5, brightness);
    analogWrite(11, brightness);
    delay(50);
  }
  delay(50);
  for (brightness = 255; brightness >= 0; brightness -= 5) {
    analogWrite(3, brightness);
    analogWrite(5, brightness);
    analogWrite(11, brightness);
    delay(50);
  }
}
void reset()
{
  signal(5, 0);
  signal(3, 0);
  signal(11, 0);
}

void signal(int led, bool on)
{
  if (on)
  {
    digitalWrite(led, HIGH);
  }
  else
  {
    digitalWrite(led, LOW);
  }
}
void LEDCOUNT(){
  reset();
  for (int i = 0; i < 8; ++i)
  {
    signal(11, ((i >> 2) % 2) == 1);
    signal(5, ((i >> 1) % 2) == 1);
    signal(3, ((i >> 0) % 2) == 1);
    delay(2000);
  }
}
void loop(){
  if( lCnt == 0)
{
 lStart = millis();
}
lCnt++;
while( (millis() - lStart) <= 3000)
{
  delay(50);
  bstate = digitalRead(button);
  if (bstate != lastate) {
    if (bstate == HIGH) {
     count++;
     Serial.print("Number of button presses: ");
     Serial.println(count);
   }
  }
   delay(50);
  lastate = bstate;
}
  delay(500);
   if (count == 0) {
   digitalWrite(3, 0);
   digitalWrite(5, 0);
   digitalWrite(11, 0);
 }
  if (count==1){
    LEDFADE();
  }
  else if(count==2){
    LEDBLNK();
  }
  else if(count==3){
    LEDCOUNT();
    count=0;
  }
}
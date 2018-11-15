#define SCLK 5
#define SIN 4
#define XLAT 0
#define BLANK 2

int Nstars = 8;
float starsmag[] = {3.3, 0.5, 1.6, 2.23, 1.7, 1.77, 0.1, 2.09}; // top to bottom
float starsmagabs[] = {-4.25, -5.85, -2.78, -5.8, -6.89, -6, -7.84, -6.1 }; // top to bottom

float dimmer = .5;
float mag2flux(float mag){
  return dimmer*pow(2.512,-mag)*4000.;
}

float magabs2flux(float mag){
  return dimmer*pow(2.512,-mag)*2.9;
}

void setup() {
  Serial.begin(115200);
  Serial.write("Startup...\n");
  // put your setup code here, to run once:
  pinMode(SCLK, OUTPUT);  
  pinMode(SIN, OUTPUT);  
  pinMode(XLAT, OUTPUT);  
  pinMode(BLANK, OUTPUT);  

  digitalWrite(SCLK, LOW);
  digitalWrite(SIN, LOW);
  digitalWrite(XLAT, LOW);
  digitalWrite(BLANK, LOW);

  delay(100);
  Serial.write("...complete\n");
  
}

void wait(){
  delayMicroseconds(10);
}

void write12bit(unsigned int value){
  for (int i=11;i>=0;i--){
    digitalWrite(SIN, (value>>i)&1);
    wait();
    digitalWrite(SCLK, HIGH);
    wait();
    digitalWrite(SCLK, LOW);
  }
}

void latch(){
  wait();
  digitalWrite(XLAT, HIGH);
  wait();
  digitalWrite(XLAT, LOW);
  wait(); 
}

#define NLED 12
float fade = 0.;
int starmode = 0;

float wave_inset = 10;
float wave_x = 0;
float wave_dx = 0.025;
float wave_width = 1.;

unsigned int led_values[NLED]; 

void loop() {
  wave_x += wave_dx;
  if (wave_x>NLED+wave_inset){
       wave_x = -wave_inset;
  }
  for(int i=0;i<NLED;i++){
    led_values[i] = floor(4095.*exp(-(i-wave_x)*(i-wave_x)/(wave_width*wave_width)));
  }
  for(int i=0;i<NLED;i++){
    write12bit(led_values[i]);
  }
  
  latch();
  delay(1);


  
//  for(int i=0; i<Nstars; i++){
//    
//    //if (i!=act) v = 0;
//    if (starmode==0){
//      int v = int(fade*mag2flux(starsmag[i]));
//      int vabs = int((1.-fade)*magabs2flux(starsmagabs[i]));
//      write12bit(v+vabs);
//    }
//    if (starmode==1){
//      int v = int((1.-fade)*mag2flux(starsmag[i]));
//      int vabs = int(fade*magabs2flux(starsmagabs[i]));
//      write12bit(v+vabs);
//    }
//    if (starmode==2){
//      int v = 0;
//      if (floor(fade*8.)==i) v = 4095;
//      write12bit(v);
//    }
//    if (starmode==3){
//      int v = int((1.-fade)*magabs2flux(starsmagabs[i]));
//      write12bit(v);
//    }
//      
//    
//    //Serial.write("%d",v);
//    //Serial.write("\n");
//
//  } 
//  fade += 0.01;
//  if (fade>1.){
//    fade=0.;
//    starmode +=1;
//    if (starmode>=3) starmode =0;
//  }
//  
//  //Serial.write("\n");
//  latch();
//  delay(10);

}

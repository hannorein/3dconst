#define SCLK 5
#define SIN 4
#define XLAT 0
#define BLANK 2  // also LED (low = on)



void setup() {
  Serial.begin(115200);
  Serial.write("Startup...\n");
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

#define NSTARS 12
#define NLED 16
#define LED_OFFSET 4

float flux_on_earth[NSTARS] = {2243, 1470, 1139, 1795, 1158, 2199, 4095, 1928, 1052, 3677, 2288, 1453};
float flux_from_fixed_distance[NSTARS] = {4095, 2619, 1286, 1526, 952, 1488, 2478, 1017, 328, 680, 136, 11};
float flux_zero[NSTARS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int starmode = 0;

float wave_inset = 10.;
float wave_x = 0.;
float wave_dx = 0.025;
float wave_width = 1.;

float fade_x;
int fade(float from[NSTARS],float to[NSTARS], unsigned int* out, float dx){
  for (int i=0;i<NSTARS;i++){
    out[i+LED_OFFSET] = from[i]*(1.-fade_x)+to[i]*fade_x;
  }
  
  fade_x += dx;
  if (fade_x>=1.){
    fade_x = 0.;
    return 1;
  }else{
    return 0;
  }
}


unsigned int led_values[NLED]; 
void loop() {

  switch (starmode){
    case 0: // Wave Left to Right
      wave_x += wave_dx;
      for(int i=0;i<NSTARS;i++){
        led_values[i+LED_OFFSET] = floor(4095.*exp(-(i-wave_x)*(i-wave_x)/(wave_width*wave_width)));
      }
      if (wave_x>NSTARS+wave_inset){
        wave_x = -wave_inset;
        starmode++;
      }
      break;
    case 1: 
      starmode += fade(flux_zero,flux_on_earth, led_values, 0.01);
      break;
    case 2: 
      starmode += fade(flux_on_earth,flux_from_fixed_distance, led_values, 0.01);
      break;  
    case 3: 
      starmode += fade(flux_from_fixed_distance,flux_on_earth, led_values, 0.01);
      break;
    case 4: 
      starmode += fade(flux_on_earth,flux_from_fixed_distance, led_values, 0.01);
      break;    
    case 5: 
      starmode += fade(flux_from_fixed_distance,flux_zero, led_values, 0.01);
      break;          
   default:
      starmode = 0;
  }
  
  
  for(int i=0;i<NLED;i++){
    write12bit(led_values[i]);
  }
  latch();
  delay(1);

}

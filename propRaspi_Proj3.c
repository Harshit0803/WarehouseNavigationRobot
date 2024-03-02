// header files and static and pre-processor directivrs are declared
#include "simpletools.h"

#include "servo.h"

#define IR_PINC 4
#define IR_PINL 5
#define IR_PINR 6
#define LED_PININT 10
#define Sig1 8
#define A 13
#define B 12
const int trigPin = 7;
const int echoPin = 11;
const int trigPin1 = 1;
const int echoPin1 = 0;

// specs required for COG declared here
static int cog, cog1, cog2;
unsigned int stack[128];
unsigned int stack1[128];
unsigned int stack2[128];

// servo pins
int servoPinL = 14;
int servoPinR = 15;

// outputs from COG
volatile int ultra_r;
volatile int ultra_l;
static volatile int a = 0; // setting it to base value

// funcs declared
void lineFollow();
void dir_right();
void dir_left();
void rot_right();
void rot_left();
void turn_right();
void turn_left();
void hard_right();
void hard_left();
void end_right();
void end_left();
void correct_right();
void correct_left();
void ultra1(void * par);
void ultra2(void * par1);
void led_int(void * par2);

int main()                                    // Main function
{
  // Add startup code here.
  //cogs enabled
  cog = cogstart( & ultra1, NULL, stack, sizeof(stack));
  cog1 = cogstart( & ultra2, NULL, stack1, sizeof(stack1)); 
  cog2 = cogstart( & led_int, NULL, stack2, sizeof(stack2));
  pause(32000);
  int i =0;
  while(!(i==3))
  {
	// pin used for communication with Pi
    low(Sig1);
    
    int ir_valueC = input(IR_PINC);
    int ir_valueL = input(IR_PINL);
    int ir_valueR = input(IR_PINR);
    
	// linefollow until otherwise
    while(!(ir_valueC == 1 && ir_valueL == 1 && ir_valueR == 1)) {
      lineFollow();
      ir_valueC = input(IR_PINC);
      ir_valueL = input(IR_PINL);
      ir_valueR = input(IR_PINR);
    }
    
	//call to COG for intersection LED
    a = 1;  
	
	// communication with Pi for enabling the directional aid program
    high(Sig1);
    printf("Sig1 on\n");
	
	//wait till direction detected
    while(input(A) == 0 && input(B) == 0) {
      servo_speed(servoPinL, 0);
      servo_speed(servoPinR, 0);
    }
	
	//setting pin to low
    low(Sig1);
    
	//taking a turn as per input received from Pi and completing one whole circuit	
    if(input(A) == 1){
      printf("Taking right\n");
      dir_right();
    }
    else { 
      printf("Taking left\n");

      dir_left();
    }  
    i=i+1;          
  }  

  //line follow after exit from all the three circuits
  int ir_valueC = input(IR_PINC);
  int ir_valueL = input(IR_PINL);
  int ir_valueR = input(IR_PINR);
  
    print("Case 4 now initiating\n");
  while(!(ir_valueC == 0 && ir_valueL == 0 && ir_valueR == 0)){
    lineFollow();
    ir_valueC = input(IR_PINC);
    ir_valueL = input(IR_PINL);
    ir_valueR = input(IR_PINR);
  } 
  

  
  high(Sig1);

  //low(Sig1);
  
  while(input(A) == 0 && input(B) == 0) {
    servo_speed(servoPinL, 0);
    servo_speed(servoPinR, 0);
  }
  if(input(A)==1){
    end_right();
  }    
  
  else{
    end_left();
  }    
      
  low(Sig1);
  
}

void ultra1(void *par) {
   while (1) {
      pulse_out(trigPin, 10);
      long tEcho = pulse_in(echoPin, 1);

      int cmDist = tEcho / 58;
      ultra_r = cmDist;

      pause(100);
   }
}

void ultra2(void *par1) {
   while (1) {
      pulse_out(trigPin1, 10);
      long tEcho = pulse_in(echoPin1, 1);

      int cmDist = tEcho / 58;
      ultra_l = cmDist;

      pause(100);
   }
}

void lineFollow() {
   int ir_valueC = input(IR_PINC);
   int ir_valueL = input(IR_PINL);
   int ir_valueR = input(IR_PINR);
   //condition for servo ultrasonic and IR in A line
   if (ir_valueC == 1 && ir_valueL == 0 && ir_valueR == 0) {
      servo_speed(servoPinL, 30);
      servo_speed(servoPinR, -30);

   } else if (ir_valueC == 1 && ir_valueL == 1 && ir_valueR == 0) {
      servo_speed(servoPinL, 0);
      servo_speed(servoPinR, -30);

   } else if (ir_valueC == 1 && ir_valueL == 0 && ir_valueR == 1) {
      servo_speed(servoPinL, 30);
      servo_speed(servoPinR, 0);

   } else if (ir_valueC == 0 && ir_valueL == 0 && ir_valueR == 0) {
      servo_speed(servoPinL, 0);
      servo_speed(servoPinR, 0);

   } else if (ir_valueC == 0 && ir_valueL == 0 && ir_valueR == 1) {
      servo_speed(servoPinL, 30);
      servo_speed(servoPinR, 0);

   } else if (ir_valueC == 0 && ir_valueL == 1 && ir_valueR == 0) {
      servo_speed(servoPinL, 0);
      servo_speed(servoPinR, -30);

   }
}

// when the camera detects a right orientated triangle, the following func() is executed:
void dir_right() {
  rot_right();
  
  int ir_valueC = input(IR_PINC);
  int ir_valueL = input(IR_PINL);
  int ir_valueR = input(IR_PINR);
  while(!(ir_valueC == 1 && ir_valueL == 1 && ir_valueR == 1)) {
    lineFollow();
    ir_valueC = input(IR_PINC);
    ir_valueL = input(IR_PINL);
    ir_valueR = input(IR_PINR);
  } 
     
  rot_left();
  print("%d cm1\n", ultra_r);
	print("%d cm2\n", ultra_l);
  
  while(ultra_r > 20) {
    ir_valueC = input(IR_PINC);
    ir_valueL = input(IR_PINL);
    ir_valueR = input(IR_PINR);
    if(ir_valueC == 1 && ir_valueL == 1 && ir_valueR == 1){
      break;
    }      
    else{
      lineFollow();
      print("%d cm1\n", ultra_r);
	    print("%d cm2\n", ultra_l);
    } 
       
  } 
  //cogstop(cog);
  print("%d cm1\n", ultra_r);
	print("%d cm2\n", ultra_l);
  
  servo_speed(servoPinL, 30);
  servo_speed(servoPinR, -30);
  pause(500);
     
  turn_right();
  
  servo_speed(servoPinL, -30);
  servo_speed(servoPinR, 30);
  pause(500);
  
  high(Sig1);

  while(input(A) == 0 && input(B) == 0) {
    servo_speed(servoPinL, 0);
    servo_speed(servoPinR, 0);
  } 
  low(Sig1);
  
  servo_speed(servoPinL, 30);
  servo_speed(servoPinR, -30);
  pause(500);
  
  turn_right();
  
  ir_valueC = input(IR_PINC);
  ir_valueL = input(IR_PINL);
  ir_valueR = input(IR_PINR);
  while(!(ir_valueC == 1 && ir_valueL == 1 && ir_valueR == 1)) {
    lineFollow();
    ir_valueC = input(IR_PINC);
    ir_valueL = input(IR_PINL);
    ir_valueR = input(IR_PINR);
  }
  
  rot_right();
  
  ir_valueC = input(IR_PINC);
  ir_valueL = input(IR_PINL);
  ir_valueR = input(IR_PINR);
  while(!(ir_valueC == 1 && ir_valueL == 1 && ir_valueR == 1)) {
    lineFollow();
    ir_valueC = input(IR_PINC);
    ir_valueL = input(IR_PINL);
    ir_valueR = input(IR_PINR);
  }
  
  a = 1; 
  
  servo_speed(servoPinL, 30);
  servo_speed(servoPinR, -30);
  pause(500);
  
  
  ir_valueC = input(IR_PINC);
  ir_valueL = input(IR_PINL);
  ir_valueR = input(IR_PINR);
  while(!(ir_valueC == 1 && ir_valueL == 1 && ir_valueR == 1)) {
    lineFollow();
    ir_valueC = input(IR_PINC);
    ir_valueL = input(IR_PINL);
    ir_valueR = input(IR_PINR);
  }
  
  rot_right();
  
  while(ultra_l > 20) { 
    
    lineFollow();
    
    
  }
  
  servo_speed(servoPinL, 30);
  servo_speed(servoPinR, -30);
  pause(500);
  
  turn_left();
  
  servo_speed(servoPinL, -30);
  servo_speed(servoPinR, 30);
  pause(500);
  
  high(Sig1);
  while(input(A) == 0 && input(B) == 0) {
    servo_speed(servoPinL, 0);
    servo_speed(servoPinR, 0);
  }
  low(Sig1);
  
  servo_speed(servoPinL, 30);
  servo_speed(servoPinR, -30);
  pause(500);
  
  turn_right();
  
  ir_valueC = input(IR_PINC);
  ir_valueL = input(IR_PINL);
  ir_valueR = input(IR_PINR);
  while(!(ir_valueC == 1 && ir_valueL == 1 && ir_valueR == 1)) {
    lineFollow();
    ir_valueC = input(IR_PINC);
    ir_valueL = input(IR_PINL);
    ir_valueR = input(IR_PINR);
  }
  
  hard_right();
  
  ir_valueC = input(IR_PINC);
  ir_valueL = input(IR_PINL);
  ir_valueR = input(IR_PINR);
  while(!(ir_valueC == 1 && ir_valueL == 1 && ir_valueR == 1)) {
    lineFollow();
    ir_valueC = input(IR_PINC);
    ir_valueL = input(IR_PINL);
    ir_valueR = input(IR_PINR);
  }
  
  a = 1; 
  
  rot_left();
  
  
}  


// when the camera detects a left orientated triangle, the following func() is executed:
void dir_left() {
  rot_left();
  
  int ir_valueC = input(IR_PINC);
  int ir_valueL = input(IR_PINL);
  int ir_valueR = input(IR_PINR);
  while(!(ir_valueC == 1 && ir_valueL == 1 && ir_valueR == 1)) {
    lineFollow();
    ir_valueC = input(IR_PINC);
    ir_valueL = input(IR_PINL);
    ir_valueR = input(IR_PINR);
  }
  
  printf("Intersection\n");
  /*servo_speed(servoPinL, 0);
  servo_speed(servoPinR, 0);
  pause(100);*/
  rot_right();
  while(ultra_l > 20) {
    lineFollow();
  }
  
  servo_speed(servoPinL, 30);
  servo_speed(servoPinR, -30);
  pause(500);
  
  printf("Turning\n");
  turn_left();
  
  servo_speed(servoPinL, -30);
  servo_speed(servoPinR, 30);
  pause(500);
  
  high(Sig1);
  while(input(A) == 0 && input(B) == 0) {
    servo_speed(servoPinL, 0);
    servo_speed(servoPinR, 0);
  }
  low(Sig1);
  
  servo_speed(servoPinL, 30);
  servo_speed(servoPinR, -30);
  pause(500);
  
  turn_left();
  
  ir_valueC = input(IR_PINC);
  ir_valueL = input(IR_PINL);
  ir_valueR = input(IR_PINR);
  while(!(ir_valueC == 1 && ir_valueL == 1 && ir_valueR == 1)) {
    lineFollow();
    ir_valueC = input(IR_PINC);
    ir_valueL = input(IR_PINL);
    ir_valueR = input(IR_PINR);
  }
  
  rot_left();
  
  ir_valueC = input(IR_PINC);
  ir_valueL = input(IR_PINL);
  ir_valueR = input(IR_PINR);
  while(!(ir_valueC == 1 && ir_valueL == 1 && ir_valueR == 1)) {
    lineFollow();
    ir_valueC = input(IR_PINC);
    ir_valueL = input(IR_PINL);
    ir_valueR = input(IR_PINR);
  }
  
  a = 1; 
  servo_speed(servoPinL, 30);
  servo_speed(servoPinR, -30);
  pause(500);
  
  ir_valueC = input(IR_PINC);
  ir_valueL = input(IR_PINL);
  ir_valueR = input(IR_PINR);
  while(!(ir_valueC == 1 && ir_valueL == 1 && ir_valueR == 1)) {
    lineFollow();
    ir_valueC = input(IR_PINC);
    ir_valueL = input(IR_PINL);
    ir_valueR = input(IR_PINR);
  }
  
  rot_left();
  
  print("%d cm1\n", ultra_r);
	print("%d cm2\n", ultra_l);

  while(ultra_r > 20) {
    lineFollow();
    print("%d cm1\n", ultra_r);
	print("%d cm2\n", ultra_l);
  }
  print("%d cm1\n", ultra_r);
	print("%d cm2\n", ultra_l);

  servo_speed(servoPinL, 30);
  servo_speed(servoPinR, -30);
  pause(550);
  
  turn_right();
  
  servo_speed(servoPinL, -30);
  servo_speed(servoPinR, 30);
  pause(500);
  
  high(Sig1);
  while(input(A) == 0 && input(B) == 0) {
    servo_speed(servoPinL, 0);
    servo_speed(servoPinR, 0);
  }
  low(Sig1);
  
  servo_speed(servoPinL, 30);
  servo_speed(servoPinR, -30);
  pause(500);
  
  turn_left();
  
  ir_valueC = input(IR_PINC);
  ir_valueL = input(IR_PINL);
  ir_valueR = input(IR_PINR);
  while(!(ir_valueC == 1 && ir_valueL == 1 && ir_valueR == 1)) {
    lineFollow();
    ir_valueC = input(IR_PINC);
    ir_valueL = input(IR_PINL);
    ir_valueR = input(IR_PINR);
  }
  
  hard_left();
  
  ir_valueC = input(IR_PINC);
  ir_valueL = input(IR_PINL);
  ir_valueR = input(IR_PINR);
  while(!(ir_valueC == 1 && ir_valueL == 1 && ir_valueR == 1)) {
    lineFollow();
    ir_valueC = input(IR_PINC);
    ir_valueL = input(IR_PINL);
    ir_valueR = input(IR_PINR);
  }
  
  a = 1;
   
  rot_right(); 
  
 
}  

void hard_right(){
  servo_speed(servoPinL, 30);
  servo_speed(servoPinR, -30);
  pause(700);
  servo_speed(servoPinL, 30);
  servo_speed(servoPinR, 30);
  pause(1400);
  
  int ir_valueC = input(IR_PINC);
  int ir_valueL = input(IR_PINL);
  int ir_valueR = input(IR_PINR);
  if(ir_valueC == 0 && ir_valueL == 0 && ir_valueR == 0){
    correct_right();
  }    

}

void hard_left(){
  servo_speed(servoPinL, 30);
  servo_speed(servoPinR, -30);
  pause(700);
  servo_speed(servoPinL, -30);
  servo_speed(servoPinR, -30);
  pause(1400);
  
  int ir_valueC = input(IR_PINC);
  int ir_valueL = input(IR_PINL);
  int ir_valueR = input(IR_PINR);
  if(ir_valueC == 0 && ir_valueL == 0 && ir_valueR == 0){
    correct_left();
  }

}
  

void rot_right() {
  printf("Right\n");
  servo_speed(servoPinL, 30);
  servo_speed(servoPinR, -30);
  pause(400);
  
  servo_speed(servoPinL, 30);
  servo_speed(servoPinR, 30);
  pause(1600);
  
  int ir_valueC = input(IR_PINC);
  int ir_valueL = input(IR_PINL);
  int ir_valueR = input(IR_PINR);
  if(ir_valueC == 0 && ir_valueL == 0 && ir_valueR == 0){
    correct_right();
  }

}  


void rot_left() {
  printf("Left\n");
  servo_speed(servoPinL, 30);
  servo_speed(servoPinR, -30);
  pause(250);
  
  servo_speed(servoPinL, -30);
  servo_speed(servoPinR, -30);
  pause(1600);
  
  
  int ir_valueC = input(IR_PINC);
  int ir_valueL = input(IR_PINL);
  int ir_valueR = input(IR_PINR);
  if(ir_valueC == 0 && ir_valueL == 0 && ir_valueR == 0){
    correct_left();
  }
}  

void turn_right() {
  print("turn right\n");
  servo_speed(servoPinL, 30);
  servo_speed(servoPinR, 30);
  pause(1300);
  int ir_valueC = input(IR_PINC);
  int ir_valueL = input(IR_PINL);
  int ir_valueR = input(IR_PINR);
  
} 

void turn_left() {
  servo_speed(servoPinL, -30);
  servo_speed(servoPinR, -30);
  pause(1500);
  int ir_valueC = input(IR_PINC);
  int ir_valueL = input(IR_PINL);
  int ir_valueR = input(IR_PINR);
  

}   

// func for rotating the BOT clockwise till the widget at the docking station is detected.
void end_right(){
  servo_speed(servoPinL, 0);
  servo_speed(servoPinR, 0);
  pause(10000);
  
//  high(Sig1);
  
  while(input(B)== 0){
    servo_speed(servoPinL, 10);
    servo_speed(servoPinR, 10);
  }
  
  int ir_valueC = input(IR_PINC);
  int ir_valueL = input(IR_PINL);
  int ir_valueR = input(IR_PINR);
  while(!(ir_valueC == 1 || ir_valueL == 1 || ir_valueR == 1)) {
    servo_speed(servoPinL, 10);
    servo_speed(servoPinR, -15);
    ir_valueC = input(IR_PINC);
    ir_valueL = input(IR_PINL);
    ir_valueR = input(IR_PINR);
  }
  servo_speed(servoPinL, 0);
  servo_speed(servoPinR, 0);
//  low(Sig1);
  
  
}  


// func for rotating the BOT anti-clockwise till the widget at the docking station is detected.
void end_left(){
  servo_speed(servoPinL, 0);
  servo_speed(servoPinR, 0);
  pause(10000);
 
//  high(Sig1);
  
  while(input(B)== 0){
    servo_speed(servoPinL, -10);
    servo_speed(servoPinR, -10);
  }
  
  int ir_valueC = input(IR_PINC);
  int ir_valueL = input(IR_PINL);
  int ir_valueR = input(IR_PINR);
  while(!(ir_valueC == 1 || ir_valueL == 1 || ir_valueR == 1)) {
    servo_speed(servoPinL, 10);
    servo_speed(servoPinR, -15);
    ir_valueC = input(IR_PINC);
    ir_valueL = input(IR_PINL);
    ir_valueR = input(IR_PINR);
  }
  servo_speed(servoPinL, 0);
  servo_speed(servoPinR, 0);
//   low(Sig1);
  
}  

void led_int(void * par2){
  while(1){ 
    if(a==1)
    {
      high(LED_PININT);
      pause(1000);
      low(LED_PININT);
    }   
  a=0; 
  pause(10);
  } 
} 

// correcting algo performed to correct the overshoot if any
void correct_right(){
  int ir_valueC = input(IR_PINC);
  int ir_valueL = input(IR_PINL);
  int ir_valueR = input(IR_PINR); 
  while(ir_valueC == 0 && ir_valueL == 0 && ir_valueR == 0){
    servo_speed(servoPinL, 0);
    servo_speed(servoPinR, -30);
    ir_valueC = input(IR_PINC);
    ir_valueL = input(IR_PINL);
    ir_valueR = input(IR_PINR);
  }    

}  
// correcting algo performed to correct the overshoot if any
void correct_left(){
  int ir_valueC = input(IR_PINC);
  int ir_valueL = input(IR_PINL);
  int ir_valueR = input(IR_PINR); 
  while(ir_valueC == 0 && ir_valueL == 0 && ir_valueR == 0){
    servo_speed(servoPinL, 30);
    servo_speed(servoPinR, 0);
    ir_valueC = input(IR_PINC);
    ir_valueL = input(IR_PINL);
    ir_valueR = input(IR_PINR);
  }    

} 
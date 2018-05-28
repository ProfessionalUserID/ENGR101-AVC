pi@raspberrypi:~/Desktop $ cat q8.cpp                                                                                                                                                                                                                                          
                                                                                                                                                                                                                                                                               
#include <stdio.h>                                                                                                                                                                                                                                                             
#include "E101.h"                                                                                                                                                                                                                                                              
#include <time.h>                                                                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                               
double whiteThreshold = 255;                                                                                                                                                                                                                                                   
int quad = 1;                                                                                                                                                                                                                                                                  
                                                                                                                                                                                                                                                                               
void gate1() {                                                                                                                                                                                                                                                                 
        char password[24];                                                                                                                                                                                                                                                     
        char IP_addr [15] = {'1','3','0','.','1','9','5','.','6','.','1','9','6'}; //Nathan's IP                                                                                                                                                                               
        char send [24] = {'P','l','e','a','s','e'};                                                                                                                                                                                                                            
        connect_to_server(IP_addr, 1024);                                                                                                                                                                                                                                      
        send_to_server(send);                                                                                                                                                                                                                                                  
        receive_from_server(password);                                                                                                                                                                                                                                         
        send_to_server(password);                                                                                                                                                                                                                                              
        sleep1(0,200000);                                                                                                                                                                                                                                                      
}                                                                                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                               
double updateWhiteThreshold() {                                                                                                                                                                                                                                                
      take_picture();                                                                                                                                                                                                                                                          
      int min = 255;                                                                                                                                                                                                                                                           
      int max = 0;                                                                                                                                                                                                                                                             
      for (int i = 0; i < 320; i++) {                                                                                                                                                                                                                                          
              int currentPixel = get_pixel(120, i, 3);                                                                                                                                                                                                                         
                                                                                                                                                                                                                                                                               
              if (currentPixel > max) {                                                                                                                                                                                                                                        
                      max = currentPixel;                                                                                                                                                                                                                                      
              }                                                                                                                                                                                                                                                                
              else if (currentPixel < min) {                                                                                                                                                                                                                                   
                      min = currentPixel;                                                                                                                                                                                                                                      
              }                                                                                                                                                                                                                                                                
      }                                                                                                                                                                                                                                                                        

      whiteThreshold = (max + min) / 2;
                                                                                                                                                                                                                                         
}                                                                                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                               
double kp = 0.005;  

double getPropSignal() {
        int white[320];
        int whitePixelCount = 0;

        take_picture();        
        for (int i = 0; i < 320; i++) {
              white[i] = 0;
                
              int pixel = get_pixel(120, i, 3);
 
              if (pixel > whiteThreshold) {
                      white[i] = 1;
                        whitePixelCount++;
              }
      }
      double error = 0;
        
      for (int i = 0; i < 320; i++) {
              error += (i-160)*white[i];
      }

        if (whiteThreshold > 90 && whitePixelCount > 220) {
                quad = 3;
        }
 
      return error * kp;
}

void stopMotors() {
        set_motor(1,0);
        set_motor(2,0);
        sleep1(1,0);
}

int speed = 47;

void turnLeft() {
        stopMotors();
        set_motor(1, -80);
        set_motor(2, 60);
        sleep1(0,400000);
}

void turnRight() {
        stopMotors();
        set_motor(1, 60);
        set_motor(1, -80);
        sleep1(0,400000);
} 

int baseSpeed = 44;

void goForward() {
               double propSignal =  getPropSignal();
               double leftSpeed = baseSpeed + propSignal;
               double rightSpeed = baseSpeed - propSignal;

                //printf("left=%f right=%f\n", leftSpeed, -rightSpeed);
                set_motor(2, -leftSpeed);
                set_motor(1, -rightSpeed);
}

void doQuadThree() {
        goForward();

        if (whiteThreshold < 40) {
                if (getPropSignal() < 20) {
                        printf("Left Turn \n");
                        turnLeft();
                }
                else {
                        printf("Turn Right \n");
                        turnRight();
                }
        } 
}

bool startQuad3 = true;

int main() {
        init();

        /*while (1) {
                printf("white thresh=%f\n", whiteThreshold);
                updateWhiteThreshold();
                //sleep1(1,0);
        }*/
        time_t start = time(NULL);
        gate1();
        quad++;
        //updateWhiteThreshold();

       while(1) {
        updateWhiteThreshold();

        if (quad == 2) {
                time_t end = time(NULL);

                if (whiteThreshold > 100) {
                //if (end - start >= 22) {
                        //quad++;
                        //printf("QUAD3");
                }
                goForward();
        }
        else if (quad == 3) {
                //printf("QUAD 3\n");
                if (startQuad3) {
                        printf("QUAD 3\n");
                        set_motor(1, -50);
                        set_motor(2, -55);
                        startQuad3 = false;
                        sleep1(2,300000);
                }
                doQuadThree();
        }
       }
}

          

/*
 * Author: Jerry Shao  
 * Student Number: 84982321
 * UBC Alumni Email: shaojerry@alumni.ubc.ca
 * Lab Section: L2K
 * Date:             4/9/2020 9:00 AM
 *           
 * Purpose: The Simon Game flashes LEDs in a random order to the user, who has to return by clicking the correct LEDs and in correct order back. Difficultly is changeable.
 *
 */
#define _CRT_SECURE_NO_WARNINGS

#include <DAQlib.h> //Includes any (potentially) useful header files for use
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>

#define MAX_ROUNDS 5 //As defined in Canvas, this is number of rounds (5 max) the user will have to  play to win


#define DAQMOD 6 //Some useful constants, mostly standard
#define MYUPPER 3
#define ON 1
#define OFF 0
#define TRUE 1
#define FALSE 0

//Funtion prototypes
void runSimon(int[], int[]); 
void generateSequence(int length, int data[]);
int EvalPlayerInput(int button);
void Blinker(int led, int times);


int main(void)
{
    int RandSequence[MAX_ROUNDS] = { 0 }; //Array to hold the randomized numbers corresponding to LEDs
    int ResultSeq[MAX_ROUNDS] = { 0 }; //Array to hold user keystrokes

    srand((unsigned)time(NULL)); //Preparing the random number function
    if (setupDAQ(DAQMOD) == TRUE) {
        runSimon(RandSequence, ResultSeq); //Standard DAQ setup steps
    }
    else {
        printf("Error: cannot initialise");
        exit(0);
    }

	return 0;
}



void runSimon(int RandSequence[], int ResultSeq[]) {
    int i = 0, //index
        diffCount = 0, //"difficulty rating" or LED sequence length 
        keyCount = 0, //keystroke counter
        win = 0; //win indicator

    int isNewGame = TRUE; //Preparing variables for use, they are given "true" as inital to reinforce the "start of the game"
    int warning = TRUE;
    generateSequence(MAX_ROUNDS, RandSequence); //Generates sequence of pseudo-random LED order

    while (continueSuperLoop() == TRUE) { 
        
       if (warning == TRUE) { //Gives user a warning before game starts formally
            printf("Ready in 3 seconds!\n");
            warning = FALSE;
            Sleep(3000);
        }

        if (isNewGame == TRUE) { //If the new game has started:
            diffCount++;

            for (i = 0; i < diffCount; i++) { //Flashes the LEDs in pseudo-random order
                digitalWrite(RandSequence[i], ON);
                Sleep(800);
                digitalWrite(RandSequence[i], OFF);
                Sleep(600);
            }

            isNewGame = FALSE; //This gives the user an opportunity to put his/her "guess" in
        }

        else {
            int i = 0;
                for (i=0;i<4; i++ ) {
                    if (EvalPlayerInput(i) == TRUE) { //Records the user actions
                        ResultSeq[keyCount++] = i;
                    }               
                }
        }
      
        if (keyCount == diffCount) {//Number of keystrokes matches the expected number
            i = 0;
            for (i = 0; i < diffCount; i++) {
                if (ResultSeq[i] != RandSequence[i]) { //Compares user input to "actual" answer
                    break; //If any element doesn't match, it "breaks" and undergoes comparison processes
                }
            }

            if (i == diffCount) { //Correctly matches all necessary numbers
                win = TRUE;
            }
            else { //Loses, must restart, flashes LEDs
                win = FALSE;
                isNewGame = TRUE;
                //blink red 3 times
                Blinker(1, 3);
                generateSequence(MAX_ROUNDS, RandSequence); //Generates new sequence for new game
                diffCount = keyCount = 0; // content not matched, start over 
            }

        }
        else { //Doesn't even need to compare; FALSE by default; forces user to enter correct number of presses
            win = FALSE;
        }

        if (win == TRUE)
        {
            int i=0;

            win = FALSE; //Resets "win"

            for (i = 0; i < MAX_ROUNDS; i++) {
                ResultSeq[i] = 0; //Clears keystroke array
            }



            if (keyCount == MAX_ROUNDS) { //This means the user has won the last round and therefore the game has been won
                //blink green 3 times
                Blinker(0, 3); 

                printf("YOU WIN!!!\n");
                generateSequence(MAX_ROUNDS, RandSequence); //Generates new sequence for new game
                diffCount = 0; //Resets values
                warning = TRUE;
            }
            isNewGame = TRUE; //Time to start a new game
            keyCount = 0;

            
        }

    }
}

void generateSequence(int length, int data[]) { //Generates pseudo-random numbers and put them in array
    int i = 0;
    for (i = 0; i < length; i++) { //Population of array
        data[i] = rand()%4;
    }
}



int EvalPlayerInput(int button) //Controlls button press mechanism
{
    if (digitalRead(button) == OFF) { //Has not been pressed
        return FALSE;
    }

    while (digitalRead(button) == ON) { //Has not released
    }

    return TRUE; //Pressed and released
}

void Blinker(int led, int times) { //Controls the LED flash sequence for win/loss
    int i = 0;
    for (i = 0; i < times; i++) {
        digitalWrite(led, ON);
        Sleep(1000);
        digitalWrite(led, OFF);
        Sleep(1000);
    }
    Sleep(1000);
}
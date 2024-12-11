
// IMPORTS SECTION
#include "System/system.h"  // System initialization and configuration
#include "oledDriver/oledC_colors.h"// Predefined OLED colors
#include "oledDriver/oledC_shapes.h"// Functions for drawing shapes
#include "System/delay.h"  // Functions for creating delays
#include <xc.h>  // Device-specific peripheral access

typedef  unsigned char BOOL;
#define FALSE 0
#define TRUE  !FALSE

static void oledC_clearScreen(void) 
{    
    uint8_t x; // Column index
    uint8_t y; // Row index
    oledC_setColumnAddressBounds(0,96); // Set column bounds for the screen
    oledC_setRowAddressBounds(0,96); // Set row bounds for the screen
    for(x = 0; x < 96; x++) // Iterate through all columns
    {
        for(y = 0; y < 96; y++)  // Iterate through all rows
        {
            oledC_sendColorInt(OLEDC_COLOR_BLACK); // Fill each pixel with black color
        }
    }
}

// NOTE: Function to check if a button is pressed with debounce handling
BOOL CheckButtonPressed(void)
{
    static BOOL buttonPressed = FALSE;
    static unsigned long buttonPressCounter = 0;

    // CHECKS IF THE RA11 BUTTON WAS PRESSED>
    if((PORTA & (1 << 11)) == 0) // NOTE Check if button (RA11) is pressed, 0 means pressed. NOTE: the 1 << 11 Shifts the binary 1 11 positions to the left, resulting in 0b100000000000. NOTE This performs a bitwise AND between PORTA and the mask 0b100000000000.
    {
        ++buttonPressCounter;
        if(buttonPressCounter > 400)   //NOTE DeBounce - If counter exceeds debounce threshold
            buttonPressed = TRUE;
    }
    else
    {
        if(buttonPressed == TRUE) // If button was previously pressed
        {
            buttonPressed = FALSE; // Reset the state
            return TRUE; // Return TRUE indicating a button press event
        }

        buttonPressCounter=0;
    }

    return FALSE;
}

// NOTE: Constants for delay and pixel scaling
#define	DELAY       100  // NOTE Delay in milliseconds
#define PixelPerCh  6  // NOTE: Number of pixels per character position

// NOTE: Function to animate dots moving from old positions to new positions
void MoveDots( int *oP, int *nP )
{
	int i ;

	for (i=3 ; i>=0 ; --i) // NOTE Iterate over the dots BUG: MADE A CORRECTION HERE.
		if (nP[i] != oP[i]) // NOTE Check if the position has changed.
     {
            oledC_DrawCharacter((int8_t)nP[i]*PixelPerCh, 30, 1, 2, '@', OLEDC_COLOR_WHITE); // NOTE Draw dot at the new position.
            oledC_DrawRectangle((int8_t)oP[i]*PixelPerCh, 30, (int8_t)oP[i]*PixelPerCh+5, 30+16, OLEDC_COLOR_BLACK); // NOTE: Erase dot from the old position
		}

	DELAY_milliseconds(DELAY); // Pause for a smooth animation.
}

// Function to display dots at given positions
void dispDots ( int *pos, char dot )
{
	int i ;

	for (i=0 ; i<4 ; ++i) // NOTE: Iterate over the dots
    {
        oledC_DrawCharacter((int8_t)pos[i]*PixelPerCh, 30, 1, 2, dot, OLEDC_COLOR_WHITE); // NOTE: Draw each dot at its position
    }
}

// Function to copy new positions to old positions
void copyNewPos( int *oP, int *nP)
{
	int i ;
	for (i=0 ; i<4 ; ++i)  // NOTE Iterate over the dots

		*oP++ = *nP++ ;// NOTE Copy each new position to the old position array
}

// constants for movement speed and dot spacing
#define SPEED  3 // NOTE: each time we move forward a dot it will be moved with 3 pixels ahead.
#define RB     6 // NOTE: Minimum spacing between dots

int main(void)
{
    // initialize the device
    SYSTEM_Initialize(); // NOTE: Initialize the system (pins, peripherals, etc.)
    TRISA |= (1<<11);   // NOTE: Configure RA11 as input for the button


  int old_pos[] = { 0, 1, 2, 3 }; // Initial positions of the dots
  int new_pos[4]; // Array to store new positions of the dots
  int end_loc = 15; // Maximum position for the dots


    int i, minPos, maxPos; // Variables for position calculations
    
    DELAY_milliseconds(1000); // Initial delay before starting

    while(1)		
    {
      oledC_clearScreen(); // NOTE Clear the OLED screen
		dispDots( old_pos, '@' ) ; // NOTE Display the initial positions of the dots

		while( !CheckButtonPressed() ); // NOTE: Wait for Push-button to be pressed and released

  // NOTE: Stretch phase - Move dots to the right while maintaining spacing
		while (old_pos[3] < end_loc)
    {				
			new_pos[3] = old_pos[3] + SPEED; //  NOTE: Move the last dot forward
			
			for (i=2 ; i>0 ; --i)   // NOTE: Adjust the positions of the remaining dots
      {
				minPos = new_pos[i+1]-RB ; // NOTE Calculate the minimum position for the dot
				new_pos[i] = (old_pos[i] < minPos) ? minPos : old_pos[i] ; // Update position
			}
		
			MoveDots( old_pos, new_pos ) ; // NOTE: Animate the movement of the dots

			copyNewPos( old_pos, new_pos ) ; //NOTE: Update old positions with the new ones
		}

   // NOTE: Contract phase: Move dots closer together
		while (new_pos[0] < end_loc-3)  
		{
			for (i=2 ; i>=0 ; --i)  // NOTE: Adjust the positions of the dots
      {
				maxPos = new_pos[i+1] ; // Calculate the maximum position for the dot
				new_pos[i] = old_pos[i] + SPEED ; // Move the dot forward
				if (new_pos[i] > maxPos) // NOTE Ensure the dot does not exceed the maximum position
					new_pos[i] = maxPos ;
			}
			MoveDots( old_pos, new_pos ) ; // NOTE Animate the movement of the dots
			copyNewPos( old_pos, new_pos ) ; // NOTE: Update old positions with the new ones
		}


		while( !CheckButtonPressed() ) //NOTE Wait for Push-button to be pressed and released
			;

		dispDots( new_pos, ' ' ) ; // NOTE: Clear the final positions
		for (i=0 ; i<4 ; ++i ) 
			old_pos[i] = i ; //NOTE Reset the dots to their initial positions
		dispDots( old_pos, '@' ) ;// NOTE Display the reset dots
    }

    return 1; // NOTE: Program never reaches this point due to the infinite loop
}


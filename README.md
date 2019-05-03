# SD-Bioreactor

User Manual
#Set Up Code/ Wiring
GND -> PUL-, DIR-
8 -> PUL+
9 -> DIR+

User defined variables:
TOTAL_DATA             // Personalized for each file
GAIN 		// Adjust the gain to amplifier the force being applied to the scaffold
T      	              // T = 1*E6ms / Frequency
HVR 		// The ratio of the force applied to horizontal and vertical


#Run Control
Unplug the wire on reset
Upload code to the board
Open ImportData file via processing IDE
Plug the wire to avoid reset
Open the Serial Monitor and enter "t" to tare the strain value measurement
Open PLX-DAQ and choose the right port, click connect.
Use excel to plot a live strain value/ steps of the motor


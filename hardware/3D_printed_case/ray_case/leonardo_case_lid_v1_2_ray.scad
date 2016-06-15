//  ----------------------------------------------------------------------- LICENSE
//  This "3D Printed Case for Arduino Uno, Leonardo" by Zygmunt Wojcik is licensed
//  under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
//  To view a copy of this license, visit
//  http://creativecommons.org/licenses/by-sa/3.0/
//  or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.


include <leonardo_case_param_v1_2.scad>

//------------------------------------------------------------------------- PARAMETERS
// lid holes dimensions
connectorRoundR = 2.54/2;
connectorWide = 2.54*3;

upperConnectorWidth = 44.704 + 2.54*2;
upperConnectorXPos = 18.796 - 2.54;
upperConnectorYPos = 50.8 - 2.54*2;
lowerConnectorWidth = 35.56 + 2.54*2;
lowerConnectorXPos = 27.94 - 2.54;
lowerConnectorYPos = 2.54 - 2.54;

icspConnectorWidth = 5.08 + 2.54*2;
icspConnectorWide = 2.54 + 2.54*2;
icspConnectorXPos = 63.64 - 2.54;
icspConnectorYPos = 30.43 + 2.54;

ledHoleWidth = 2.54 + 1.27*2;
ledHoleWide = 11.5 + 1.27*2;
ledHoleXPos = 4.45;
ledHoleYPos = 22 ;

buttonSize = 4;
buttonXPos = 6.15;
buttonYPos = 50;
buttonBaseHeight = 2;
buttonBaseR = buttonSize/2 + 0.25 + layerWidth*3;


//------------------------------------------------------------------------- MODULES
module pcbLeg() {		
	translate([0, 0, 0])
	difference() { 											
		cylinder(h = height - floorHeight - pcbPositionZ - pcbHeight, r = 5.7/2);
	}
}

module buttonFrame() {
	translate([0, 0, -0.05])
	cylinder(h=1.01, r1=buttonSize/2 + 0.5 + 1, r2=buttonSize/2 + 0.5);
}


//------------------------------------------------------------------------- MAIN BLOCK
difference()
{
																		// ADD
	union()
	{
		// Add Base
		linear_extrude(height = height/2 + blockLockSize, convexity = 10)
		minkowski()
		{									
			square([width, wide], center = true);
			circle(roundR);
		}
	}
																		// SUBSTRACT
	union()
	{
		// lift floor height
		translate([0, 0, floorHeight])
		{
			// Cut base inner hole
			linear_extrude(height = height, convexity = 10)
			minkowski()
			{
				square([width, wide], center = true);
				circle(roundR - pillarSize);
			}
			// Cut block lock
			translate([0, 0, height/2 - blockLockSize])
			linear_extrude(height = height, convexity = 10)
			minkowski()
			{
				square([width, wide], center = true);
				circle(roundR - layerWidth*3);
			}
			// Cut x panels 
			for (i = [0 : 180 : 180])				
			rotate([0, 0, i])
			translate([width/2 + roundR - pillarSize/2 - layerWidth*7, 0, 0])
			{
                // Cut X panel hole
				translate([0, 0, height/2])
				cube([pillarSize, sidePanelXWidth, height], center=true);

				// Cut X, Y srew holes
				for (i = [wide/2, -wide/2])
				{
					translate([-(roundR - pillarSize/2 - layerWidth*7), i, 0])
					if (i>0) 
					{
						rotate([0, 0, 45])
						translate([screwHoleRoundR, 0, 0])
						cylinder(h=height/2, r=verConnectionHoleR);
					}
					else
					{
						rotate([0, 0, -45])
						translate([screwHoleRoundR, 0, 0])
						cylinder(h=height/2, r=verConnectionHoleR);
					}
				}
			}
            // Cut Y panels 
			for (i = [90 : 180 : 270])
			rotate([0, 0, i])
			translate([wide/2 + roundR - pillarSize/2 - layerWidth*7, 0, 0])
			{
				// Cut Y panel hole
				translate([0, 0, height/2])
				cube([pillarSize, sidePanelYWidth, height], center=true);
			}
            
            // Cut USB and Power holes
			// Rotate due to panel upside down
			mirror([0, 1 , 0])
            
			// translate to pcb position
			translate([-pcbPositionX, -pcbWide/2, height - floorHeight*2 -pcbPositionZ-pcbHeight])
			{
				// Cut power hole
				translate([0, powerJackPosition, -(powerJackHeight-2)/2])
				cube([10, powerJackWide, powerJackHeight], center=true);
				// Cut usb hole
				translate([0, usbHolePosition, -1.3])
				cube([10, usbWide, usbHeight], center=true);
                
			}
		}
	}
}



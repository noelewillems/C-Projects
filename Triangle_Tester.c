/*
 Created by Noel Willems for COS326 Lab 3: Triangle
 Sources:
 sscanf understanding: https://www.geeksforgeeks.org/converting-strings-numbers-cc/
*/
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	// If user doesn't input 3 sides
	if(argc != 4) { // For some reason, 3 args = 4 argc
		printf("Um... triangles have 3 sides, lol... bye\n");
		exit(0);
	}
	double side1, side2, side3;

	char* input1 = argv[1];
	char* input2 = argv[2];
	char* input3 = argv[3];

	// sscanf to convert to double
	sscanf(input1, "%lf", &side1);
	sscanf(input2, "%lf", &side2);
	sscanf(input3, "%lf", &side3);

	// Testing for negative values
	if(side1 < 0 || side2 < 0 || side3 < 0) {
		printf("Invalid triangle - negative value!\n");
		exit(0);
	}

	// Testing if valid triangle (if sum of shortest 2 sides < longest side, invalid)
	if(side1 > side2) {
		if(side1 > side3) {
			// side1 is longest
			if((side2 + side3) < side1) {
				printf("Invalid triangle!\n");
				exit(0);
			}
		}
	} else if(side2 > side3) {
		// side2 is longest
		if((side1 + side3) < side2) {
			printf("Invalid triangle!\n");
			exit(0);
		}
	} else {
		// side3 is longest
		if((side1 + side2) < side3) {
			printf("Invalid triangle!\n");
			exit(0);
		}
	}

	// Testing for equilateral (3 equal sides)
	if((side1 == side2) && (side2 == side3) && (side3 == side1)) {
		printf("Equilateral triangle.\n");
		exit(0);
	}

	// Testing for isosceles (2 equal sides)
	if((side1 == side2) || (side1 == side3) || (side2 == side3)) {
		printf("Isosceles triangle.\n");
		exit(0);
	}

	// If all else fails... scalene
	printf("Scalene triangle.\n");
	return(0);
}

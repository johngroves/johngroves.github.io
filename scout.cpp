

float getheading(float lat1, float lon1, float lat2, float lon2){ // Calculates heading from location to waypoint

  float heading;  
  lon1 = radians(lon1);  
  lon2 = radians(lon2);  
  
  heading = atan2(sin(lon2-lon1)*cos(lat2),cos(lat1)*sin(lat2)-sin(lat1)*cos(lat2)*cos(lon2-lon1)),2*3.1415926535;
  heading = heading*180/3.1415926535;  // convert from radians to degrees
  
  int head =heading;
  
    if(head<0){
      heading+=360;   //if the heading is negative then add 360 to make it positive
    }
    
    return heading;

}

float degsBetween(float magneticBearing, float magneticHeading){ // Calculates minimum angle between two headings

	float alpha = magneticBearing - magneticHeading;
	float beta = magneticBearing - magneticHeading + 360;
	float gamma = magneticBearing - magneticHeading - 360;

	float between;

	if (abs(alpha) < abs(beta)){

		if(abs(alpha) <= abs(gamma)){

			between = alpha;

		}
	}
	else{

		if (abs(beta) <= abs(gamma)){

			between = beta;

		}

		else{

			between = gamma;

		}
	}

	return between;
	
}

void turn(float motorHeading, float desiredDelta){
	float rconst = 0.5;
	
	float desiredAngle = motorHeading + desiredDelta * rconst;

	float oDiff = abs(degsBetween(motorHeading,desiredAngle));
	
	float nDiff = oDiff - 1;
	
	bool clockWise = desiredDelta > 0;

	while(oDiff > nDiff){

		oDiff = nDiff;

		step(clockWise);

		nDiff = abs(degsBetween(getMotorHeading(),desiredAngle));

	}
}

void step(bool clockWise){

int turnDuration = 1000;
	
	if(clockWise){

	}

	else{

	}

	delay(turnDuration);
}

void navigate(){
	
	float magneticHeading = getMagneticHeading();
	
	float magneticBearing = getMagneticBearing();
	
	float delta = degsBetween(magneticHeading,magneticBearing;
	
	bool closeEnough = abs(delta) <= minimumAccuracy;

	if(!closeEnough){

	float motorHeading = getMotorHeading();

		turn(motorHeading, degsBetween(motorHeading,delta));
		
		turn(motorHeading, degsBetween(motorHeading,-delta));

	}
}
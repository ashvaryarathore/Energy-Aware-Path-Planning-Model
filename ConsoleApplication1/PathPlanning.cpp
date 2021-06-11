// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
//   Run program: Ctrl + F5 or Debug > Start Without Debugging menu
//   Debug program: F5 or Debug > Start Debugging menu

//This is for Windows only as of now.

// Overlap_Ratio_Calculation.cpp : 
//file contains the 'main' function. 
//Program execution begins and ends there.

//header files
#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <stdio.h>
#include <fstream> 
#include <xlsxwriter.h>
#include <math.h>

using namespace std;
//new = 0.0065
//new = 0.016725
//old = 
//global params
long double objectDistance = 0.0082259;
long double bearing = 0;
int objectType = 2;
const long double PI = 3.141592653589793238463;

//anticlockwise / anticlock straight = 0;
//clockwise, 1
int direction = 1;
//1: straight path
//2: circular path

double imageDistance = 0.00981;

vector<long double> oldValues(8, 0);
/*values in this vector:
vector<long double> oldValues(7, 0);
setValues = 0 : default
lastFOV
lat1
long1
old bearing
average OR
average BR
count
*/

/***********************************************************************
Function: calculateInitialBearingDistance()
Function will calculate and store the bearing with object distance using
the initial locations provided by the user.
Later, the calculated bearing and obj distance will be used to find new
object cordinates. (Assuming the user will fly UAV in a straight path)
Will not work otherwise. :')
For circular paths (ideal scenario), the center will be treated as obj
location for all cases. (Hope user fly's the UAV in circular path.)
************************************************************************/
void calculateBearingDistance(long double objectDistance, long double &bearing, vector<long double> cameraPos1, vector<long double> cameraPos2)
{
	long double lat1, lat2, long1, long2;
	//const long double PI = 3.141592653589793238463;

	//convert the lat, longs to radians
	lat1 = cameraPos1[0] * (PI / 180);
	long1 = cameraPos1[1] * (PI / 180);
	lat2 = cameraPos2[0] * (PI / 180);
	long2 = cameraPos2[1] * (PI / 180);

	//distance is calculated using the Haversine formula
	//Haversine formula : a = sin²(Δφ / 2) + cos φ1 ⋅ cos φ2 ⋅ sin²(Δλ / 2)
	//  c = 2 ⋅ atan2(√a, √(1−a))
	//  d = R ⋅ c
	int radius = 6371; // Km
	long double a = sin((lat2 - lat1) / 2) * sin((lat2 - lat1) / 2) + cos(lat1) * cos(lat2) * sin((long2 - long1) / 2) * sin((long2 - long1) / 2);
	long double c = 2 * atan2(sqrt(a), sqrt(1 - a));
	//cout << "\n calculated distance = " << radius * c;
	objectDistance = radius * c;
	cout << " objectDistance = " << objectDistance;

	//bearing calculation
	long double y = sin(long2 - long1) * cos(lat2);
	long double x = cos(lat1) * sin(lat2) - (sin(lat1) * cos(lat2) * cos(long2 - long1));
	bearing = atan2(y, x);

	bearing = bearing * (180 / PI);
	bearing = fmod(bearing + 360, 360);
	cout << "\n bearing = " << bearing;
	//changing it back to radians
	bearing = bearing * (PI / 180);
	//cout << "\n ---------------------- \n changed bearing = " << bearing;
}



/*******************************************************************
Function: calculateNextCameraCordinates(vector<long double> &objectCordinates)
Will evaluate the nxt camera object cordinate given bearing and
********************************************************************/
void calculateNextCameraCordinates(vector<long double> &cameraPos1, vector<long double> &cameraPos2, long double objectDistance, long double &bearing)
{
	//using the camera cords, distance and bearing find the obj cords.
	long double lat1, lat2, long1, long2;
	const long double PI = 3.141592653589793238463;
	//cout <<std::fixed<< " \n after called : lat1: " << cameraCordinates[0] << "  long 1 : " << cameraCordinates[1];

	//convert the lat, longs to radians
	lat1 = cameraPos1[0] * (PI / 180);
	long1 = cameraPos1[1] * (PI / 180);
	int radius = 6371; // Km
	cout << "\n objectDistance = " << objectDistance;
	//cout << " \n bearing = " << bearing;
	//cout << " \n lat1: " << cameraCordinates[0] << " long1: " << cameraCordinates[1];

	//objectDistance = 0.0007;
	
	//imageDistance += 0.0005101;

	bearing = bearing + (imageDistance / objectDistance );
	bearing = bearing * (180 / PI);
	bearing = fmod(bearing + 360, 360);
	cout << "\n bearing = " << bearing;
	//changing it back to radians
	bearing = bearing * (PI / 180);
	
	lat2 = asin(sin(lat1) * cos(objectDistance / radius) + cos(lat1) * sin(objectDistance / radius) * cos(bearing));
	long2 = long1 + atan2(sin(bearing) * sin(objectDistance / radius) * cos(lat1), cos(objectDistance / radius) - sin(lat1) * sin(lat2));

	lat2 = lat2 * (180 / PI);
	long2 = long2 * (180 / PI);
	cameraPos2[0] = (lat2);
	cameraPos2[1] = (long2);
	cout << std::fixed << "\n ******************************* lat2: " << lat2 << " |  long 2 :" << long2 << "\n *********************************************";
	//cout<<std::fixed<< "\n lat2: " << lat2 * (PI / 180) << " |  long 2 :" << long2 * (PI / 180);

}




/************************************************************************
Function: writeToXls()
This function will write the passed parameters to xls file.
Making a generic funtion to reuse for different parameters.
*************************************************************************/
void writeToXls(lxw_worksheet* worksheet, vector<long double>& objectCordinates, vector<long double>& cameraCordinates, int& row, std::string imageName, long double objectDistance, long double bearing)
{
	const char* cstr = imageName.c_str();

	int column = 0;
	const long double PI = 3.141592653589793238463;

	worksheet_write_number(worksheet, row, column++, objectCordinates[0], NULL);
	worksheet_write_number(worksheet, row, column++, objectCordinates[1], NULL);
	worksheet_write_number(worksheet, row, column++, cameraCordinates[0], NULL);
	worksheet_write_number(worksheet, row, column++, cameraCordinates[1], NULL);

	//change cordinates to radian
	long double cam_lat, cam_long, obj_lat, obj_long, long1, long2, lat1, lat2;
	int radius = 6371;

	//convert coridnates to radian 
	cam_lat = cameraCordinates[0] * (PI / 180);
	cam_long = cameraCordinates[1] * (PI / 180);
	obj_lat = objectCordinates[0] * (PI / 180);
	obj_long = objectCordinates[1] * (PI / 180);


	
	//change bearing back
	bearing = bearing * (180 / PI);
	//bearing = fmod(bearing + 360, 360);
	long double bearing45Plus = (bearing + 222) < 360 ? (bearing + 222) : ((bearing + 222) - 360);
	long double bearing45Minus = (bearing - 222) > 0 ? (bearing - 222) : (360 + (bearing - 222));
	//bearing45Plus = fmod(bearing45Plus + 360, 360);
	//bearing45Minus = fmod(bearing45Minus + 360, 360);
	
	cout << "\n checking if bearing is radian or degree:   " << bearing;
	cout << "\n bearing45Plus: " << bearing45Plus;
	cout << "\n bearing45Minus: " << bearing45Minus;
	cout << " \***********************************";

	bearing45Plus = bearing45Plus * (PI / 180);
	bearing45Minus = bearing45Minus * (PI / 180);

	
	//calculate cosFOV and the side length of the FOV triangle = side/cos. 
	long double cosFOV = cos(42 * (PI / 180));
	long double sideFOVTriangle = objectDistance / cosFOV;

	//calculating the end point cordinates for the given camera location. 
	//we will usw both the bearings (45 + and 45- to find cordinates 1 and 2 )
	//Formula:	φ2 = asin( sin φ1 ⋅ cos δ + cos φ1 ⋅ sin δ ⋅ cos θ )
	//λ2 = λ1 + atan2(sin θ ⋅ sin δ ⋅ cos φ1, cos δ − sin φ1 ⋅ sin φ2)
	//where	φ is latitude, λ is longitude, θ is the bearing(clockwise from north), δ is the angular distance d / R; d being the distance travelled, R the earth’s radius
	//bearing 45+  
	lat1 = asin(sin(cam_lat) * cos(objectDistance / radius) + cos(cam_lat) * sin(objectDistance / radius) * cos(bearing45Plus));
	long1 = cam_long + atan2(sin(bearing45Plus) * sin(objectDistance / radius) * cos(cam_lat), cos(objectDistance / radius) - sin(cam_lat) * sin(lat1));
	//bearing 45-  
	lat2 = asin(sin(cam_lat) * cos(objectDistance / radius) + cos(cam_lat) * sin(objectDistance / radius) * cos(bearing45Minus));
	long2 = cam_long + atan2(sin(bearing45Minus) * sin(objectDistance / radius) * cos(cam_lat), cos(objectDistance / radius) - sin(cam_lat) * sin(lat2));


	//calculating FOV distance for the points 
	//Haversine formula : a = sin²(Δφ / 2) + cos φ1 ⋅ cos φ2 ⋅ sin²(Δλ / 2)
	//  c = 2 ⋅ atan2(√a, √(1−a))
	//  d = R ⋅ c
	long double a = sin((lat1 - lat2) / 2) * sin((lat1 - lat2) / 2) + cos(lat2) * cos(lat1) * sin((long1 - long2) / 2) * sin((long1 - long2) / 2);
	long double c = 2 * atan2(sqrt(a), sqrt(1 - a));
	long double FOVDistance = radius * c;


	long double lat1Deg = lat1 * (180 / PI);
	long double long1Deg = long1 * (180 / PI);
	long double lat2Deg = lat2 * (180 / PI);
	long double long2Deg = long2 * (180 / PI);

	worksheet_write_number(worksheet, row, column++, cam_lat, NULL);
	worksheet_write_number(worksheet, row, column++, cam_long, NULL);
	worksheet_write_number(worksheet, row, column++, objectDistance, NULL);
	worksheet_write_number(worksheet, row, column++, bearing, NULL);
	worksheet_write_number(worksheet, row, column++, bearing45Plus, NULL);
	worksheet_write_number(worksheet, row, column++, bearing45Minus, NULL);
	worksheet_write_number(worksheet, row, column++, cosFOV, NULL);
	worksheet_write_number(worksheet, row, column++, sideFOVTriangle, NULL);
	worksheet_write_number(worksheet, row, column++, radius, NULL);
	worksheet_write_number(worksheet, row, column++, lat1Deg, NULL);
	worksheet_write_number(worksheet, row, column++, long1Deg, NULL);
	worksheet_write_number(worksheet, row, column++, lat2Deg, NULL);
	worksheet_write_number(worksheet, row, column++, long2Deg, NULL);
	worksheet_write_number(worksheet, row, column++, FOVDistance, NULL);

	/*values in this vector:
	vector<long double> oldValues(7, 0);
	[0] setValues = 0 : default
	[1] lastFOV
	[2] lat1
	[3] long1
	[4] old bearing
	[5] average OR
	[6] average BR
	[7] count
	*/

	if (oldValues[0] == 0)
	{
		oldValues[1] = FOVDistance;

		if (direction == 0)
		{
			oldValues[2] = lat1;
			oldValues[3] = long1;
		}
		else
		{
			oldValues[2] = lat2;
			oldValues[3] = long2;
		}

		oldValues[4] = bearing;
		oldValues[0] = 1;
	}

	if ((row % 2) == 0)
	{
		long double FOVaverage = (FOVDistance + oldValues[1]) / 2;
		worksheet_write_number(worksheet, row, column++, FOVaverage, NULL);
		long double bearingDifference = abs(bearing - oldValues[4]);
		//cout << " \n\n bearing now = "<< bearing <<",  old = "<< oldValues[4]  <<"difference = " << bearing - oldValues[4];
		//check to see if the bearing difference is greater than 300
		if (bearingDifference >= 300)
		{
			//we need to modify the difference
			bearingDifference = 360 - bearingDifference;
		}
		else if (bearingDifference < 0.01)
		{
			//cout << "true";
			bearingDifference = 0;
		}
		worksheet_write_number(worksheet, row, column++, bearingDifference, NULL);

		//overalp ratio calcuation. 
		//calculating FOV distance for the points 
		//Haversine formula : a = sin²(Δφ / 2) + cos φ1 ⋅ cos φ2 ⋅ sin²(Δλ / 2)
		//  c = 2 ⋅ atan2(√a, √(1−a))
		//  d = R ⋅ c

		if (direction == 1)
		{
			long double lat2Var = lat2;
			long double long2Var = long2;
			lat2 = lat1;
			long2 = long1;
			lat1 = lat2Var;
			long1 = long2Var;
		}

		a = sin((oldValues[2] - lat2) / 2) * sin((oldValues[2] - lat2) / 2) + cos(lat2) * cos(oldValues[2]) * sin((oldValues[3] - long2) / 2) * sin((oldValues[3] - long2) / 2);
		c = 2 * atan2(sqrt(a), sqrt(1 - a));
		long double ORDistance = radius * c;
		//cout << "\n distance or: " << ORDistance;
		//cout << " \n calculatin OR between points: lat1: "<<lat2 * (180/PI)<<" LONG 1 = "<< long2 * (180 / PI);
		//cout << " \n calculatin OR between points: lat2: " << oldValues[2] * (180 / PI) << " LONG 2 = " << oldValues[3] * (180 / PI);
		worksheet_write_number(worksheet, row, column++, ORDistance, NULL);
		long double ORRatio = ORDistance / FOVaverage;
		worksheet_write_number(worksheet, row, column++, ORRatio, NULL);
		oldValues[5] += ORRatio;
		oldValues[6] += bearingDifference;
		oldValues[7]++;
		oldValues[0] = 0;
	}
	else
	{
		worksheet_write_number(worksheet, row, column++, 0, NULL);
		worksheet_write_number(worksheet, row, column++, 0, NULL);
		worksheet_write_number(worksheet, row, column++, 0, NULL);
		worksheet_write_number(worksheet, row, column++, 0, NULL);

	}

	worksheet_write_string(worksheet, row, column++, cstr, NULL);


	row = row + 1;
}











/****************************************************************************
Function: addColumNames()
Will add the required column names for calculation based on the case
****************************************************************************/
void addColumnName(lxw_worksheet* worksheet, int row, int column)
{
	//populate the column names.
	worksheet_write_string(worksheet, row, column++, "Object_Lat", NULL);
	worksheet_write_string(worksheet, row, column++, "Object_Long", NULL);
	worksheet_write_string(worksheet, row, column++, "Camera_Lat_degrees", NULL);
	worksheet_write_string(worksheet, row, column++, "Camera_Long_degrees", NULL);
	worksheet_write_string(worksheet, row, column++, "Camera_Lat_radians", NULL);
	worksheet_write_string(worksheet, row, column++, "Camera_Long_radians", NULL);
	worksheet_write_string(worksheet, row, column++, "Distance_Obj_Camera", NULL);
	worksheet_write_string(worksheet, row, column++, "Bearing_C_O_Degrees", NULL);
	worksheet_write_string(worksheet, row, column++, "Bearing_C_O_Radian_45_Plus", NULL);
	worksheet_write_string(worksheet, row, column++, "Bearing_C_O_Radian_45_Minus", NULL);
	worksheet_write_string(worksheet, row, column++, "COS(42)", NULL);
	worksheet_write_string(worksheet, row, column++, "Fanning_Distance_Side", NULL);
	worksheet_write_string(worksheet, row, column++, "Radius_Of_Earth(m)", NULL);
	worksheet_write_string(worksheet, row, column++, "Lat1_Rad", NULL);
	worksheet_write_string(worksheet, row, column++, "Long1_Rad", NULL);
	worksheet_write_string(worksheet, row, column++, "Lat2_Rad", NULL);
	worksheet_write_string(worksheet, row, column++, "Long2_Rad", NULL);
	worksheet_write_string(worksheet, row, column++, "Fanning_Distance", NULL);
	worksheet_write_string(worksheet, row, column++, "FD_Average", NULL);
	worksheet_write_string(worksheet, row, column++, "Bearing Difference", NULL);
	worksheet_write_string(worksheet, row, column++, "Overlap_Distance", NULL);
	worksheet_write_string(worksheet, row, column++, "Overlap_Ratio", NULL);
	worksheet_write_string(worksheet, row, column++, "ImageName", NULL);
}


int main()
{
	//it worked. 
	//no compilation issues so far. :P 
	std::cout << "Hello World!\n";

	
	//create a wokbook for the specific folder
	int column = 0;
	int row = 0;
	lxw_workbook* workbook = workbook_new("D:\\Aish\\Sample\\Overlap_Calculation.xlsx");
	lxw_worksheet* worksheet = workbook_add_worksheet(workbook, "Overlap_Calculation");

	addColumnName(worksheet, row, column);
	row++;

	vector<long double> cameraPos1;
	vector<long double> cameraPos2;
	vector<long double> previousCameraCordinates;
	vector<long double>	cameraCordinates;
	vector<long double>	objectCordinates;
	string imageName;
	string previousImage;



	/*
	Bearcat Statue:

		cameraPos1.push_back(39.1305959);
		cameraPos1.push_back(-84.5127526);
		cameraPos2.push_back(39.13064419);
		cameraPos2.push_back(-84.51280683);
	*/

	//calculate bearing and ditance using the first two points user provides.
	cameraPos1.push_back(39.130962);
	cameraPos1.push_back(-84.51332619);
	cameraPos2.push_back(39.1309487);
	cameraPos2.push_back(-84.51346238);
	calculateBearingDistance(objectDistance, bearing, cameraPos1, cameraPos2);

	objectCordinates = cameraPos1;

	int numberOfPictures = (2 * PI * objectDistance)/ imageDistance;
	numberOfPictures += 2;
	calculateNextCameraCordinates(cameraPos1, cameraPos2, objectDistance, bearing);

	//we use for each loop to iterate all the sub files
	for (int i = 1; i < numberOfPictures; i++)
	{	
		/*worksheet_write_number(worksheet, row, column++, cameraPos2[0], NULL);
		worksheet_write_number(worksheet, row, column++, cameraPos2[1], NULL);
		row++;
		column = 0;*/

		cameraCordinates = cameraPos2;

		imageName = to_string(i) + '_' + to_string(cameraCordinates[0]) + '_' + to_string(cameraCordinates[1]);
		
		if (i == 1)
		{
			writeToXls(worksheet, objectCordinates, cameraCordinates, row, imageName, objectDistance, bearing);
		}
		
		if ((row % 2) == 0  && (i!=1))
		{
			previousCameraCordinates = cameraCordinates;
			previousImage = imageName;
			writeToXls(worksheet, objectCordinates, cameraCordinates, row, imageName, objectDistance, bearing);
		}
		
		if (((row % 2) != 0) && (previousCameraCordinates.size() > 0) && (i != 1))
		{
			writeToXls(worksheet, objectCordinates, previousCameraCordinates, row, previousImage, objectDistance, bearing);
		}

		calculateNextCameraCordinates(cameraPos1, cameraPos2, objectDistance, bearing);

		
	}


	worksheet_write_string(worksheet, row, column++, "Avg_OR", NULL);
	long double ORAverage = oldValues[5] / oldValues[7];
	worksheet_write_number(worksheet, row, column++, ORAverage, NULL);
	worksheet_write_string(worksheet, row, column++, "Bearing_Difference_Average", NULL);
	long double BearingDifferenceAverage = oldValues[6] / oldValues[7];
	worksheet_write_number(worksheet, row, column++, BearingDifferenceAverage, NULL);
	worksheet_write_string(worksheet, row, column++, "NumberOfPictures", NULL);
	worksheet_write_number(worksheet, row, column++, numberOfPictures, NULL);
	long double SpatialIndex = 4.138112452;
	worksheet_write_string(worksheet, row, column++, "SpatialIndex", NULL);
	worksheet_write_number(worksheet, row, column++, SpatialIndex, NULL);
	worksheet_write_string(worksheet, row, column++, "DistanceFromObject", NULL);
	worksheet_write_number(worksheet, row, column++, objectDistance, NULL);

	cout << "\n \n \n ************* \n Avg_OR = " << ORAverage;
	cout << "\n \n \n ************* \n BearingDifferenceAverage = " << BearingDifferenceAverage;
	cout << "\n \n \n ************* \n numberOfPictures = " << numberOfPictures;
	cout << "\n \n \n ************* \n SpatialIndex = " << SpatialIndex;
	cout << "\n \n \n ************* \n DistanceFromObject = " << objectDistance;

	//calcualte Accuracy: 
	/*
	Accuracy = ABS (0.0158 + 7.7616 * EXP (-6.919 * X1) - 19.628* EXP ( -0.9004*X2) - 0.0256 * EXP (-0.0139 * X2) - 9.55213E-08 * POWER (X3, 2.44377) + (SQRT(1/X5) * (X4/(X5/2))))

	Overlap Ratio = X1,
	Number of Pictures = X2,
	Sensing Direction Difference = X3
	Distance(Spherical) = X4
	Spatial Index = X5
	*/

	long double Accuracy = abs(0.0158 + 7.7616 * exp(-6.919 * ORAverage) - 19.628 * exp(-0.9004 * numberOfPictures) - 0.0256 * exp(-0.0139 * numberOfPictures) - 9.55213E-08 * pow(BearingDifferenceAverage, 2.44377) + (sqrt(1 / SpatialIndex) * (objectDistance / (SpatialIndex / 2))));
	
	worksheet_write_string(worksheet, row, column++, "Accuracy", NULL);
	worksheet_write_number(worksheet, row, column++, Accuracy, NULL);
	cout << "\n \n \n ************* \n Accuracy = " << Accuracy;
	
	workbook_close(workbook);
	getchar();
}



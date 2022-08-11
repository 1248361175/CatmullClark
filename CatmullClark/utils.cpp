#include"vector_mine.h"
#include<math.h>
#include<iostream>
using namespace std;

Cartesian3::Cartesian3()
{
};

Cartesian3::Cartesian3(float a, float b, float c)
{
	x = a;
	y = b;
	z = c;
};

void Cartesian3::operator+=(Cartesian3 a)
{
	x = x + a.x;
	y = y + a.y;
	z = z + a.z;
}

Cartesian3 Cartesian3::operator+(Cartesian3 a)
{
	Cartesian3 sum;
	sum.x = x + a.x;
	sum.y = y + a.y;
	sum.z = z + a.z;
	return sum;
};

Cartesian3 Cartesian3::operator-(Cartesian3 a)
{
	Cartesian3 sum;
	sum.x = x - a.x;
	sum.y = y - a.y;
	sum.z = z - a.z;
	return sum;
};

Cartesian3 Cartesian3::operator*(float a)
{
	Cartesian3 sum;
	sum.x = a * x;
	sum.y = a * y;
	sum.z = a * z;
	return sum;
};

Cartesian3 Cartesian3::cross(Cartesian3& other) {
	Cartesian3 returnVec(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
	return returnVec;
}


face::face()
{

};

face::face(std::vector<int> c)
{
	points = c;
};
















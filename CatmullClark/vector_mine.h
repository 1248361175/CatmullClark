#include <GL/glut.h>
#include <vector>
#include <map>

class edge;

//class Cartesian3 is for vertex 3D coordinates and normals
class Cartesian3
{
public:
	float x, y, z;
	Cartesian3();
	Cartesian3(float, float, float);

	void operator+=(Cartesian3);
	Cartesian3 operator+(Cartesian3);
	Cartesian3 operator-(Cartesian3);
	Cartesian3 operator*(float);
	Cartesian3 cross(Cartesian3& other);

	std::vector<edge*>lines;
	
};

//class edge represents the edges of the model
class edge
{
public:
	int start;
	int end;
	int point_count;

	edge(int s, int e)
	{
		start = s;
		end = e;
		point_count = -1;
	};

	std::vector<int> faces;
};

//class face contains vertex serial number of each face, not real 3D coordinates
class face
{
public:
	face();
	face(std::vector<int>);

	std::vector<int> points;
	std::vector<edge*> lines;
	int numberOfPoint;
	
};

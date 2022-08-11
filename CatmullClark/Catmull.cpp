#include <io.h>
#include <iostream>
#include "vector_mine.h"

char pathName[255] = { "1.obj" }; //select the object
float mouseX, mouseY;
float angleX, angleY;
float scale = 1;
bool mouseLeftButtonDown;
std::vector<Cartesian3> vertices; //vector for vertices, contains x, y, z
std::vector<face> faces; //vector for the faces, contains vectors for each quad. in each vector, it has the index of the vertex
std::vector<face> newFaces; //vector for new faces
std::vector<Cartesian3> normals; //vector for normals

//read the obj file and seve the vertices data
void initVertices()
{
	//open obj file
	FILE* file;
	fopen_s(&file, pathName, "rb");
	if (file == NULL)
	{
		std::cout << "obj not found!" << std::endl;
		return;
	}

	//read the file and save them 
	while (ftell(file) < _filelength(_fileno(file)))
	{
		char tmp[255] = { 0 };
		fgets(tmp, 255, file);
		std::string s(tmp);

		int ret; //return, initial symbol of each line in the obj file.
		int sti = 1; //string, use for divide the line into severial parts and save them

		//save vertices
		ret = s.find("v ");
		if (ret == 0)
		{
			while (s[sti] == ' ')
				sti++;
			int offx = s.find(" ", sti);
			int offy = s.find(" ", offx + 1);

			std::string x = s.substr(sti, offx - sti); //x value of the vertice
			std::string y = s.substr(offx + 1, offy - offx - 1); //y value of the vertice
			std::string z = s.substr(offy, s.length() - offy); //z value of the vertice

			//change them into float
			float xf = atof(x.c_str());
			float yf = atof(y.c_str());
			float zf = atof(z.c_str());

			//save them in the vecs
			Cartesian3 v(xf, yf, zf);
			vertices.push_back(v);
		}

		//save faces
		ret = s.find("f ");
		if (ret == 0)
		{

			std::vector<int> points;
			int i = 0;
			while (s[i] == ' ')
				i++;
			int off1 = s.find(" ", i);
			int off2 = s.find(" ", off1 + 1);
			int off3 = s.find(" ", off2 + 2);
			int off4 = s.find(" ", off3 + 3);

			std::string s1 = s.substr(off1 + 1, off2 - off1 - 1); //first vertice in this quad
			std::string s2 = s.substr(off2 + 1, off3 - off2 - 1); //second vertice in this quad
			std::string s3 = s.substr(off3 + 1, off4 - off3 - 1); //third vertice in this quad
			std::string s4 = s.substr(off4 + 1, s.length() - off4 - 1); //forth vertice in this quad

			int i1 = atoi((s1.substr(0, s1.find_first_of('/'))).c_str()); //first vertice(index)
			int i2 = atoi((s2.substr(0, s2.find_first_of('/'))).c_str()); //second vertice(index)
			int i3 = atoi((s3.substr(0, s3.find_first_of('/'))).c_str()); //third vertice(index)
			int i4 = atoi((s4.substr(0, s4.find_first_of('/'))).c_str()); //forth vertice(index)

			//save them in one int vector
			points.push_back(i1);
			points.push_back(i2);
			points.push_back(i3);
			points.push_back(i4);

			//make it a face
			face face(points);
			//save it in the faces vector
			faces.push_back(face);
		}

		//calculate vertex normal

	}

}

//compute the new vertices
static int finished = 0;
void Compute_Parameter()
{
	if (finished == 1) {
		return;
	}
	finished = 1;

	normals.clear();

	//calculate vertex normal

	//Area is not involved in the calculation
	for (int i = 0; i < vertices.size(); i++) {
		std::vector<Cartesian3> normalVector;//create a vector to save all the face normals connected with each vertex
		for (int j = 0; j < faces.size(); j++) {
			for (int k = 0; k < faces[j].points.size(); k++) {
				if ((i + 1) == faces[j].points[k]) {
					//calcutate the normal
					Cartesian3 normal1 = vertices[faces[j].points[2] - 1] - vertices[faces[j].points[0] - 1];
					Cartesian3 normal2 = vertices[faces[j].points[3] - 1] - vertices[faces[j].points[1] - 1];
					Cartesian3 normal = normal1.cross(normal2);

					float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
					normal = normal * (1.f / length);//unit normal
					//save it in the vector
					normalVector.push_back(normal);

				}
			}
		}

		Cartesian3 avgNormal;//crate the vertex normal
		for (int i = 0; i < normalVector.size(); i++) {
			avgNormal += normalVector[i];
		}

		float length2 = sqrt(avgNormal.x * avgNormal.x + avgNormal.y * avgNormal.y + avgNormal.z * avgNormal.z);
		avgNormal = avgNormal * (1.f / length2);//unit the normal
		normals.push_back(avgNormal);//save the vertex normal

	}


	/*
	//area as the parameter
	for (int i = 0; i < vertices.size(); i++) {
		std::vector<Cartesian3> normalVector;//create a vector to save all the face normals connected with each vertex
		std::vector<float> areaVector;//create a vector to save all the face area connected with each vertex
		for (int j = 0; j < faces.size(); j++) {
			for (int k = 0; k < faces[j].points.size(); k++) {
				if ((i + 1) == faces[j].points[k]) {
					//calculate the normals
					Cartesian3 normal1 = vertices[faces[j].points[2] - 1] - vertices[faces[j].points[0] - 1];
					Cartesian3 normal2 = vertices[faces[j].points[3] - 1] - vertices[faces[j].points[1] - 1];
					Cartesian3 normal = normal1.cross(normal2);

					float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
					normal = normal * (1.f / length);
					normalVector.push_back(normal);

					//area, using the Heron's formula twice, because a quad is formed by two triangles
					//S = ¡Ìp(p - a)(p - b)(p - c),p is half of the perimeter
					float e1, e2, e3, e4, e5 = 0;
					Cartesian3 v1 = vertices[faces[j].points[0] - 1];
					Cartesian3 v2 = vertices[faces[j].points[1] - 1];
					Cartesian3 v3 = vertices[faces[j].points[2] - 1];
					Cartesian3 v4 = vertices[faces[j].points[3] - 1];

					e1 = sqrt(pow((v2.x - v1.x), 2) + pow((v2.y - v1.y), 2) + pow((v2.z - v1.z), 2));
					e2 = sqrt(pow((v3.x - v2.x), 2) + pow((v3.y - v2.y), 2) + pow((v3.z - v2.z), 2));
					e3 = sqrt(pow((v4.x - v3.x), 2) + pow((v4.y - v3.y), 2) + pow((v4.z - v3.z), 2));
					e4 = sqrt(pow((v1.x - v4.x), 2) + pow((v1.y - v4.y), 2) + pow((v1.z - v4.z), 2));
					e5 = sqrt(pow((v2.x - v4.x), 2) + pow((v2.y - v4.y), 2) + pow((v2.z - v4.z), 2));

					float p1, p2 = 0;
					p1 = (e1 + e5 + e4) / 2;
					p2 = (e2 + e3 + e5) / 2;

					float s, s1, s2 = 0;
					s1 = sqrt(p1 * (p1 - e1) * (p1 - e5) * (p1 - e4));
					s2 = sqrt(p2 * (p2 - e2) * (p2 - e3) * (p2 - e5));

					s = s1 + s2;
					areaVector.push_back(s);

				}

			}

		}

		float sumArea = 0;
		for (int i = 0; i < areaVector.size(); i++) {
			sumArea+= areaVector[i];
		}
		std::cout << sumArea << std::endl;

		//calcolate the face normal
		Cartesian3 avgNormal;
		for (int i = 0; i < normalVector.size(); i++) {
			avgNormal += normalVector[i] * (areaVector[i] / sumArea);
		}
		//unit the normal
		float length2 = sqrt(avgNormal.x * avgNormal.x + avgNormal.y * avgNormal.y + avgNormal.z * avgNormal.z);
		avgNormal = avgNormal * (1.f / length2);
		std::cout << avgNormal.x << " " << avgNormal.y << " " << avgNormal.z << " " << std::endl;
		//save the vertex normal
		normals.push_back(avgNormal);
	}
	*/

	int oldVerticesCount = vertices.size();
	newFaces.clear();

	//face points
	for (int i = 0; i < faces.size(); i++)
	{
		//sum up all vertices coodinates in a quad, and divide it using the number of the vertices
		Cartesian3 facePoint;
		for (int j = 0; j < faces[i].points.size(); j++)
			facePoint += vertices[faces[i].points[j] - 1];
		facePoint = facePoint * (1.f / faces[i].points.size());
		vertices.push_back(facePoint);
		faces[i].numberOfPoint = vertices.size();
	}


	//save all lines
	for (int i = 0; i < faces.size(); i++)
	{
		for (int j = 0; j < faces[i].lines.size(); j++)
		{
			faces[i].lines[j]->faces.clear();
			faces[i].lines[j]->faces.push_back(faces[i].numberOfPoint);

			for (int i1 = 0; i1 < faces.size(); i1++)
			{
				if (i1 == i)
					continue;
				for (int j1 = 0; j1 < faces[i1].lines.size(); j1++)
				{
					if (((faces[i].lines[j]->start == faces[i1].lines[j1]->start) && (faces[i].lines[j]->end == faces[i1].lines[j1]->end)) ||
						((faces[i].lines[j]->start == faces[i1].lines[j1]->end) && (faces[i].lines[j]->end == faces[i1].lines[j1]->start)))
					{
						faces[i].lines[j]->faces.push_back(faces[i1].numberOfPoint);
					}
				}
			}
		}
	}

	//edge points
	std::vector<std::pair<edge*, int>> nvv;
	for (int i = 0; i < faces.size(); i++)
	{
		for (int j = 0; j < faces[i].lines.size(); j++)
		{
			Cartesian3 tmp;
			Cartesian3 inputVertice1 = vertices[faces[i].lines[j]->start - 1];
			Cartesian3 inputVertice2 = vertices[faces[i].lines[j]->end - 1];
			Cartesian3 edgePoint = inputVertice1 + inputVertice2;
			edgePoint = edgePoint * 0.5;
			for (int k = 0; k < faces[i].lines[j]->faces.size(); k++)
			{
				tmp += vertices[faces[i].lines[j]->faces[k] - 1];
			}
			if (faces.size() > 1)
			{
				tmp = tmp * (1.f / faces[i].lines[j]->faces.size());
				tmp = tmp + edgePoint;
				tmp = tmp * 0.5;
			}
			else
			{
				tmp = edgePoint;
			}
			bool skip = false;
			for (int x = 0; x < nvv.size(); x++)
			{
				if (((nvv[x].first->start == faces[i].lines[j]->start) && (nvv[x].first->end == faces[i].lines[j]->end)) ||
					((nvv[x].first->start == faces[i].lines[j]->end) && (nvv[x].first->end == faces[i].lines[j]->start)))
				{
					skip = true;
					faces[i].lines[j]->point_count = nvv[x].second;
				}
			}
			if (!skip)
			{
				vertices.push_back(tmp);
				faces[i].lines[j]->point_count = vertices.size();
				nvv.push_back(std::pair<edge*, int>(faces[i].lines[j], vertices.size()));
			}
		}
	}

	//create a new faces vector
	for (int i = 0; i < faces.size(); i++)
	{
		std::vector<int> ps;
		ps.push_back(faces[i].lines[0]->start);
		ps.push_back(faces[i].lines[0]->point_count);
		ps.push_back(faces[i].numberOfPoint);
		ps.push_back(faces[i].lines[faces[i].lines.size() - 1]->point_count);
		newFaces.push_back(ps);
		ps.clear();

		ps.push_back(faces[i].lines[0]->point_count);
		ps.push_back(faces[i].lines[0]->end);
		ps.push_back(faces[i].lines[1]->point_count);
		ps.push_back(faces[i].numberOfPoint);
		newFaces.push_back(ps);
		ps.clear();

		if (faces[i].lines.size() > 3)
		{
			ps.push_back(faces[i].lines[3]->point_count);
			ps.push_back(faces[i].numberOfPoint);
			ps.push_back(faces[i].lines[2]->point_count);
			ps.push_back(faces[i].lines[2]->end);
			newFaces.push_back(ps);
			ps.clear();
		}

		ps.push_back(faces[i].numberOfPoint);
		ps.push_back(faces[i].lines[1]->point_count);
		ps.push_back(faces[i].lines[1]->end);
		ps.push_back(faces[i].lines[2]->point_count);
		newFaces.push_back(ps);
	}

	//for Q
	std::vector<Cartesian3> Q;
	for (int i = 0; i < oldVerticesCount; i++) {//For each origin

		std::vector<Cartesian3> QVector;//For each origin£¬ create one vector for Q

		for (int j = 0; j < faces.size(); j++) {// for each origin, we do one loop for each face

			for (int k = 0; k < faces[j].points.size(); k++) {//For each face, loop once for each point

				if ((i + 1) == faces[j].points[k]) {//When a point is read, the i +1 used here is the serial number of the point

					Cartesian3 qSet;//face point

					for (int x = 0; x < faces[j].points.size(); x++) {
						qSet += vertices[faces[j].points[x] - 1];
					}

					qSet = qSet * (1.f / faces[j].points.size());

					QVector.push_back(qSet);

				}
			}
		}

		Cartesian3 q; //finnal q saved in the vector
		for (int i = 0; i < QVector.size(); i++) {
			q = q + QVector[i];
		}
		q = q * (1.f / QVector.size());
		Q.push_back(q);

	}

	//for R
	std::vector<Cartesian3> R;
	for (int i = 0; i < oldVerticesCount; i++) {//For each origin

		std::vector<Cartesian3> RVector;//For each origin create one vector for R

		for (int j = 0; j < faces.size(); j++) {// for each origin, we do one loop for each face

			for (int k = 0; k < faces[j].points.size(); k++) {//For each face, loop once for each point

				if ((i + 1) == faces[j].points[k]) {//When a point is read, the i +1 used here is the serial number of the point

					Cartesian3 rSet; //center of one edge

					if (k == 0) {
						rSet = vertices[faces[j].points[0] - 1] + vertices[faces[j].points[1] - 1];
					}
					else if (k == 1) {
						rSet = vertices[faces[j].points[1] - 1] + vertices[faces[j].points[2] - 1];
					}
					else if (k == 2) {
						rSet = vertices[faces[j].points[2] - 1] + vertices[faces[j].points[3] - 1];
					}
					else if (k == 3) {
						rSet = vertices[faces[j].points[3] - 1] + vertices[faces[j].points[0] - 1];
					}
					rSet = rSet * 0.5;
					RVector.push_back(rSet);
				}
			}
		}


		Cartesian3 r; //finnal r saved in the vector
		for (int i = 0; i < RVector.size(); i++) {
			r = r + RVector[i];
		}
		r = r * (1.f / RVector.size());
		R.push_back(r);

	}

	std::vector<Cartesian3> Sum;
	for (int i = 0; i < oldVerticesCount; i++) {
		Cartesian3 sum;
		int degree = 0;
		for (int j = 0; j < faces.size(); j++) {
			for (int k = 0; k < faces[j].points.size(); k++) {
				if ((i + 1) == faces[j].points[k]) {
					degree++;//compute the degree of the vertex
				}
			}
		}

		//different formula for different degree
		if (degree == 3) {
			sum = Q[i] * 0.33 + R[i] * 0.66;
		}
		else if (degree == 4) {
			sum = Q[i] * 0.25 + R[i] * 0.5 + vertices[i] * 0.25;
		}
		else if (degree == 5) {
			sum = Q[i] * 0.2 + R[i] * 0.4 + vertices[i] * 0.4;
		}
		else if (degree == 6) {
			sum = Q[i] * 0.16 + R[i] * 0.33 + vertices[i] * 0.5;
		}
		else if (degree == 7) {
			sum = Q[i] * 0.14 + R[i] * 0.28 + vertices[i] * 0.57;
		}

		Sum.push_back(sum);
	}

	for (int i = 0; i < oldVerticesCount; i++) {//For each origin
		for (int j = 0; j < faces.size(); j++) {//for each origin, we do one loop for each face
			for (int k = 0; k < faces[j].points.size(); k++) {//For each face, loop once for each point
				if ((i + 1) == faces[j].points[k]) {

					int otherVec; //get the vertex next to current vertex

					if (k == 0) {
						otherVec = faces[j].points[1] - 1;
					}
					else if (k == 1) {
						otherVec = faces[j].points[2] - 1;
					}
					else if (k == 2) {
						otherVec = faces[j].points[3] - 1;
					}
					else if (k == 3) {
						otherVec = faces[j].points[0] - 1;
					}

					//dot product
					float result = normals[i].x * normals[otherVec].x + normals[i].y * normals[otherVec].y + normals[i].z * normals[otherVec].z;

					//Setting the level of subdivision
					if (result > 0.8 || result < -0.8) {
						vertices[i] = Sum[i];
					}

				}

			}

		}

	}

	faces = newFaces; //update the face vector

	glutPostRedisplay();
};

//function for mouse button
void MouseControl(int button, int state, int x, int y)
{
	mouseX = x;
	mouseY = y;

	//left button control
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseLeftButtonDown = true;
		}
		else if (state == GLUT_UP)
			mouseLeftButtonDown = false;
	}
}

//function for mouse movement
void mouseMotionControl(int x, int y)
{
	if (mouseLeftButtonDown)
	{
		angleY += (x - mouseX);
		angleX += (y - mouseY);
		mouseX = x;
		mouseY = y;
	}

	glutPostRedisplay();
}

//set the button for subdivision
void KeyboardControl(unsigned char key, int x, int y)
{
	//click m to subdivide the model
	switch (key)
	{
	case'm':
		Compute_Parameter();
		break;
	}
};

//for resize
void ChangeSize(GLsizei w, GLsizei h)
{
	GLfloat aspectRatio, windowWidth, windowHight;

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	aspectRatio = (GLfloat)w / (GLfloat)h;
	windowWidth = 150.0 * aspectRatio;
	windowHight = windowWidth;
	glOrtho(-windowWidth, windowWidth, -windowWidth, windowWidth, 1500, -1500);
};

//function for render the mesh
void RenderScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glRotatef(angleX, 1, 0, 0);
	glRotatef(angleY, 0, 1, 0);
	glScalef(scale * 6, scale * 6, scale * 6);
	glColor3f(1.0f, 0.0f, 0.0f);

	for (int i = 0; i < faces.size(); i++)
	{
		/*
		for (int x = 0; x < faces[i].lines.size(); x++)
			delete faces[i].lines[x];
		*/

		faces[i].lines.clear();

		int j = 0;
		//draw 3 lines
		for (j = 0; j < faces[i].points.size() - 1; j++)
		{
			glBegin(GL_LINES); //line mode

			Cartesian3 startVertice = vertices[faces[i].points[j] - 1]; //start point
			Cartesian3 endVertice = vertices[faces[i].points[j + 1] - 1]; //end point
			glVertex3f(startVertice.x, startVertice.y, startVertice.z);
			glVertex3f(endVertice.x, endVertice.y, endVertice.z);

			edge* Line = new edge(faces[i].points[j], faces[i].points[j + 1]); //create a new line
			faces[i].lines.push_back(Line); //save it in faces
			vertices[Line->start - 1].lines.push_back(Line);
			vertices[Line->end - 1].lines.push_back(Line);

			glEnd();
		}

		glBegin(GL_LINES);//line mode 

		Cartesian3 startVertice = vertices[faces[i].points[j] - 1];
		Cartesian3 endVertice = vertices[faces[i].points[0] - 1];
		glVertex3f(startVertice.x, startVertice.y, startVertice.z);
		glVertex3f(endVertice.x, endVertice.y, endVertice.z);

		edge* Line = new edge(faces[i].points[j], faces[i].points[0]);
		faces[i].lines.push_back(Line);
		vertices[Line->start - 1].lines.push_back(Line);
		vertices[Line->end - 1].lines.push_back(Line);

		glEnd();
	}

	glPopMatrix();
	glutSwapBuffers();
	finished = 0;
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	initVertices();
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("catmull");
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutKeyboardFunc(KeyboardControl);
	glutMouseFunc(MouseControl);
	glutMotionFunc(mouseMotionControl);
	glutMainLoop();
	return 0;
}
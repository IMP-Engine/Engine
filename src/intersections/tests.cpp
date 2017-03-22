#include "tests.h"

Triangle t;

using namespace glm;
using namespace std;

#define TEST_RADIUS 0.5f
#define TEST_OFFSET 0.2f

void printVec(vec3 v) {
    cout << " [" << v.x << "," << v.y << "," << v.z << "] ";
}

void printResult(string test, Intersection i, bool result, Intersection expectedI, bool expectedResult) {
    cout << "Test: " << test << " ";
    if (expectedResult == result && i == expectedI) {
        cout << "passed!" << endl;
    } else {
        cout << "failed!" << endl;
		cout << "Intersection: " << endl;
		cout << "    Gradient: ";
		printVec(i.responseGradient);
		cout << endl << "    Scale: " << i.responseDistance << endl;
    }
}

void doIntersectionTests() {

    t.v0 = vec3(0);
    t.v1 = vec3(1);
    t.v2 = vec3(1,1,0);
    t.u = t.v1 - t.v0;
    t.v = t.v2 - t.v0;
    t.normal = normalize(cross(t.u, t.v));



    doPlaneSeparateTest();

    doVertexIntersectionTests();

    doCenterInTriangleTest();

    doEdgeIntersectionTests();


}

void doPlaneSeparateTest() {
	Intersection i;
	Particle p;
	p.pos = vec3(0, 1, 0);
	p.pPos = vec3(0, 1, 0);
	p.radius = TEST_RADIUS;
	bool result = intersect(t, p, i);
	printResult("PlaneSeparate", i, result, i, false);
}

void doVertexIntersectionTests() {

	Intersection i, expectedI;
	Particle p;
	p.pPos = t.v1 + TEST_OFFSET*(t.v1 - t.v0);

	expectedI.responseDistance = TEST_RADIUS;
	expectedI.responseGradient = t.normal;

	p.radius = TEST_RADIUS;
	bool result = intersect(t, p, i);
	printResult("Vertex 1 Intersection", i, result, expectedI, true);


	p.pPos = t.v2 + TEST_OFFSET*(t.v2 - t.v0);
	result = intersect(t, p, i);
	printResult("Vertex 2 Intersection", i, result, i, true);


	p.pPos = t.v0 + TEST_OFFSET*(t.v0 - t.v1);
	result = intersect(t, p, i);
	printResult("Vertex 3 Intersection", i, result, i, true);
}


void doCenterInTriangleTest() {
	Intersection i;
	Particle p;
	p.pPos = vec3(.7f, .8f, 0.5f);
	p.radius = 0.2f;
	bool result = intersect(t, p, i);
	printResult("Center Inside Intersection", i, result, i, true);
}

void doEdgeIntersectionTests() {

	Intersection i, expectedI;
	Particle p;
	p.radius = TEST_RADIUS;


	expectedI.responseDistance = TEST_RADIUS;
	expectedI.responseGradient = t.normal;

	p.pPos = vec3(.5f,.5f,1.f);
	bool result = intersect(t, p, i);
	printResult("Edge 1 Intersection", i, result, expectedI, true);


	p.pPos = vec3(0.5, .5, -0.3);
	result = intersect(t, p, i);
	printResult("Edge 2 Intersection", i, result, expectedI, true);


	p.pPos = vec3(1.3, 1.3, 0.5);
	result = intersect(t, p, i);
	printResult("Edge 3 Intersection", i, result, expectedI, true);
}
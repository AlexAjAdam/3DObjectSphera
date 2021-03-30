
#include <QtWidgets>

class WEdge;

class Vertex {
private:
	float x, y, z;
	WEdge* edge;

public:
	Vertex() { x = 0; y = 0; z = 0; edge = nullptr; }
	Vertex(const Vertex &point) { x = point.x; y = point.y; z = point.z; edge = nullptr; }
	Vertex(float x1, float y1, float z1) { x = x1; y = y1; z = z1; edge = nullptr; }
	Vertex(float x1, float y1, float z1, WEdge* edge1) { x = x1; y = y1; z = z1; edge = edge1; }

	void setVertex(float x1, float y1, float z1) { x = x1; y = y1; z = z1; }
	void setX(float x1) { x = x1; }
	void setY(float y1) { y = y1; }
	void setZ(float z1) { z = z1; }
	void setEdge(WEdge* edge1) { edge = edge1; }

	float getX() { return x; }
	float getY() { return y; }
	float getZ() { return z; }
	WEdge* getEdge() { return edge; }

	QVector3D getVertex() { return QVector3D(x, y, z); }

};

class Face {
private:
	WEdge* edge;

public:
	Face() { edge = nullptr; }
	Face(const Face& face) { edge = face.edge; }
	Face(WEdge* e) { edge = e; }

	void setEdge(WEdge* edge1) { edge = edge1; }

	WEdge* getEdge() { return edge; }
};

class WEdge {
private:
	Vertex* originVertex;
	Vertex* endVertex;
	Face* rightFace;
	Face* leftFace;
	WEdge* prevEdgeRight; 
	WEdge* prevEdgeLeft;
	WEdge* nextEdgeRight;
	WEdge* nextEdgeLeft;

public:

	WEdge() { originVertex = nullptr; endVertex = nullptr; rightFace = nullptr; leftFace = nullptr; prevEdgeRight = nullptr; prevEdgeLeft = nullptr; nextEdgeRight = nullptr; nextEdgeLeft = nullptr; }
	WEdge(const WEdge& edge) { originVertex = edge.originVertex; endVertex = edge.endVertex; rightFace = edge.rightFace; leftFace = edge.leftFace; prevEdgeRight = edge.prevEdgeRight; prevEdgeLeft = edge.prevEdgeLeft; nextEdgeRight = edge.nextEdgeRight; nextEdgeLeft = edge.nextEdgeLeft; }
	WEdge(Vertex* origin, Face* fR, Face* fL, WEdge* prevER, WEdge* prevEL, WEdge* nextER, WEdge* nextEL) { originVertex = origin; rightFace = fR; leftFace = fL; prevEdgeRight = prevER; prevEdgeLeft = prevEL; nextEdgeRight = nextER; nextEdgeLeft = nextEL; }

	void setOriginVertex(float x, float y, float z) { originVertex->setVertex(x, y, z); }
	void setEndVertex(float x, float y, float z) { endVertex->setVertex(x, y, z); }
	void setPrevEdgeRight(WEdge* e) { prevEdgeRight = e; }
	void setPrevEdgeLeft(WEdge* e) { prevEdgeLeft = e; }
	void setNextEdgeRight(WEdge* e) { nextEdgeRight = e; }
	void setNextEdgeLeft(WEdge* e) { nextEdgeLeft = e; }

	Vertex* getOriginVertex() { return originVertex; }
	Face* getRightFace() { return rightFace; }
	Face* getLeftFace() { return leftFace; }
	WEdge* getPrevEdgeRight() { return prevEdgeRight; }
	WEdge* getPrevEdgeLeft() { return prevEdgeLeft; }
	WEdge* getNextEdgeRight() { return nextEdgeRight; }
	WEdge* getNextEdgeLeft() { return nextEdgeLeft; }

};
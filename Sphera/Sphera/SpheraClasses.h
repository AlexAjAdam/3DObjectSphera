
#include <QtWidgets>

class HEdge;

class Vertex {
private:
	float x, y, z;
	HEdge* edge;

public:
	Vertex() { x = 0; y = 0; z = 0; edge = nullptr; }
	Vertex(const Vertex &point) { x = point.x; y = point.y; z = point.z; edge = nullptr; }
	Vertex(float x1, float y1, float z1) { x = x1; y = y1; z = z1; edge = nullptr; }
	Vertex(float x1, float y1, float z1, HEdge* edge1) { x = x1; y = y1; z = z1; edge = edge1; }

	void setVertex(float x1, float y1, float z1) { x = x1; y = y1; z = z1; }
	void setX(float x1) { x = x1; }
	void setY(float y1) { y = y1; }
	void setZ(float z1) { z = z1; }
	void setEdge(HEdge* edge1) { edge = edge1; }

	float getX() { return x; }
	float getY() { return y; }
	float getZ() { return z; }
	HEdge* getEdge() { return edge; }

	QVector3D getVertex() { return QVector3D(x, y, z); }

	void printVertex() { qDebug() << "(" << x << "," << y << "," << z << ")"; }

};

class Face {
private:
	HEdge* edge;

public:
	Face() { edge = nullptr; }
	Face(const Face& face) { edge = face.edge; }
	Face(HEdge* e) { edge = e; }

	void setEdge(HEdge* edge1) { edge = edge1; }

	HEdge* getEdge() { return edge; }
};

class HEdge {
private:

	Vertex* originVertex;

	Face* face;
	
	HEdge* prevEdge;
	HEdge* nextEdge;
	HEdge* pair;

public:

	HEdge() { originVertex = nullptr; face = nullptr; prevEdge = nullptr; nextEdge = nullptr; pair = nullptr; }
	HEdge(const HEdge& edge) { originVertex = edge.originVertex; face = edge.face; prevEdge = edge.prevEdge; nextEdge = edge.nextEdge; pair = edge.pair; }
	HEdge(Vertex* origin, Face* f, HEdge* prevE, HEdge* nextE, HEdge* pairE) { originVertex = origin; face = f; prevEdge = prevE; nextEdge = nextE; pair = pairE; }
	HEdge(Vertex* origin) { originVertex = origin; face = nullptr; prevEdge = nullptr; nextEdge = nullptr; pair = nullptr; }

	void setOriginVertex(float x, float y, float z) { originVertex->setVertex(x, y, z); }
	void setOriginVertex(Vertex* point) { originVertex = point; }
	void setFace(Face* f) { face = f; }
	void setPrevEdge(HEdge* e) { prevEdge = e; }
	void setNextEdge(HEdge* e) { nextEdge = e; }
	void setPairEdge(HEdge* e) { pair = e; }

	Vertex* getOriginVertex() { return originVertex; }
	Face* getFace() { return face; }
	HEdge* getPrevEdge() { return prevEdge; }
	HEdge* getNextEdge() { return nextEdge; }
	HEdge* getPairEdge() { return pair; }

	void printEdge() 
	{ 
		qDebug() << "OriginVertex (" << originVertex->getX() << "," << originVertex->getY() << "," << originVertex->getZ() << ")";
		qDebug() << "Previous Edge {origin Vertex} (" << prevEdge->getOriginVertex()->getX() << "," << prevEdge->getOriginVertex()->getY() << "," << prevEdge->getOriginVertex()->getZ() << ")}";
		qDebug() << "Next Edge {origin Vertex} (" << nextEdge->getOriginVertex()->getX() << "," << nextEdge->getOriginVertex()->getY() << "," << nextEdge->getOriginVertex()->getZ() << ")}";
		qDebug() << "Pair Edge {origin Vertex} (" << pair->getOriginVertex()->getX() << "," << pair->getOriginVertex()->getY() << "," << pair->getOriginVertex()->getZ() << ")}";
		qDebug() << "Face {origin Vertex} (" << face->getEdge()->getOriginVertex()->getX() << "," << face->getEdge()->getOriginVertex()->getY() << "," << face->getEdge()->getOriginVertex()->getZ() << ")}";
	}
	void printEdgeOriginVertex() {
		qDebug() << "OriginVertex (" << originVertex->getX() << "," << originVertex->getY() << "," << originVertex->getZ() << ")";
	}
};
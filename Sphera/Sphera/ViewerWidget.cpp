#include   "ViewerWidget.h"

ViewerWidget::ViewerWidget(QString viewerName, QSize imgSize, QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_StaticContents);
	setMouseTracking(true);
	name = viewerName;
	if (imgSize != QSize(0, 0)) {
		img = new QImage(imgSize, QImage::Format_ARGB32);
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
		setDataPtr();
	}
}
ViewerWidget::~ViewerWidget()
{
	delete painter;
	delete img;
}
void ViewerWidget::resizeWidget(QSize size)
{
	this->resize(size);
	this->setMinimumSize(size);
	this->setMaximumSize(size);
}

//Image functions
bool ViewerWidget::setImage(const QImage& inputImg)
{
	if (img != nullptr) {
		delete img;
	}
	img = new QImage(inputImg);
	if (!img) {
		return false;
	}
	resizeWidget(img->size());
	setPainter();
	update();

	return true;
}
bool ViewerWidget::isEmpty()
{
	if (img->size() == QSize(0, 0)) {
		return true;
	}
	return false;
}

//Data function
void ViewerWidget::setPixel(int x, int y, const QColor& color)
{
	if (isInside(x, y)) {
		data[x + y * img->width()] = color.rgb();
	}
}
void ViewerWidget::setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	if (isInside(x, y)) {
		QColor color(r, g, b);
		setPixel(x, y, color);
	}
}

void ViewerWidget::clear(QColor color)
{
	for (size_t x = 0; x < img->width(); x++)
	{
		for (size_t y = 0; y < img->height(); y++)
		{
			setPixel(x, y, color);
		}
	}
	update();
}

void ViewerWidget::loadDataFromVTKFile(QVector<Vertex*> points, QVector<HEdge*> edges, QVector<Face*> faces)
{
	QVector<QString> fileLines;
	QString fileName = "octahedron.vtk";

	if (fileName.isEmpty()) {
		qDebug() << "file is empty";
		return;
	}

	QFileInfo Finfo(fileName);

	if (Finfo.suffix() == "vtk") {

		QFile file(fileName);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			return;

		QTextStream in(&file);

		while (!in.atEnd()) {
			QString line = in.readLine();
			fileLines.push_back(line);
		}

		file.close();
	}

	if (fileLines.isEmpty()) {
		QMessageBox msgBox;
		msgBox.setText("Data did not load.");
		msgBox.exec();
	}

	QStringList list = fileLines[4].split(QLatin1Char(' '));
	int pointsSize = list.at(1).toInt();
	qDebug() << pointsSize; 

	for (int i = 5; i < pointsSize+5; i++)
	{
		Vertex* point = new Vertex;
		QStringList list = fileLines[i].split(QLatin1Char(' '));
		point->setX(list.at(0).toFloat());
		point->setY(list.at(1).toFloat());
		point->setZ(list.at(2).toFloat());
		points.push_back(point);
	}

	/*for (int i = 0; i <  points.size(); i++)
	{
		points[i].printVertex();
	}*/

	QStringList list3 = fileLines[pointsSize+6].split(QLatin1Char(' '));
	int polygoneSize = list3.at(1).toInt();
	qDebug() << polygoneSize;

	for (int i = pointsSize + 7; i < polygoneSize + pointsSize + 7; i++)
	{
		int first, second, third;
		QStringList list = fileLines[i].split(QLatin1Char(' '));
		first = list.at(1).toInt(); second = list.at(2).toInt(); third = list.at(3).toInt();

		HEdge* edge1 = new HEdge; HEdge* edge2 = new HEdge; HEdge* edge3 = new HEdge;
		Face* face = new Face;

		edge1->setOriginVertex(points[first]);
		edge1->setPrevEdge(edge3);
		edge1->setNextEdge(edge2);
		face->setEdge(edge1);
		edge1->setFace(face);

		edge2->setOriginVertex(points[second]);
		edge2->setPrevEdge(edge1);
		edge2->setNextEdge(edge3);
		edge2->setFace(face);

		edge3->setOriginVertex(points[third]);
		edge3->setPrevEdge(edge2);
		edge3->setNextEdge(edge1);
		edge3->setFace(face);

		edges.push_back(edge1);
		edges.push_back(edge2);
		edges.push_back(edge3);
		faces.push_back(face);
	}

	for (int i = 0; i < edges.size(); i++) //funguje iba pre octahedron
	{
		if (i==0)
		{
			edges[0]->setPairEdge(edges[12]);
			edges[12]->setPairEdge(edges[0]);
		}
		if (i==2) // o +/-8
		{
			edges[i]->setPairEdge(edges[i + 8]);
			edges[i + 8]->setPairEdge(edges[i]);
		}
		if (i==1 || i==4 || i==7) // o +/-4
		{
			edges[i]->setPairEdge(edges[i + 4]);
			edges[i + 4]->setPairEdge(edges[i]);
		}
		if (i==14 || i==17 || i==20) //o +/-2
		{
			edges[i]->setPairEdge(edges[i + 2]);
			edges[i + 2]->setPairEdge(edges[i]);
		}
		if (i==3 || i==6 || i==9)// o +/-12
		{
			edges[i]->setPairEdge(edges[i + 12]);
			edges[i + 12]->setPairEdge(edges[i]);
		}
		if (i==13) //o +/-10
		{
			edges[i]->setPairEdge(edges[i + 10]);
			edges[i + 10]->setPairEdge(edges[i]);
		}
	}
	
	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->printEdge();
		qDebug() << " ";
	}
}

//Slots
void ViewerWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QRect area = event->rect();
	painter.drawImage(area, *img, area);
}
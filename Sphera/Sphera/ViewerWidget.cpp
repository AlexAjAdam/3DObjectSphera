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

void ViewerWidget::loadDataFromVTKFile(QVector<Vertex> points)
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
	//qDebug() << pointsSize;

	//QVector<Vertex> points; 

	for (int i = 5; i < pointsSize+5; i++)
	{
		Vertex point;
		QStringList list1 = fileLines[i].split(QLatin1Char(' '));
		point.setX(list1.at(0).toFloat());
		point.setY(list1.at(1).toFloat());
		point.setZ(list1.at(2).toFloat());
		points.push_back(point);
	}

	//vertexes = points;

	/*QStringList list3 = fileLines[pointsSize+5].split(QLatin1Char(' '));
	int facesSize = list3.at(1).toInt();
	qDebug() << facesSize;*/

}

//Slots
void ViewerWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QRect area = event->rect();
	painter.drawImage(area, *img, area);
}
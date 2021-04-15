#include "ImageViewer.h"

ImageViewer::ImageViewer(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::ImageViewerClass)
{
	ui->setupUi(this);

	ui->menuBar->setDisabled(true);

	int width = 500;
	int height = 500;
	QString name = "Scatch";
	openNewTabForImg(new ViewerWidget(name, QSize(width, height)));
	ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
}

ImageViewer::~ImageViewer()
{
	dealloc();
}

//ViewerWidget functions
ViewerWidget* ImageViewer::getViewerWidget(int tabId)
{
	QScrollArea* s = static_cast<QScrollArea*>(ui->tabWidget->widget(tabId));
	if (s) {
		ViewerWidget* vW = static_cast<ViewerWidget*>(s->widget());
		return vW;
	}
	return nullptr;
}
ViewerWidget* ImageViewer::getCurrentViewerWidget()
{
	return getViewerWidget(ui->tabWidget->currentIndex());
}

// Event filters
bool ImageViewer::eventFilter(QObject* obj, QEvent* event)
{
	if (obj->objectName() == "ViewerWidget") {
		return ViewerWidgetEventFilter(obj, event);
	}
	return false;
}

//ViewerWidget Events
bool ImageViewer::ViewerWidgetEventFilter(QObject* obj, QEvent* event)
{
	ViewerWidget* w = static_cast<ViewerWidget*>(obj);

	if (!w) {
		return false;
	}

	if (event->type() == QEvent::MouseButtonPress) {
		ViewerWidgetMouseButtonPress(w, event);
	}
	else if (event->type() == QEvent::MouseButtonRelease) {
		ViewerWidgetMouseButtonRelease(w, event);
	}
	else if (event->type() == QEvent::MouseMove) {
		ViewerWidgetMouseMove(w, event);
	}
	else if (event->type() == QEvent::Leave) {
		ViewerWidgetLeave(w, event);
	}
	else if (event->type() == QEvent::Enter) {
		ViewerWidgetEnter(w, event);
	}
	else if (event->type() == QEvent::Wheel) {
		ViewerWidgetWheel(w, event);
	}

	return QObject::eventFilter(obj, event);
}
void ImageViewer::ViewerWidgetMouseButtonPress(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
	if (e->button() == Qt::LeftButton) {
	}
}
void ImageViewer::ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
}
void ImageViewer::ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
}
void ImageViewer::ViewerWidgetLeave(ViewerWidget* w, QEvent* event)
{
}
void ImageViewer::ViewerWidgetEnter(ViewerWidget* w, QEvent* event)
{
}
void ImageViewer::ViewerWidgetWheel(ViewerWidget* w, QEvent* event)
{
	QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
}

//ImageViewer Events
void ImageViewer::closeEvent(QCloseEvent* event)
{
	if (QMessageBox::Yes == QMessageBox::question(this, "Close Confirmation", "Are you sure you want to exit?", QMessageBox::Yes | QMessageBox::No))
	{
		event->accept();
	}
	else {
		event->ignore();
	}
}

//Image functions
void ImageViewer::openNewTabForImg(ViewerWidget* vW)
{
	QScrollArea* scrollArea = new QScrollArea;
	scrollArea->setWidget(vW);

	scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setWidgetResizable(true);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	vW->setObjectName("ViewerWidget");
	vW->installEventFilter(this);

	QString name = vW->getName();

	ui->tabWidget->addTab(scrollArea, name);
}
bool ImageViewer::openImage(QString filename)
{
	QFileInfo fi(filename);

	QString name = fi.baseName();
	openNewTabForImg(new ViewerWidget(name, QSize(0, 0)));
	ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);

	ViewerWidget* w = getCurrentViewerWidget();

	QImage loadedImg(filename);
	return w->setImage(loadedImg);
}
bool ImageViewer::saveImage(QString filename)
{
	QFileInfo fi(filename);
	QString extension = fi.completeSuffix();
	ViewerWidget* w = getCurrentViewerWidget();

	QImage* img = w->getImage();
	return img->save(filename, extension.toStdString().c_str());
}
void ImageViewer::clearImage()
{
	ViewerWidget* w = getCurrentViewerWidget();
	w->clear();
}
void ImageViewer::setBackgroundColor(QColor color)
{
	ViewerWidget* w = getCurrentViewerWidget();
	w->clear(color);
}

//Slots

//Tabs slots
void ImageViewer::on_tabWidget_tabCloseRequested(int tabId)
{
	ViewerWidget* vW = getViewerWidget(tabId);
	delete vW; //vW->~ViewerWidget();
	ui->tabWidget->removeTab(tabId);
}
void ImageViewer::on_actionRename_triggered()
{
	if (!isImgOpened()) {
		msgBox.setText("No image is opened.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return;
	}
	ViewerWidget* w = getCurrentViewerWidget();
	bool ok;
	QString text = QInputDialog::getText(this, QString("Rename image"), tr("Image name:"), QLineEdit::Normal, w->getName(), &ok);
	if (ok && !text.trimmed().isEmpty())
	{
		w->setName(text);
		ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), text);
	}
}

//Image slots
void ImageViewer::on_actionNew_triggered()
{
	newImgDialog = new NewImageDialog(this);
	connect(newImgDialog, SIGNAL(accepted()), this, SLOT(newImageAccepted()));
	newImgDialog->exec();
}
void ImageViewer::newImageAccepted()
{
	NewImageDialog* newImgDialog = static_cast<NewImageDialog*>(sender());

	int width = newImgDialog->getWidth();
	int height = newImgDialog->getHeight();
	QString name = newImgDialog->getName();
	openNewTabForImg(new ViewerWidget(name, QSize(width, height)));
	ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
}
void ImageViewer::on_actionOpen_triggered()
{
	QString folder = settings.value("folder_img_load_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getOpenFileName(this, "Load image", folder, fileFilter);
	if (fileName.isEmpty()) { return; }

	QFileInfo fi(fileName);
	settings.setValue("folder_img_load_path", fi.absoluteDir().absolutePath());

	if (!openImage(fileName)) {
		msgBox.setText("Unable to open image.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
}
void ImageViewer::on_actionSave_as_triggered()
{
	if (!isImgOpened()) {
		msgBox.setText("No image to save.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return;
	}
	QString folder = settings.value("folder_img_save_path", "").toString();

	ViewerWidget* w = getCurrentViewerWidget();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getSaveFileName(this, "Save image", folder + "/" + w->getName(), fileFilter);
	if (fileName.isEmpty()) { return; }

	QFileInfo fi(fileName);
	settings.setValue("folder_img_save_path", fi.absoluteDir().absolutePath());

	if (!saveImage(fileName)) {
		msgBox.setText("Unable to save image.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
	else {
		msgBox.setText(QString("File %1 saved.").arg(fileName));
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
	}
}
void ImageViewer::on_actionClear_triggered()
{
	if (!isImgOpened()) {
		msgBox.setText("No image is opened.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return;
	}
	clearImage();
}
void ImageViewer::on_actionSet_background_color_triggered()
{
	QColor backgroundColor = QColorDialog::getColor(Qt::white, this, "Select color of background");
	if (backgroundColor.isValid()) {
		setBackgroundColor(backgroundColor);
	}
}

void ImageViewer::loadDataFromVTK()
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
		QMessageBox msgBox1;
		msgBox1.setText("Data did not load.");
		msgBox1.exec();
	}

	QStringList list = fileLines[4].split(QLatin1Char(' '));
	int pointsSize = list.at(1).toInt();
	//qDebug() << pointsSize; 

	for (int i = 5; i < pointsSize + 5; i++)
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

	QStringList list3 = fileLines[pointsSize + 6].split(QLatin1Char(' '));
	int polygoneSize = list3.at(1).toInt();
	//qDebug() << polygoneSize;

	for (int i = pointsSize + 7; i < polygoneSize + pointsSize + 7; i++)
	{
		int first, second, third;
		QStringList list = fileLines[i].split(QLatin1Char(' '));
		first = list.at(1).toInt(); second = list.at(2).toInt(); third = list.at(3).toInt();

		HEdge* edge1 = new HEdge; 
		HEdge* edge2 = new HEdge; 
		HEdge* edge3 = new HEdge;
		Face* face = new Face;

		edge1->setOriginVertex(points[first]);
		points[first]->setEdge(edge1);
		edge2->setOriginVertex(points[second]);
		points[second]->setEdge(edge2);
		edge3->setOriginVertex(points[third]);
		points[third]->setEdge(edge3);
		
		edge1->setPrevEdge(edge3);
		edge1->setNextEdge(edge2);
		edge1->setFace(face);
		face->setEdge(edge1);

		edge2->setPrevEdge(edge1);
		edge2->setNextEdge(edge3);
		edge2->setFace(face);

		edge3->setPrevEdge(edge2);
		edge3->setNextEdge(edge1);
		edge3->setFace(face);

		edges.push_back(edge1);
		edges.push_back(edge2);
		edges.push_back(edge3);
		faces.push_back(face);
	}

	pairEdges();

	/*for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->printEdge();
		qDebug() << " ";
	}*/

}

void ImageViewer::pairEdges()
{
	for (int i = 0; i < edges.size(); i++)
	{
		Vertex* start1; Vertex* end1;

		start1 = edges[i]->getOriginVertex();
		end1 = edges[i]->getNextEdge()->getOriginVertex();

		for (int j = 0; j < edges.size(); j++)
		{
			Vertex* start2; Vertex* end2;

			start2 = edges[j]->getOriginVertex();
			end2 = edges[j]->getNextEdge()->getOriginVertex();

			if (start1 == end2 && start2 == end1)
			{
				edges[i]->setPairEdge(edges[j]);
				edges[j]->setPairEdge(edges[i]);
			}
		}

		if (edges[i]->getPairEdge()==nullptr)
		{
			qDebug() << "chybna hrana";
		}
	}
}

void ImageViewer::divide()
{
	QVector<HEdge*> edges2 = QVector<HEdge*>();
	QVector<HEdge*> dividedEdges = QVector<HEdge*>();
	QVector<Face*> faces2 = QVector<Face*>();

	for (int i = 0; i < faces.size(); i++)
	{
		Vertex* middle1;
		Vertex* middle2;
		Vertex* middle3;

		HEdge* edge1 = faces[i]->getEdge();
		HEdge* edge2 = faces[i]->getEdge()->getNextEdge();
		HEdge* edge3 = faces[i]->getEdge()->getPrevEdge();

		if (dividedEdges.size() > 1 && (edge1 == dividedEdges[1] || edge2 == dividedEdges[1] || edge3 == dividedEdges[1])) {
			int x = 10;
		}

		bool edge1In = false; bool edge2In = false; bool edge3In = false;

		for (int j = 0; j < dividedEdges.size(); j++)
		{
			if (dividedEdges[j]==edge1)
			{
				edge1In = true;
			}
			if (dividedEdges[j] == edge2)
			{
				edge2In = true;
			}
			if (dividedEdges[j] == edge3)
			{
				edge3In = true;
			}
		}

		HEdge* edge1new1; HEdge* edge1new2; 

		if (edge1In)
		{
			for (int j = 0; j < edges2.size(); j++)
			{
				if (edge1->getOriginVertex() == edges2[j]->getOriginVertex() && edges2[j]->getPairEdge()->getNextEdge() != nullptr)
				{
					if (edge1->getPairEdge()->getOriginVertex() == edges2[j]->getPairEdge()->getPrevEdge()->getPairEdge()->getPrevEdge()->getPairEdge()->getPrevEdge()->getOriginVertex())
					{
						edge1new1 = edges2[j];						
					}

				}
			}
			for (int j = 0; j < edges2.size(); j++)
			{
				if (edge1new1->getPairEdge()->getOriginVertex() == edges2[j]->getOriginVertex() && edges2[j]->getPairEdge()->getOriginVertex() == edge1->getPairEdge()->getOriginVertex())
				{
					edge1new2 = edges2[j];
					middle1 = edge1new2->getOriginVertex();
				}
			}
		}
		else
		{
			HEdge* edge1new3; HEdge* edge1new4;
			float x, y, z;

			x = (edge1->getOriginVertex()->getX() + edge2->getOriginVertex()->getX()) / 2;
			y = (edge1->getOriginVertex()->getY() + edge2->getOriginVertex()->getY()) / 2;
			z = (edge1->getOriginVertex()->getZ() + edge2->getOriginVertex()->getZ()) / 2;

			middle1 = new Vertex(x, y, z);
			middle1 = middle1->normalize(middle1);

			points.push_back(middle1);

			edge1new1 = new HEdge(edge1->getOriginVertex()); 
			edge1new2 = new HEdge(middle1);
			edge1new3 = new HEdge(middle1);
			edge1new4 = new HEdge(edge1->getPairEdge()->getOriginVertex());
			edge1new1->setPairEdge(edge1new3);
			edge1new3->setPairEdge(edge1new1);
			edge1new2->setPairEdge(edge1new4);
			edge1new4->setPairEdge(edge1new2);
			dividedEdges.push_back(edge1);
			dividedEdges.push_back(edge1->getPairEdge());
			edges2.push_back(edge1new1); edges2.push_back(edge1new2); edges2.push_back(edge1new3); edges2.push_back(edge1new4);
		}

		HEdge* edge2new1; HEdge* edge2new2; 

		if (edge2In)
		{
			for (int j = 0; j < edges2.size(); j++)
			{
				if (edge2->getOriginVertex() == edges2[j]->getOriginVertex() && edges2[j]->getPairEdge()->getNextEdge() != nullptr)
				{
					if (edge2->getPairEdge()->getOriginVertex() == edges2[j]->getPairEdge()->getPrevEdge()->getPairEdge()->getPrevEdge()->getPairEdge()->getPrevEdge()->getOriginVertex())
					{
						edge2new1 = edges2[j];
					}
				}
			}
			for (int j = 0; j < edges2.size(); j++)
			{
				if (edge2new1->getPairEdge()->getOriginVertex() == edges2[j]->getOriginVertex() && edges2[j]->getPairEdge()->getOriginVertex() == edge2->getPairEdge()->getOriginVertex())
				{
					edge2new2 = edges2[j];
					middle2 = edge2new2->getOriginVertex();
				}
			}
		}
		else
		{
			HEdge* edge2new3; HEdge* edge2new4;
			float x, y, z;

			x = (edge2->getOriginVertex()->getX() + edge3->getOriginVertex()->getX()) / 2;
			y = (edge2->getOriginVertex()->getY() + edge3->getOriginVertex()->getY()) / 2;
			z = (edge2->getOriginVertex()->getZ() + edge3->getOriginVertex()->getZ()) / 2;

			middle2 = new Vertex(x, y, z);
			middle2 = middle2->normalize(middle2);

			points.push_back(middle2);

			edge2new1 = new HEdge(edge2->getOriginVertex());
			edge2new2 = new HEdge(middle2);	
			edge2new3 = new HEdge(middle2);
			edge2new4 = new HEdge(edge2->getPairEdge()->getOriginVertex());

			edge2new1->setPairEdge(edge2new3);
			edge2new3->setPairEdge(edge2new1);
			edge2new2->setPairEdge(edge2new4);
			edge2new4->setPairEdge(edge2new2);
			dividedEdges.push_back(edge2);
			dividedEdges.push_back(edge2->getPairEdge());
			edges2.push_back(edge2new1); edges2.push_back(edge2new2); edges2.push_back(edge2new3); edges2.push_back(edge2new4);
		}

		HEdge* edge3new1; HEdge* edge3new2; 

		if (edge3In)
		{
			for (int j = 0; j < edges2.size(); j++)
			{
				if (edge3->getOriginVertex() == edges2[j]->getOriginVertex() && edges2[j]->getPairEdge()->getNextEdge() != nullptr)
				{
					if (edge3->getPairEdge()->getOriginVertex() == edges2[j]->getPairEdge()->getPrevEdge()->getPairEdge()->getPrevEdge()->getPairEdge()->getPrevEdge()->getOriginVertex())
					{
						edge3new1 = edges2[j];
					}
				}
			}
			for (int j = 0; j < edges2.size(); j++)
			{
				if (edge3new1->getPairEdge()->getOriginVertex() == edges2[j]->getOriginVertex() && edges2[j]->getPairEdge()->getOriginVertex() == edge3->getPairEdge()->getOriginVertex())
				{
					edge3new2 = edges2[j];
					middle3 = edge3new2->getOriginVertex();
				}
			}
		}
		else
		{
			HEdge* edge3new3; HEdge* edge3new4;
			float x, y, z;

			x = (edge3->getOriginVertex()->getX() + edge1->getOriginVertex()->getX()) / 2;
			y = (edge3->getOriginVertex()->getY() + edge1->getOriginVertex()->getY()) / 2;
			z = (edge3->getOriginVertex()->getZ() + edge1->getOriginVertex()->getZ()) / 2;

			middle3 = new Vertex(x, y, z);
			middle3 = middle3->normalize(middle3);

			points.push_back(middle3);

			edge3new1 = new HEdge(edge3->getOriginVertex());
			edge3new2 = new HEdge(middle3);	
			edge3new3 = new HEdge(middle3);
			edge3new4 = new HEdge(edge3->getPairEdge()->getOriginVertex());

			edge3new1->setPairEdge(edge3new3);
			edge3new3->setPairEdge(edge3new1);
			edge3new2->setPairEdge(edge3new4);
			edge3new4->setPairEdge(edge3new2);
			dividedEdges.push_back(edge3);
			dividedEdges.push_back(edge3->getPairEdge());
			edges2.push_back(edge3new1); edges2.push_back(edge3new2); edges2.push_back(edge3new3); edges2.push_back(edge3new4);
		}

		HEdge* middleEdge1; HEdge* middleEdge1Pair; HEdge* middleEdge2; HEdge* middleEdge2Pair; HEdge* middleEdge3; HEdge* middleEdge3Pair;
		
		middleEdge1 = new HEdge(middle1);
		middleEdge1Pair = new HEdge(middle2);
		middleEdge2 = new HEdge(middle2);
		middleEdge2Pair = new HEdge(middle3);
		middleEdge3 = new HEdge(middle3);
		middleEdge3Pair = new HEdge(middle1);

		Face* face4 = new Face(middleEdge2);

		middleEdge1->setPrevEdge(middleEdge3);
		middleEdge1->setNextEdge(middleEdge2);
		middleEdge1->setFace(face4);

		middleEdge2->setPrevEdge(middleEdge1);
		middleEdge2->setNextEdge(middleEdge3);
		middleEdge2->setFace(face4);

		middleEdge3->setPrevEdge(middleEdge2);
		middleEdge3->setNextEdge(middleEdge1);
		middleEdge3->setFace(face4);

		middleEdge1->setPairEdge(middleEdge1Pair);
		middleEdge1Pair->setPairEdge(middleEdge1);
		middleEdge2->setPairEdge(middleEdge2Pair);
		middleEdge2Pair->setPairEdge(middleEdge2);
		middleEdge3->setPairEdge(middleEdge3Pair);
		middleEdge3Pair->setPairEdge(middleEdge3);

		Face* face1 = new Face(edge1new1);
		edge1new1->setFace(face1);
		edge1new1->setPrevEdge(edge3new2);
		edge1new1->setNextEdge(middleEdge3Pair);

		edge3new2->setFace(face1);
		edge3new2->setNextEdge(edge1new1);
		edge3new2->setPrevEdge(middleEdge3Pair);

		middleEdge3Pair->setNextEdge(edge3new2);
		middleEdge3Pair->setPrevEdge(edge1new1);
		middleEdge3Pair->setFace(face1);

		Face* face2 = new Face(edge1new2);
		edge1new2->setNextEdge(edge2new1);
		edge1new2->setPrevEdge(middleEdge1Pair);
		edge1new2->setFace(face2);

		middleEdge1Pair->setNextEdge(edge1new2);
		middleEdge1Pair->setPrevEdge(edge2new1);
		middleEdge1Pair->setFace(face2);

		edge2new1->setNextEdge(middleEdge1Pair);
		edge2new1->setPrevEdge(edge1new2);
		edge2new1->setFace(face2);

		Face* face3 = new Face(middleEdge2Pair);
		middleEdge2Pair->setNextEdge(edge2new2);
		middleEdge2Pair->setFace(face3);
		middleEdge2Pair->setPrevEdge(edge3new1);

		edge3new1->setNextEdge(middleEdge2Pair);
		edge3new1->setFace(face3);
		edge3new1->setPrevEdge(edge2new2);

		edge2new2->setNextEdge(edge3new1);
		edge2new2->setPrevEdge(middleEdge2Pair);
		edge2new2->setFace(face3);

		edges2.push_back(middleEdge1); edges2.push_back(middleEdge1Pair);
		edges2.push_back(middleEdge2); edges2.push_back(middleEdge2Pair); 
		edges2.push_back(middleEdge3); edges2.push_back(middleEdge3Pair);

		faces2.push_back(face1); faces2.push_back(face2); faces2.push_back(face3); faces2.push_back(face4);
	}

	for (int i = 0; i < edges.size(); i++)
	{
		delete edges[i];
	}
	for (int i = 0; i < faces.size(); i++)
	{
		delete faces[i];
	}

	edges = edges2;
	faces = faces2;

	/*for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->printEdge();
		qDebug() << " ";
	}
	
	for (int i = 0; i < points.size(); i++)
	{
		points[i]->printVertex();
		qDebug() << " ";
	}*/
}

void ImageViewer::dealloc()
{
	for (int i = 0; i < points.size(); i++)
	{
		delete points[i];
	}
	for (int i = 0; i < edges.size(); i++)
	{
		delete edges[i];
	}
	for (int i = 0; i < faces.size(); i++)
	{
		delete faces[i];
	}
}

void ImageViewer::saveToVTK()
{
	for (int i = 0; i < points.size(); i++)
	{
		points[i]->printVertex();
		qDebug() << " ";
	}

	QString fileName = "NewSphere.vtk";

	QFile file(fileName);

	file.remove();

	if (file.open(QIODevice::WriteOnly)) {
		QTextStream out(&file);

		out << "# vtk DataFile Version 3.0" << "\n";
		out << "vtk output" << "\n";
		out << "ASCII" << "\n";
		out << "DATASET POLYDATA" << "\n";
		out << "POINTS " << points.size() << " float" << "\n";

		for (int i = 0; i < points.size(); i++)
		{
			out << points[i]->getVertex().x() << " " << points[i]->getVertex().y() << " " << points[i]->getVertex().z() << "\n";
		}

		out << "\n";
		out << "POLYGONS " << faces.size() << " " << faces.size() * 4 << "\n";

		for ( int i = 0; i < faces.size(); i++)
		{
			out << 3 << " " << getPointIndex(faces[i]->getEdge()->getOriginVertex()) << " " << getPointIndex(faces[i]->getEdge()->getNextEdge()->getOriginVertex()) << " " << getPointIndex(faces[i]->getEdge()->getPrevEdge()->getOriginVertex()) << "\n";
		}

		file.close();

		qDebug() << "new sphere in vtk file.";
		QMessageBox msgBox;
		msgBox.setText("Data did load to vtk file.");
		msgBox.exec();
	}
	else {
		qDebug() << "file did not open.";
		qDebug() << file.errorString();
		return;
	}
}

int ImageViewer::getPointIndex(Vertex* point)
{
	int index;

	for (int i = 0; i < points.size(); i++)
	{
		if (point == points[i])
		{
			return index = i;
		}
	}
}

void ImageViewer::on_pushButtonGenerate_clicked()
{
	qDebug() << "Generataring Sphere";
	int division = ui->spinBox->value();

	qDebug() << division;

	dealloc();
	points.clear();
	edges.clear();
	faces.clear();

	loadDataFromVTK();

	if (division > 0)
	{
		for (int i = 0; i < division; i++)
		{
			divide();
		}
	}

	saveToVTK();
}

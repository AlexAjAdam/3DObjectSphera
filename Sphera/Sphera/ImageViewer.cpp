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
		QMessageBox msgBox;
		msgBox.setText("Data did not load.");
		msgBox.exec();
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

		HEdge* edge1 = new HEdge; HEdge* edge2 = new HEdge; HEdge* edge3 = new HEdge;
		Face* face = new Face;

		edge1->setOriginVertex(points[first]);
		points[first]->setEdge(edge1);
		edge2->setOriginVertex(points[second]);
		points[second]->setEdge(edge2);
		edge3->setOriginVertex(points[third]);
		points[third]->setEdge(edge3);
		face->setEdge(edge1);

		edge1->setPrevEdge(edge3);
		edge1->setNextEdge(edge2);
		edge1->setFace(face);

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

	for (int i = 0; i < edges.size(); i++) //funguje iba pre octahedron
	{
		if (i == 0)
		{
			edges[0]->setPairEdge(edges[12]);
			edges[12]->setPairEdge(edges[0]);
		}
		if (i == 2) // o +/-8
		{
			edges[i]->setPairEdge(edges[i + 8]);
			edges[i + 8]->setPairEdge(edges[i]);
		}
		if (i == 1 || i == 4 || i == 7) // o +/-4
		{
			edges[i]->setPairEdge(edges[i + 4]);
			edges[i + 4]->setPairEdge(edges[i]);
		}
		if (i == 14 || i == 17 || i == 20) //o +/-2
		{
			edges[i]->setPairEdge(edges[i + 2]);
			edges[i + 2]->setPairEdge(edges[i]);
		}
		if (i == 3 || i == 6 || i == 9)// o +/-12
		{
			edges[i]->setPairEdge(edges[i + 12]);
			edges[i + 12]->setPairEdge(edges[i]);
		}
		if (i == 13) //o +/-10
		{
			edges[i]->setPairEdge(edges[i + 10]);
			edges[i + 10]->setPairEdge(edges[i]);
		}
	}

	/*for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->printEdge();
		qDebug() << " ";
	}*/
}

void ImageViewer::divide(int division)
{
	for (int i = 0; i < edges.size(); i++)
	{
		edges[i]->printEdge();
		qDebug() << " ";
	}
	for (int i = 0; i < points.size(); i++)
	{
		points[i]->printVertex();
		qDebug() << " ";
	}

	for (int j = 0; j < division; j++)
	{
		for (int i = 0; i < faces.size(); i++)
		{
			QVector<Vertex*> points2 = QVector<Vertex*>();
			QVector<HEdge*> edges2 = QVector<HEdge*>();
			QVector<Face*> faces2 = QVector<Face*>();

			QVector<Vertex*> middlePoints = QVector<Vertex*>(3);
			float x[3], y[3], z[3];

			x[0] = (faces[i]->getEdge()->getOriginVertex()->getX() + faces[i]->getEdge()->getNextEdge()->getOriginVertex()->getX()) / 2;
			x[1] = (faces[i]->getEdge()->getNextEdge()->getOriginVertex()->getX() + faces[i]->getEdge()->getPrevEdge()->getOriginVertex()->getX()) / 2;
			x[2] = (faces[i]->getEdge()->getPrevEdge()->getOriginVertex()->getX() + faces[i]->getEdge()->getOriginVertex()->getX()) / 2;

			y[0] = (faces[i]->getEdge()->getOriginVertex()->getY() + faces[i]->getEdge()->getNextEdge()->getOriginVertex()->getY()) / 2;
			y[1] = (faces[i]->getEdge()->getNextEdge()->getOriginVertex()->getY() + faces[i]->getEdge()->getPrevEdge()->getOriginVertex()->getY()) / 2;
			y[2] = (faces[i]->getEdge()->getPrevEdge()->getOriginVertex()->getY() + faces[i]->getEdge()->getOriginVertex()->getY()) / 2;

			z[0] = (faces[i]->getEdge()->getOriginVertex()->getZ() + faces[i]->getEdge()->getNextEdge()->getOriginVertex()->getZ()) / 2;
			z[1] = (faces[i]->getEdge()->getNextEdge()->getOriginVertex()->getZ() + faces[i]->getEdge()->getPrevEdge()->getOriginVertex()->getZ()) / 2;
			z[2] = (faces[i]->getEdge()->getPrevEdge()->getOriginVertex()->getZ() + faces[i]->getEdge()->getOriginVertex()->getZ()) / 2;

			Vertex* middle1 = new Vertex; Vertex* middle2 = new Vertex; Vertex* middle3 = new Vertex;
			middle1->setVertex(x[0], y[0], z[0]);
			middle2->setVertex(x[1], y[1], z[1]);
			middle3->setVertex(x[2], y[2], z[2]);

			points2.push_back(faces[i]->getEdge()->getOriginVertex());
			points2.push_back(middle1);
			points2.push_back(faces[i]->getEdge()->getNextEdge()->getOriginVertex());
			points2.push_back(middle2);
			points2.push_back(faces[i]->getEdge()->getPrevEdge()->getOriginVertex());
			points2.push_back(middle3);
			
			HEdge* edge1 = new HEdge; HEdge* edge2 = new HEdge; HEdge* edge3 = new HEdge; HEdge* edge4 = new HEdge; HEdge* edge5 = new HEdge; HEdge* edge6 = new HEdge; 
			HEdge* edge7 = new HEdge; HEdge* edge8 = new HEdge; HEdge* edge9 = new HEdge; HEdge* edge10 = new HEdge; HEdge* edge11 = new HEdge; HEdge* edge12 = new HEdge; 
			Face* face1 = new Face; Face* face2 = new Face; Face* face3 = new Face; Face* face4 = new Face; //delete niekde?

			edge1->setOriginVertex(points2[0]);
			edge2->setOriginVertex(points2[1]);
			edge3->setOriginVertex(points2[5]);
			edge4->setOriginVertex(points2[1]);
			edge5->setOriginVertex(points2[2]);
			edge6->setOriginVertex(points2[3]);
			edge7->setOriginVertex(points2[5]);
			edge8->setOriginVertex(points2[3]);
			edge9->setOriginVertex(points2[4]);
			edge10->setOriginVertex(points2[3]);
			edge11->setOriginVertex(points2[5]);
			edge12->setOriginVertex(points2[1]);

			edge1->setNextEdge(edge2);
			edge1->setPrevEdge(edge3);
			face1->setEdge(edge1);
			edge1->setFace(face1);
			edge2->setNextEdge(edge3);
			edge2->setPrevEdge(edge1);
			edge3->setNextEdge(edge1);
			edge3->setPrevEdge(edge2);

			edge4->setNextEdge(edge5);
			edge4->setPrevEdge(edge6);
			face2->setEdge(edge4);
			edge4->setFace(face2);
			edge5->setNextEdge(edge6);
			edge5->setPrevEdge(edge4);
			edge6->setNextEdge(edge4);
			edge6->setPrevEdge(edge5);

			edge7->setNextEdge(edge8);
			edge7->setPrevEdge(edge9);
			face3->setEdge(edge7);
			edge7->setFace(face3);
			edge8->setNextEdge(edge9);
			edge8->setPrevEdge(edge7);
			edge9->setNextEdge(edge7);
			edge9->setPrevEdge(edge8);

			edge10->setNextEdge(edge11);
			edge10->setPrevEdge(edge12);
			edge10->setPairEdge(edge7);
			face4->setEdge(edge10);
			edge10->setFace(face4);
			edge11->setNextEdge(edge12);
			edge11->setPrevEdge(edge10);
			edge11->setPairEdge(edge2);
			edge12->setNextEdge(edge10);
			edge12->setPrevEdge(edge11);
			edge12->setPairEdge(edge6);

			edges2.push_back(edge1); edges2.push_back(edge2); edges2.push_back(edge3); edges2.push_back(edge4); edges2.push_back(edge5); edges2.push_back(edge6); 
			edges2.push_back(edge7); edges2.push_back(edge8); edges2.push_back(edge9); edges2.push_back(edge10); edges2.push_back(edge11); edges2.push_back(edge12);

			faces2.push_back(face1); faces2.push_back(face2); faces2.push_back(face3); faces2.push_back(face4);

			for (int l = 0; l < faces2.size(); l++)
			{
				faces.push_back(faces2[l]);
			}
			faces.remove(i);
		}
	}
}


void ImageViewer::on_pushButtonGenerate_clicked()
{
	qDebug() << "Generataring Sphere";
	int division = ui->spinBox->value();

	loadDataFromVTK();
	divide(division);
}

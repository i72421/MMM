#include "MPic.h"
#include <QBitmap>
#include <QPainter>
#include <QPixmap>
#include <QMessageBox>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDropEvent>
#include <QMimeData>
#include <QClipboard>
#include <QDesktopWidget>
#include <QApplication>

MPic::MPic(QWidget *parent)
	: QWidget(parent)
{
	resize(80, 80);

	QDesktopWidget *desktop = QApplication::desktop();
	this->move(desktop->width() - this->width() - 100, 100);
	this->setAcceptDrops(true);

	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
	setWindowIcon(QIcon(QStringLiteral(":/MPic/Resource/MPic.ico")));

	QBitmap bmp(this->size());
	bmp.fill();

	QPainter p(&bmp);
	p.setPen(Qt::NoPen);
	p.setBrush(Qt::black);
	p.setRenderHint(QPainter::Antialiasing);
	p.drawRoundedRect(bmp.rect(), 15, 15);
	setMask(bmp);

	this->palette_normal.setBrush(QPalette::Background, QBrush(QPixmap(":/MPic/Resources/normal.png")));
	this->palette_upload.setBrush(QPalette::Background, QBrush(QPixmap(":/MPic/Resources/upload.png")));
	this->palette_ok.setBrush(QPalette::Background, QBrush(QPixmap(":/MPic/Resources/ok.png")));
	this->palette_error.setBrush(QPalette::Background, QBrush(QPixmap(":/MPic/Resources/error.png")));

	this->setPalette(this->palette_normal);

	QAction *url = new QAction("link", this);
	addAction(url);
	connect(url, SIGNAL(triggered()), this, SLOT(url()));

	QAction *exit = new QAction("exit", this);
	addAction(exit);
	connect(exit, SIGNAL(triggered()), this, SLOT(exit()));

	setContextMenuPolicy(Qt::ActionsContextMenu);
}

MPic::~MPic()
{

}

void MPic::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
		m_moving = false;
}

void MPic::mousePressEvent(QMouseEvent *event)
{
	if ((event->button() == Qt::LeftButton))
	{
		m_moving = true;
		m_offset = event->pos();
	}
	else
	{
		this->actions();
	}
}

void MPic::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (!ClipboardImage())
	{
		this->open();
	}

	if (path.length() != 0)
	{
		this->upload();
	}

	event->ignore();
}

bool MPic::ClipboardImage()
{
	QClipboard *board = QApplication::clipboard();
	QImage pic = board->image();

	board->clear();

	if (!pic.isNull())
	{
		QString strDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/MPic";
		QDir dir(strDir);
		if (!dir.exists())
		{
			dir.mkdir(strDir);
		}

		QString filename = strDir + "/ScreenShot" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") + ".png";
		pic.save(filename);

		path = filename;

		return true;
	}

	return false;
}

void MPic::mouseMoveEvent(QMouseEvent *event)
{
	//方法1：
	QDesktopWidget* desktop = QApplication::desktop();
	QRect windowRect(desktop->availableGeometry());
	QRect widgetRect(this->geometry());
	QPoint point(event->globalPos() - m_offset);

	//以下是防止窗口拖出可见范围外
	//左边
	if (point.x() <= 0)
	{
		point = QPoint(0, point.y());
	}
	//右边
	int y = windowRect.bottomRight().y() - this->size().height();
	if (point.y() >= y && widgetRect.topLeft().y() >= y)
	{
		point = QPoint(point.x(), y);
	}
	//上边
	if (point.y() <= 0)
	{
		point = QPoint(point.x(), 0);
	}
	//下边
	int x = windowRect.bottomRight().x() - this->size().width();
	if (point.x() >= x && widgetRect.topLeft().x() >= x)
	{
		point = QPoint(x, point.y());
	}
	move(point);
	//    move(event->globalPos() - m_offset);
}

void MPic::open()
{
	path = QFileDialog::getOpenFileName(this, tr("Open Image"), ".", tr("Image Files(*.jpg *.png *.gif *.bmp *.jpeg)"));
}

void MPic::dropEvent(QDropEvent *event)
{
	auto list = event->mimeData()->urls();
	path = list[0].toLocalFile();

	QString filename = QFileInfo(path).fileName();
	if (filename.contains(".jpg") || filename.contains(".png") || filename.contains(".bmp") || filename.contains(".jpeg") || filename.contains(".gif"))
	{
		this->upload();
	}
	else
	{
		this->setPalette(palette_error);
		this->repaint();
		sleep(1);
		this->setPalette(palette_normal);
	}
}

void MPic::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasUrls())
	{
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}
}

void MPic::dragMovedEvent(QDragMoveEvent *event)
{
	if (event->mimeData()->hasUrls())
	{
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}
}

void MPic::exit()
{
	QCoreApplication::quit();
}

void MPic::url()
{
	QUrl Url("https://sm.ms/");
	QDesktopServices::openUrl(Url);
}

void MPic::upload()
{
	this->setPalette(palette_upload);
	QUrl url("https://sm.ms/api/upload");
	QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);


	QHttpPart imagePart;
	imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
	imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data;name=\"smfile\";filename=\"" + path + "\""));
	QFile *file = new QFile(path);
	file->open(QIODevice::ReadOnly);
	imagePart.setBodyDevice(file);

	file->setParent(multiPart); // we cannot delete the file now, so delete it with the multiPart
	multiPart->append(imagePart);

	request.setUrl(url);

	accessManager = new QNetworkAccessManager();

	reply = accessManager->post(request, multiPart);
	connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

	multiPart->setParent(reply);

}

void MPic::replyFinished(QNetworkReply *reply)
{
	int error_flag;

	QClipboard *board = QApplication::clipboard();//获取系统剪贴板
	//无错误返回
	if (reply->error() == QNetworkReply::NoError)
	{
		QString result;
		QByteArray bytes = reply->readAll();
		QJsonDocument json = QJsonDocument::fromJson(bytes, NULL);
		if (json.isObject())
		{
			QJsonObject obj = json.object();

			if (obj.contains("data"))
			{
				QJsonValue value = obj.value("data");
				if (value.isObject())
				{
					QJsonObject obj = value.toObject();
					if (obj.contains("url"))
					{
						QJsonValue value = obj.value("url");
						result = "![](" + value.toString() + ")";
						error_flag = 0;
					}
				}

			}
			if (obj.contains("msg"))//错误信息
			{
				result = obj.value("msg").toString();
				error_flag = 1;
			}
		}

		board->setText(result);
	}
	else
	{
		board->setText(reply->errorString());
		error_flag = 1;
	}

	reply->deleteLater();//要删除reply，但是不能在replyfinished里直接delete，要调用deletelater;

	if (error_flag)
	{
		this->setPalette(palette_error);
	}
	else
	{
		this->setPalette(palette_ok);
	}
	this->repaint();
	sleep(1);
	this->setPalette(palette_normal);
}

void MPic::sleep(int s)
{
	QDateTime n2 = QDateTime::currentDateTime();

	QDateTime now;

	do{
		now = QDateTime::currentDateTime();
	} while (n2.secsTo(now) <= s);  //s为需要延时的秒数
}
#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <QAction>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QHttpPart>
//#include "ui_MPic.h"

class MPic : public QWidget
{
	Q_OBJECT

public:
	MPic(QWidget *parent = Q_NULLPTR);
	~MPic();

	void mousePressEvent(QMouseEvent* event);  // 鼠标按下
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseDoubleClickEvent(QMouseEvent* event);

	void sleep(int s);

	void open();
	bool ClipboardImage();
	void dropEvent(QDropEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMovedEvent(QDragMoveEvent *event);

private:
	bool m_moving;//用来标记是否鼠标移动
	QPoint m_offset;

	QString path;
	QFile *file;
	QNetworkAccessManager *accessManager;
	QNetworkRequest request;
	QNetworkReply *reply;

	QPalette palette_normal;
	QPalette palette_upload;
	QPalette palette_ok;
	QPalette palette_error;

	private slots:
	void exit();
	void url();

	void upload();
	void replyFinished(QNetworkReply*reply);
};
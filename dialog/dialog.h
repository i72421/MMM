#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include <QAction>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QHttpPart>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

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
    Ui::Dialog *ui;

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


#endif // DIALOG_H

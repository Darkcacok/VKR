#ifndef RECORDFORM_H
#define RECORDFORM_H

#include <QDialog>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>

#include "senddata.h"
#include "isofs.h"
#include "burn.h"

class RecordForm : public QDialog
{
    Q_OBJECT
public:
    explicit RecordForm(QWidget *parent = nullptr);
    ~RecordForm();

    void recordImgae(InfoForRecord ifr);

private:
    /***************Layout*************/
    QBoxLayout *v_layout;

    QProgressBar *progressBar;
    QLabel *status;

    /********Functions*********/
    void createWindow();


signals:
    void sendData(InfoForRecord ifr);
    void setValue(int);
    void setText(QString);
    void sendRez(QString status);

public slots:
    void recieveData(InfoForRecord ifr);
    void recieveRez(QString status);
};

#endif // RECORDFORM_H

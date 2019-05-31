#ifndef RECORDFORM_H
#define RECORDFORM_H

#include <QWidget>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QLabel>


#include "senddata.h"
#include "isofs.h"

class RecordForm : public QWidget
{
    Q_OBJECT
public:
    explicit RecordForm(QWidget *parent = nullptr);

private:
    /***************Layout*************/
    QBoxLayout *v_layout;

    QProgressBar *progressBar;
    QLabel *status;

    /********Functions*********/
    void createWindow();

    //static void updateProgress(float persent)



signals:
    void sendData(InfoForRecord *ifr);
    void emitUpdateProgress(float persent);

public slots:
    void recieveData(InfoForRecord *ifr);
};

#endif // RECORDFORM_H

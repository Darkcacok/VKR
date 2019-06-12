#include "recordform.h"

RecordForm::RecordForm(QWidget *parent) : QDialog(parent)
{
    createWindow();
    connect(this, SIGNAL(setValue(int)), progressBar, SLOT(setValue(int)));
    connect(this, SIGNAL(setText(QString)), status, SLOT(setText(QString)));
}

RecordForm::~RecordForm()
{

}

void RecordForm::createWindow()
{
    v_layout = new QVBoxLayout();

    status = new QLabel("Статус");
    status->setAlignment(Qt::AlignHCenter);

    progressBar = new QProgressBar();

    v_layout->addWidget(progressBar);
    v_layout->addWidget(status);

    this->setWindowTitle("Запись Образа");
    this->setLayout(v_layout);
}

void RecordForm::recordImgae(InfoForRecord ifr)
{
    int ret;
    int stages;
    int currentStage = 1;
    Burn *burn = new Burn();

    if(ifr.discPath.empty())
        stages = 1;
    else
        stages = 2;

    if(stages == 2)
    {
        if(burn->driveScanAndGrab(ifr.discPath) < 0)
            return;
    }

    emit setText("Создание Образа (" + QString::number(currentStage) + "/" + QString::number(stages) + ")");

    IsoFS * isoFs = new IsoFS();
    isoFs->setExtnsnRockRidge();
    isoFs->CreateImage(ifr.dir, ifr.imgName);
    ret = isoFs->writeImage(ifr.isoPath, [=](float p){
        emit setValue(p);
    });

    if(ret < 0)
    {
        QMessageBox::information(this, "Ошибка", isoFs->getLastError().c_str());
        close();
    }

    if(stages == 2 && ret > 0)
    {
        emit setText("Запись образа на диск (" + QString::number(++currentStage) + "/" + QString::number(stages) + ")");
        emit setValue(0);
        ret = burn->writeIso(ifr.isoPath, [=](float p){
            emit setValue(p);
        });
    }


    if(stages == 2)
    {
        burn->~Burn();
    }
    if(ret > 0)
    {
        QMessageBox::information(this, "Успех", "Образ успешно записан");
    }
    else
    {
        QMessageBox::information(this, "Ошибка", burn->getLastError().c_str());
    }

    close();
}


void RecordForm::recieveData(InfoForRecord ifr)
{
    QtConcurrent::run(this, &RecordForm::recordImgae, ifr);
}

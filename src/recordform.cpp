#include "recordform.h"

RecordForm::RecordForm(QWidget *parent) : QWidget(parent)
{
    createWindow();
}

void RecordForm::createWindow()
{
    v_layout = new QVBoxLayout();
    status = new QLabel("Статус");
    status->setAlignment(Qt::AlignHCenter);

    progressBar = new QProgressBar();
    //connect(progressBar, SIGNAL(updateProgress(float)), this, SLOT(sevValue(int)));

    v_layout->addWidget(progressBar);
    v_layout->addWidget(status);

    this->setWindowTitle("Запись Образа");
    this->setLayout(v_layout);
}


void RecordForm::recieveData(InfoForRecord *ifr)
{
    int stages;
    int currentStage = 0;

    if(ifr->discPath.empty())
        stages = 1;
    else
        stages = 2;

    status->setText("Создание Образа (" + QString::number(currentStage) + "/" + QString::number(stages) + ")");

    IsoFS * isoFs = new IsoFS();
    isoFs->setExtnsnRockRidge();
    isoFs->CreateImage(ifr->dir, ifr->imgName);
    isoFs->writeImage(ifr->isoPath, [this](float p){
        progressBar->setValue(p);
    });

    status->setText("Создание Образа (" + QString::number(++currentStage) + "/" + QString::number(stages) + ")");
}
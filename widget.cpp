#include "widget.h"
#include "ui_widget.h"
#include "algorithm"
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //ui->ResultLabel.
    s = new QGraphicsScene;
    ui->resultLabel->setVisible(false);
    ui->toInt->setChecked(true);
    ui->tabWidget->setTabText(0,"Создание видео");
    ui->tabWidget->setTabText(1,"Предпросмотр сетки");
    ui->render->setEnabled(false);
    ui->outputPath->setText(QDir::currentPath());
}

Widget::~Widget()
{
    delete ui;
}





void Widget::on_loadCSVModel_clicked()
{

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open CSV with data"), "", tr("CSV Files (*.csv)"));



    //Очистка
    data.clear();
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(0);
    //
    data =    Data::makeFramesFromCSV(fileName,ui->separator->text().at(0).toLatin1());
    if(data.size() > 0)//Если все ок повторно загружаем в таблицу
    {
        ui->tableWidget->setColumnCount(data.size());
        ui->rowsSize->setText(QString::number(data.first().data.size() > 10 ? 10 : data.first().data.size()));
        ui->tableWidget->setRowCount(data.first().data.size()+1);

        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            int rowId = 0;
            while (!file.atEnd()) {
                QByteArray line = file.readLine();
                auto row = line.split(ui->separator->text().at(0).toLatin1());
                for(int i = 0;i< row.size();i++)
                    ui->tableWidget->setItem(rowId,i,new QTableWidgetItem(QString(row[i])));

                rowId++;

            }
            file.close();
        }
        ui->resultLabel->setText("<span style=\"color:#008000;\">CSV файл успешно загружен!</span>");
        ui->render->setEnabled(true);
    }
    else
    {
        ui->resultLabel->setText("<span style=\" color:#ff0000;\">CSV файл не подходит для импорта!</span>");
        ui->render->setEnabled(false);
    }


    ui->resultLabel->setVisible(true);

}

void Widget::on_render_clicked()
{

    QFile::remove(ui->outputPath->text()+"/"+ui->outputName->text()+".webm");//удаление предыдущего файла если есть
    QFile::remove(ui->outputPath->text()+"/"+ui->outputName->text()+"_audio.webm");//c аудиодорожкой тоже самое если есть
    deletePNGinFolder(ui->outputPath->text());//очистка картинок в директории перед стартом
    ui->ffmpegoutput->clear();
    ui->resultLabel->setText("Интерполяция значений....");
    ui->resultLabel->repaint();
    QVector<ColumnPart> interpolData;
    if(ui->interpolation->text().toUInt() != 0) interpolData = Data::sliceFrameData(data,ui->interpolation->text().toUInt());

    unsigned height =ui->resolution->currentIndex() == 0 ? 360: ui->resolution->currentIndex() == 1 ?720 : ui->resolution->currentIndex() == 2 ? 1440 : 2160;
    unsigned width =ui->resolution->currentIndex() == 0 ? 640: ui->resolution->currentIndex() == 1 ?1280 : ui->resolution->currentIndex() == 2 ? 2560 : 3840;
    QGraphicsScene * scene = new QGraphicsScene(0,0,width,height);

    ui->resultLabel->setText("Рендеринг кадров....");
    ui->resultLabel->repaint();
    if(ui->mt->isChecked())
        Painter::paintFramesMT((ui->interpolation->text().toUInt() != 0) ? interpolData : data,ui->rowsSize->text().toUInt(),ui->outputPath->text()+"/",scene,ui->toInt->isChecked());
    else
        Painter::paintFrames((ui->interpolation->text().toUInt() != 0) ? interpolData : data,ui->rowsSize->text().toUInt(),ui->outputPath->text()+"/",scene,ui->toInt->isChecked());
    //Painter::paintFrames(data,55,"/home/alab/frames/",scene,ui->toInt->isChecked());
    ui->resultLabel->setText("Склейка....");
    ui->resultLabel->repaint();
    QThread::sleep(1);
    QProcess process;
    QString exe = "ffmpeg -f image2 -i "+ui->outputPath->text()+"/%d.png -r 60 "+ui->outputPath->text()+"/"+ui->outputName->text()+".webm";
    process.start(exe);
    process.waitForFinished(uint(-1));
    QString output = process.readAllStandardError();
    ui->ffmpegoutput->setText(output);

    if(ui->deleteAfterRend->isChecked())
    {
        deletePNGinFolder(ui->outputPath->text());

    }
    if(ui->audioPath->text() != "")
    {



        process.start("ffmpeg -i "+ui->outputPath->text()+"/"+ui->outputName->text()+".webm"+" -i "+ui->audioPath->text()+" -filter_complex \" [1:0] apad \" -shortest "+ui->outputPath->text()+"/"+ui->outputName->text()+"_audio.webm");
        process.waitForFinished(uint(-1));
        ui->ffmpegoutput->setText(ui->ffmpegoutput->toPlainText()+process.readAllStandardError());

    }
    interpolData.clear();
    ui->resultLabel->setText("Готово. Смотрите ответ ffmpeg.");

}

void Widget::on_selectFolder_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    ui->outputPath->setText(dir);
}

void Widget::on_selectAudio_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open WAV"), "", tr("WAV Files (*.wav)"));

    ui->audioPath->setText(fileName);
}

void Widget::on_tabWidget_currentChanged(int index)
{

    s->clear();

    float maxValue = 0;
    //поиск максимального значения
    for(auto  c: data)
    {
        for(auto r:c.data)
        {
            if(r.value>maxValue)
                maxValue = r.value;
        }

    }

    if(index == 1 && data.size() > 0)
    {
        s->setSceneRect(0,0,ui->graphicsView->width(),ui->graphicsView->height());
        ui->graphicsView->setScene(s);
        Painter::paintFrame(data[0],ui->rowsSize->text().toUInt(),"",s,maxValue,ui->toInt->isChecked(),false);

    }
}

void Widget::deletePNGinFolder(QString path)
{
    QDir dir(path);

    dir.setFilter(QDir::Files);
    QFileInfoList list = dir.entryInfoList();
    for(auto f:list)
    {
        if (f.filePath().contains(".png")) QFile::remove(f.filePath());

    }
}

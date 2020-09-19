#include "Painter.h"

Painter::Painter(QObject *parent) : QObject(parent)
{

}

void Painter::paintFrame(ColumnPart &data, unsigned rowsDisplay, QString path, QGraphicsScene *scene, float maxValue, bool toReal,bool save)
{

    //Сортировка и поиск максимального значения во всех фреймах для градуировки шкалы

    unsigned maxItemNameHeightId = 0;
    //Сортировка по убыванию
    std::sort(data.data.begin(),data.data.end(),[](const DataPart& l, const DataPart& r){
        return l.value > r.value;});
    //Поиск максимального значения и максимально длинного значения строки
    for(int i = 0; i < data.data.size();i++)
    {
        if (data.data[i].name.size() > data.data[maxItemNameHeightId].name.size())
            maxItemNameHeightId = i;
    }



    //Значения разделителей
    float leftSpacing = scene->width()/15;
    float topSpacing = scene->height()/(rowsDisplay+2);
    float upSpacing = scene->height()/20;
    float spacingBetweenNameAndGraph = scene->width()/20;

    //Добавляем белый фон
    QGraphicsRectItem * background = scene->addRect(0,0,scene->width(),scene->height(),QPen(Qt::white),QBrush(Qt::white));

    //Первым делом выводим надписи
    QVector<QGraphicsTextItem *> texts;
    QVector<QGraphicsTextItem *> textsValues;
    QVector<QGraphicsRectItem *> rects;
    float textdivider = scene->height()/(rowsDisplay+2)/2;


    //Выводим градуировку
    float maxWidthText = scene->width();

    while(maxWidthText > scene->width()/3)
    {
        textdivider /= 1.5;
        QGraphicsTextItem * t = scene->addText(data.data.at(maxItemNameHeightId).name,QFont("Times", textdivider));
        maxWidthText = t->boundingRect().width();
        scene->removeItem(t);
        delete t;
    }
    for(int i = 0; i < rowsDisplay;i++)
    {

        QGraphicsTextItem * text =  scene->addText(data.data.at(i).name,QFont("Times", textdivider));
        texts.push_back(text); //Сохраняем для полученных значений
        text->setPos(leftSpacing,upSpacing+topSpacing+i*topSpacing);
    }

    QGraphicsTextItem * minValueGrad = scene->addText("0",QFont("Times", textdivider, QFont::Bold));
    minValueGrad->setPos(leftSpacing+spacingBetweenNameAndGraph+maxWidthText,upSpacing);

    QGraphicsTextItem * maxValueGrad = scene->addText(QString::number(maxValue),QFont("Times", textdivider));
    maxValueGrad->setPos(scene->width() - leftSpacing - maxValueGrad->boundingRect().width(),upSpacing);
    //Запоминаем позиции для пропорционального построения
    float minPos = minValueGrad->pos().x()+minValueGrad->boundingRect().width()/2;
    float maxPos = maxValueGrad->pos().x()+maxValueGrad->boundingRect().width()/2;
    float graduate = maxPos - minPos;
    //Строим столбец
    for(int i = 0; i < rowsDisplay;i++)
    {
        QGraphicsRectItem * rect = scene->addRect(0,0,fabs(data.data.at(i).value)*graduate/maxValue,texts.at(i)->boundingRect().height(),QPen(data.data.at(i).color),QBrush(data.data.at(i).color));
        rects.push_back(rect);
        rect->setPos(minPos,texts.at(i)->pos().y());
    }
    //К каждому столбцу вконец добавляем численное значение
    for(int i = 0; i < rowsDisplay;i++)
    {
        //QGraphicsRectItem * rect = scene->addRect(0,0,data.data.at(i).value*graduate/maxValue,texts.at(i)->boundingRect().height()-topSpacing/6,QPen(data.data.at(i).color),QBrush(data.data.at(i).color));
        QGraphicsTextItem * textValue =  scene->addText(QString::number(toReal ? (int)data.data.at(i).value:data.data.at(i).value),QFont("Times", textdivider));
        textValue->setPos(minPos+ rects.at(i)->boundingRect().width()  ,texts.at(i)->pos().y());
        textsValues.push_back(textValue);
    }

    //    QGraphicsRectItem * recttest = scene->addRect(0,0,minValueGrad->boundingRect().width(),minValueGrad->boundingRect().height());
    //    recttest->setPos(minValueGrad->pos().x(),minValueGrad->pos().y());

    //Добавление иконок
    float imageMaxWidth = 0;
    for(int i = 0; i < rowsDisplay;i++)
    {
        if( data.data.at(i).icon.isNull()) continue;
        QImage image = data.data.at(i).icon;

        image = image.scaledToHeight(rects.at(i)->boundingRect().height());
        QGraphicsPixmapItem * px = scene->addPixmap(QPixmap::fromImage(image));
        if(image.width() < rects.at(i)->boundingRect().width())
        {

            px->setPos(minPos,texts.at(i)->pos().y());
        }
        else
        {

            px->setPos(textsValues.at(i)->pos().x()+textsValues.at(i)->boundingRect().width(),texts.at(i)->pos().y());
        }

    }
    //Добавление вниз названия фрейма(как правило год)
    QGraphicsTextItem * text =  scene->addText(data.columname,QFont("Arial", scene->height()/(rowsDisplay+2)/2));//добавить пропорциональный расчет фонта
    texts.push_back(text);
    text->setPos(leftSpacing/4,upSpacing/4);

    if (save)
    {
        QImage image(scene->width(), scene->height(), QImage::Format_ARGB32_Premultiplied);
        QPainter painter(&image);
        scene->render(&painter);
        image.save(path);
    }
}

void Painter::paintFrames(QVector<ColumnPart> &data, unsigned rowsDisplay,QString path, QGraphicsScene *scene,bool toReal )
{
    unsigned int i =0;
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
    for(auto & col:data)
    {
        paintFrame(col,rowsDisplay,path+QString::number(i)+".png",scene,maxValue,toReal);

        scene->clear();
        i++;
    }
}

void Painter::paintFramesMT(QVector<ColumnPart> &data, unsigned rowsDisplay, QString path, QGraphicsScene *scene, bool toReal)
{


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
    //стилистически так некорректно на Qt парралелить, нужно через qtconcurrent,а не в стилистике std::thread
    unsigned hwt = QThread::idealThreadCount();
    QVector<QThread*> threads(hwt);
    unsigned sizePerThread = data.size()/hwt;
    for(int i = 0;i < hwt;i++)
    {
        threads[i] = QThread::create([i,&data,&path,&rowsDisplay,&sizePerThread,&maxValue,&toReal,&scene,&hwt](){

            QGraphicsScene * s = new QGraphicsScene();
            s->setSceneRect(0,0,scene->width(),scene->height());


            for(int j= 0 +i*sizePerThread; j < (i == hwt-1 ? data.size() :sizePerThread*(i+1)) ;j++)
            {

                paintFrame(data[j],rowsDisplay,path+QString::number(j)+".png",s,maxValue,toReal);
                scene->clear();

            }
            delete s;
            ;});



    }
    for(auto &t:threads)
    {
        t->start();
    }
    for(auto &t:threads)
    {
        t->wait();//aka std::thread.join()
    }
    for(auto &t:threads)
    {
        delete t;
    }



}

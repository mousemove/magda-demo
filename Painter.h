#ifndef PAINTER_H
#define PAINTER_H

#include <QObject>
#include <QtWidgets>
#include <QDebug>
#include <thread>
#include <vector>
#include <QFuture>
#include "Data.h"
class Painter : public QObject
{
    Q_OBJECT
public:
    explicit Painter(QObject *parent = nullptr);
    static void paintFrame(ColumnPart &data, unsigned rowsDisplay, QString path,QGraphicsScene * scene,float maxValue,bool toInt = false);
    static void paintFrames(QVector<ColumnPart> &data, unsigned rowsDisplay, QString path,QGraphicsScene * scene,bool toReal = false);
    static void paintFramesMT(QVector<ColumnPart> &data, unsigned rowsDisplay, QString path, QGraphicsScene * scene, bool toReal = false);
signals:

};

#endif // PAINTER_H

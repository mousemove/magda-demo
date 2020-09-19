#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "Data.h"
#include <QtWidgets>
#include <QDebug>
#include "Painter.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT
private:
    QVector<ColumnPart> data;
    QGraphicsScene * s ;
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:



    void on_loadCSVModel_clicked();

    void on_render_clicked();

    void on_selectFolder_clicked();

    void on_selectAudio_clicked();

    void on_tabWidget_currentChanged(int index);
    void deletePNGinFolder(QString path);

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H

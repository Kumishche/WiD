#ifndef NOTE_H
#define NOTE_H

#include <QTimer>
#include <QDialog>
#include <QScrollBar>
#include <QFile>
#include <QMessageBox>
#include <QShortcut>
//#include "mainwindow.h"

namespace Ui {
class Note;
}

class Note : public QDialog
{
    Q_OBJECT

public:
    explicit Note(QWidget *parent = nullptr);
    ~Note();

    void getText(int i);    // - открывание заметки (i - номер заметки)
    int newText(int i);     // - создание заметки (i - номер заметки)

private:
    const QString d_theme = "(1, 31, 43)";
    const QString l_theme = "(242, 231, 225)";

    Ui::Note *ui;

    // ТАЙМЕРЫ
    QTimer timer;   // - цикл loop

    // ШРИФТЫ
    QFont f;        // - запись шрифта заметки;

    // ПЕРЕМЕННЫЕ
    int k = -1;                 // - номер заметки
    int r = 0, g = 0, b = 0;    // - цвета
    int c1 = 0;                 // - номер цвета выбранной пользователем темы
    int w = 0, h = 0;           // - высота и ширина окна
    int colors2[5][3] = {{212, 253, 217}, {203, 231, 254}, {255, 211, 225}, {219, 191, 254}, {166, 158, 154}};    // - цвета кнопок
    int colors3[5][3] = {{198, 182, 149}, {150, 150, 150}, {150, 211, 181}, {163, 175, 147}, {165, 140, 128}};    // - цвета кнопок
    QString colors[3] = {"(39, 40, 42)", "(255, 204, 102)", "(204, 0, 0)"};       // - цвета "звездочки"
    QString color = "";         // - выбранный по номеру "c1" цвет в виде строки
    QPushButton* btns[4] = {};  // - кнопки Ж К Ч З
    bool tf = true;

    QString theme;
    int cm = 0;

private slots:
    void OkNote();          // - сохранение заметки по нажатии кнопки Ok
    void changeFont();      // - изменение шрифта
    void changeStyle();     // - изменение стиля (Ж, К, Ч, З, размер шрифта)
    void resizeWidth();     // - измениние ui-элементов при изменении размеров окна
    void roleUp();          // - скрывание/раскрывание панели с настройками текста
    void colorChange();     // - изменение цвета "звездочки"
    void setList();
    void translation();
};

#endif // NOTE_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QFontDatabase>
#include <QWidget>
#include <QString>
#include <QSystemTrayIcon>
#include <QThread>
#include <QShortcut>
#include "note.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    const QString d_theme = "(1, 31, 43)";
    const QString l_theme = "(242, 231, 225)";
    const QString d_theme_w = "(1, 24, 33)";
    const QString l_theme_w = "(214, 204, 199)";

    Ui::MainWindow *ui;

    // ФУНКЦИИ
    void MoveNote(int t);               // - сортировка заметок по новизне (t - номер выбранной заметки)
    void ResizeLayout();                // - изменение формата отображения заметок при изменении размера окна (x - номер заметки, начиная с которой надо переделать Layout)
    void ColorSelect(QString &s);       // - определение цвета темы (s - переменная, в которую записывается выбранный цвет)
    void ColorSelect_Main();
    void ColorSelect_2(int c1, int t);  // - определение цвета "звездочки" (c1 - номер цвета, t - номер "звездочки")
    void Initialization(int t);         // - создание нового QWidget (t - номер заметки)

    // ШРИФТЫ
    QFont f;      // - запись шрифта заметки;
    QFont f1;     // - основной шрифт приложения (Comfortaa)

    // ТАЙМЕРЫ
    QTimer timer; // - создание цикла loop

    // ПЕРЕМЕННЫЕ
    int c = 0;     // - номер цвета выбранной пользователем темы
    int cm = 0;
    int c1 = 0;    // - номер цвета "звездочки"
    int k = 0;     // - кол-во добавленных заметок
    int R = 110;   // - размер кнопки AddButton
    int R1 = 20;   // - размер кнопки color_btn
    int w = 0;     // - ширина окна (для ф-ии ResizeWidth)
    int h = 0;     // - высота окна (для ф-ии ResizeWidth)
    int q = 12;    // - размер шрифта при считывании из файла
    int btns[4] = {0, 0, 0, 0}; // - кнопки Ж К Ч З
    QString color = "";         // - выбранный по номеру "c" цвет в виде строки
    QString tc = d_theme;
    QString tc_w = l_theme_w;

    int search = 0;

private slots:
    void newNote();            // - создание новой заметки
    void openNote();           // - открывание заметки
    void deleteNote();         // - удаление заметки
    void ColorChange();        // - изменение цвета темы
    void ColorChange_Main();
    void ResizeWidth();        // - измениние ui-элементов при изменении размеров окна
    void ColorChange_Star();   // - изменение цвета "звездочки"
    void OpenSearchText();
};
#endif // MAINWINDOW_H

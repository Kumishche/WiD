#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // иконка приложения
//    setWindowIcon(QIcon("C:/Users/vinro/OneDrive/Рабочий стол/Icon_2.0.ico"));

    // update функция, сделанная через таймер
    connect(&timer, SIGNAL(timeout()), this, SLOT(ResizeWidth()));
    timer.start(50);

    // считывание кол-ва заметок и цвета
    QFile file("num.txt");

    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);

        k = in.readLine().toInt();
        c = in.readLine().toInt();
        cm = in.readLine().toInt();
    }
    else
        QMessageBox::critical(this, "Error", "file(r): num.txt \n MainWindow()", QMessageBox::Close);
    file.close();

    // инициализация
    int id = QFontDatabase::addApplicationFont("Comfortaa.ttf"); // путь к шрифту
    QString family = QFontDatabase::applicationFontFamilies(id).at(0); // имя шрифта
    f1.setFamily(family);  
    f1.setPointSize(11);

    // назначение цвета
    ColorSelect(color);

    ui->scrollArea->verticalScrollBar()->setStyleSheet(color);


    // создание всех добавленных пользователем заметок
    for (int i = 0; i < k; i++)
    {
        Initialization(i+1);

        findChild<QLabel*>("text_" + QString::number(i+1))->clear();
        findChild<QLabel*>("title_" + QString::number(i+1))->clear();

        QFile file("note_" + QString::number(i+1) + ".txt");

        if (file.open(QIODevice::ReadOnly))
        {
            QTextStream in(&file);

            ColorSelect_2(in.readLine().toInt(), i+1);
            for (int i = 0; i < 6; i++)
                in.readLine();
            findChild<QLabel*>("title_" + QString::number(i+1))->setText(in.readLine());
            while (!in.atEnd())
               findChild<QLabel*>("text_" + QString::number(i+1))->setText(findChild<QLabel*>("text_" + QString::number(i+1))->text() + in.readLine() + "\n");
        }
        else
            QMessageBox::critical(this, "Error", "file(r): note_" + QString::number(i+1) + ".txt \n MainWindow()", QMessageBox::Close);
        file.close();

//        findChild<QLabel*>("title_" + QString::number(i+1))->setStyleSheet("color: rgb(1, 35, 48)");
//        findChild<QLabel*>("text_" + QString::number(i+1))->setStyleSheet("color: rgb(1, 35, 48)");
//        findChild<QPushButton*>("btn_" + QString::number(i+1))->setStyleSheet("color: rgb(1, 35, 48); \nborder-radius: 5px; \n" + color);
//        findChild<QPushButton*>("delete_btn_" + QString::number(i+1))->setStyleSheet("color: rgb(1, 35, 48/*1, 39, 54*/); \nborder-radius: 5px; \n" + color);
    }

    ColorSelect_Main();

    ResizeLayout();
    connect(ui->color_btn, SIGNAL(clicked()), this, SLOT(ColorChange()));
    connect(ui->color_btn_2, SIGNAL(clicked()), this, SLOT(ColorChange_Main()));
    connect(ui->AddButton, SIGNAL(clicked()), this, SLOT(newNote()));
    connect(new QShortcut(Qt::Key_1, this), SIGNAL(activated()), this, SLOT(ColorChange_Main()));
    connect(new QShortcut(Qt::Key_2, this), SIGNAL(activated()), this, SLOT(ColorChange()));
    connect(new QShortcut(Qt::CTRL | Qt::Key_F, this), SIGNAL(activated()), this, SLOT(OpenSearchText()));
    connect(new QShortcut(Qt::Key_Enter, this), SIGNAL(activated()), this, SLOT(OpenSearchText()));
    connect(ui->search_btn, SIGNAL(clicked()), this, SLOT(OpenSearchText()));

    ui->search_text->hide();
    ui->search_text->setStyleSheet("/*border-radius: 1px; \n */border-color: rgb(255, 0, 0); \n \
                                color: rgb" + tc + "; selection-" + color + " selection-color: rgb" + tc + ";");


}


MainWindow::~MainWindow()
{
    delete ui;
}


// создание заметки
void MainWindow::newNote()
{
        Note note;
        c1 = note.newText(k);   // создание заметки и запись цвета "звездочки"

        if (note.exec() == QDialog::Accepted)  // если успех - добавили заметку
            k++;
        else
            QMessageBox::critical(this, "Error", "note exec fail \n newNote()", QMessageBox::Close);

        Initialization(k);

        //ResizeLayout(k-1/*, false*/);   // распределение в Layout в зависимости от размера окна
        MoveNote(k-1);              // сдвиг заметок по новизне

        // запись кол-ва заметок и цвета
        QFile file("num.txt");
        QTextStream out(&file);

        if (file.open(QIODevice::WriteOnly))
        {
            out << k << Qt::endl;
            out << c << Qt::endl;
            out << cm << Qt::endl;
        }
        else
            QMessageBox::critical(this, "Error", "file(w): num.txt \n newNote()", QMessageBox::Close);
        file.close();
}


// открывание выбранной заметки
void MainWindow::openNote()
{
    for (int i = 0; i < k; i++)     // определение номера нажатой кнопки
    {
        if (sender() == findChild<QPushButton*>("btn_" + QString::number(i+1)))
        {
            Note note;
            note.getText(i);

            if (note.exec() == QDialog::Accepted)
                note.newText(i);
            else
                QMessageBox::critical(this, "Error", "open note fail \n openNote()", QMessageBox::Close);

            MoveNote(i);    // сдвиг заметок по новизне
            break;
        }
    }
}


// функция удаления заметки
void MainWindow::deleteNote()
{
    QMessageBox mb;     // предупреждение об удалении заметки
    mb.setText("Вы уверены, что хотите удалить заметку?");
    mb.addButton("Да", QMessageBox::AcceptRole);
    mb.addButton("Нет", QMessageBox::RejectRole);

    if (mb.exec() == QMessageBox::AcceptRole)   // если нажата кнопка "Да"
    {
        for (int i = 0; i < k; i++)     // от первой до последней заметки
        {
            if (sender() == findChild<QPushButton*>("delete_btn_" + QString::number(i+1)))       // нахождение номера заметки по номеру нажатой кнопки
            {
                ui->gridLayout_4->removeWidget(findChild<QWidget*>("widget_" + QString::number(i+1)));

                if (i != k-1)       // если выбрана последняя заметка
                {
                    for (int j = i; j < k-1; j++)       // от выбранной заметки до предпоследней перемещение текстов влево
                    {
                        findChild<QLabel*>("title_" + QString::number(j+1))->setText(findChild<QLabel*>("title_" + QString::number(j+2))->text());
                        findChild<QLabel*>("text_" + QString::number(j+1))->setText(findChild<QLabel*>("text_" + QString::number(j+2))->text());
                        findChild<QPushButton*>("star_btn_" + QString::number(j+1))->setStyleSheet(findChild<QPushButton*>("star_btn_" + QString::number(j+2))->styleSheet());
                    }

                    for (int w = i; w < k-1; w++)       // от выбранной заметки до предпосленей
                    {
                        QFile file("note_" + QString::number(w+2) + ".txt");     // запись в f шрифта следующей заметки
                        QTextStream in(&file);

                        if (file.open(QIODevice::ReadOnly))
                        {
                            c1 = in.readLine().toInt();
                            for (int i = 0; i < 4; i++)
                                btns[i] = in.readLine().toInt();
                            q = in.readLine().toInt();
                            f = in.readLine();
                            f.setPointSize(q);
                        }
                        else
                            QMessageBox::critical(this, "Error", "file(r): note_" + QString::number(w+2) + ".txt \n deleteNote()", QMessageBox::Close);
                        file.close();


                        QFile file1("note_" + QString::number(w+1) + ".txt");      // открывание выбранного файла и запись данных следующего справа
                        QTextStream out(&file1);

                        if (file1.open(QIODevice::WriteOnly))
                        {
                            out << c1 << Qt::endl;
                            for (int i = 0; i < 4; i++)
                                out << btns[i] << Qt::endl;
                            out << f.pointSize() << Qt::endl;
                            out << f.toString() << Qt::endl;
                            out << findChild<QLabel*>("title_" + QString::number(w+1))->text() << Qt::endl;
                            out << findChild<QLabel*>("text_" + QString::number(w+1))->text() << Qt::endl;
                        }
                        else
                            QMessageBox::critical(this, "Error", "file(w): note_" + QString::number(w+1) + ".txt \n deleteNote()", QMessageBox::Close);
                        file1.close();
                    }
                }

                findChild<QLabel*>("title_" + QString::number(k))->clear();
                findChild<QLabel*>("text_" + QString::number(k))->clear();
                findChild<QWidget*>("widget_" + QString::number(k))->deleteLater();

                QFile file1("note_" + QString::number(k) + ".txt");    // запись в последний файл NULL
                QTextStream out(&file1);

                if (file1.open(QIODevice::WriteOnly))
                {
                    out << "";
                }
                else
                    QMessageBox::critical(this, "Error", "file(w): note_" + QString::number(k) + ".txt \n deleteNote()", QMessageBox::Close);
                file1.close();
                file1.remove();     // удаление последнего файла

                k--;    // уменьшение кол-ва заметок

                QFile file2("num.txt");     // запись обновленного кол-ва заметок и номера цвета
                QTextStream out2(&file2);

                if (file2.open(QIODevice::WriteOnly))
                {
                    out2 << k << Qt::endl;
                    out2 << c << Qt::endl;
                    out2 << cm << Qt::endl;
                }
                else
                    QMessageBox::critical(this, "Error", "file(w): num.txt \n deleteNote()", QMessageBox::Close);
                file2.close();

                if (k != 0)
                    MoveNote(0);
                break;
            }
        }
    }
}


// функция для перемещения заметок влево (сортировка по новизне)
void MainWindow::MoveNote(int t)
{
    // доп. переменные для копирования файлов
    int btns2[] = {0, 0, 0, 0};  // кнопки Ж К Ч З
    int c2 = 0, c3 = -1;         // цвет
    bool tf = false;             // проверка на совпадение
    QFont f2;                    // шрифт


    QFile file("note_" + QString::number(t+1) + ".txt");   // открываем выбранный файл и считываем номер цвета "звездочки"
    QTextStream in(&file);

    if (file.open(QIODevice::ReadOnly))
        c2 = in.readLine().toInt();
    else
        QMessageBox::critical(this, "Error", "file(r): note_" + QString::number(t+1) + ".txt \n MoveNote()", QMessageBox::Close);
    file.close();


    for (int i = 0; i < k; i++) // цикл от первой до последней заметки
    {
        QFile file("note_" + QString::number(i+1) + ".txt");   // открываем первый файл и считываем данные
        QTextStream in(&file);

        if (file.open(QIODevice::ReadOnly))
            c1 = in.readLine().toInt();
        else
            QMessageBox::critical(this, "Error", "file(r): note_" + QString::number(i+1) + ".txt \n MoveNote()", QMessageBox::Close);
        file.close();

        if (i != k-1)
        {
            QFile file1("note_" + QString::number(i+2) + ".txt");   // открываем предыдущий файл, есои он есть и считываем данные
            QTextStream in1(&file1);

            if (file1.open(QIODevice::ReadOnly))
                c3 = in1.readLine().toInt();
            else
                QMessageBox::critical(this, "Error", "file(r): note_" + QString::number(i+2) + ".txt \n MoveNote()", QMessageBox::Close);
            file1.close();
        }
        else        // если i = 0 (предыдущего файла нет) - доп. цвет приравниваем к -1
            c3 = -1;


        if (c1 == c2 and c2 >= c3)  // если цвет выбранной заметки совпадает с цветом i-той заметки И цвет следующей заметки не меньше по номеру
        {
            if (t == i)     // если номер выбранной заметки совпадает с номер первой заметки, в которой встречается этот цвет (тогда нет смысла переставлять заметки местами)
            {
                QFile file("note_" + QString::number(t+1) + ".txt");   // открываем выбранный файл и считываем данные
                QTextStream in(&file);

                if (file.open(QIODevice::ReadOnly))
                {
                    findChild<QLabel*>("title_" + QString::number(t+1))->clear();
                    findChild<QLabel*>("text_"+ QString::number(t+1))->clear();

                    c1 = in.readLine().toInt();
                    ColorSelect_2(c1, t+1);
                    for (int i = 0; i < 6; i++)
                        in.readLine();
                    findChild<QLabel*>("title_"+ QString::number(t+1))->setText(in.readLine());
                    while (!in.atEnd())
                        findChild<QLabel*>("text_"+ QString::number(t+1))->setText(findChild<QLabel*>("text_"+ QString::number(t+1))->text() + in.readLine() + "\n");
                }
                else
                    QMessageBox::critical(this, "Error", "file(r): note_" + QString::number(t+1) + ".txt \n MoveNote()", QMessageBox::Close);
                file.close();
            }
            else if (t == i+1)   // если номер выбранной заметки на один больше номера первой заметки, в которой встречается этот цвет
            {
                QFile file("note_" + QString::number(i+1) + ".txt");   // открываем перввый файл, в котором такой же цвет, и считываем данные в доп. переменные
                QTextStream in(&file);

                if (file.open(QIODevice::ReadOnly))
                {
                   findChild<QLabel*>("title_" + QString::number(t+1))->clear();
                   findChild<QLabel*>("text_" + QString::number(t+1))->clear();

                   c2 = in.readLine().toInt();
                   ColorSelect_2(c2, t+1);
                   for (int i = 0; i < 4; i++)
                        btns2[i] = in.readLine().toInt();
                   q = in.readLine().toInt();
                   f2 = in.readLine();
                   f2.setPointSize(q);
                   findChild<QLabel*>("title_" + QString::number(t+1))->setText(in.readLine());
                   while (!in.atEnd())
                       findChild<QLabel*>("text_" + QString::number(t+1))->setText(findChild<QLabel*>("text_"  + QString::number(t+1))->text() + in.readLine() + "\n");
                }
                else
                    QMessageBox::critical(this, "Error", "file(r): note_"  + QString::number(i+1) + ".txt \n MoveNote()", QMessageBox::Close);
                file.close();

                QFile file3("note_" + QString::number(t+1) + ".txt");  // открываем выбранный файл и считываем данные в осн. переменные
                QTextStream in2(&file3);

                if (file3.open(QIODevice::ReadOnly))
                {
                   findChild<QLabel*>("title_" + QString::number(i+1))->clear();
                   findChild<QLabel*>("text_" + QString::number(i+1))->clear();

                   c1 = in2.readLine().toInt();
                   ColorSelect_2(c1, i+1);
                   for (int i = 0; i < 4; i++)
                        btns[i] = in2.readLine().toInt();
                   q = in2.readLine().toInt();
                   f = in2.readLine();
                   f.setPointSize(q);
                   findChild<QLabel*>("title_" + QString::number(i+1))->setText(in2.readLine());
                   while (!in2.atEnd())
                       findChild<QLabel*>("text_" + QString::number(i+1))->setText(findChild<QLabel*>("text_" + QString::number(i+1))->text() + in2.readLine() + "\n");
                }
                else
                    QMessageBox::critical(this, "Error", "file(r): note_" + QString::number(t+1) + ".txt \n MoveNote()", QMessageBox::Close);
                file3.close();

                QFile file4("note_" + QString::number(i+1) + ".txt");  // открываем певрый файл и записываем доп. переменные
                QTextStream out2(&file4);

                if (file4.open(QIODevice::WriteOnly))
                {
                    out2 << c1 << Qt::endl;
                    for (int i = 0; i < 4; i++)
                        out2 << btns[i] << Qt::endl;
                    out2 << f.pointSize() << Qt::endl;
                    out2 << f.toString() << Qt::endl;
                    out2 << findChild<QLabel*>("title_" + QString::number(i+1))->text() << Qt::endl;
                    out2 << findChild<QLabel*>("text_" + QString::number(i+1))->text() << Qt::endl;
                }
                else
                    QMessageBox::critical(this, "Error", "file(r): note_" + QString::number(i+1) + ".txt \n MoveNote()", QMessageBox::Close);
                file4.close();

                QFile file6("note_" + QString::number(t+1) + ".txt");  // открываем выбранный файл и записываем осн. переменные
                QTextStream out3(&file6);

                if (file6.open(QIODevice::WriteOnly))
                {
                    out3 << c2 << Qt::endl;
                    for (int i = 0; i < 4; i++)
                        out3 << btns2[i] << Qt::endl;
                    out3 << f2.pointSize() << Qt::endl;
                    out3 << f2.toString() << Qt::endl;
                    out3 << findChild<QLabel*>("title_" + QString::number(t+1))->text() << Qt::endl;
                    out3 << findChild<QLabel*>("text_" + QString::number(t+1))->text() << Qt::endl;
                }
                else
                    QMessageBox::critical(this, "Error", "file(w): note_" + QString::number(t+1) + ".txt \n MoveNote()", QMessageBox::Close);
                file6.close();
            }
            else  // если номер выбранной заметки отличается от номера первой заметки, содержащей этот цвет
            {
                if (t > i)  // если выбранная заметка находится дальше и её номер больше номера первой заметки
                {
                    for (int j = t; j > i; j--) // перемещение всех текстов, заголовков и звездочек на одну вправо
                    {
                        findChild<QLabel*>("title_" + QString::number(j+1))->setText(findChild<QLabel*>("title_" + QString::number(j))->text());
                        findChild<QLabel*>("text_" + QString::number(j+1))->setText(findChild<QLabel*>("text_" + QString::number(j))->text());
                        findChild<QPushButton*>("star_btn_" + QString::number(j+1))->setStyleSheet(findChild<QPushButton*>("star_btn_" + QString::number(j))->styleSheet());
                    }

                    findChild<QLabel*>("title_" + QString::number(i+1))->clear();
                    findChild<QLabel*>("text_" + QString::number(i+1))->clear();

                    QFile file("note_" + QString::number(t+1) + ".txt");    // открываем выбранный файл и считываем данные в доп. переменные
                    QTextStream in(&file);

                    if (file.open(QIODevice::ReadOnly))
                    {
                        c2 = in.readLine().toInt();
                        ColorSelect_2(c2, i+1);
                        for (int i = 0; i < 4; i++)
                             btns2[i] = in.readLine().toInt();
                        q = in.readLine().toInt();
                        f2 = in.readLine();
                        f2.setPointSize(q);
                        findChild<QLabel*>("title_" + QString::number(i+1))->setText(in.readLine());
                        while (!in.atEnd())
                            findChild<QLabel*>("text_" + QString::number(i+1))->setText(findChild<QLabel*>("text_" + QString::number(i+1))->text() + in.readLine() + "\n");
                    }
                    else
                        QMessageBox::critical(this, "Error", "file(r): note_" + QString::number(t+1) + ".txt \n MoveNote()", QMessageBox::Close);
                    file.close();
                }
                else        // если выбранная заметка находится перед первой и её номер меньше
                {
                    for (int j = t; j < i-1; j++) // перемещение всех текстов, заголовков и звездочек на одну вправо
                    {
                        findChild<QLabel*>("title_" + QString::number(j+1))->setText(findChild<QLabel*>("title_" + QString::number(j+2))->text());
                        findChild<QLabel*>("text_" + QString::number(j+1))->setText(findChild<QLabel*>("text_" + QString::number(j+2))->text());
                        findChild<QPushButton*>("star_btn_" + QString::number(j+1))->setStyleSheet(findChild<QPushButton*>("star_btn_" + QString::number(j+2))->styleSheet());
                    }

                    findChild<QLabel*>("title_" + QString::number(i))->clear();
                    findChild<QLabel*>("text_" + QString::number(i))->clear();

                    QFile file("note_" + QString::number(t+1) + ".txt");    // открываем выбранный файл и считываем данные в доп. переменные
                    QTextStream in(&file);

                    if (file.open(QIODevice::ReadOnly))
                    {
                        c2 = in.readLine().toInt();
                        ColorSelect_2(c2, i);
                        for (int i = 0; i < 4; i++)
                             btns2[i] = in.readLine().toInt();
                        q = in.readLine().toInt();
                        f2 = in.readLine();
                        f2.setPointSize(q);
                        findChild<QLabel*>("title_" + QString::number(i))->setText(in.readLine());
                        while (!in.atEnd())
                            findChild<QLabel*>("text_" + QString::number(i))->setText(findChild<QLabel*>("text_" + QString::number(i))->text() + in.readLine() + "\n");
                    }
                    else
                        QMessageBox::critical(this, "Error", "file(r): note_" + QString::number(t+1) + ".txt \n MoveNote()", QMessageBox::Close);
                    file.close();
                }

                if (t > i)  // если выбранная заметка находится дальше и её номер больше номера первой заметки
                {
                    for (int w = t; w > i; w--)    // цикл от выбранной до первой заметок (сдвиг всех данный о заметке на один вправо)
                    {
                        QFile file2("note_" + QString::number(w) + ".txt");    // открываем файл w и считываем данные в осн. переменные
                        QTextStream in(&file2);

                        if (file2.open(QIODevice::ReadOnly))
                        {
                           c1 = in.readLine().toInt();
                           ColorSelect_2(c1, w+1);
                           for (int i = 0; i < 4; i++)
                                btns[i] = in.readLine().toInt();
                           q = in.readLine().toInt();
                           f = in.readLine();
                           f.setPointSize(q);
                        }
                        else
                            QMessageBox::critical(this, "Error", "file(r): note_" + QString::number(w) + ".txt \n MoveNote()", QMessageBox::Close);
                        file2.close();

                        QFile file3("note_" + QString::number(w+1) + ".txt");   // открываем файл w+1 и записываем данные из осн. переменных
                        QTextStream out(&file3);

                        if (file3.open(QIODevice::WriteOnly))
                        {
                            out << c1 << Qt::endl;
                            for (int i = 0; i < 4; i++)
                                out << btns[i] << Qt::endl;
                            out << f.pointSize() << Qt::endl;
                            out << f.toString() << Qt::endl;
                            out << findChild<QLabel*>("title_" + QString::number(w+1))->text() << Qt::endl;
                            out << findChild<QLabel*>("text_" + QString::number(w+1))->text() << Qt::endl;
                        }
                        else
                            QMessageBox::critical(this, "Error", "file(w): note_" + QString::number(w+1) + ".txt \n MoveNote()", QMessageBox::Close);
                        file3.close();
                    }

                    QFile file3("note_" + QString::number(i+1) + ".txt");  // открываем первый файл и записываем данные из доп. переменных
                    QTextStream out2(&file3);

                    if (file3.open(QIODevice::WriteOnly))
                    {
                        out2 << c2 << Qt::endl;
                        for (int i = 0; i < 4; i++)
                            out2 << btns2[i] << Qt::endl;
                        out2 << f2.pointSize() << Qt::endl;
                        out2 << f2.toString() << Qt::endl;
                        out2 << findChild<QLabel*>("title_" + QString::number(i+1))->text() << Qt::endl;
                        out2 << findChild<QLabel*>("text_" + QString::number(i+1))->text() << Qt::endl;
                    }
                    else
                        QMessageBox::critical(this, "Error", "file(w): note_" + QString::number(i+1) + ".txt \n MoveNote()", QMessageBox::Close);
                    file3.close();
                }
                else        // если выбранная заметка находится перед первой и её номер меньше
                {
                    for (int w = t; w < i-1; w++)    // цикл от выбранной до первой заметок (сдвиг всех данный о заметке на один влево)
                    {
                        QFile file2("note_" + QString::number(w+2) + ".txt");    // открываем файл w и считываем данные в осн. переменные
                        QTextStream in(&file2);

                        if (file2.open(QIODevice::ReadOnly))
                        {
                           c1 = in.readLine().toInt();
                           ColorSelect_2(c1, w+1);
                           for (int i = 0; i < 4; i++)
                                btns[i] = in.readLine().toInt();
                           q = in.readLine().toInt();
                           f = in.readLine();
                           f.setPointSize(q);
                        }
                        else
                            QMessageBox::critical(this, "Error", "file(r): note_" + QString::number(w+2) + ".txt \n MoveNote()", QMessageBox::Close);
                        file2.close();

                        QFile file3("note_" + QString::number(w+1) + ".txt");   // открываем файл w+1 и записываем данные из осн. переменных
                        QTextStream out(&file3);

                        if (file3.open(QIODevice::WriteOnly))
                        {
                            out << c1 << Qt::endl;
                            for (int i = 0; i < 4; i++)
                                out << btns[i] << Qt::endl;
                            out << f.pointSize() << Qt::endl;
                            out << f.toString() << Qt::endl;
                            out << findChild<QLabel*>("title_" + QString::number(w+1))->text() << Qt::endl;
                            out << findChild<QLabel*>("text_" + QString::number(w+1))->text() << Qt::endl;
                        }
                        else
                            QMessageBox::critical(this, "Error", "file(w): note_" + QString::number(w+1) + ".txt \n MoveNote()", QMessageBox::Close);
                        file3.close();
                    }

                    QFile file3("note_" + QString::number(i) + ".txt");  // открываем первый файл и записываем данные из доп. переменных
                    QTextStream out2(&file3);

                    if (file3.open(QIODevice::WriteOnly))
                    {
                        out2 << c2 << Qt::endl;
                        for (int i = 0; i < 4; i++)
                            out2 << btns2[i] << Qt::endl;
                        out2 << f2.pointSize() << Qt::endl;
                        out2 << f2.toString() << Qt::endl;
                        out2 << findChild<QLabel*>("title_" + QString::number(i+1))->text() << Qt::endl;
                        out2 << findChild<QLabel*>("text_" + QString::number(i+1))->text() << Qt::endl;
                    }
                    else
                        QMessageBox::critical(this, "Error", "file(w): note_" + QString::number(i+1) + ".txt \n MoveNote()", QMessageBox::Close);
                    file3.close();
                }
            }

            tf = true;
            break;
        }
        else if (c2 > c1)   // если в проходе по заметка такого же номера цвета не оказалось, но появился цвет, меньший по номеру
        {
            for (int j = t; j > i; j--) // перемещение всех текстов, заголовков и звездочек на одну вправо
            {
                findChild<QLabel*>("title_" + QString::number(j+1))->setText(findChild<QLabel*>("title_" + QString::number(j))->text());
                findChild<QLabel*>("text_" + QString::number(j+1))->setText(findChild<QLabel*>("text_" + QString::number(j))->text());
                findChild<QPushButton*>("star_btn_" + QString::number(j+1))->setStyleSheet(findChild<QPushButton*>("star_btn_" + QString::number(j))->styleSheet());
            }

            findChild<QLabel*>("title_" + QString::number(i+1))->clear();
            findChild<QLabel*>("text_" + QString::number(i+1))->clear();

            QFile file("note_" + QString::number(t+1) + ".txt");    // открываем выбранный файл и считываем данные в доп. переменные
            QTextStream in(&file);

            if (file.open(QIODevice::ReadOnly))
            {
                c2 = in.readLine().toInt();
                ColorSelect_2(c2, i+1);
                for (int i = 0; i < 4; i++)
                     btns2[i] = in.readLine().toInt();
                q = in.readLine().toInt();
                f2 = in.readLine();
                f2.setPointSize(q);
                findChild<QLabel*>("title_" + QString::number(i+1))->setText(in.readLine());
                while (!in.atEnd())
                    findChild<QLabel*>("text_" + QString::number(i+1))->setText(findChild<QLabel*>("text_" + QString::number(i+1))->text() + in.readLine() + "\n");
            }
            else
                QMessageBox::critical(this, "Error", "file(r): note_" + QString::number(t+1) + ".txt \n MoveNote()", QMessageBox::Close);
            file.close();

            for (int w = t; w > i; w--)    // цикл от выбранной до первой заметок
            {
                QFile file2("note_" + QString::number(w) + ".txt");    // открываем файл w и считываем данные в осн. переменные
                QTextStream in(&file2);

                if (file2.open(QIODevice::ReadOnly))
                {
                   c1 = in.readLine().toInt();
                   ColorSelect_2(c1, w+1);
                   for (int i = 0; i < 4; i++)
                        btns[i] = in.readLine().toInt();
                   q = in.readLine().toInt();
                   f = in.readLine();
                   f.setPointSize(q);
                }
                else
                    QMessageBox::critical(this, "Error", "file(r): note_" + QString::number(w) + ".txt \n MoveNote()", QMessageBox::Close);
                file2.close();

                QFile file3("note_" + QString::number(w+1) + ".txt");   // открываем файл w+1 и записываем данные из осн. переменных
                QTextStream out(&file3);

                if (file3.open(QIODevice::WriteOnly))
                {
                    out << c1 << Qt::endl;
                    for (int i = 0; i < 4; i++)
                        out << btns[i] << Qt::endl;
                    out << f.pointSize() << Qt::endl;
                    out << f.toString() << Qt::endl;
                    out << findChild<QLabel*>("title_" + QString::number(w+1))->text() << Qt::endl;
                    out << findChild<QLabel*>("text_" + QString::number(w+1))->text() << Qt::endl;
                }
                else
                    QMessageBox::critical(this, "Error", "file(w): note_" + QString::number(w+1) + ".txt \n MoveNote()", QMessageBox::Close);
                file3.close();
            }

            QFile file3("note_" + QString::number(i+1) + ".txt");  // открываем первый файл и записываем данные из доп. переменных
            QTextStream out2(&file3);

            if (file3.open(QIODevice::WriteOnly))
            {
                out2 << c2 << Qt::endl;
                for (int i = 0; i < 4; i++)
                    out2 << btns2[i] << Qt::endl;
                out2 << f2.pointSize() << Qt::endl;
                out2 << f2.toString() << Qt::endl;
                out2 << findChild<QLabel*>("title_" + QString::number(i+1))->text() << Qt::endl;
                out2 << findChild<QLabel*>("text_" + QString::number(i+1))->text() << Qt::endl;
            }
            else
                QMessageBox::critical(this, "Error", "file(w): note_" + QString::number(i+1) + ".txt \n MoveNote()", QMessageBox::Close);
            file3.close();
            tf = true;
            break;
        }
    }


    if (!tf)    // если ни разу не попалась заметка с таким же цветом И этот цвет меньше по значению, чем остальные цвета, встреченные в других заметках
    {
        for (int j = t; j < k-1; j++) // перемещение всех текстов, заголовков и звездочек на одну влево
        {
            findChild<QLabel*>("title_" + QString::number(j+1))->setText(findChild<QLabel*>("title_" + QString::number(j+2))->text());
            findChild<QLabel*>("text_" + QString::number(j+1))->setText(findChild<QLabel*>("text_" + QString::number(j+2))->text());
            findChild<QPushButton*>("star_btn_" + QString::number(j+1))->setStyleSheet(findChild<QPushButton*>("star_btn_" + QString::number(j+2))->styleSheet());
        }

        findChild<QLabel*>("title_" + QString::number(k))->clear();
        findChild<QLabel*>("text_" + QString::number(k))->clear();

        QFile file("note_" + QString::number(t+1) + ".txt");    // открываем выбранный файл и считываем данные в доп. переменные
        QTextStream in(&file);

        if (file.open(QIODevice::ReadOnly))
        {
            c2 = in.readLine().toInt();
            ColorSelect_2(c2, k);
            for (int i = 0; i < 4; i++)
                 btns2[i] = in.readLine().toInt();
            q = in.readLine().toInt();
            f2 = in.readLine();
            f2.setPointSize(q);
            findChild<QLabel*>("title_" + QString::number(k))->setText(in.readLine());
            while (!in.atEnd())
                findChild<QLabel*>("text_" + QString::number(k))->setText(findChild<QLabel*>("text_" + QString::number(k))->text() + in.readLine() + "\n");
        }
        else
            QMessageBox::critical(this, "Error", "file(r): note_" + QString::number(t+1) + ".txt \n MoveNote()", QMessageBox::Close);
        file.close();

        for (int w = t; w < k-1; w++)    // цикл от выбранной до последней заметок (сдвиг влево)
        {
            QFile file2("note_" + QString::number(w+2) + ".txt");    // открываем файл w+2 и считываем данные в осн. переменные
            QTextStream in(&file2);

            if (file2.open(QIODevice::ReadOnly))
            {
               c1 = in.readLine().toInt();
               ColorSelect_2(c1, w+1);
               for (int i = 0; i < 4; i++)
                    btns[i] = in.readLine().toInt();
               q = in.readLine().toInt();
               f = in.readLine();
               f.setPointSize(q);
            }
            else
                QMessageBox::critical(this, "Error", "file(r): note_" + QString::number(w+2) + ".txt \n MoveNote()", QMessageBox::Close);
            file2.close();

            QFile file3("note_" + QString::number(w+1) + ".txt");   // открываем файл w+1 и записываем данные из осн. переменных
            QTextStream out(&file3);

            if (file3.open(QIODevice::WriteOnly))
            {
                out << c1 << Qt::endl;
                for (int i = 0; i < 4; i++)
                    out << btns[i] << Qt::endl;
                out << f.pointSize() << Qt::endl;
                out << f.toString() << Qt::endl;
                out << findChild<QLabel*>("title_" + QString::number(w+1))->text() << Qt::endl;
                out << findChild<QLabel*>("text_" + QString::number(w+1))->text() << Qt::endl;
            }
            else
                QMessageBox::critical(this, "Error", "file(w): note_" + QString::number(w+1) + ".txt \n MoveNote()", QMessageBox::Close);
            file3.close();
        }

        QFile file3("note_" + QString::number(k) + ".txt");  // открываем первый файл и записываем данные из доп. переменных
        QTextStream out2(&file3);

        if (file3.open(QIODevice::WriteOnly))
        {
            out2 << c2 << Qt::endl;
            for (int i = 0; i < 4; i++)
                out2 << btns2[i] << Qt::endl;
            out2 << f2.pointSize() << Qt::endl;
            out2 << f2.toString() << Qt::endl;
            out2 << findChild<QLabel*>("title_" + QString::number(k))->text() << Qt::endl;
            out2 << findChild<QLabel*>("text_" + QString::number(k))->text() << Qt::endl;
        }
        else
            QMessageBox::critical(this, "Error", "file(w): note_" + QString::number(k) + ".txt \n MoveNote()", QMessageBox::Close);
        file3.close();
    }

    ResizeLayout();
}


// изменение элементов под размер окна приложения
void MainWindow::ResizeWidth()
{
    if (w != this->width() or h != this->height())  // если высота или ширина окна изменилась
    {
        w = this->width();
        h = this->height();

        f = ui->label->font();
        QFont f1(ui->AddButton->font());

        if (w >= 1400 and h >= 600)
        {
            f.setPointSize(33);
            f1.setPointSize(72);
            R = 120;
            R1 = 28;
            ui->label->setText("Write It Down");
            ui->horizontalLayout_3->setContentsMargins(0, 0, 52, 0);
//            ui->horizontalSpacer_4->changeSize(36, 20);
        }
        else if (w >= 970 and h >= 450)
        {
            f.setPointSize(29);
            f1.setPointSize(62);
            R = 110;
            R1 = 24;
            ui->label->setText("Write It Down");
            ui->horizontalLayout_3->setContentsMargins(0, 0, 45, 0);
//            ui->horizontalLayout_2->setSpacing(13);
//            ui->horizontalSpacer_4->changeSize(32, 20);
        }
        else if (w >= 640 and h >= 350)
        {
            f.setPointSize(26);
            f1.setPointSize(57);
            R = 100;
            R1 = 22;
            ui->label->setText("Write It Down");
            ui->horizontalLayout_3->setContentsMargins(0, 0, 37, 0);
//            ui->horizontalLayout_2->setSpacing(11);
//            ui->horizontalSpacer_4->changeSize(28, 20);
        }
        else
        {
            f.setPointSize(21);
            f1.setPointSize(52);
            R = 90;
            R1 = 18;
            ui->label->setText("WiD");
            ui->horizontalLayout_3->setContentsMargins(0, 0, 20, 0);
//            ui->horizontalLayout_2->setSpacing(8);
//            ui->horizontalSpacer_4->changeSize(24, 20);
        }

        ui->label->setFont(f);
        ui->AddButton->setFont(f1);
        ui->AddButton->setFixedSize(R, R);
        ui->AddButton->setStyleSheet("color: rgb" + tc + "; \nborder-radius: " + QString::number(R/2) + "px; \n" + color);
        ui->search_btn->setFixedSize(R1, R1);
        ui->search_btn->setStyleSheet("border-radius: " + QString::number(R1/2) + "px; \n background-color: rgb" + tc_w + ";");
        ui->set_btn->setFixedSize(R1, R1);
        ui->set_btn->setStyleSheet("border-radius: " + QString::number(R1/2) + "px; \n background-color: rgb" + tc_w + ";");
        ui->color_btn->setFixedSize(R1, R1);
        ui->color_btn->setStyleSheet("border-radius: " + QString::number(R1/2) + "px; \n" + color);
        ui->color_btn_2->setFixedSize(R1, R1);
        ui->color_btn_2->setStyleSheet("border-radius: " + QString::number(R1/2) + "px; \n background-color: rgb" + tc + ";");
        ResizeLayout();  //распрееление в Layout в зависимости от размера окна
    }
}


// распределение заметок в зависимости от размера приложения
void MainWindow::ResizeLayout()
{
    int g = -1;     // номер строчки для записи заметок в Layout
    int h1 = 1;     // номер столбца для записи заметок в Layout
    int h = 0;      // кол-во заметок в ряду
    int c2 = c1;    // доп. номер цвета
    q = 0;          // коэф. для формулы h1

    if (w >= 970 and w < 1400)      // назначение кол-ва заметок в ряду, в зависимости от размера окна
        h = 3;
    else if (w >= 1400)
        h = 4;
    else if (w >= 640 and w < 970)
        h = 2;
    else
        h = 1;

    for (int i = 0; i < k; i++) // от первой до последней заметки
    {
        QFile file("note_" + QString::number(i+1) + ".txt");   // открываем i-тый файл и считываем номер цвета
        QTextStream in(&file);

        if (file.open(QIODevice::ReadOnly))
            c1 = in.readLine().toInt();
        else
            QMessageBox::critical(this, "Error", "file(r): note_" + QString::number(i+1) + ".txt \n MoveNote()", QMessageBox::Close);
        file.close();

        if (i != 0)     // если заметка не первая (есть предыдущий файл)
        {
            QFile file1("note_" + QString::number(i) + ".txt");   // открываем предыдущий файл и считываем номер цвета
            QTextStream in1(&file1);

            if (file1.open(QIODevice::ReadOnly))
                c2 = in1.readLine().toInt();
            else
                QMessageBox::critical(this, "Error", "file(r): note_" + QString::number(i) + ".txt \n MoveNote()", QMessageBox::Close);
            file1.close();
        }
        else            // если первая
            c2 = c1;

        h1 = (i-q) % h;

        if (c1 != c2 and c1 == 0)   // если цвета не совпадают и второй цвет равен 0 (ничего)
        {
            q = i;      // увеличение коэффициента
            h1 = 0;     // номер столбца обнуляется (т.к. перенос на следующую строку)
        }

        if (h1 == 0)    // если предыдущее условие верно ИЛИ кол-во заметок на опред. строке закончилось и происходить вынужденный перехож на след. строку
        {
            g++;
        }

        ui->gridLayout_4->addWidget(findChild<QWidget*>("widget_" + QString::number(i+1)), g, h1);
    }

    ui->scrollAreaWidgetContents->setLayout(ui->gridLayout_4);
}


// изменение цвета по нажатии кнопки
void MainWindow::ColorChange()
{
    if (c < 4)      //всего 5 цветов
        c++;
    else
        c = 0;

    //назначение цвета
    ColorSelect(color);

    ui->color_btn->setStyleSheet("border-radius: " + QString::number(R1/2) + "px; \n" + color);
    ui->AddButton->setStyleSheet("color: rgb" + tc + "; \nborder-radius: " + QString::number(R/2) + "px; \n" + color);
    ui->scrollArea->verticalScrollBar()->setStyleSheet(color);
    ui->search_text->setStyleSheet("/*border-radius: 1px; \n */border-color: rgb(255, 0, 0); \n \
                                color: rgb" + tc + "; selection-" + color + " selection-color: rgb" + tc + ";");
    for (int i = 0; i < k; i++)
    {
        findChild<QPushButton*>("btn_" + QString::number(i+1))->setStyleSheet("color: rgb" + tc + "; \nborder-radius: 5px; \n" + color);
        findChild<QPushButton*>("delete_btn_" + QString::number(i+1))->setStyleSheet("color: rgb" + tc + "; \nborder-radius: 5px; \n" + color);
    }

    QFile file("num.txt");      //запись кол-ва заметок, номера доп. цвета и номера цвета темы
    QTextStream out(&file);

    if (file.open(QIODevice::WriteOnly))
    {  
        out << k << Qt::endl;
        out << c << Qt::endl;
        out << cm << Qt::endl;
    }
    else
        QMessageBox::critical(this, "Error", "file(w): num.txt \n ColorChanged()", QMessageBox::Close);
    file.close();
}



void MainWindow::ColorChange_Main()
{
    cm = (cm + 1) % 2;
    ColorSelect_Main();

    QFile file("num.txt");      //запись кол-ва заметок и номера цвета
    QTextStream out(&file);

    if (file.open(QIODevice::WriteOnly))
    {
        out << k << Qt::endl;
        out << c << Qt::endl;
        out << cm << Qt::endl;
    }
    else
        QMessageBox::critical(this, "Error", "file(w): num.txt \n ColorChanged()", QMessageBox::Close);
    file.close();
//    QString tc2 = (cm == 0 ? l_theme : d_theme);
//    tc = (cm == 0 ? d_theme : l_theme);
//    tc_w = (cm == 0 ? l_theme_w : d_theme_w);

//    this->setStyleSheet("background-color: rgb" + tc2);
//    ui->scrollAreaWidgetContents->setStyleSheet("background-color: rgb" + tc2);
//    ui->label->setStyleSheet("color: rgb" + tc);
//    ui->AddButton->setStyleSheet("color: rgb" + tc + "; \nborder-radius: " + QString::number(R/2) + "px; \n" + color);
//    ui->color_btn_2->setStyleSheet("border-radius: " + QString::number(R1/2) + "px; \n" + "background-color: rgb" + tc + ";");

//    for (int i = 0; i < k; i++)
//    {
//        findChild<QWidget*>("widget_" + QString::number(i+1))->setStyleSheet("border-radius: 5px; \n background-color: rgb" + tc_w);
//        findChild<QLabel*>("text_" + QString::number(i+1))->setStyleSheet("color: rgb" + tc);
//        findChild<QPushButton*>("btn_" + QString::number(i+1))->setStyleSheet("color: rgb" + tc + "; \nborder-radius: 5px; \n" + color);
//        findChild<QPushButton*>("delete_btn_" + QString::number(i+1))->setStyleSheet("color: rgb" + tc + "; \nborder-radius: 5px; \n" + color);
//    }

//    QFile file("num.txt");      //запись кол-ва заметок и номера цвета

//    if (file.open(QIODevice::WriteOnly))
//    {
//        QTextStream out(&file);
//        out << k << Qt::endl;
//        out << c << Qt::endl;
//        out << cm << Qt::endl;
//    }
//    else
//        QMessageBox::critical(this, "Error", "file(w): num.txt \n ColorChanged()", QMessageBox::Close);
//    file.close();
}

// изменение цвета "звездочки" по нажатии кнопки
void MainWindow::ColorChange_Star()
{
//    QThread::msleep(200)
    for (int i = 0; i < k; i++)
    {
        if (sender() == findChild<QPushButton*>("star_btn_" + QString::number(i+1)))
        {
            QFile file("note_" + QString::number(i+1) + ".txt");        // считывание номера цвета из выбранной заметки
            QTextStream in(&file);

            if (file.open(QIODevice::ReadOnly))
            {
                c1 = in.readLine().toInt();
                for (int i = 0; i < 4; i++)
                    btns[i] = in.readLine().toInt();
                q = in.readLine().toInt();
                f = in.readLine();
                f.setPointSize(q);
            }
            else
                QMessageBox::critical(this, "Error", "file(r): note_" + QString::number(i+1) + ".txt \n ColorChange_2()", QMessageBox::Close);
            file.close();

            if (c1 == 2)    // изменение номера цвета
                c1 = 0;
            else
                c1++;

            QFile file2("note_" + QString::number(i+1) + ".txt");       // запись номера цвета в заметку
            QTextStream out(&file2);

            if (file2.open(QIODevice::WriteOnly))
            {
                out << c1 << Qt::endl;
                for (int i = 0; i < 4; i++)
                    out << btns[i] << Qt::endl;
                out << f.pointSize() << Qt::endl;
                out << f.toString() << Qt::endl;
                out << findChild<QLabel*>("title_" + QString::number(i+1))->text() << Qt::endl;
                out << findChild<QLabel*>("text_" + QString::number(i+1))->text() << Qt::endl;
            }
            else
                QMessageBox::critical(this, "Error", "file(w): note_" + QString::number(i+1) + ".txt \n ColorChange_2()", QMessageBox::Close);
            file2.close();

            ColorSelect_2(c1, i+1);     // назначение измененного цвета
            MoveNote(i);                // сдвиг заметок с учетом другого цвета "звездочки"
            ResizeLayout();
            break;
        }
    }
}


// определение цвета, выбранного пользователем
void MainWindow::ColorSelect(QString &s)
{
    s = "background-color: rgb(";
    switch(c)
    {
    case 0:
        s += QString::number(212-(198/*163*/*cm)) + ", " + QString::number(253-(182/*182*/*cm)) + ", "  + QString::number(217-(149/*150*/*cm)) /*"212, 253, 217"*/;    ///*227, 252, 230*//*230, 255, 238*/"/*"240, 225, 187"*//*"201, 255, 223"*//*"124, 154, 143"*/;
        break;

    case 1:
        s += QString::number(203-(150*cm)) + ", " + QString::number(231-(150*cm)) + ", "  + QString::number(254-(150*cm)) /*"203, 231, 254"*/;    ///*209, 235, 255*/"/*"75, 245, 56"*//*"197, 255, 252"*//*"111, 67, 104"*/;
        break;

    case 2:
        s += QString::number(255-(150*cm)) + ", " + QString::number(211-(211*cm)) + ", "  + QString::number(225-(181*cm)) /*"255, 211, 225"*/;    ///*255, 204, 199*/"/*"62, 158, 222"*//*"255, 203, 203"*//*"127, 57, 50"*/;
        break;

    case 3:
        s += QString::number(219-(163*cm)) + ", " + QString::number(191-(175*cm)) + ", "  + QString::number(254-(147*cm)) /*"219, 191, 254"*/;   ///*226, 199, 254*//*229, 210, 255*/"/*"240, 213, 245"*//*"221, 203, 255"*//*"45, 50, 122"*/;
        break;

    default:
        s += QString::number(166-(165*cm)) + ", " + QString::number(158-(140*cm)) + ", "  + QString::number(154-(128*cm)) /*"219, 191, 254"*/;   ///*226, 199, 254*//*229, 210, 255*/"/*"240, 213, 245"*//*"221, 203, 255"*//*"45, 50, 122"*/;
        break;
    }
    s += ");";
}


void MainWindow::ColorSelect_Main()
{
    QString tc2 = (cm == 0 ? l_theme : d_theme);
    tc = (cm == 0 ? d_theme : l_theme);
    tc_w = (cm == 0 ? l_theme_w : d_theme_w);

    ColorSelect(color);

    this->setStyleSheet("background-color: rgb" + tc2);
    ui->scrollAreaWidgetContents->setStyleSheet("background-color: rgb" + tc2);
    ui->label->setStyleSheet("color: rgb" + tc);
    ui->AddButton->setStyleSheet("color: rgb" + tc + "; \nborder-radius: " + QString::number(R/2) + "px; \n" + color);
    ui->color_btn_2->setStyleSheet("border-radius: " + QString::number(R1/2) + "px; \n" + "background-color: rgb" + tc + ";");
    ui->color_btn->setStyleSheet("border-radius: " + QString::number(R1/2) + "px; \n" + color);
    ui->search_btn->setStyleSheet("border-radius: " + QString::number(R1/2) + "px; \n background-color: rgb" + tc_w + ";");
    ui->set_btn->setStyleSheet("border-radius: " + QString::number(R1/2) + "px; \n background-color: rgb" + tc_w + ";");
    ui->search_text->setStyleSheet("/*border-radius: 1px; \n */border-color: rgb(255, 0, 0); \n \
                                color: rgb" + tc + "; selection-" + color + " selection-color: rgb" + tc + ";");

    for (int i = 0; i < k; i++)
    {
        QFile file("note_" + QString::number(i+1) + ".txt");        // считывание номера цвета из выбранной заметки
        QTextStream in(&file);

        if (file.open(QIODevice::ReadOnly))
            c1 = in.readLine().toInt();
        else
            QMessageBox::critical(this, "Error", "file(r): note_" + QString::number(i+1) + ".txt \n ColorChange_2()", QMessageBox::Close);
        file.close();

        ColorSelect_2(c1, i+1);
        findChild<QWidget*>("widget_" + QString::number(i+1))->setStyleSheet("border-radius: 5px; \n background-color: rgb" + tc_w);
        findChild<QLabel*>("title_" + QString::number(i+1))->setStyleSheet("color: rgb" + tc);
        findChild<QLabel*>("text_" + QString::number(i+1))->setStyleSheet("color: rgb" + tc);
        findChild<QPushButton*>("btn_" + QString::number(i+1))->setStyleSheet("color: rgb" + tc + "; \nborder-radius: 5px; \n" + color);
        findChild<QPushButton*>("delete_btn_" + QString::number(i+1))->setStyleSheet("color: rgb" + tc + "; \nborder-radius: 5px; \n" + color);
    }
}

// определение цвета звездочки выбранного ползователем
void MainWindow::ColorSelect_2(int c1, int t)
{
    QString s = "background-color: rgb";
    switch (c1)
    {
    case 0:
        s += (cm == 0 ? l_theme_w : d_theme_w) + "; \n border: 1px " + (cm == 0 ? "solid grey" : "solid black");
        break;
    case 1:
        s += "(255, 204, 102);";
        break;
    default:
        s += "(204, 0, 0);";
        break;
    }

    findChild<QPushButton*>("star_btn_" + QString::number(t))->setStyleSheet(s);
}


// создание новой заметки
void MainWindow::Initialization(int t)
{
    QWidget* wd = new QWidget(ui->scrollAreaWidgetContents);
    wd->setFixedSize(271, 120);
    wd->setStyleSheet("border-radius: 5px; \n background-color: rgb" + tc_w);
    wd->setObjectName("widget_" + QString::number(t));
    wd->show();

    QPushButton* btn = new QPushButton("открыть", findChild<QWidget*>("widget_" + QString::number(t)));
    btn->setObjectName("btn_" + QString::number(t));
    btn->setGeometry(170, 0, 101, 41);
    f1.setPointSize(11);
    btn->setFont(f1);
    btn->setStyleSheet("color: rgb" + tc + "; \nborder-radius: 5px; \n" + color);
    btn->show();
    connect(btn, SIGNAL(clicked()), this, SLOT(openNote()));

    QPushButton* dBtn = new QPushButton("удалить", findChild<QWidget*>("widget_" + QString::number(t)));
    dBtn->setObjectName("delete_btn_" + QString::number(t));
    dBtn->setGeometry(210, 70, 62, 30);
    f1.setPointSize(8);
    dBtn->setFont(f1);
    dBtn->setStyleSheet("color: rgb" + tc + "; \nborder-radius: 5px; \n" + color);
    dBtn->show();
    connect(dBtn, SIGNAL(clicked()), this, SLOT(deleteNote()));

    QPushButton* sBtn = new QPushButton(findChild<QWidget*>("widget_" + QString::number(t)));
    sBtn->setObjectName("star_btn_" + QString::number(t));
    sBtn->setGeometry(140, 10, 21, 21);
    ColorSelect_2(c1, t);
    sBtn->show();
    connect(sBtn, SIGNAL(clicked()), this, SLOT(ColorChange_Star()));

    QLabel* title = new QLabel(findChild<QWidget*>("widget_" + QString::number(t)));
    title->setObjectName("title_" + QString::number(t));
    title->setStyleSheet("color: rgb" + tc);
    title->setGeometry(10, 5, 121, 31);
    f1.setPointSize(14);
    title->setFont(f1);
    title->show();

    QLabel* txt = new QLabel(findChild<QWidget*>("widget_" + QString::number(t)));
    txt->setObjectName("text_" + QString::number(t));
    txt->setStyleSheet("color: rgb" + tc);
    txt->setGeometry(10, 53, 180, 55);
    f1.setPointSize(10);
    txt->setFont(f1);
    txt->show();
}



void MainWindow::OpenSearchText()
{
    if (search == 0)
    {
        ui->search_text->show();
        search++;
    }
    else if (search == 1 && ui->search_text->toPlainText() != "")
    {
        for (int i = 0; i < k; i++)
        {
            if (findChild<QLabel*>("title_" + QString::number(i+1))->text().contains(ui->search_text->toPlainText()) || findChild<QLabel*>("text_" + QString::number(i+1))->text().contains(ui->search_text->toPlainText()))
            {
               findChild<QWidget*>("widget_" + QString::number(i+1))->setStyleSheet("border: 2px solid yellow; \n border-radius: 5px; \n background-color: rgb" + tc_w);
               findChild<QLabel*>("title_" + QString::number(i+1))->setStyleSheet("border: 0px");
               findChild<QLabel*>("text_" + QString::number(i+1))->setStyleSheet("border: 0px");
//               findChild<QPushButton*>("star_btn_" + QString::number(i+1))->setStyleSheet(findChild<QPushButton*>("star_btn_" + QString::number(i+1))->styleSheet() + "border: 0px")
//               findChild<QPushButton*>("star_btn_" + QString::number(i+1))->setStyleSheet("border: 0px");
//               findChild<QPushButton*>("delete_btn_" + QString::number(i+1))->setStyleSheet("border: 0px");
//               findChild<QPushButton*>("btn_" + QString::number(i+1))->setStyleSheet("border: 0px");
            }
        }

        ui->search_btn->setText("X");       // привязать esc к кнопке!!!!!!!!!!
        search++;
    }
    else
    {
        ui->search_text->clear();
        ui->search_text->hide();
        search = 0;
        ui->search_btn->setText("O");

        for (int i = 0; i < k; i++)
        {
            if (findChild<QWidget*>("widget_" + QString::number(i+1))->styleSheet().contains("yellow"))
            {
               findChild<QWidget*>("widget_" + QString::number(i+1))->setStyleSheet("border-radius: 5px; \n background-color: rgb" + tc_w);
            }
        }
    }
}

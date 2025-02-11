#include "note.h"
#include "ui_note.h"

Note::Note(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Note)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowMinMaxButtonsHint);

    // сделать появление заметки левй верхний угол в левом верхнем углу меню !!!!!!!!!

    //loop функция, сделанная через таймер
    connect(&timer, SIGNAL(timeout()), this, SLOT(resizeWidth()));
    timer.start(50);

    connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(changeStyle()));
    connect(ui->color_btn, SIGNAL(clicked()), this, SLOT(colorChange()));
    connect(ui->fontComboBox, SIGNAL(currentFontChanged(QFont)), ui->textEdit, SLOT(setCurrentFont(QFont)));
    connect(ui->fontComboBox, SIGNAL(currentFontChanged(QFont)), ui->textEdit_2, SLOT(setCurrentFont(QFont)));
    connect(ui->fontComboBox, SIGNAL(currentFontChanged(QFont)), this, SLOT(changeFont()));
    connect(ui->btn_RoleUp, SIGNAL(clicked()), this, SLOT(roleUp()));
    connect(ui->Ok_btn, SIGNAL(clicked()), this, SLOT(OkNote()));
    connect(new QShortcut(Qt::Key_Escape, this), SIGNAL(activated()), this, SLOT(OkNote()));
    connect(new QShortcut(Qt::CTRL | Qt::Key_H, this), SIGNAL(activated()), this, SLOT(roleUp()));
//    new QShortcut(QKeySequence(Qt::CTRL), this, SLOT(OkNote()));
    connect(new QShortcut(Qt::Key_S | Qt::CTRL, this), SIGNAL(activated()), this, SLOT(colorChange()));
    connect(new QShortcut(Qt::CTRL | Qt::Key_T, this), SIGNAL(activated()), this, SLOT(translation()));
    connect(ui->list_btn, SIGNAL(clicked()), this, SLOT(setList()));
    connect(ui->t_btn, SIGNAL(clicked()), this, SLOT(translation()));

    int id = QFontDatabase::addApplicationFont("Comfortaa.ttf"); //путь к шрифту
    QString family = QFontDatabase::applicationFontFamilies(id).at(0); //имя шрифта
    QFont bf(family);
    bf.setPointSize(15);
    ui->Ok_btn->setFont(bf);
    ui->Ok_btn->setText("OK");

    // убираем scrollBar в поле с заголовком
    ui->textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // первоначальная инициализация
    ui->textEdit->setFont(ui->fontComboBox->currentFont());
    ui->textEdit_2->setFont(ui->fontComboBox->currentFont());
    ui->spinBox->setValue(ui->fontComboBox->currentFont().pointSize());
//    ui->textEdit_2->verticalScrollBar()->setPalette(ui->Ok_btn->palette());

    // находим выбранный пользователем ЦВЕТ, записанный 2-ой строкой в файл num.txt
    QFile file("num.txt");
    int c = 0;  //номер цвета

    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);

        in.readLine();
        c = in.readLine().toInt();
        cm = in.readLine().toInt();
    }
    else
        QMessageBox::critical(this, "Error", "file(r): num.txt \n Note()", QMessageBox::Close);
    file.close();

    theme = (cm == 0 ? d_theme : l_theme);
    colors[0] = (cm == 1 ? d_theme : l_theme/*QString("(1, 31, 43)") : QString("(242, 231, 225)")*/);
    r = colors2[c][0] - colors3[c][0]*cm;
    g = colors2[c][1] - colors3[c][1]*cm;
    b = colors2[c][2] - colors3[c][2]*cm;

    color = "background-color: rgb(" + QString::number(r) + ", " + QString::number(g) + ", " + QString::number(b) + ")";

    this->setStyleSheet("background-color: rgb" + (cm == 1 ? QString("(1, 31, 43)") : QString("(242, 231, 225)")));
    ui->Ok_btn->setStyleSheet("color: rgb" + theme + "; \nborder-radius: 10px; \n " + color);

    ui->spinBox->setStyleSheet("QSpinBox { padding-left: 4px; \nborder-radius: 5px; \ncolor: rgb" + theme + "; \n" + color + "; \n\
                               selection-background-color: rgb(" + (cm == 1 ? QString("1, 31, 43)") : QString("242, 231, 225)")) + "; \n\
                               selection-color: rgb(" + (cm == 0 ? QString("1, 31, 43)") : QString("242, 231, 225)")) + " }"
                               "QSpinBox::up-button: { width: 30px; }"
                               /*"QSpinBox::up-arrow { image: url(:/images/down_arrow.png); \nborder-radius: 5px; \n" + color + " }"*/);

    ui->fontComboBox->setStyleSheet("QFontComboBox { padding-left: 4px; \nborder-radius: 5px; \ncolor: rgb" + theme + "; \n" + color + "; \n\
                                    selection-background-color: rgb(" + (cm == 1 ? QString("1, 31, 43)") : QString("242, 231, 225)")) + "; \n\
                                    selection-color: rgb(" + (cm == 0 ? QString("1, 31, 43)") : QString("242, 231, 225)")) + " }"
                                    "QFontComboBox::drop-down { width: 32px;\n\
                                    border-top-right-radius: 3px;\n\
                                    border-bottom-right-radius: 3px; }"
                                    "QFontComboBox::down-arrow { image: url(:/buttons/Icons/down_arrow.png); \n\
                                    width: 32px;\n\
                                    height: 32px; }");

    ui->t_btn->setStyleSheet("color: rgb" + theme + "; \nborder-radius: 5px; \n " + color);
    ui->list_btn->setStyleSheet("color: rgb" + theme + "; \nborder-radius: 5px; \n " + color);
    ui->color_btn->setStyleSheet("border: 1px " + (cm == 0 ? QString("solid grey") : QString("solid black")) + "; \n border-radius: 5px; \n background-color: rgb" + colors[c1] + ";");
    ui->btn_RoleUp->setStyleSheet("color: rgb" + theme + "; \nborder-radius: 5px; \n " + color);
    ui->textEdit->setStyleSheet("color: rgb" + theme + "; selection-" + color + "; selection-color: rgb" + theme);
    ui->textEdit_2->setStyleSheet("color: rgb" + theme + "; selection-" + color + "; selection-color: rgb" + theme);


    ui->textEdit_2->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {"
                                                       "background: transparent; \n"
//                                                       "background-color: red; \n"
                                                       "border: 0px; \n"
                                                       "border-radius: 5px; \n"
                                                       "width: 10px; "
                                                       "margin-right: 2px"
                                                       "}"
                                                       "QScrollBar::down-button {"
                                                       "color: transparent; \n"
                                                       "background: transparent }"
                                                       "QScrollBar::up-button {"
                                                       "color: transparent; \n"
                                                       "background: transparent }"
                                                       "QScrollBar::handle:vertical {"
                                                       "min-height: 8px; \n "
                                                       "border-radius: 5px; \n"
                                                       "" + color + "; }"

                                                       "QScrollBar::add-page { background-color: rgb" + (cm == 1 ? QString("(1, 31, 43)") : QString("(242, 231, 225)")) + " }"
                                                       "QScrollBar::sub-page { background-color: rgb" + (cm == 1 ? QString("(1, 31, 43)") : QString("(242, 231, 225)")) + " }");
                                                       /*"QScrollBar::up-button { background-color: red }"*/


    for (int i = 0; i < 4; i++)
    {
        btns[i] = findChild<QPushButton *>("btn_" + QString::number(i+1));
        btns[i]->setChecked(false);
        btns[i]->setStyleSheet("color: rgb" + theme + "; \nborder-radius: 5px; \n" + color);

        connect(btns[i], SIGNAL(clicked()), this, SLOT(changeStyle()));
    }
}


Note::~Note()
{
    delete ui;
}


// при нажатии кнопки OK
void Note::OkNote()
{
    // сохраняем все данные о заметке
    QFile file("note_" + QString::number(k+1) + ".txt");

    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream out(&file);

        out << c1 << Qt::endl;                                          // номер цвета заметки
        for (int i = 0; i < 4; i++)
            out << btns[i]->isChecked() << Qt::endl;                    // значения кнопок Ж, К, Ч, З
        out << ui->spinBox->value() << Qt::endl;                        // размер текста
        out << ui->fontComboBox->currentFont().toString() << Qt::endl;  // шрифт текста в виде строки
        out << ui->textEdit->toPlainText() << Qt::endl;                 // заголовок
        out << ui->textEdit_2->toPlainText() << Qt::endl;               // осн. текст
    }
    else
        QMessageBox::critical(this, "Error", "file(w): note_" + QString::number(k+1) + ".txt  \n OkNote()", QMessageBox::Close);
    file.close();

    this->accept();
}


// получение информации из файла заметки
void Note::getText(int i)
{  
    k = i;

    QFile file("note_" + QString::number(i+1) + ".txt");

    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);

        c1 = in.readLine().toInt();
        ui->btn_1->setChecked(bool(in.readLine().toInt()));
        ui->btn_2->setChecked(bool(in.readLine().toInt()));
        ui->btn_3->setChecked(bool(in.readLine().toInt()));
        ui->btn_4->setChecked(bool(in.readLine().toInt()));
        ui->spinBox->setValue(in.readLine().toInt());
        ui->fontComboBox->setCurrentFont(in.readLine());

        QString s = (c1 == 0 ? "border: 1px " + (cm == 0 ? QString("solid grey") : QString("solid black")) + "; \n " : "");
        ui->color_btn->setStyleSheet(s + "border-radius: 5px; \n background-color: rgb" + colors[c1] + ";");

        changeStyle();

        ui->textEdit->setText("");
        ui->textEdit_2->setText("");

        ui->textEdit->setText(in.readLine());
        while (!in.atEnd())
            ui->textEdit_2->setText(ui->textEdit_2->toPlainText() + in.readLine() + "\n");
    }
    else
        QMessageBox::critical(this, "Error", "file(r): note_" + QString::number(i+1) + ".txt  \n getText()", QMessageBox::Close);
    file.close();

    // запись всех данных в тот же файл (на экстренный случай, при закрытии заметки)
    QFile file1("note_" + QString::number(i+1) + ".txt");

    if (file1.open(QIODevice::WriteOnly))
    {
        QTextStream out(&file1);

        out << c1 << Qt::endl;
        for (int i = 0; i < 4; i++)
            out << btns[i]->isChecked() << Qt::endl;
        out << ui->spinBox->value() << Qt::endl;
        out << ui->fontComboBox->currentFont().toString() << Qt::endl;
        out << ui->textEdit->toPlainText() << Qt::endl;
        out << ui->textEdit_2->toPlainText() << Qt::endl;
    }
    else
        QMessageBox::critical(this, "Error", "file(w): note_" + QString::number(i+1) + ".txt  \n getText()", QMessageBox::Close);
    file1.close();
}


// создание новой заметки (getText() не вызывается, т.к. заметка новая, и в файле пусто)
int Note::newText(int i)
{
    k = i;
    return c1;
}


// при смене шрифта есть встроенные функции, проведенные в Qt Designer от fontComboBox до текстов
// здесь вся информация в заметках переписывается, но уже с назначенным новым шрифтом
void Note::changeFont()
{   
    if (ui->spinBox->value() > 20)
    {
        f = ui->fontComboBox->currentFont();
        f.setPointSize(20);
        ui->textEdit->setCurrentFont(f);
    }

    ui->textEdit->setText(ui->textEdit->toPlainText());
    ui->textEdit_2->setText(ui->textEdit_2->toPlainText());
}


// изменение стилей текста
// QFont f присваивается значение fontComboBox,
// потом перезписывается размер шрифта с spinBox или другие свойства
// и присваиваем текущему шрифту шрифт f
void Note::changeStyle()
{
    f = ui->fontComboBox->currentFont();

    if (sender() == ui->spinBox)
    {
        f.setPointSize(ui->spinBox->value());
    }
    else if (sender() == NULL)
    {
        f.setPointSize(ui->spinBox->value());
        f.setBold(btns[0]->isChecked());
        f.setItalic(btns[1]->isChecked());
        f.setUnderline(btns[2]->isChecked());
        f.setStrikeOut(btns[3]->isChecked());
        for (int i = 0; i < 4; i++)
            btns[i]->setStyleSheet("color: rgb" + theme + "; \nborder-radius: 5px; \nbackground-color: rgb(" + QString::number(r-((35-53*cm)*btns[i]->isChecked())) +
                                   ", " + QString::number(g-((35-53*cm)*btns[i]->isChecked())) + ", " + QString::number(b-((35-53*cm)*btns[i]->isChecked())) + ");");
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            if (sender() == btns[i])
            {
                switch (i)
                {
                case 0:
                    f.setBold(btns[i]->isChecked());
                    break;
                case 1:
                    f.setItalic(btns[i]->isChecked());
                    break;
                case 2:
                    f.setUnderline(btns[i]->isChecked());
                    break;
                case 3:
                    f.setStrikeOut(btns[i]->isChecked());
                }
                btns[i]->setStyleSheet("color: rgb" + theme + "; \nborder-radius: 5px; \n background-color: rgb(" + QString::number(r-((35-53*cm)*btns[i]->isChecked())) + ", " +
                                       QString::number(g-((35-53*cm)*btns[i]->isChecked())) + ", " + QString::number(b-((35-53*cm)*btns[i]->isChecked())) + ");");
                break;
            }
        }

    }

    ui->fontComboBox->setCurrentFont(f);
    changeFont();
}


// изменение ui-элементов при изменении размера окна
void Note::resizeWidth()
{
    int R = 100;
    QFont f1 = ui->Ok_btn->font();

    if (w != this->width() or h != this->height())
    {
        w = this->width();
        h = this->height();

        if (w >= 1400 and h >= 600)
        {
            f1.setPointSize(19);
            R = 88;
        }
        else if (w >= 970 and h >= 450)
        {
            f1.setPointSize(17);
            R = 78;
        }
        else if (w >= 550 and h >= 350)
        {
            f1.setPointSize(15);
            R = 68;
        }
        else
        {
            f1.setPointSize(13);
            R = 56;
        }

        ui->Ok_btn->setFont(f1);
        ui->Ok_btn->setFixedSize(R, R);
        ui->Ok_btn->setStyleSheet("color: rgb" + theme + "; \nborder-radius: " + QString::number(R/2) + "px; \n " + color);
    }
}


// скрывание/раскрывание панели с настройками текста
void Note::roleUp()
{
    if (sender() != ui->btn_RoleUp)
        ui->btn_RoleUp->setChecked(!ui->btn_RoleUp->isChecked());

    if (ui->btn_RoleUp->isChecked())
    {
        ui->btn_RoleUp->setText("<");
        for (int i = 0; i < 4; i++)
        {
            btns[i]->hide();
        }
        ui->fontComboBox->hide();
        ui->spinBox->hide();
        ui->list_btn->hide();
        ui->t_btn->hide();
    }
    else
    {
        ui->btn_RoleUp->setText(">");
        for (int i = 0; i < 4; i++)
        {
            btns[i]->show();
        }
        ui->fontComboBox->show();
        ui->spinBox->show();
        ui->list_btn->show();
        ui->t_btn->show();
    }
}


// изменение цвета "звездочки"
void Note::colorChange()
{
    if (c1 == 2)
        c1 = 0;
    else
        c1++;

    QString s = (c1 == 0 ? "border: 1px " + (cm == 0 ? QString("solid grey") : QString("solid black")) + "; \n " : "");
    ui->color_btn->setStyleSheet(s + "border-radius: 5px; \n background-color: rgb" + colors[c1] + ";");
}


void Note::translation()
{
    QString s = ui->textEdit_2->textCursor().selectedText();
    QString s1 = "";
    QString h = "qwertyuiop[]asdfghjkl;'\\zxcvbnm,./";
    QString h1 = "йцукенгшщзхъфывапролджэ\\ячсмитьбю.";

    for (int i = 0; i < s.length(); i++)
    {
        for (int j = 0; j < h.length(); j++)
        {
            if (s[i] == h[j])
                s1 += h1[j];
        }

        if (s1.length() < i+1)
            s1 += s[i];
    }

    ui->textEdit_2->setText(ui->textEdit_2->toPlainText().replace(ui->textEdit_2->textCursor().selectionStart(), s.length(), s1));

}



void Note::setList()
{
//    if (tf)
//    {
//        ui->textEdit_2->insertHtml("<ul><li>");
//        tf = false;
//    }
//    else
//    {
//        ui->textEdit_2->insertHtml("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\
//                                   <html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\
//                                   p, li { white-space: pre-wrap; }\
//                                   </style></head><body style=\" font-family:'Arial'; font-size:12pt; font-weight:400; font-style:normal;\">\
//                                   <p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\
//                                     -qt-block-indent:0; text-indent:0px; font-family:'MS Shell Dlg 2'; font-size:8.25pt;\"><br /></p></body></html>");
//        tf = true;
//    }
    QTextDocument* document = ui->textEdit_2->document();
    QTextCursor* cursor = new QTextCursor(document);

    QTextListFormat listFormat;
    listFormat.setStyle(QTextListFormat::ListDisc);
    cursor->insertList(listFormat);

    cursor->insertText("");
//    cursor->insertText("\ntwo");
//    cursor->insertText("\nthree");


}

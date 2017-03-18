/********************************************************************************
** Form generated from reading UI file 'unitinfowindow.ui'
**
** Created: Fri Mar 22 10:18:11 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UNITINFOWINDOW_H
#define UI_UNITINFOWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QSpacerItem>
#include <QtGui/QTableWidget>
#include <QtGui/QToolBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

namespace gui {

class Ui_UnitInfoWindow
{
public:
    QWidget *wdg_main_widget;
    QHBoxLayout *horizontalLayout_3;
    QScrollArea *scrollArea;
    QWidget *wdg_scroll_area_content;
    QVBoxLayout *wdg_scroll_area_content_layout;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QGridLayout *gridLayout_3;
    QSpacerItem *horizontalSpacer;
    QLabel *lbl_weight;
    QLabel *lbl_unit_value;
    QLabel *lbl_weight_value;
    QGroupBox *grp_outputs;
    QGridLayout *gridLayout_2;
    QLabel *lbl_output_pos;
    QLabel *lbl_output_neg;
    QLabel *lbl_output_pos_value;
    QLabel *lbl_output_neg_value;
    QLabel *lbl_unit;
    QSpacerItem *verticalSpacer_2;
    QVBoxLayout *horizontalLayout_2;
    QFrame *frm_unit_picture;
    QVBoxLayout *frm_cell_paint_layout;
    QLabel *lbl_unit_pixmap;
    QSpacerItem *verticalSpacer;
    QSpacerItem *verticalSpacer_3;
    QLabel *lbl_atom_symbol;
    QTableWidget *tbl_atom_symbol;
    QWidget *wdg_other_labels;
    QVBoxLayout *wdg_other_labels_layout;
    QToolBox *tbx_other_labels;
    QWidget *wdg_label_1_page;
    QVBoxLayout *wdg_label_1_page_layout;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *btn_close;

    void setupUi(QMainWindow *gui__UnitInfoWindow)
    {
        if (gui__UnitInfoWindow->objectName().isEmpty())
            gui__UnitInfoWindow->setObjectName(QString::fromUtf8("gui__UnitInfoWindow"));
        gui__UnitInfoWindow->resize(307, 497);
        gui__UnitInfoWindow->setMinimumSize(QSize(230, 100));
        gui__UnitInfoWindow->setMaximumSize(QSize(480, 16777215));
        gui__UnitInfoWindow->setStyleSheet(QString::fromUtf8("QWidget#wdg_scroll_area_content {\n"
"	background-color:  #F5F5F5;\n"
"}\n"
""));
        wdg_main_widget = new QWidget(gui__UnitInfoWindow);
        wdg_main_widget->setObjectName(QString::fromUtf8("wdg_main_widget"));
        horizontalLayout_3 = new QHBoxLayout(wdg_main_widget);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        scrollArea = new QScrollArea(wdg_main_widget);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setFrameShadow(QFrame::Plain);
        scrollArea->setWidgetResizable(true);
        wdg_scroll_area_content = new QWidget();
        wdg_scroll_area_content->setObjectName(QString::fromUtf8("wdg_scroll_area_content"));
        wdg_scroll_area_content->setGeometry(QRect(0, 0, 307, 497));
        wdg_scroll_area_content_layout = new QVBoxLayout(wdg_scroll_area_content);
        wdg_scroll_area_content_layout->setContentsMargins(6, 6, 6, 6);
        wdg_scroll_area_content_layout->setObjectName(QString::fromUtf8("wdg_scroll_area_content_layout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(-1, 6, -1, -1);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setHorizontalSpacing(15);
        gridLayout_3->setVerticalSpacing(6);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer, 2, 2, 1, 1);

        lbl_weight = new QLabel(wdg_scroll_area_content);
        lbl_weight->setObjectName(QString::fromUtf8("lbl_weight"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        lbl_weight->setFont(font);
        lbl_weight->setStyleSheet(QString::fromUtf8("color: #4B4B4B;"));

        gridLayout_3->addWidget(lbl_weight, 2, 0, 1, 1);

        lbl_unit_value = new QLabel(wdg_scroll_area_content);
        lbl_unit_value->setObjectName(QString::fromUtf8("lbl_unit_value"));

        gridLayout_3->addWidget(lbl_unit_value, 1, 1, 1, 1);

        lbl_weight_value = new QLabel(wdg_scroll_area_content);
        lbl_weight_value->setObjectName(QString::fromUtf8("lbl_weight_value"));

        gridLayout_3->addWidget(lbl_weight_value, 2, 1, 1, 1);

        grp_outputs = new QGroupBox(wdg_scroll_area_content);
        grp_outputs->setObjectName(QString::fromUtf8("grp_outputs"));
        grp_outputs->setStyleSheet(QString::fromUtf8("QGroupBox::tab {\n"
"	color: #4B4B4B;\n"
"}"));
        gridLayout_2 = new QGridLayout(grp_outputs);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setHorizontalSpacing(10);
        gridLayout_2->setContentsMargins(0, 9, 0, 3);
        lbl_output_pos = new QLabel(grp_outputs);
        lbl_output_pos->setObjectName(QString::fromUtf8("lbl_output_pos"));
        lbl_output_pos->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_2->addWidget(lbl_output_pos, 0, 0, 1, 1);

        lbl_output_neg = new QLabel(grp_outputs);
        lbl_output_neg->setObjectName(QString::fromUtf8("lbl_output_neg"));
        lbl_output_neg->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_2->addWidget(lbl_output_neg, 1, 0, 1, 1);

        lbl_output_pos_value = new QLabel(grp_outputs);
        lbl_output_pos_value->setObjectName(QString::fromUtf8("lbl_output_pos_value"));

        gridLayout_2->addWidget(lbl_output_pos_value, 0, 1, 1, 1);

        lbl_output_neg_value = new QLabel(grp_outputs);
        lbl_output_neg_value->setObjectName(QString::fromUtf8("lbl_output_neg_value"));

        gridLayout_2->addWidget(lbl_output_neg_value, 1, 1, 1, 1);


        gridLayout_3->addWidget(grp_outputs, 3, 0, 1, 2);

        lbl_unit = new QLabel(wdg_scroll_area_content);
        lbl_unit->setObjectName(QString::fromUtf8("lbl_unit"));
        lbl_unit->setFont(font);
        lbl_unit->setStyleSheet(QString::fromUtf8("color: #4B4B4B;"));

        gridLayout_3->addWidget(lbl_unit, 1, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_3->addItem(verticalSpacer_2, 3, 2, 1, 1);


        horizontalLayout->addLayout(gridLayout_3);

        horizontalLayout_2 = new QVBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        frm_unit_picture = new QFrame(wdg_scroll_area_content);
        frm_unit_picture->setObjectName(QString::fromUtf8("frm_unit_picture"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frm_unit_picture->sizePolicy().hasHeightForWidth());
        frm_unit_picture->setSizePolicy(sizePolicy);
        frm_unit_picture->setMinimumSize(QSize(80, 80));
        frm_unit_picture->setFrameShape(QFrame::Box);
        frm_unit_picture->setFrameShadow(QFrame::Plain);
        frm_cell_paint_layout = new QVBoxLayout(frm_unit_picture);
        frm_cell_paint_layout->setSpacing(0);
        frm_cell_paint_layout->setContentsMargins(0, 0, 0, 0);
        frm_cell_paint_layout->setObjectName(QString::fromUtf8("frm_cell_paint_layout"));
        lbl_unit_pixmap = new QLabel(frm_unit_picture);
        lbl_unit_pixmap->setObjectName(QString::fromUtf8("lbl_unit_pixmap"));
        lbl_unit_pixmap->setStyleSheet(QString::fromUtf8("background-color: #FFFFFF;"));
        lbl_unit_pixmap->setAlignment(Qt::AlignCenter);
        lbl_unit_pixmap->setIndent(0);

        frm_cell_paint_layout->addWidget(lbl_unit_pixmap);


        horizontalLayout_2->addWidget(frm_unit_picture);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(verticalSpacer);

        horizontalLayout_2->setStretch(1, 1);

        horizontalLayout->addLayout(horizontalLayout_2);

        horizontalLayout->setStretch(0, 1);

        verticalLayout->addLayout(horizontalLayout);

        verticalSpacer_3 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_3);

        verticalLayout->setStretch(1, 1);

        wdg_scroll_area_content_layout->addLayout(verticalLayout);

        lbl_atom_symbol = new QLabel(wdg_scroll_area_content);
        lbl_atom_symbol->setObjectName(QString::fromUtf8("lbl_atom_symbol"));
        lbl_atom_symbol->setFont(font);
        lbl_atom_symbol->setStyleSheet(QString::fromUtf8("color: #4B4B4B;"));

        wdg_scroll_area_content_layout->addWidget(lbl_atom_symbol);

        tbl_atom_symbol = new QTableWidget(wdg_scroll_area_content);
        if (tbl_atom_symbol->columnCount() < 2)
            tbl_atom_symbol->setColumnCount(2);
        if (tbl_atom_symbol->rowCount() < 2)
            tbl_atom_symbol->setRowCount(2);
        tbl_atom_symbol->setObjectName(QString::fromUtf8("tbl_atom_symbol"));
        tbl_atom_symbol->setFrameShape(QFrame::Box);
        tbl_atom_symbol->setFrameShadow(QFrame::Plain);
        tbl_atom_symbol->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        tbl_atom_symbol->setShowGrid(true);
        tbl_atom_symbol->setWordWrap(false);
        tbl_atom_symbol->setRowCount(2);
        tbl_atom_symbol->setColumnCount(2);
        tbl_atom_symbol->horizontalHeader()->setHighlightSections(false);
        tbl_atom_symbol->horizontalHeader()->setStretchLastSection(true);
        tbl_atom_symbol->verticalHeader()->setVisible(false);
        tbl_atom_symbol->verticalHeader()->setHighlightSections(false);

        wdg_scroll_area_content_layout->addWidget(tbl_atom_symbol);

        wdg_other_labels = new QWidget(wdg_scroll_area_content);
        wdg_other_labels->setObjectName(QString::fromUtf8("wdg_other_labels"));
        wdg_other_labels->setStyleSheet(QString::fromUtf8("QToolBox::tab {\n"
"	background: qlineargradient(\n"
"			x1: 0, y1: 0, x2: 0, y2: 1, \n"
"			stop: 0 #E1E1E1, stop: 0.4 #DDDDDD, \n"
"			stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);\n"
"	border-radius: 3px;\n"
"	color: #4B4B4B;\n"
" }\n"
"\n"
"QToolBox::tab:pressed {\n"
"	background: darkgray;\n"
" }\n"
"\n"
""));
        wdg_other_labels_layout = new QVBoxLayout(wdg_other_labels);
        wdg_other_labels_layout->setSpacing(0);
        wdg_other_labels_layout->setContentsMargins(0, 0, 0, 0);
        wdg_other_labels_layout->setObjectName(QString::fromUtf8("wdg_other_labels_layout"));
        tbx_other_labels = new QToolBox(wdg_other_labels);
        tbx_other_labels->setObjectName(QString::fromUtf8("tbx_other_labels"));
        tbx_other_labels->setFont(font);
        wdg_label_1_page = new QWidget();
        wdg_label_1_page->setObjectName(QString::fromUtf8("wdg_label_1_page"));
        wdg_label_1_page->setGeometry(QRect(0, 0, 295, 79));
        wdg_label_1_page_layout = new QVBoxLayout(wdg_label_1_page);
        wdg_label_1_page_layout->setSpacing(0);
        wdg_label_1_page_layout->setContentsMargins(0, 0, 0, 0);
        wdg_label_1_page_layout->setObjectName(QString::fromUtf8("wdg_label_1_page_layout"));
        tbx_other_labels->addItem(wdg_label_1_page, QString::fromUtf8("Page 1"));

        wdg_other_labels_layout->addWidget(tbx_other_labels);


        wdg_scroll_area_content_layout->addWidget(wdg_other_labels);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(-1, 0, -1, -1);
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);

        btn_close = new QPushButton(wdg_scroll_area_content);
        btn_close->setObjectName(QString::fromUtf8("btn_close"));

        horizontalLayout_4->addWidget(btn_close);


        wdg_scroll_area_content_layout->addLayout(horizontalLayout_4);

        wdg_scroll_area_content_layout->setStretch(3, 1);
        scrollArea->setWidget(wdg_scroll_area_content);

        horizontalLayout_3->addWidget(scrollArea);

        gui__UnitInfoWindow->setCentralWidget(wdg_main_widget);

        retranslateUi(gui__UnitInfoWindow);
        QObject::connect(btn_close, SIGNAL(clicked()), gui__UnitInfoWindow, SLOT(close()));

        tbx_other_labels->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(gui__UnitInfoWindow);
    } // setupUi

    void retranslateUi(QMainWindow *gui__UnitInfoWindow)
    {
        lbl_weight->setText(QApplication::translate("gui::UnitInfoWindow", "Weight", 0, QApplication::UnicodeUTF8));
        lbl_unit_value->setText(QApplication::translate("gui::UnitInfoWindow", "(16, 22)", 0, QApplication::UnicodeUTF8));
        lbl_weight_value->setText(QApplication::translate("gui::UnitInfoWindow", "23.12423", 0, QApplication::UnicodeUTF8));
        grp_outputs->setTitle(QApplication::translate("gui::UnitInfoWindow", "Outputs", 0, QApplication::UnicodeUTF8));
        lbl_output_pos->setText(QApplication::translate("gui::UnitInfoWindow", "Positive", 0, QApplication::UnicodeUTF8));
        lbl_output_neg->setText(QApplication::translate("gui::UnitInfoWindow", "Negative", 0, QApplication::UnicodeUTF8));
        lbl_output_pos_value->setText(QApplication::translate("gui::UnitInfoWindow", "123 (90%)", 0, QApplication::UnicodeUTF8));
        lbl_output_neg_value->setText(QApplication::translate("gui::UnitInfoWindow", "12 (10%)", 0, QApplication::UnicodeUTF8));
        lbl_unit->setText(QApplication::translate("gui::UnitInfoWindow", "Unit", 0, QApplication::UnicodeUTF8));
        lbl_atom_symbol->setText(QApplication::translate("gui::UnitInfoWindow", "Atom Symbol", 0, QApplication::UnicodeUTF8));
        tbx_other_labels->setItemText(tbx_other_labels->indexOf(wdg_label_1_page), QApplication::translate("gui::UnitInfoWindow", "Page 1", 0, QApplication::UnicodeUTF8));
        btn_close->setText(QApplication::translate("gui::UnitInfoWindow", "Close", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(gui__UnitInfoWindow);
    } // retranslateUi

};

} // namespace gui

namespace gui {
namespace Ui {
    class UnitInfoWindow: public Ui_UnitInfoWindow {};
} // namespace Ui
} // namespace gui

#endif // UI_UNITINFOWINDOW_H

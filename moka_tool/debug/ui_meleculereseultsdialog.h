/********************************************************************************
** Form generated from reading UI file 'meleculereseultsdialog.ui'
**
** Created: Fri Mar 22 10:18:11 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MELECULERESEULTSDIALOG_H
#define UI_MELECULERESEULTSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MeleculeReseultsDialog
{
public:
    QHBoxLayout *horizontalLayout;
    QWidget *wdg_main_content;
    QVBoxLayout *verticalLayout;
    QLabel *lbl_molecule_id;
    QLabel *lbl_molecule_svg;
    QTableWidget *tbl_results;
    QLabel *label;

    void setupUi(QDialog *MeleculeReseultsDialog)
    {
        if (MeleculeReseultsDialog->objectName().isEmpty())
            MeleculeReseultsDialog->setObjectName(QString::fromUtf8("MeleculeReseultsDialog"));
        MeleculeReseultsDialog->resize(755, 480);
        MeleculeReseultsDialog->setStyleSheet(QString::fromUtf8("QWidget#wdg_main_content {\n"
"	background-color:  #F5F5F5;\n"
"}\n"
""));
        horizontalLayout = new QHBoxLayout(MeleculeReseultsDialog);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        wdg_main_content = new QWidget(MeleculeReseultsDialog);
        wdg_main_content->setObjectName(QString::fromUtf8("wdg_main_content"));
        verticalLayout = new QVBoxLayout(wdg_main_content);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        lbl_molecule_id = new QLabel(wdg_main_content);
        lbl_molecule_id->setObjectName(QString::fromUtf8("lbl_molecule_id"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        lbl_molecule_id->setFont(font);
        lbl_molecule_id->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(lbl_molecule_id);

        lbl_molecule_svg = new QLabel(wdg_main_content);
        lbl_molecule_svg->setObjectName(QString::fromUtf8("lbl_molecule_svg"));
        lbl_molecule_svg->setMinimumSize(QSize(0, 210));
        lbl_molecule_svg->setStyleSheet(QString::fromUtf8("background-color: #FFFFFF;"));
        lbl_molecule_svg->setFrameShape(QFrame::Box);
        lbl_molecule_svg->setPixmap(QPixmap(QString::fromUtf8(":/images/molecule.png")));
        lbl_molecule_svg->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(lbl_molecule_svg);

        tbl_results = new QTableWidget(wdg_main_content);
        if (tbl_results->columnCount() < 7)
            tbl_results->setColumnCount(7);
        if (tbl_results->rowCount() < 1)
            tbl_results->setRowCount(1);
        tbl_results->setObjectName(QString::fromUtf8("tbl_results"));
        tbl_results->setFrameShape(QFrame::Box);
        tbl_results->setFrameShadow(QFrame::Plain);
        tbl_results->setAlternatingRowColors(true);
        tbl_results->setShowGrid(true);
        tbl_results->setRowCount(1);
        tbl_results->setColumnCount(7);
        tbl_results->horizontalHeader()->setStretchLastSection(true);

        verticalLayout->addWidget(tbl_results);

        label = new QLabel(wdg_main_content);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);


        horizontalLayout->addWidget(wdg_main_content);


        retranslateUi(MeleculeReseultsDialog);

        QMetaObject::connectSlotsByName(MeleculeReseultsDialog);
    } // setupUi

    void retranslateUi(QDialog *MeleculeReseultsDialog)
    {
        MeleculeReseultsDialog->setWindowTitle(QApplication::translate("MeleculeReseultsDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        lbl_molecule_id->setText(QApplication::translate("MeleculeReseultsDialog", "MOLECULE ID", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MeleculeReseultsDialog", "Res = sign( ... ) = 0.032", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MeleculeReseultsDialog: public Ui_MeleculeReseultsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MELECULERESEULTSDIALOG_H

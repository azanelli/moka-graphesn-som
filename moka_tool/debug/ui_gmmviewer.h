/********************************************************************************
** Form generated from reading UI file 'gmmviewer.ui'
**
** Created: Fri Mar 22 10:18:11 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GMMVIEWER_H
#define UI_GMMVIEWER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

namespace gui {

class Ui_GMMViewer
{
public:
    QHBoxLayout *horizontalLayout;
    QWidget *wdg_model_view;
    QVBoxLayout *verticalLayout_2;
    QFrame *frm_model_info;
    QVBoxLayout *verticalLayout_3;
    QLabel *lbl_model_name;
    QLabel *lbl_model_info;
    QWidget *wdg_map_view;
    QHBoxLayout *wdg_map_view_vlayout;
    QLabel *lbl_wait_map;
    QFrame *frm_model_view_tools;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *btn_zoom_out;
    QPushButton *btn_zoom_in;
    QWidget *wdg_view_controls;
    QVBoxLayout *verticalLayout;
    QPushButton *btn_more_model_info;
    QSpacerItem *verticalSpacer;
    QPushButton *btn_close_map;

    void setupUi(QWidget *gui__GMMViewer)
    {
        if (gui__GMMViewer->objectName().isEmpty())
            gui__GMMViewer->setObjectName(QString::fromUtf8("gui__GMMViewer"));
        gui__GMMViewer->resize(701, 483);
        gui__GMMViewer->setStyleSheet(QString::fromUtf8("QWidget#wdg_view_controls {\n"
"	background-color: #FFFFFF;\n"
"	border-left: 1px solid #CECECE;\n"
"}\n"
"\n"
"QFrame#frm_model_info {\n"
"	background-color: #FFFFFF;\n"
"	border-bottom: 1px solid #CECECE;\n"
"}\n"
"\n"
"QFrame#frm_model_view_tools {\n"
"	background-color: #FFFFFF;\n"
"	border-top: 1px solid #CECECE;\n"
"}"));
        horizontalLayout = new QHBoxLayout(gui__GMMViewer);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        wdg_model_view = new QWidget(gui__GMMViewer);
        wdg_model_view->setObjectName(QString::fromUtf8("wdg_model_view"));
        verticalLayout_2 = new QVBoxLayout(wdg_model_view);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        frm_model_info = new QFrame(wdg_model_view);
        frm_model_info->setObjectName(QString::fromUtf8("frm_model_info"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frm_model_info->sizePolicy().hasHeightForWidth());
        frm_model_info->setSizePolicy(sizePolicy);
        frm_model_info->setFrameShape(QFrame::NoFrame);
        frm_model_info->setFrameShadow(QFrame::Plain);
        verticalLayout_3 = new QVBoxLayout(frm_model_info);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(6, 3, 6, 3);
        lbl_model_name = new QLabel(frm_model_info);
        lbl_model_name->setObjectName(QString::fromUtf8("lbl_model_name"));
        lbl_model_name->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(lbl_model_name);

        lbl_model_info = new QLabel(frm_model_info);
        lbl_model_info->setObjectName(QString::fromUtf8("lbl_model_info"));
        lbl_model_info->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(lbl_model_info);


        verticalLayout_2->addWidget(frm_model_info);

        wdg_map_view = new QWidget(wdg_model_view);
        wdg_map_view->setObjectName(QString::fromUtf8("wdg_map_view"));
        wdg_map_view_vlayout = new QHBoxLayout(wdg_map_view);
        wdg_map_view_vlayout->setSpacing(0);
        wdg_map_view_vlayout->setContentsMargins(0, 0, 0, 0);
        wdg_map_view_vlayout->setObjectName(QString::fromUtf8("wdg_map_view_vlayout"));
        lbl_wait_map = new QLabel(wdg_map_view);
        lbl_wait_map->setObjectName(QString::fromUtf8("lbl_wait_map"));
        lbl_wait_map->setAlignment(Qt::AlignCenter);

        wdg_map_view_vlayout->addWidget(lbl_wait_map);


        verticalLayout_2->addWidget(wdg_map_view);

        frm_model_view_tools = new QFrame(wdg_model_view);
        frm_model_view_tools->setObjectName(QString::fromUtf8("frm_model_view_tools"));
        sizePolicy.setHeightForWidth(frm_model_view_tools->sizePolicy().hasHeightForWidth());
        frm_model_view_tools->setSizePolicy(sizePolicy);
        frm_model_view_tools->setMinimumSize(QSize(0, 40));
        horizontalLayout_2 = new QHBoxLayout(frm_model_view_tools);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(6, 3, 6, 3);
        horizontalSpacer = new QSpacerItem(572, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        btn_zoom_out = new QPushButton(frm_model_view_tools);
        btn_zoom_out->setObjectName(QString::fromUtf8("btn_zoom_out"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(btn_zoom_out->sizePolicy().hasHeightForWidth());
        btn_zoom_out->setSizePolicy(sizePolicy1);
        btn_zoom_out->setMinimumSize(QSize(30, 30));
        btn_zoom_out->setMaximumSize(QSize(30, 30));
        btn_zoom_out->setFocusPolicy(Qt::TabFocus);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/zoom-out-32x32.png"), QSize(), QIcon::Normal, QIcon::Off);
        btn_zoom_out->setIcon(icon);
        btn_zoom_out->setIconSize(QSize(22, 22));
        btn_zoom_out->setAutoRepeat(true);

        horizontalLayout_2->addWidget(btn_zoom_out);

        btn_zoom_in = new QPushButton(frm_model_view_tools);
        btn_zoom_in->setObjectName(QString::fromUtf8("btn_zoom_in"));
        sizePolicy1.setHeightForWidth(btn_zoom_in->sizePolicy().hasHeightForWidth());
        btn_zoom_in->setSizePolicy(sizePolicy1);
        btn_zoom_in->setMinimumSize(QSize(30, 30));
        btn_zoom_in->setMaximumSize(QSize(30, 30));
        btn_zoom_in->setFocusPolicy(Qt::TabFocus);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/images/zoom-in-32x32.png"), QSize(), QIcon::Normal, QIcon::Off);
        btn_zoom_in->setIcon(icon1);
        btn_zoom_in->setIconSize(QSize(22, 22));
        btn_zoom_in->setAutoRepeat(true);

        horizontalLayout_2->addWidget(btn_zoom_in);


        verticalLayout_2->addWidget(frm_model_view_tools);


        horizontalLayout->addWidget(wdg_model_view);

        wdg_view_controls = new QWidget(gui__GMMViewer);
        wdg_view_controls->setObjectName(QString::fromUtf8("wdg_view_controls"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(wdg_view_controls->sizePolicy().hasHeightForWidth());
        wdg_view_controls->setSizePolicy(sizePolicy2);
        wdg_view_controls->setMinimumSize(QSize(42, 0));
        wdg_view_controls->setMaximumSize(QSize(42, 16777215));
        verticalLayout = new QVBoxLayout(wdg_view_controls);
        verticalLayout->setContentsMargins(6, 6, 6, 6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        btn_more_model_info = new QPushButton(wdg_view_controls);
        btn_more_model_info->setObjectName(QString::fromUtf8("btn_more_model_info"));
        sizePolicy1.setHeightForWidth(btn_more_model_info->sizePolicy().hasHeightForWidth());
        btn_more_model_info->setSizePolicy(sizePolicy1);
        btn_more_model_info->setMinimumSize(QSize(30, 30));
        btn_more_model_info->setMaximumSize(QSize(30, 30));
        btn_more_model_info->setFocusPolicy(Qt::TabFocus);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/images/table-blue-32x32.png"), QSize(), QIcon::Normal, QIcon::Off);
        btn_more_model_info->setIcon(icon2);
        btn_more_model_info->setIconSize(QSize(22, 22));

        verticalLayout->addWidget(btn_more_model_info);

        verticalSpacer = new QSpacerItem(20, 390, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        btn_close_map = new QPushButton(wdg_view_controls);
        btn_close_map->setObjectName(QString::fromUtf8("btn_close_map"));
        sizePolicy1.setHeightForWidth(btn_close_map->sizePolicy().hasHeightForWidth());
        btn_close_map->setSizePolicy(sizePolicy1);
        btn_close_map->setMinimumSize(QSize(30, 30));
        btn_close_map->setMaximumSize(QSize(30, 30));
        btn_close_map->setFocusPolicy(Qt::TabFocus);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/images/window-close-24x24.png"), QSize(), QIcon::Normal, QIcon::Off);
        btn_close_map->setIcon(icon3);
        btn_close_map->setIconSize(QSize(18, 18));

        verticalLayout->addWidget(btn_close_map);


        horizontalLayout->addWidget(wdg_view_controls);


        retranslateUi(gui__GMMViewer);

        QMetaObject::connectSlotsByName(gui__GMMViewer);
    } // setupUi

    void retranslateUi(QWidget *gui__GMMViewer)
    {
        lbl_model_name->setText(QApplication::translate("gui::GMMViewer", "Model name", 0, QApplication::UnicodeUTF8));
        lbl_model_info->setText(QApplication::translate("gui::GMMViewer", "Model quick info", 0, QApplication::UnicodeUTF8));
        lbl_wait_map->setText(QApplication::translate("gui::GMMViewer", "Loading...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        btn_more_model_info->setToolTip(QApplication::translate("gui::GMMViewer", "Show More Model Info", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        btn_close_map->setToolTip(QApplication::translate("gui::GMMViewer", "Close Model View", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        Q_UNUSED(gui__GMMViewer);
    } // retranslateUi

};

} // namespace gui

namespace gui {
namespace Ui {
    class GMMViewer: public Ui_GMMViewer {};
} // namespace Ui
} // namespace gui

#endif // UI_GMMVIEWER_H

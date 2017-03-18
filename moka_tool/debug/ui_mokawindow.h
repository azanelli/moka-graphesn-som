/********************************************************************************
** Form generated from reading UI file 'mokawindow.ui'
**
** Created: Fri Mar 22 10:18:11 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MOKAWINDOW_H
#define UI_MOKAWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

namespace gui {

class Ui_MokaWindow
{
public:
    QAction *act_exit;
    QAction *act_view_model;
    QWidget *wdg_main_widget;
    QHBoxLayout *wdg_main_widget_hlayout;
    QWidget *wdg_open_molecules_dock;
    QVBoxLayout *verticalLayout_2;
    QPushButton *btn_open_molecules_dock;
    QWidget *wdg_load;
    QHBoxLayout *horizontalLayout_2;
    QWidget *wdg_load_panel;
    QGridLayout *gridLayout;
    QSpacerItem *verticalSpacer_3;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *verticalSpacer_2;
    QLabel *lbl_load_model;
    QSpacerItem *horizontalSpacer;
    QLabel *lbl_load_unit_info;
    QLabel *lbl_load_dataset_config;
    QWidget *wdg_load_controls;
    QVBoxLayout *verticalLayout_3;
    QSpacerItem *verticalSpacer;
    QPushButton *btn_view_model;
    QMenuBar *menu_bar;
    QMenu *mnu_file;
    QMenu *menuView;
    QStatusBar *status_bar;
    QDockWidget *dck_molecules;
    QWidget *wdg_molecules;
    QVBoxLayout *verticalLayout;
    QListWidget *lst_molecules_list;
    QWidget *wdg_molecules_controls;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *btn_molecules_open;
    QPushButton *btn_molecules_calculate;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QMainWindow *gui__MokaWindow)
    {
        if (gui__MokaWindow->objectName().isEmpty())
            gui__MokaWindow->setObjectName(QString::fromUtf8("gui__MokaWindow"));
        gui__MokaWindow->resize(748, 511);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/moka-logo-256x256.png"), QSize(), QIcon::Normal, QIcon::Off);
        gui__MokaWindow->setWindowIcon(icon);
        gui__MokaWindow->setStyleSheet(QString::fromUtf8("QMenuBar {\n"
"	background-color: qlineargradient(x1:0, y1:1, x2:0, y2:0, stop:0 #D1D1D1, stop:1 #E7E7E7);\n"
"	border-bottom-style: solid;\n"
"	border-bottom-width: 1px;\n"
"	border-bottom-color: #8A8A8A;\n"
"}\n"
"\n"
"QMenuBar::item {\n"
"	spacing: 3px; /* spacing between menu bar items */\n"
"	padding: 2px 4px;\n"
"	background: transparent;\n"
"	border-top-right-radius: 2px;\n"
"	border-top-left-radius: 2px;\n"
"}\n"
"\n"
"QMenuBar::item:pressed {\n"
"	background: #717171;\n"
"	color: #FFFFFF; \n"
"	border-style: solid;\n"
"	border-width: 1px;\n"
"	border-color: #313131;\n"
"	border-bottom-style: none;\n"
"}\n"
"\n"
"QMenu {\n"
"	background: #717171;\n"
"	color: #FFFFFF; 	\n"
"	border-style: solid;\n"
"	border-width: 1px;\n"
"	border-color: #313131;\n"
"}\n"
"\n"
"QMenu::item:selected {\n"
"	background: #404040;\n"
"}\n"
"\n"
"\n"
"QMenu::item:disabled {\n"
"	color: #B7B7B7; \n"
"	background: #717171;\n"
"}\n"
"\n"
"/*\n"
"QMenu::item:disabled:selected { \n"
"	background: transparent;\n"
"}\n"
"*/\n"
"\n"
""
                        "QWidget#wdg_map_view, QWidget#wdg_molecules {\n"
"	background-color: #F5F5F5;\n"
"}\n"
"\n"
"QDockWidget {\n"
"	border: 1px solid lightgray;\n"
"	color: #808080;\n"
"}\n"
"\n"
"QDockWidget::title {\n"
"	text-align: left;\n"
"	background: #E8E8E8;\n"
"	/* padding-left: 5px; */\n"
"	padding: 5px;\n"
"}\n"
""));
        act_exit = new QAction(gui__MokaWindow);
        act_exit->setObjectName(QString::fromUtf8("act_exit"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/images/application-exit-32x32.png"), QSize(), QIcon::Normal, QIcon::Off);
        act_exit->setIcon(icon1);
        act_view_model = new QAction(gui__MokaWindow);
        act_view_model->setObjectName(QString::fromUtf8("act_view_model"));
        act_view_model->setEnabled(false);
        wdg_main_widget = new QWidget(gui__MokaWindow);
        wdg_main_widget->setObjectName(QString::fromUtf8("wdg_main_widget"));
        wdg_main_widget_hlayout = new QHBoxLayout(wdg_main_widget);
        wdg_main_widget_hlayout->setSpacing(0);
        wdg_main_widget_hlayout->setContentsMargins(0, 0, 0, 0);
        wdg_main_widget_hlayout->setObjectName(QString::fromUtf8("wdg_main_widget_hlayout"));
        wdg_open_molecules_dock = new QWidget(wdg_main_widget);
        wdg_open_molecules_dock->setObjectName(QString::fromUtf8("wdg_open_molecules_dock"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(wdg_open_molecules_dock->sizePolicy().hasHeightForWidth());
        wdg_open_molecules_dock->setSizePolicy(sizePolicy);
        wdg_open_molecules_dock->setMinimumSize(QSize(12, 0));
        wdg_open_molecules_dock->setMaximumSize(QSize(12, 16777215));
        wdg_open_molecules_dock->setCursor(QCursor(Qt::PointingHandCursor));
        wdg_open_molecules_dock->setStyleSheet(QString::fromUtf8("QPushButton#btn_open_molecules_dock {\n"
"	background-color: #FFFFFF;\n"
"	border-right: 1px solid #CECECE;\n"
"	border-style: outset;\n"
"}\n"
"\n"
"QPushButton#btn_open_molecules_dock:pressed {\n"
"	background-color: #D1D1D1;\n"
"	/* border-style: inset; */\n"
"}\n"
"\n"
""));
        verticalLayout_2 = new QVBoxLayout(wdg_open_molecules_dock);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        btn_open_molecules_dock = new QPushButton(wdg_open_molecules_dock);
        btn_open_molecules_dock->setObjectName(QString::fromUtf8("btn_open_molecules_dock"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(btn_open_molecules_dock->sizePolicy().hasHeightForWidth());
        btn_open_molecules_dock->setSizePolicy(sizePolicy1);
        btn_open_molecules_dock->setFocusPolicy(Qt::TabFocus);

        verticalLayout_2->addWidget(btn_open_molecules_dock);


        wdg_main_widget_hlayout->addWidget(wdg_open_molecules_dock);

        wdg_load = new QWidget(wdg_main_widget);
        wdg_load->setObjectName(QString::fromUtf8("wdg_load"));
        wdg_load->setStyleSheet(QString::fromUtf8("QWidget#wdg_load_controls {\n"
"	background-color: #FFFFFF;\n"
"	border-left: 1px solid #CECECE;\n"
"}\n"
"\n"
"QWidget#wdg_load_panel {\n"
"	background-color: #F5F5F5;\n"
"}\n"
""));
        horizontalLayout_2 = new QHBoxLayout(wdg_load);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        wdg_load_panel = new QWidget(wdg_load);
        wdg_load_panel->setObjectName(QString::fromUtf8("wdg_load_panel"));
        wdg_load_panel->setStyleSheet(QString::fromUtf8("QLabel {\n"
"	background-color: transparent;\n"
"	color: #CECECE; \n"
"	border: 3px dashed #CECECE;\n"
"	border-radius: 10px;\n"
"	/* min-width: 200px;  */\n"
"	/* min-height: 120px; */\n"
"	/* padding: 60px; */\n"
"	font: bold;\n"
"}\n"
""));
        gridLayout = new QGridLayout(wdg_load_panel);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(6, 6, 6, 6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setVerticalSpacing(24);
        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_3, 4, 1, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 1, 2, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 0, 1, 1, 1);

        lbl_load_model = new QLabel(wdg_load_panel);
        lbl_load_model->setObjectName(QString::fromUtf8("lbl_load_model"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(lbl_load_model->sizePolicy().hasHeightForWidth());
        lbl_load_model->setSizePolicy(sizePolicy2);
        lbl_load_model->setMinimumSize(QSize(200, 120));
        lbl_load_model->setCursor(QCursor(Qt::PointingHandCursor));
        lbl_load_model->setAlignment(Qt::AlignCenter);
        lbl_load_model->setWordWrap(true);
        lbl_load_model->setMargin(10);
        lbl_load_model->setIndent(0);

        gridLayout->addWidget(lbl_load_model, 1, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 1, 0, 1, 1);

        lbl_load_unit_info = new QLabel(wdg_load_panel);
        lbl_load_unit_info->setObjectName(QString::fromUtf8("lbl_load_unit_info"));
        sizePolicy2.setHeightForWidth(lbl_load_unit_info->sizePolicy().hasHeightForWidth());
        lbl_load_unit_info->setSizePolicy(sizePolicy2);
        lbl_load_unit_info->setMinimumSize(QSize(200, 120));
        lbl_load_unit_info->setCursor(QCursor(Qt::PointingHandCursor));
        lbl_load_unit_info->setAlignment(Qt::AlignCenter);
        lbl_load_unit_info->setWordWrap(true);
        lbl_load_unit_info->setMargin(10);
        lbl_load_unit_info->setIndent(0);

        gridLayout->addWidget(lbl_load_unit_info, 2, 1, 1, 1);

        lbl_load_dataset_config = new QLabel(wdg_load_panel);
        lbl_load_dataset_config->setObjectName(QString::fromUtf8("lbl_load_dataset_config"));
        sizePolicy2.setHeightForWidth(lbl_load_dataset_config->sizePolicy().hasHeightForWidth());
        lbl_load_dataset_config->setSizePolicy(sizePolicy2);
        lbl_load_dataset_config->setMinimumSize(QSize(200, 120));
        lbl_load_dataset_config->setCursor(QCursor(Qt::PointingHandCursor));
        lbl_load_dataset_config->setAlignment(Qt::AlignCenter);
        lbl_load_dataset_config->setWordWrap(true);
        lbl_load_dataset_config->setMargin(10);
        lbl_load_dataset_config->setIndent(0);

        gridLayout->addWidget(lbl_load_dataset_config, 3, 1, 1, 1);

        gridLayout->setColumnStretch(1, 1);

        horizontalLayout_2->addWidget(wdg_load_panel);

        wdg_load_controls = new QWidget(wdg_load);
        wdg_load_controls->setObjectName(QString::fromUtf8("wdg_load_controls"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(wdg_load_controls->sizePolicy().hasHeightForWidth());
        wdg_load_controls->setSizePolicy(sizePolicy3);
        wdg_load_controls->setMinimumSize(QSize(42, 0));
        wdg_load_controls->setMaximumSize(QSize(42, 16777215));
        verticalLayout_3 = new QVBoxLayout(wdg_load_controls);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(6, 6, 6, 6);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalSpacer = new QSpacerItem(20, 362, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

        btn_view_model = new QPushButton(wdg_load_controls);
        btn_view_model->setObjectName(QString::fromUtf8("btn_view_model"));
        btn_view_model->setEnabled(false);
        QSizePolicy sizePolicy4(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(btn_view_model->sizePolicy().hasHeightForWidth());
        btn_view_model->setSizePolicy(sizePolicy4);
        btn_view_model->setMaximumSize(QSize(30, 30));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/images/map-32x32.png"), QSize(), QIcon::Normal, QIcon::Off);
        btn_view_model->setIcon(icon2);
        btn_view_model->setIconSize(QSize(22, 22));

        verticalLayout_3->addWidget(btn_view_model);


        horizontalLayout_2->addWidget(wdg_load_controls);


        wdg_main_widget_hlayout->addWidget(wdg_load);

        gui__MokaWindow->setCentralWidget(wdg_main_widget);
        menu_bar = new QMenuBar(gui__MokaWindow);
        menu_bar->setObjectName(QString::fromUtf8("menu_bar"));
        menu_bar->setGeometry(QRect(0, 0, 748, 21));
        mnu_file = new QMenu(menu_bar);
        mnu_file->setObjectName(QString::fromUtf8("mnu_file"));
        menuView = new QMenu(menu_bar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        gui__MokaWindow->setMenuBar(menu_bar);
        status_bar = new QStatusBar(gui__MokaWindow);
        status_bar->setObjectName(QString::fromUtf8("status_bar"));
        gui__MokaWindow->setStatusBar(status_bar);
        dck_molecules = new QDockWidget(gui__MokaWindow);
        dck_molecules->setObjectName(QString::fromUtf8("dck_molecules"));
        dck_molecules->setFeatures(QDockWidget::DockWidgetClosable);
        dck_molecules->setAllowedAreas(Qt::LeftDockWidgetArea);
        wdg_molecules = new QWidget();
        wdg_molecules->setObjectName(QString::fromUtf8("wdg_molecules"));
        wdg_molecules->setStyleSheet(QString::fromUtf8("QWidget#wdg_molecules_controls {\n"
"	background-color: #FFFFFF;\n"
"	border-top: 1px solid #CECECE;\n"
"}\n"
""));
        verticalLayout = new QVBoxLayout(wdg_molecules);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        lst_molecules_list = new QListWidget(wdg_molecules);
        lst_molecules_list->setObjectName(QString::fromUtf8("lst_molecules_list"));

        verticalLayout->addWidget(lst_molecules_list);

        wdg_molecules_controls = new QWidget(wdg_molecules);
        wdg_molecules_controls->setObjectName(QString::fromUtf8("wdg_molecules_controls"));
        sizePolicy2.setHeightForWidth(wdg_molecules_controls->sizePolicy().hasHeightForWidth());
        wdg_molecules_controls->setSizePolicy(sizePolicy2);
        wdg_molecules_controls->setMinimumSize(QSize(0, 40));
        horizontalLayout_3 = new QHBoxLayout(wdg_molecules_controls);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(6, 3, 6, 3);
        btn_molecules_open = new QPushButton(wdg_molecules_controls);
        btn_molecules_open->setObjectName(QString::fromUtf8("btn_molecules_open"));
        btn_molecules_open->setEnabled(false);
        sizePolicy4.setHeightForWidth(btn_molecules_open->sizePolicy().hasHeightForWidth());
        btn_molecules_open->setSizePolicy(sizePolicy4);
        btn_molecules_open->setMinimumSize(QSize(0, 0));
        btn_molecules_open->setMaximumSize(QSize(30, 30));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/images/document-open-32x32.png"), QSize(), QIcon::Normal, QIcon::Off);
        btn_molecules_open->setIcon(icon3);
        btn_molecules_open->setIconSize(QSize(22, 22));

        horizontalLayout_3->addWidget(btn_molecules_open);

        btn_molecules_calculate = new QPushButton(wdg_molecules_controls);
        btn_molecules_calculate->setObjectName(QString::fromUtf8("btn_molecules_calculate"));
        btn_molecules_calculate->setEnabled(false);
        sizePolicy4.setHeightForWidth(btn_molecules_calculate->sizePolicy().hasHeightForWidth());
        btn_molecules_calculate->setSizePolicy(sizePolicy4);
        btn_molecules_calculate->setMaximumSize(QSize(30, 30));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/images/system-run-32x32.png"), QSize(), QIcon::Normal, QIcon::Off);
        btn_molecules_calculate->setIcon(icon4);
        btn_molecules_calculate->setIconSize(QSize(22, 22));

        horizontalLayout_3->addWidget(btn_molecules_calculate);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout->addWidget(wdg_molecules_controls);

        dck_molecules->setWidget(wdg_molecules);
        gui__MokaWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(1), dck_molecules);

        menu_bar->addAction(mnu_file->menuAction());
        menu_bar->addAction(menuView->menuAction());
        mnu_file->addAction(act_exit);
        menuView->addAction(act_view_model);

        retranslateUi(gui__MokaWindow);
        QObject::connect(act_exit, SIGNAL(activated()), gui__MokaWindow, SLOT(close()));
        QObject::connect(btn_view_model, SIGNAL(clicked()), act_view_model, SLOT(trigger()));

        QMetaObject::connectSlotsByName(gui__MokaWindow);
    } // setupUi

    void retranslateUi(QMainWindow *gui__MokaWindow)
    {
        act_exit->setText(QApplication::translate("gui::MokaWindow", "Exit", 0, QApplication::UnicodeUTF8));
        act_exit->setShortcut(QApplication::translate("gui::MokaWindow", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        act_view_model->setText(QApplication::translate("gui::MokaWindow", "Model", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        act_view_model->setToolTip(QApplication::translate("gui::MokaWindow", "View Model", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        act_view_model->setShortcut(QApplication::translate("gui::MokaWindow", "Ctrl+M", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        wdg_open_molecules_dock->setToolTip(QApplication::translate("gui::MokaWindow", "SDF/MOL File", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        btn_open_molecules_dock->setText(QApplication::translate("gui::MokaWindow", ">", 0, QApplication::UnicodeUTF8));
        lbl_load_model->setText(QApplication::translate("gui::MokaWindow", "Load Model", 0, QApplication::UnicodeUTF8));
        lbl_load_unit_info->setText(QApplication::translate("gui::MokaWindow", "Load Unit Info", 0, QApplication::UnicodeUTF8));
        lbl_load_dataset_config->setText(QApplication::translate("gui::MokaWindow", "Load Dataset Configuration File", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        btn_view_model->setToolTip(QApplication::translate("gui::MokaWindow", "View Model", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        mnu_file->setTitle(QApplication::translate("gui::MokaWindow", "File", 0, QApplication::UnicodeUTF8));
        menuView->setTitle(QApplication::translate("gui::MokaWindow", "View", 0, QApplication::UnicodeUTF8));
        dck_molecules->setWindowTitle(QApplication::translate("gui::MokaWindow", "SDF/MOL File", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        btn_molecules_open->setToolTip(QApplication::translate("gui::MokaWindow", "Load SDF/MOL File", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        btn_molecules_calculate->setToolTip(QApplication::translate("gui::MokaWindow", "Calculate", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        Q_UNUSED(gui__MokaWindow);
    } // retranslateUi

};

} // namespace gui

namespace gui {
namespace Ui {
    class MokaWindow: public Ui_MokaWindow {};
} // namespace Ui
} // namespace gui

#endif // UI_MOKAWINDOW_H

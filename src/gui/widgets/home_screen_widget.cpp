/*
 *    Copyright 2012, 2013 Thomas Schöps, Kai Pastor
 *
 *    This file is part of OpenOrienteering.
 *
 *    OpenOrienteering is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    OpenOrienteering is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with OpenOrienteering.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "home_screen_widget.h"

#if QT_VERSION < 0x050000
#include <QtGui>
#else
#include <QtWidgets>
#endif

#include "../home_screen_controller.h"
#include "../main_window.h"


HomeScreenWidget::HomeScreenWidget(HomeScreenController* controller, QWidget* parent)
: QWidget(parent),
  controller(controller)
{
	Q_ASSERT(controller->getWindow() != NULL);
	
	QLabel* title_label = new QLabel(QString("<img src=\":/images/title.png\"/>"));
	title_label->setAlignment(Qt::AlignCenter);
	QWidget* menu_widget = makeMenuWidget(controller, parent);
	QWidget* recent_files_widget = makeRecentFilesWidget(controller, parent);
	QWidget* tips_widget = makeTipsWidget(controller, parent);
	
	QGridLayout* layout = new QGridLayout();
	layout->setSpacing(2 * layout->spacing());
	layout->addWidget(title_label, 0, 0, 1, 2);
	layout->addWidget(menu_widget, 1, 0, 2, 1);
	layout->addWidget(recent_files_widget, 1, 1);
	layout->setRowStretch(1, 4);
	layout->addWidget(tips_widget, 2, 1);
	layout->setRowStretch(2, 3);
	setLayout(layout);
	
	setAutoFillBackground(false);
}

HomeScreenWidget::~HomeScreenWidget()
{
	// nothing
}

void HomeScreenWidget::paintEvent(QPaintEvent* event)
{
	// Background
	QPainter p(this);
	p.setPen(Qt::NoPen);
	p.setBrush(Qt::gray);
	p.drawRect(rect());
}

QWidget* HomeScreenWidget::makeMenuWidget(HomeScreenController* controller, QWidget* parent)
{
	MainWindow* window = controller->getWindow();
	
	QVBoxLayout* menu_layout = new QVBoxLayout();
	
	QLabel* menu_headline = makeHeadline(tr("Activities"));
	menu_layout->addWidget(menu_headline);
	QAbstractButton* button_new_map = makeButton(
	  tr("Create a new map ..."), QIcon(":/images/new.png"));
	menu_layout->addWidget(button_new_map);
	QAbstractButton* button_open_map = makeButton(
	  tr("Open map ..."), QIcon(":/images/open.png"));
	menu_layout->addWidget(button_open_map);
	
	menu_layout->addStretch(1);
	
	QAbstractButton* button_settings = makeButton(
	  tr("Settings"), QIcon(":/images/settings.png"));
	menu_layout->addWidget(button_settings);
	QAbstractButton* button_about = makeButton(
	  tr("About %1", "As in 'About OpenOrienteering Mapper'").arg(window->appName()), QIcon(":/images/about.png"));
	menu_layout->addWidget(button_about);
	QAbstractButton* button_help = makeButton(
	  tr("Help"), QIcon(":/images/help.png"));
	menu_layout->addWidget(button_help);
	QAbstractButton* button_exit = makeButton(
	  tr("Exit")); // TODO: Exit icon
	menu_layout->addWidget(button_exit);
	
	connect(button_new_map, SIGNAL(clicked(bool)), window, SLOT(showNewMapWizard()));
	connect(button_open_map, SIGNAL(clicked(bool)), window, SLOT(showOpenDialog()));
	connect(button_settings, SIGNAL(clicked(bool)), window, SLOT(showSettings()));
	connect(button_about, SIGNAL(clicked(bool)), window, SLOT(showAbout()));
	connect(button_help, SIGNAL(clicked(bool)), window, SLOT(showHelp()));
	connect(button_exit, SIGNAL(clicked(bool)), qApp, SLOT(closeAllWindows()));
	
	QWidget* menu_widget = new QWidget();
	menu_widget->setLayout(menu_layout);
	menu_widget->setAutoFillBackground(true);
	return menu_widget;
}

QWidget* HomeScreenWidget::makeRecentFilesWidget(HomeScreenController* controller, QWidget* parent)
{
	QGridLayout* recent_files_layout = new QGridLayout();
	
	QLabel* recent_files_headline = makeHeadline(tr("Recent maps"));
	recent_files_layout->addWidget(recent_files_headline, 0, 0, 1, 2);
	
	recent_files_list = new QListWidget();
	QFont list_font = recent_files_list->font();
	list_font.setPointSize((int)list_font.pointSize()*1.5);
	recent_files_list->setFont(list_font);
	recent_files_list->setSpacing(list_font.pointSize()/2);
	recent_files_list->setWhatsThis("<a href=\"opening.html\">See more</a>");
	recent_files_list->setCursor(Qt::PointingHandCursor);
	recent_files_list->setStyleSheet(" \
	  QListWidget::item:hover { \
	    color: palette(highlighted-text); \
	    background: palette(highlight); \
	  } ");
	recent_files_layout->addWidget(recent_files_list, 1, 0, 1, 2);
	
	open_mru_file_check = new QCheckBox(tr("Open most recently used file on start"));
	open_mru_file_check->setWhatsThis("<a href=\"opening.html\">See more</a>");
	recent_files_layout->addWidget(open_mru_file_check, 2, 0, 1, 1);
	
	QPushButton* clear_list_button = new QPushButton(tr("Clear list"));
	clear_list_button->setWhatsThis("<a href=\"opening.html\">See more</a>");
	recent_files_layout->addWidget(clear_list_button, 2, 1, 1, 1);
	
	recent_files_layout->setRowStretch(1, 1);
	recent_files_layout->setColumnStretch(0, 1);
	
	connect(recent_files_list, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(recentFileClicked(QListWidgetItem*)));
	connect(open_mru_file_check, SIGNAL(clicked(bool)), controller, SLOT(setOpenMRUFile(bool)));
	connect(clear_list_button, SIGNAL(clicked(bool)), controller, SLOT(clearRecentFiles()));
	
	QWidget* recent_files_widget = new QWidget();
	recent_files_widget->setLayout(recent_files_layout);
	recent_files_widget->setAutoFillBackground(true);
	return recent_files_widget;
}

QWidget* HomeScreenWidget::makeTipsWidget(HomeScreenController* controller, QWidget* parent)
{
	QGridLayout* tips_layout = new QGridLayout();
	QWidget* tips_headline = makeHeadline(tr("Tip of the day"));
	tips_layout->addWidget(tips_headline, 0, 0, 1, 3);
	tips_label = new QLabel();
	tips_label->setWordWrap(true);
	tips_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	tips_check = new QCheckBox(tr("Show tip of the day"));
	tips_check->setChecked(true);
	tips_layout->addWidget(tips_check, 2, 0, 1, 1);
	tips_layout->addWidget(tips_label, 1, 0, 1, 3);
	QPushButton* prev_button = new QPushButton(QIcon(":/images/arrow-left.png"), tr("Previous"));
	tips_layout->addWidget(prev_button, 2, 1, 1, 1);
	QPushButton* next_button = new QPushButton(QIcon(":/images/arrow-right.png"), tr("Next"));
	tips_layout->addWidget(next_button, 2, 2, 1, 1);
	
	tips_layout->setRowStretch(1, 1);
	tips_layout->setColumnStretch(0, 1);
	
	tips_children.reserve(4);
	tips_children.push_back(tips_headline);
	tips_children.push_back(tips_label);
	tips_children.push_back(prev_button);
	tips_children.push_back(next_button);
	
	MainWindow* window = controller->getWindow();
	connect(tips_label, SIGNAL(linkActivated(QString)), window, SLOT(linkClicked(QString)));
	connect(tips_check, SIGNAL(clicked(bool)), controller, SLOT(setTipsVisible(bool)));
	connect(prev_button, SIGNAL(clicked(bool)), controller, SLOT(goToPreviousTip()));
	connect(next_button, SIGNAL(clicked(bool)), controller, SLOT(goToNextTip()));
	
	QWidget* tips_widget = new QWidget();
	tips_widget->setLayout(tips_layout);
	tips_widget->setAutoFillBackground(true);
	return tips_widget;
}

QLabel* HomeScreenWidget::makeHeadline(const QString& text, QWidget* parent) const
{
	QLabel* title_label = new QLabel(text, parent);
	QFont title_font = title_label->font();
	title_font.setPointSize(2 * title_font.pointSize());
	title_font.setBold(true);
	title_label->setFont(title_font);
	title_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	return title_label;
}

QAbstractButton* HomeScreenWidget::makeButton(const QString& text, QWidget* parent) const
{
	QAbstractButton* button = new QCommandLinkButton(text, parent);
	QFont button_font = button->font();
	button_font.setPointSize((int)(1.5 * button_font.pointSize()));
	button->setFont(button_font);
	return button;
}

QAbstractButton* HomeScreenWidget::makeButton(const QString& text, const QIcon& icon, QWidget* parent) const
{
	QAbstractButton* button = makeButton(text, parent);
	button->setIcon(icon);
	return button;
}

void HomeScreenWidget::setRecentFiles(const QStringList& files)
{
	recent_files_list->clear();
	Q_FOREACH(QString file, files)
	{
		QListWidgetItem* new_item = new QListWidgetItem(QFileInfo(file).fileName());
		new_item->setData(Qt::UserRole, file);
		new_item->setToolTip(file);
		recent_files_list->addItem(new_item);
	}
}

void HomeScreenWidget::recentFileClicked(QListWidgetItem* item)
{
	QString path = item->data(Qt::UserRole).toString();
	controller->getWindow()->openPath(path);
}

void HomeScreenWidget::setOpenMRUFileChecked(bool state)
{
	open_mru_file_check->setChecked(state);
}

void HomeScreenWidget::setTipOfTheDay(const QString& text)
{
	tips_label->setText(text);
}

void HomeScreenWidget::setTipsVisible(bool state)
{
	QGridLayout* layout = qobject_cast<QGridLayout*>(this->layout());
	Q_FOREACH(QWidget* widget, tips_children)
	{
		widget->setVisible(state);
	}
	if (layout != NULL)
		layout->setRowStretch(2, state ? 3 : 0);
	
	tips_check->setChecked(state);
}
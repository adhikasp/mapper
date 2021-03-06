/*
 *    Copyright 2012, 2013, 2014 Thomas Schöps
 *    Copyright 2012-2015 Kai Pastor
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

#ifndef _OPENORIENTEERING_TEXT_BROWSER_DIALOG_H_
#define _OPENORIENTEERING_TEXT_BROWSER_DIALOG_H_

#include <QDialog>

class QTextBrowser;
class QUrl;

/**
 * @brief A dialog for basic browsing of HTML pages.
 */
class TextBrowserDialog : public QDialog
{
Q_OBJECT
public:
	/**
	 * @brief Construct a new dialog.
	 */
	TextBrowserDialog(const QUrl& initial_url, QWidget* parent = nullptr);
	
protected slots:
	/**
	 * @brief Sets custom HTML content when the URL identifies the first page.
	 */
	virtual void sourceChanged(const QUrl& url);
	
	/**
	 * @brief Updates the window title from the current document title.
	 */
	virtual void updateWindowTitle();
	
	/**
	 * @brief Displays a tooltip showing the link if is an external document.
	 */
	void highlighted(const QString& link);
	
protected:
	/**
	 * @brief Returns a size hint based on the text browser's content.
	 */
	virtual QSize sizeHint() const override;
	
	/**
	 * @brief The TextBrowser is the main widget of this dialog.
	 */
	QTextBrowser* const text_browser;
};

#endif

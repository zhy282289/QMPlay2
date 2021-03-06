/*
	QMPlay2 is a video and audio player.
	Copyright (C) 2010-2016  Błażej Szczygieł

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published
	by the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ENTRYPROPERTIES_HPP
#define ENTRYPROPERTIES_HPP

#include <QDialog>

class QTreeWidgetItem;
class QToolButton;
class AddressBox;
class QLineEdit;
class QCheckBox;
#ifdef QMPlay2_TagEditor
	class TagEditor;
#endif
class QLabel;

class EntryProperties : public QDialog
{
	Q_OBJECT
public:
	EntryProperties(QWidget *, QTreeWidgetItem *, bool &, bool &);
private:
	QTreeWidgetItem *tWI;
	QLineEdit *nameE, *pthE;
#ifdef QMPlay2_TagEditor
	TagEditor *tagEditor;
#endif
	QLabel *fileSizeL;
	QCheckBox *catalogCB;
	QToolButton *browseDirB, *browseFileB;
	AddressBox *addrB;
	QString origDirPth;
	bool &sync;
private slots:
	void setDirPthEEnabled(int);
#ifdef QMPlay2_TagEditor
	void directAddressChanged();
#endif
	void browse();
	void accept();
	void reject();
};

#endif //ENTRYPROPERTIES_HPP

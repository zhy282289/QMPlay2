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

#ifndef PLAYLISTWIDGET_HPP
#define PLAYLISTWIDGET_HPP

#include <IOController.hpp>
#include <Functions.hpp>
#include <Playlist.hpp>

#include <QTreeWidget>
#include <QThread>
#include <QQueue>
#include <QMutex>
#include <QTimer>
#include <QUrl>

class QTreeWidgetItem;
class PlaylistWidget;
class Demuxer;

class UpdateEntryThr : public QThread
{
	Q_OBJECT
public:
	UpdateEntryThr(PlaylistWidget &pLW);

	void updateEntry(QTreeWidgetItem *item, const QString &name = QString(), double length = -2.0);

	void stop();
private:
	void run();

	IOController<> ioCtrl;
	PlaylistWidget &pLW;
	bool timeChanged;
	QMutex mutex;

	struct ItemToUpdate
	{
		QTreeWidgetItem *item;
		QString url;
		double oldLength;

		QString name;
		double length;
	};
	QQueue<ItemToUpdate> itemsToUpdate;

	struct ItemUpdated
	{
		QTreeWidgetItem *item;
		QString name;
		QImage img;

		bool updateLength, updateImg;
		double length;
	};
private slots:
	void updateItem(ItemUpdated iu);
	void finished();
};

class AddThr : public QThread
{
	Q_OBJECT
public:
	enum SYNC {NO_SYNC = 0, DIR_SYNC = 1, FILE_SYNC = 2};

	AddThr(PlaylistWidget &pLW);

	void setData(const QStringList &, QTreeWidgetItem *, bool, SYNC = NO_SYNC);
	void setDataForSync(const QString &, QTreeWidgetItem *, bool);

	void stop();
private slots:
	void changeItemText0(QTreeWidgetItem *tWI, QString name);
	void deleteTreeWidgetItem(QTreeWidgetItem *);
private:
	void run();

	bool add(const QStringList &urls, QTreeWidgetItem *parent, const Functions::DemuxersInfo &demuxersInfo, bool loadList = false);
	QTreeWidgetItem *insertPlaylistEntries(const Playlist::Entries &entries, QTreeWidgetItem *parent, const Functions::DemuxersInfo &demuxersInfo);

	PlaylistWidget &pLW;
	QStringList urls;
	QTreeWidgetItem *par;
	bool loadList;
	SYNC sync;
	IOController<> ioCtrl;
	QTreeWidgetItem *firstItem, *lastItem;
private slots:
	void finished();
};

class PlaylistWidget : public QTreeWidget
{
	friend class AddThr;
	friend class UpdateEntryThr;
	Q_OBJECT
public:
	enum CHILDREN {ONLY_GROUPS = 0x10, ONLY_NON_GROUPS = 0x100, ALL_CHILDREN = ONLY_GROUPS | ONLY_NON_GROUPS};
	enum REFRESH  {REFRESH_QUEUE = 0x10, REFRESH_GROUPS_TIME = 0x100, REFRESH_CURRPLAYING = 0x1000, REFRESH_ALL = REFRESH_QUEUE | REFRESH_GROUPS_TIME | REFRESH_CURRPLAYING};

	PlaylistWidget();

	QString getUrl(QTreeWidgetItem *tWI = NULL) const;

	void setItemsResizeToContents(bool);

	void sortCurrentGroup(int column, Qt::SortOrder sortOrder);

	bool add(const QStringList &, QTreeWidgetItem *par, bool loadList = false);
	bool add(const QStringList &, bool atEndOfList = false);
	void sync(const QString &, QTreeWidgetItem *, bool);

	void setCurrentPlaying(QTreeWidgetItem *tWI);

	void clear();
	void clearCurrentPlaying(bool url = true);

	void stopLoading();

	QList<QTreeWidgetItem *> topLevelNonGroupsItems() const;
	QList<QUrl> getUrls() const;

	QTreeWidgetItem *newGroup(const QString &name, const QString &url = QString(), QTreeWidgetItem *parent = NULL, bool insertChildAt0Idx = false);
	QTreeWidgetItem *newGroup();

	QTreeWidgetItem *newEntry(const Playlist::Entry &, QTreeWidgetItem *, const Functions::DemuxersInfo &);

	QList<QTreeWidgetItem *> getChildren(CHILDREN children = ALL_CHILDREN, const QTreeWidgetItem *parent = NULL) const;

	bool canModify(bool all = true) const;

	void enqueue();
	void refresh(REFRESH Refresh = REFRESH_ALL);

	void processItems(QList<QTreeWidgetItem *> *itemsToShow = NULL, bool hideGroups = false);

	QString currentPlayingUrl;
	QTreeWidgetItem *currentPlaying;
	QIcon currentPlayingItemIcon;

	QList<QTreeWidgetItem *> queue;

	AddThr addThr;
	UpdateEntryThr updateEntryThr;

	bool dontUpdateAfterAdd;

	static inline bool isGroup(QTreeWidgetItem *tWI)
	{
		return tWI ? (bool)(tWI->flags() & Qt::ItemIsDropEnabled) : false;
	}
private:
	void setEntryIcon(const QImage &, QTreeWidgetItem *);

	void mouseMoveEvent(QMouseEvent *);
	void dragEnterEvent(QDragEnterEvent *);
	void dragMoveEvent(QDragMoveEvent *);
	void dropEvent(QDropEvent *);
	void paintEvent(QPaintEvent *);
	void scrollContentsBy(int dx, int dy);

	QRect getArcRect(int size);

	bool internalDrag, selectAfterAdd, hasHiddenItems;
	QString currPthToSave;
	struct AddData
	{
		QStringList urls;
		QTreeWidgetItem *par;
		bool loadList;
	};
	QQueue<AddData> enqueuedAddData;
	QTimer animationTimer, addTimer;
	bool repaintAll;
	int rotation;
private slots:
	void insertItem(QTreeWidgetItem *, QTreeWidgetItem *, bool);
	void popupContextMenu(const QPoint &);
	void setItemIcon(QTreeWidgetItem *, const QImage &);
	void animationUpdate();
	void addTimerElapsed();
public slots:
	void modifyMenu();
signals:
	void returnItem(QTreeWidgetItem *);
	void visibleItemsCount(int);
};

#endif

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

#include <MenuBar.hpp>

#include <VideoAdjustment.hpp>
#include <DockWidget.hpp>
#include <Settings.hpp>
#include <Main.hpp>
#include <ShortcutHandler.hpp>

#include <QWidgetAction>
#include <QMainWindow>

static QAction *newAction(const QString &txt, QMenu *parent, QAction *&act, bool autoRepeat, const QIcon &icon, bool checkable)
{
	act = new QAction(txt, parent);
	act->setAutoRepeat(autoRepeat);
	act->setCheckable(checkable);
	parent->addAction(act);
	act->setIcon(icon);
	return act;
}

/**/

MenuBar::MenuBar()
{
	addMenu(window = new Window(this));
	addMenu(widgets = new Widgets(this));
	addMenu(playlist = new Playlist(this));
	addMenu(player = new Player(this));
	addMenu(playback = new Playback(this));
	addMenu(options = new Options(this));
	addMenu(help = new Help(this));
	connect(widgets, SIGNAL(aboutToShow()), this, SLOT(widgetsMenuShow()));
#ifdef Q_OS_MAC
	widgets->addAction(QString()); //Mac must have got at least one item inside menu, otherwise the menu is not shown (QTBUG?)
#endif
}

MenuBar::Window::Window(MenuBar *parent) :
	QMenu(Window::tr("&Window"), parent)
{
	newAction(QString(), this, toggleVisibility, false, QIcon(), false);
	newAction(Window::tr("&Full screen"), this, toggleFullScreen, false, QMPlay2Core.getIconFromTheme("view-fullscreen"), false);
	newAction(Window::tr("&Compact view"), this, toggleCompactView, false, QIcon(), true );
	addSeparator();
	newAction(Window::tr("&Close"), this, close, false, QMPlay2Core.getIconFromTheme("application-exit"), false);
}

MenuBar::Widgets::Widgets(MenuBar *parent) :
	QMenu(Widgets::tr("&Widgets"), parent)
{}
void MenuBar::Widgets::menuShow()
{
	clear();
	QMenu *menu = qobject_cast<QMainWindow *>(QMPlay2GUI.mainW)->createPopupMenu();
	if (menu)
	{
		foreach (QAction *act, menu->actions())
		{
			if (act->parent() == menu)
				act->setParent(this);
			addAction(act);
		}
		menu->deleteLater();
	}
}

MenuBar::Playlist::Playlist(MenuBar *parent) :
	QMenu(Playlist::tr("&Playlist"), parent)
{
	add = new Add(this);
	addMenu(add);

	addSeparator();
	newAction(Playlist::tr("&Stop loading"), this, stopLoading, false, QMPlay2Core.getIconFromTheme("process-stop"), false);
	addSeparator();
	newAction(Playlist::tr("&Synchronize group"), this, sync, false, QMPlay2Core.getIconFromTheme("view-refresh"), false);
	addSeparator();
	newAction(Playlist::tr("Load &list"), this, loadPlist, false, QIcon(), false);
	newAction(Playlist::tr("Save &list"), this, savePlist, false, QIcon(), false);
	newAction(Playlist::tr("Save &group"), this, saveGroup, false, QIcon(), false);
	addSeparator();
	newAction(Playlist::tr("&Remove selected entries"), this, delEntries, true, QMPlay2Core.getIconFromTheme("list-remove"), false);
	newAction(Playlist::tr("Remove entries &without groups"), this, delNonGroupEntries, false, QMPlay2Core.getIconFromTheme("list-remove"), false);
	newAction(Playlist::tr("&Clear list"), this, clear, false, QMPlay2Core.getIconFromTheme("archive-remove"), false);
	addSeparator();
	newAction(Playlist::tr("&Copy"), this, copy, false, QMPlay2Core.getIconFromTheme("edit-copy"), false);
	newAction(Playlist::tr("&Paste"), this, paste, false, QMPlay2Core.getIconFromTheme("edit-paste"), false);
	addSeparator();

	extensions = new QMenu(Playlist::tr("&Extensions"), this);
	extensions->setEnabled(false);
	addMenu(extensions);

	addSeparator();
	newAction(Playlist::tr("&Create &group"), this, newGroup, false, QMPlay2Core.getIconFromTheme("folder-new"), false);
	newAction(Playlist::tr("&Rename"), this, renameGroup, false, QIcon(), false);
	addSeparator();
	newAction(Playlist::tr("&Find entries"), this, find, false, QMPlay2Core.getIconFromTheme("edit-find"), false);
	addSeparator();

	sort = new Sort(this);
	addMenu(sort);

	addSeparator();
	newAction(Playlist::tr("&Collapse all"), this, collapseAll, false, QIcon(), false);
	newAction(Playlist::tr("&Expand all"), this, expandAll, false, QIcon(), false);
	addSeparator();
	newAction(Playlist::tr("&Go to the playback"), this, goToPlayback, false, QIcon(), false);
	addSeparator();
	newAction(Playlist::tr("&Enqueue"), this, queue, false, QIcon(), false);
	addSeparator();
	newAction(Playlist::tr("&Properties"), this, entryProperties, false, QMPlay2Core.getIconFromTheme("document-properties"), false);
}
MenuBar::Playlist::Add::Add(QMenu *parent) :
	QMenu(Add::tr("&Add"), parent)
{
	setIcon(QMPlay2Core.getIconFromTheme("list-add"));
	newAction(Add::tr("&Files"), this, file, false, QMPlay2Core.getIconFromTheme("applications-multimedia"), false);
	newAction(Add::tr("&Directory"), this, dir, false, QMPlay2Core.getIconFromTheme("folder"), false);
	newAction(Add::tr("&Address"), this, address, false, QMPlay2Core.getIconFromTheme("application-x-mswinurl"), false);
	addSeparator();
}
MenuBar::Playlist::Sort::Sort(QMenu *parent) :
	QMenu(Sort::tr("&Sort"), parent)
{
	newAction(Sort::tr("&From the shortest to the longest"), this, timeSort1, false, QIcon(), false);
	newAction(Sort::tr("&From the longest to the shortest"), this, timeSort2, false, QIcon(), false);
	addSeparator();
	newAction(Sort::tr("&A-Z"), this, titleSort1, false, QIcon(), false);
	newAction(Sort::tr("&Z-A"), this, titleSort2, false, QIcon(), false);
}

MenuBar::Player::Player(MenuBar *parent) :
	QMenu(Player::tr("&Player"), parent)
{
	newAction(QString(), this,  togglePlay, false, QMPlay2Core.getIconFromTheme("media-playback-start"), false);
	newAction(Player::tr("&Stop"), this, stop, false, QMPlay2Core.getIconFromTheme("media-playback-stop"), false);
	newAction(Player::tr("&Next"), this, next, true, QMPlay2Core.getIconFromTheme("media-skip-forward"), false);
	newAction(Player::tr("&Previous"), this, prev, true, QMPlay2Core.getIconFromTheme("media-skip-backward"), false);
	newAction(Player::tr("Next &frame"), this, nextFrame, true, QIcon(), false);
	addSeparator();

	repeat = new Repeat(this);
	addMenu(repeat);

	newAction(Player::tr("A&-B Repeat"), this, abRepeat, true, QIcon(), false);

	addSeparator();
	newAction(Player::tr("Seek &forward"), this, seekF, true, QIcon(), false);
	newAction(Player::tr("Seek &backward"), this, seekB, true, QIcon(), false);
	newAction(Player::tr("Long &seek &forward"), this, lSeekF, true, QIcon(), false);
	newAction(Player::tr("Long s&eek backward"), this, lSeekB, true, QIcon(), false);
	addSeparator();
	newAction(Player::tr("Fa&ster"), this, speedUp, true, QIcon(), false);
	newAction(Player::tr("Slowe&r"), this, slowDown, true, QIcon(), false);
	newAction(Player::tr("&Set speed"), this, setSpeed, false, QIcon(), false);
	addSeparator();
	newAction(Player::tr("Zoom i&n"), this, zoomIn, true, QIcon(), false);
	newAction(Player::tr("Zoom ou&t"), this, zoomOut, true, QIcon(), false);
	newAction(Player::tr("Toggle &aspect ratio"), this, switchARatio, true, QIcon(), false);

	aRatio = new AspectRatio(this);
	addMenu(aRatio);

	newAction(Player::tr("Reset image &settings"), this, reset, false, QIcon(), false);

	addSeparator();
	newAction(Player::tr("Volume &up"), this, volUp, true, QIcon(), false);
	newAction(Player::tr("Volume &down"), this, volDown, true, QIcon(), false);
	newAction(Player::tr("&Mute"), this, toggleMute, false, QMPlay2Core.getIconFromTheme("audio-volume-high"), true);

	if (!QMPlay2GUI.pipe)
		detach = NULL;
	else
	{
		addSeparator();
		newAction(Player::tr("Detach from receiving &commands"), this, detach, false, QIcon(), false);
	}

	if (!QMPlay2CoreClass::canSuspend())
		suspend = NULL;
	else
	{
		addSeparator();
		newAction(Player::tr("Suspend after playbac&k is finished"), this, suspend, false, QIcon(), true);
	}
}
MenuBar::Player::Repeat::Repeat(QMenu *parent) :
	QMenu(Repeat::tr("&Repeat"), parent)
{
	choice = new QActionGroup(this);
	choice->addAction(newAction(Repeat::tr("&No repeating"), this, repeatActions[RepeatNormal], false, QIcon(), true));
	addSeparator();
	choice->addAction(newAction(Repeat::tr("&Entry repeating"), this, repeatActions[RepeatEntry], false, QIcon(), true));
	choice->addAction(newAction(Repeat::tr("&Group repeating"), this, repeatActions[RepeatGroup], false, QIcon(), true));
	choice->addAction(newAction(Repeat::tr("&Playlist repeating"), this, repeatActions[RepeatList], false, QIcon(), true));
	addSeparator();
	choice->addAction(newAction(Repeat::tr("R&andom"), this, repeatActions[RandomMode], false, QIcon(), true));
	choice->addAction(newAction(Repeat::tr("Random in &group"), this, repeatActions[RandomGroupMode], false, QIcon(), true));
	addSeparator();
	choice->addAction(newAction(Repeat::tr("Random and &repeat"), this, repeatActions[RepeatRandom], false, QIcon(), true));
	choice->addAction(newAction(Repeat::tr("Random in group and repea&t"), this, repeatActions[RepeatRandomGroup], false, QIcon(), true));

	for (int i = 0; i < RepeatModeCount; ++i)
		repeatActions[i]->setProperty("enumValue", i);
}
MenuBar::Player::AspectRatio::AspectRatio(QMenu *parent) :
	QMenu(AspectRatio::tr("&Aspect ratio"), parent)
{
	choice = new QActionGroup(this);
	choice->addAction(newAction(AspectRatio::tr("&Auto"), this, _auto, false, QIcon(), true));
	addSeparator();
	choice->addAction(newAction("&1:1", this, _1x1, false, QIcon(), true));
	choice->addAction(newAction("&4:3", this, _4x3, false, QIcon(), true));
	choice->addAction(newAction("&5:4", this, _5x4, false, QIcon(), true));
	choice->addAction(newAction("&16:9", this, _16x9, false, QIcon(), true));
	choice->addAction(newAction("&3:2", this, _3x2, false, QIcon(), true));
	choice->addAction(newAction("&21:9", this, _21x9, false, QIcon(), true));
	addSeparator();
	choice->addAction(newAction(AspectRatio::tr("D&epends on size"), this, sizeDep, false, QIcon(), true));
	choice->addAction(newAction(AspectRatio::tr("&Disabled"), this, off, false, QIcon(), true));

	_auto->setObjectName("auto");
	_1x1->setObjectName("1");
	_4x3->setObjectName(QString::number(4.0 / 3.0));
	_5x4->setObjectName(QString::number(5.0 / 4.0));
	_16x9->setObjectName(QString::number(16.0 / 9.0));
	_3x2->setObjectName(QString::number(3.0 / 2.0));
	_21x9->setObjectName(QString::number(21.0 / 9.0));
	sizeDep->setObjectName("sizeDep");
	off->setObjectName("off");
}
void MenuBar::Player::seekActionsEnable(bool e)
{
	Qt::ShortcutContext ctx = e ? Qt::WindowShortcut : Qt::WidgetShortcut;
	seekF->setShortcutContext(ctx);
	seekB->setShortcutContext(ctx);
	lSeekF->setShortcutContext(ctx);
	lSeekB->setShortcutContext(ctx);
}

MenuBar::Playback::Playback(MenuBar *parent) :
	QMenu(Playback::tr("&Playback"), parent)
{
	newAction(Playback::tr("&Audio enabled"), this, toggleAudio, false, QIcon(), true)->setObjectName("toggleAudio");
	toggleAudio->setChecked(true);

	audioChannels = new AudioChannels(this);
	addMenu(audioChannels);

	addSeparator();

	newAction(Playback::tr("&Video enabled"), this, toggleVideo, false, QIcon(), true)->setObjectName("toggleVideo");
	toggleVideo->setChecked(true);

	videoFilters = new VideoFilters(this);
	addMenu(videoFilters);

	newAction(Playback::tr("Set &video delay"), this, videoSync, false, QIcon(), false);
	newAction(Playback::tr("&Delay video") + " (100ms)", this, slowDownVideo, true, QIcon(), false);
	newAction(Playback::tr("&Speed up video") + " (100ms)", this, speedUpVideo, true, QIcon(), false);
	addSeparator();
	newAction(Playback::tr("&Subtitles enabled"), this, toggleSubtitles, false, QIcon(), true)->setObjectName("toggleSubtitles");
	toggleSubtitles->setChecked(true);
	newAction(Playback::tr("Add &subtities from file"), this, subsFromFile, false, QIcon(), false);
	newAction(Playback::tr("Set &subtitles delay"), this, subtitlesSync, true, QIcon(), false);
	newAction(Playback::tr("&Delay subtitiles") + " (100ms)", this, slowDownSubtitles, true, QIcon(), false);
	newAction(Playback::tr("&Speed up subtitles") + " (100ms)", this, speedUpSubtitles, true, QIcon(), false);
	newAction(Playback::tr("Scale up subt&itles"), this, biggerSubtitles, true, QIcon(), false);
	newAction(Playback::tr("Scale down sub&titles"), this, smallerSubtitles, true, QIcon(), false);
	addSeparator();
	newAction(Playback::tr("&Playback settings"), this, playbackSettings, true, QMPlay2Core.getIconFromTheme("configure"), false);
	addSeparator();
	newAction(Playback::tr("&Screen shot"), this, screenShot, true, QIcon(), false);
}
MenuBar::Playback::VideoFilters::VideoFilters(QMenu *parent) :
	QMenu(VideoFilters::tr("Video &filters"), parent)
{
	/** Korektor wideo */
	videoAdjustmentMenu = new QMenu(VideoFilters::tr("Video &adjustment"), this);
	addMenu(videoAdjustmentMenu);
	QWidgetAction *widgetAction = new QWidgetAction(this);
	widgetAction->setDefaultWidget(QMPlay2GUI.videoAdjustment);
	QMPlay2GUI.videoAdjustment->setObjectName(videoAdjustmentMenu->title().remove('&'));
	videoAdjustmentMenu->addAction(widgetAction);
	/**/
	addSeparator();
	newAction(VideoFilters::tr("&Spherical view"), this, spherical, true, QIcon(), true);
	addSeparator();
	newAction(VideoFilters::tr("&Horizontal flip"), this, hFlip, true, QIcon(), true);
	newAction(VideoFilters::tr("&Vertical flip"), this, vFlip, true, QIcon(), true);
	newAction(VideoFilters::tr("&Rotate 90°"), this, rotate90, true, QIcon(), true);
	addSeparator();
	newAction(VideoFilters::tr("&More filters"), this, more, false, QIcon(), false);
}
MenuBar::Playback::AudioChannels::AudioChannels(QMenu *parent) :
	QMenu(AudioChannels::tr("&Channels") , parent)
{
	choice = new QActionGroup(this);
	choice->addAction(newAction(AudioChannels::tr("&Autodetect"), this, _auto, false, QIcon(), true));
	addSeparator();
	choice->addAction(newAction(AudioChannels::tr("&Mono"), this, _1, false, QIcon(), true));
	choice->addAction(newAction(AudioChannels::tr("&Stereo"), this, _2, false, QIcon(), true));
	choice->addAction(newAction("&4.0", this, _4, false, QIcon(), true));
	choice->addAction(newAction("&5.1", this, _6, false, QIcon(), true));
	choice->addAction(newAction("&7.1", this, _8, false, QIcon(), true));
	addSeparator();
	choice->addAction(newAction(AudioChannels::tr("&Other"), this, other, false, QIcon(), true));

	_auto->setObjectName("auto");
	_1->setObjectName("1");
	_2->setObjectName("2");
	_4->setObjectName("4");
	_6->setObjectName("6");
	_8->setObjectName("8");
	other->setEnabled(false);
}

MenuBar::Options::Options(MenuBar *parent) :
	QMenu(Options::tr("Op&tions"), parent)
{
	newAction(Options::tr("&Settings"), this, settings, false, QMPlay2Core.getIconFromTheme("configure"), false);
	newAction(Options::tr("&Modules settings"), this, modulesSettings, false, QMPlay2Core.getIconFromTheme("configure"), false);
	addSeparator();
	newAction(Options::tr("&Show tray icon"), this, trayVisible, false, QIcon(), true);
}

MenuBar::Help::Help(MenuBar *parent) :
	QMenu(Help::tr("&Help"), parent)
{
	newAction(Help::tr("&About QMPlay2"), this, about, false, QIcon(), false);
#ifdef UPDATER
	newAction(Help::tr("&Updates"), this, updates, false, QIcon(), false);
#endif
	addSeparator();
	newAction(Help::tr("About &Qt"), this, aboutQt, false, QIcon(), false);
}

void MenuBar::setKeyShortcuts()
{
	ShortcutHandler *shortcuts = QMPlay2GUI.shortcutHandler;
	shortcuts->appendAction(window->toggleVisibility, "KeyBindings/Window-toggleVisibility", "`");
	shortcuts->appendAction(window->toggleFullScreen, "KeyBindings/Window-toggleFullScreen", "F");
	shortcuts->appendAction(window->toggleCompactView, "KeyBindings/Window-toggleCompactView", "Alt+V");
	shortcuts->appendAction(window->close, "KeyBindings/Window-close", "Alt+F4");


	if (widgets->hideMenuAct)
		shortcuts->appendAction(widgets->hideMenuAct, "KeyBindings/Widgets-hideMenu", "Ctrl+Alt+M");
	shortcuts->appendAction(widgets->lockWidgetsAct, "KeyBindings/Widgets-lockWidgets", "Shift+L");


	shortcuts->appendAction(playlist->stopLoading, "KeyBindings/Playlist-stopLoading", "F4");
	shortcuts->appendAction(playlist->sync, "KeyBindings/Playlist-sync", "F5");
	shortcuts->appendAction(playlist->loadPlist, "KeyBindings/Playlist-loadPlist", "Ctrl+L");
	shortcuts->appendAction(playlist->savePlist, "KeyBindings/Playlist-savePlist", "Ctrl+S");
	shortcuts->appendAction(playlist->saveGroup, "KeyBindings/Playlist-saveGroup", "Ctrl+Shift+S");
	shortcuts->appendAction(playlist->delEntries, "KeyBindings/Playlist-delEntries", "Del");
	shortcuts->appendAction(playlist->delNonGroupEntries, "KeyBindings/Playlist-delNonGroupEntries", "Ctrl+Del");
	shortcuts->appendAction(playlist->clear, "KeyBindings/Playlist-clear", "Shift+Del");
	shortcuts->appendAction(playlist->copy, "KeyBindings/Playlist-copy", "Ctrl+C");
	shortcuts->appendAction(playlist->paste, "KeyBindings/Playlist-paste", "Ctrl+V");
	shortcuts->appendAction(playlist->newGroup, "KeyBindings/Playlist-newGroup", "F7");
	shortcuts->appendAction(playlist->renameGroup, "KeyBindings/Playlist-renameGroup", "F2");
	shortcuts->appendAction(playlist->find, "KeyBindings/Playlist-find", "Ctrl+F");
	shortcuts->appendAction(playlist->collapseAll, "KeyBindings/Playlist-collapseAll", "");
	shortcuts->appendAction(playlist->expandAll, "KeyBindings/Playlist-expandAll", "");
	shortcuts->appendAction(playlist->goToPlayback, "KeyBindings/Playlist-goToPlayback", "Ctrl+P");
	shortcuts->appendAction(playlist->queue, "KeyBindings/Playlist-queue", "Q");
	shortcuts->appendAction(playlist->entryProperties, "KeyBindings/Playlist-entryProperties", "Alt+Return");

	shortcuts->appendAction(playlist->add->file, "KeyBindings/Playlist-Add-file", "Ctrl+I");
	shortcuts->appendAction(playlist->add->dir, "KeyBindings/Playlist-Add-dir", "Ctrl+D");
	shortcuts->appendAction(playlist->add->address, "KeyBindings/Playlist-Add-address", "Ctrl+U");

	shortcuts->appendAction(playlist->sort->timeSort1, "KeyBindings/Playlist-Sort-timeSort1", "");
	shortcuts->appendAction(playlist->sort->timeSort2, "KeyBindings/Playlist-Sort-timeSort2", "");
	shortcuts->appendAction(playlist->sort->titleSort1, "KeyBindings/Playlist-Sort-titleSort1", "");
	shortcuts->appendAction(playlist->sort->titleSort2, "KeyBindings/Playlist-Sort-titleSort2", "");


	shortcuts->appendAction(player->togglePlay, "KeyBindings/Player-togglePlay", "Space");
	shortcuts->appendAction(player->stop, "KeyBindings/Player-stop", "V");
	shortcuts->appendAction(player->next, "KeyBindings/Player-next", "B");
	shortcuts->appendAction(player->prev, "KeyBindings/Player-prev", "Z");
	shortcuts->appendAction(player->nextFrame, "KeyBindings/Player-nextFrame", ".");
	shortcuts->appendAction(player->abRepeat, "KeyBindings/Player-abRepeat", "Ctrl+-");
	shortcuts->appendAction(player->seekF, "KeyBindings/Player-seekF", "Right");
	shortcuts->appendAction(player->seekB, "KeyBindings/Player-seekB", "Left");
	shortcuts->appendAction(player->lSeekF, "KeyBindings/Player-lSeekF", "Up");
	shortcuts->appendAction(player->lSeekB, "KeyBindings/Player-lSeekB", "Down");
	shortcuts->appendAction(player->speedUp, "KeyBindings/Player-speedUp", "]");
	shortcuts->appendAction(player->slowDown, "KeyBindings/Player-slowDown", "[");
	shortcuts->appendAction(player->setSpeed, "KeyBindings/Player-setSpeed", "Shift+S");
	shortcuts->appendAction(player->zoomIn, "KeyBindings/Player-zoomIn", "E");
	shortcuts->appendAction(player->zoomOut, "KeyBindings/Player-zoomOut", "W");
	shortcuts->appendAction(player->switchARatio, "KeyBindings/Player-switchARatio", "A");
	shortcuts->appendAction(player->reset, "KeyBindings/Player-reset", "R");
	shortcuts->appendAction(player->volUp, "KeyBindings/Player-volUp", "*");
	shortcuts->appendAction(player->volDown, "KeyBindings/Player-volDown", "/");
	shortcuts->appendAction(player->toggleMute, "KeyBindings/Player-toggleMute", "M");
	if (player->detach)
		shortcuts->appendAction(player->detach, "KeyBindings/Player-detach", "");
	if (player->suspend)
		shortcuts->appendAction(player->suspend, "KeyBindings/Player-suspend", "");

	shortcuts->appendAction(player->repeat->repeatActions[RepeatNormal], "KeyBindings/Player-Repeat-RepeatNormal", "Alt+0");
	shortcuts->appendAction(player->repeat->repeatActions[RepeatEntry], "KeyBindings/Player-Repeat-RepeatEntry", "Alt+1");
	shortcuts->appendAction(player->repeat->repeatActions[RepeatGroup], "KeyBindings/Player-Repeat-RepeatGroup", "Alt+2");
	shortcuts->appendAction(player->repeat->repeatActions[RepeatList], "KeyBindings/Player-Repeat-RepeatList", "Alt+3");
	shortcuts->appendAction(player->repeat->repeatActions[RandomMode], "KeyBindings/Player-Repeat-RandomMode", "Alt+4");
	shortcuts->appendAction(player->repeat->repeatActions[RandomGroupMode], "KeyBindings/Player-Repeat-RandomGroupMode", "Alt+5");
	shortcuts->appendAction(player->repeat->repeatActions[RepeatRandom], "KeyBindings/Player-Repeat-RepeatRandom", "Alt+6");
	shortcuts->appendAction(player->repeat->repeatActions[RepeatRandomGroup], "KeyBindings/Player-Repeat-RepeatRandomGroup", "Alt+7");


	shortcuts->appendAction(playback->toggleAudio, "KeyBindings/Playback-toggleAudio", "D");
	shortcuts->appendAction(playback->toggleVideo, "KeyBindings/Playback-toggleVideo", "O");
	shortcuts->appendAction(playback->videoSync, "KeyBindings/Playback-videoSync", "Shift+O");
	shortcuts->appendAction(playback->slowDownVideo, "KeyBindings/Playback-slowDownVideo", "-");
	shortcuts->appendAction(playback->speedUpVideo, "KeyBindings/Playback-speedUpVideo", "+");
	shortcuts->appendAction(playback->toggleSubtitles, "KeyBindings/Playback-toggleSubtitles", "N");
	shortcuts->appendAction(playback->subsFromFile, "KeyBindings/Playback-subsFromFile", "Alt+I");
	shortcuts->appendAction(playback->subtitlesSync, "KeyBindings/Playback-subtitlesSync", "Shift+N");
	shortcuts->appendAction(playback->slowDownSubtitles, "KeyBindings/Playback-slowDownSubtitles", "Shift+Z");
	shortcuts->appendAction(playback->speedUpSubtitles, "KeyBindings/Playback-speedUpSubtitles", "Shift+X");
	shortcuts->appendAction(playback->biggerSubtitles, "KeyBindings/Playback-biggerSubtitles", "Shift+R");
	shortcuts->appendAction(playback->smallerSubtitles, "KeyBindings/Playback-smallerSubtitles", "Shift+T");
	shortcuts->appendAction(playback->playbackSettings, "KeyBindings/Playback-playbackSettings", "Ctrl+Shift+P");
	shortcuts->appendAction(playback->screenShot, "KeyBindings/Playback-screenShot", "Alt+S");

	shortcuts->appendAction(playback->videoFilters->spherical, "KeyBindings/Playback-VideoFilters-spherical", "Ctrl+3");
	shortcuts->appendAction(playback->videoFilters->hFlip, "KeyBindings/Playback-VideoFilters-hFlip", "Ctrl+M");
	shortcuts->appendAction(playback->videoFilters->vFlip, "KeyBindings/Playback-VideoFilters-vFlip", "Ctrl+R");
	shortcuts->appendAction(playback->videoFilters->rotate90, "KeyBindings/Playback-VideoFilters-rotate90", "Ctrl+9");
	shortcuts->appendAction(playback->videoFilters->more, "KeyBindings/Playback-VideoFilters-more", "Alt+F");


	shortcuts->appendAction(options->settings, "KeyBindings/Options-settings", "Ctrl+O");
	shortcuts->appendAction(options->modulesSettings, "KeyBindings/Options-modulesSettings", "Ctrl+Shift+O");
	shortcuts->appendAction(options->trayVisible, "KeyBindings/Options-trayVisible", "Ctrl+T");


	shortcuts->appendAction(help->about, "KeyBindings/Help-about", "F1");
#ifdef UPDATER
	shortcuts->appendAction(help->updates, "KeyBindings/Help-updates", "F12");
#endif
}

void MenuBar::widgetsMenuShow()
{
	widgets->menuShow();
}

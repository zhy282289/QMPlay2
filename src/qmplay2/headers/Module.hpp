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

#ifndef MODULE_HPP
#define MODULE_HPP

#include <QStringList>
#include <QWidget>
#include <QMutex>

#include <Settings.hpp>

class ModuleCommon;

class Module : public Settings
{
	friend class ModuleCommon;
public:
	enum TYPE {NONE, DEMUXER, DECODER, READER, WRITER, PLAYLIST, QMPLAY2EXTENSION, SUBSDEC, AUDIOFILTER, VIDEOFILTER};
	enum FILTERTYPE {DEINTERLACE = 0x400000, DOUBLER = 0x800000, USERFLAG = 0x80000000};
	enum HWTYPE {VIDEOHWFILTER = 0x4000000};

	inline Module(const QString &mName) :
		Settings(mName),
		mName(mName)
	{}
	virtual ~Module();

	inline QString name() const
	{
		return mName;
	}

	class Info
	{
	public:
		inline Info() :
			type(NONE) {}
		inline Info(const QString &name, const quint32 type, const QImage &img = QImage(), const QString &description = QString()) :
			name(name), description(description), type(type), img(img) {}
		inline Info(const QString &name, const quint32 type, const QString &description) :
			name(name), description(description), type(type) {}
		inline Info(const QString &name, const quint32 type, const QStringList &extensions, const QImage &img = QImage(), const QString &description = QString()) :
			name(name), description(description), type(type), img(img), extensions(extensions) {}

		inline QString imgPath() const
		{
			return img.text("Path");
		}

		QString name, description;
		quint32 type;
		QImage img;
		QStringList extensions;
	};
	virtual QList<Info> getModulesInfo(const bool showDisabled = false) const = 0;
	virtual void *createInstance(const QString &) = 0;

	virtual QList<QAction *> getAddActions();

	class SettingsWidget : public QWidget
	{
	public:
		virtual void saveSettings() = 0;
		inline void flushSettings()
		{
			sets().flush();
		}
	protected:
		inline SettingsWidget(Module &module) :
			module(module)
		{
			setAttribute(Qt::WA_DeleteOnClose);
		}

		inline Settings &sets()
		{
			return module;
		}

		template<typename T>
		inline void SetInstance()
		{
			module.setInstance<T>();
		}
	private:
		Module &module;
	};
	virtual SettingsWidget *getSettingsWidget();

	virtual void videoDeintSave();

	inline QImage image() const
	{
		return moduleImg;
	}

	void setInstances(bool &);

	template<typename T>
	void setInstance();
protected:
	QImage moduleImg;
private:
	QMutex mutex;
	QString mName;
	QList<ModuleCommon *> instances;
};

template<typename T>
void Module::setInstance()
{
	QMutexLocker locker(&mutex);
	foreach (ModuleCommon *mc, instances)
	{
		T *t = dynamic_cast<T *>(mc);
		if (t)
			t->set();
	}
}

#define QMPLAY2_EXPORT_PLUGIN(ModuleClass) extern "C" Module *qmplay2PluginInstance() {return new ModuleClass;}

#endif

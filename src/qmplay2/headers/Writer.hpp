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

#ifndef WRITER_HPP
#define WRITER_HPP

#include <ModuleCommon.hpp>
#include <ModuleParams.hpp>
#include <IOController.hpp>

#include <QStringList>

class Writer : public ModuleCommon, public ModuleParams, public BasicIO
{
public:
	static Writer *create(const QString &, const QStringList &modNames = QStringList());

	inline QString getUrl() const
	{
		return _url;
	}

	virtual bool readyWrite() const = 0;

	virtual qint64 write(const QByteArray &) = 0;

	virtual QString name() const = 0;

	virtual ~Writer();
private:
	virtual bool open() = 0;

	QString _url;
};

#endif

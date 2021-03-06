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

#include <Demuxer.hpp>

class FormatContext;

class FFDemux : public Demuxer
{
	Q_DECLARE_TR_FUNCTIONS(FFDemux)
public:
	FFDemux(QMutex &, Module &);
private:
	~FFDemux();

	bool set();

	bool metadataChanged() const;

	QList<ProgramInfo> getPrograms() const;
	QList<ChapterInfo> getChapters() const;

	QString name() const;
	QString title() const;
	QList<QMPlay2Tag> tags() const;
	bool getReplayGain(bool album, float &gain_db, float &peak) const;
	double length() const;
	int bitrate() const;
	QByteArray image(bool forceCopy) const;

	bool localStream() const;

	bool seek(int pos, bool backward);
	bool read(Packet &encoded, int &idx);
	void pause();
	void abort();

	bool open(const QString &entireUrl);

	Playlist::Entries fetchTracks(const QString &url, bool &ok);

	/**/

	void addFormatContext(QString entireUrl, const QString &param = QString());

	QVector<FormatContext *> formatContexts;

	QMutex &avcodec_mutex;
	QMutex mutex;

	bool abortFetchTracks;
};

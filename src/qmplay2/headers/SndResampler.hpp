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

#ifndef SNDRESAMPLER_HPP
#define SNDRESAMPLER_HPP

#include <stddef.h>

class QByteArray;

class SndResampler
{
public:
	inline SndResampler() :
		snd_convert_ctx(NULL),
		src_samplerate(0), src_channels(0), dst_samplerate(0), dst_channels(0)
	{}
	inline ~SndResampler()
	{
		destroy();
	}

	const char *name() const;

	inline bool isOpen() const
	{
		return snd_convert_ctx != NULL;
	}

	bool create(int _src_samplerate, int _src_channels, int _dst_samplerate, int _dst_channels);
	void convert(const QByteArray &src, QByteArray &dst);
	void destroy();
private:
#ifdef QMPLAY2_AVRESAMPLE
	struct AVAudioResampleContext *snd_convert_ctx;
#else
	struct SwrContext *snd_convert_ctx;
#endif
	int src_samplerate, src_channels, dst_samplerate, dst_channels;
};

#endif

/****
DIAMOND protein aligner
Copyright (C) 2013-2018 Benjamin Buchfink <buchfink@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
****/

#ifndef DESERIALIZER_H_
#define DESERIALIZER_H_

#include <vector>
#include "stream_entity.h"

using std::vector;

struct Deserializer
{

	Deserializer(StreamEntity* buffer);
	void rewind();
	void seek(size_t pos);
	void seek_forward(size_t n);
	void close();

	Deserializer& operator>>(int &x)
	{
		read(x);
		return *this;
	}

	Deserializer& operator>>(string &s)
	{
		if (!read_until(s, '\0'))
			throw EndOfStream();
		return *this;
	}

	Deserializer& operator>>(vector<string> &v)
	{
		int n;
		*this >> n;
		v.clear();
		string s;
		for (int i = 0; i < n; ++i) {
			*this >> s;
			v.push_back(s);
		}
		return *this;
	}

	template<typename _t>
	void read(_t &x)
	{
		if (avail() <= sizeof(_t)) {
			x = *(const _t*)begin_;
			begin_ += sizeof(_t);
		}
		else if (read((char*)&x, sizeof(_t)) != sizeof(_t))
			throw EndOfStream();
	}

	template<class _t>
	size_t read(_t *ptr, size_t count)
	{
		return read_raw((char*)ptr, count*sizeof(_t)) / sizeof(_t);
	}

	size_t read_raw(char *ptr, size_t count);
	bool read_until(string &dst, char delimiter);
	bool read_until(vector<char> &dst, char delimiter);
	~Deserializer();

protected:

	template<typename _t>
	bool read_to(_t &container, char delimiter);
	void pop(char *dst, size_t n);
	bool fetch();

	size_t avail() const
	{
		return end_ - begin_;
	}
	
	StreamEntity *buffer_;
	const char *begin_, *end_;
};

#endif
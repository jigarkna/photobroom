/*
    Database entries manipulator
    Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef ENTRY_HPP
#define ENTRY_HPP

#include <string>

#include <OpenLibrary/utils/data_ptr.hpp>

class Entry
{
    public:
        Entry();
        Entry(const Entry &);
        Entry(Entry &&);
        virtual ~Entry();
        
        virtual Entry& operator=(Entry &&);
                
#ifdef OS_UNIX
        typedef __uint32_t crc32;
#elif defined OS_WIN
		typedef unsigned __int32 crc32;
#else
#error unknown os
#endif
        
        struct Data
        {
            Data(): m_crc(0xffffffff), m_path("null") {}
            
            crc32       m_crc;
            std::string m_path;         //path starts with file (when localfile), or with db: (when in database)
        };
        
        data_ptr<Data> m_d;
        
    private:
        virtual Entry& operator=(const Entry &);
        virtual bool operator==(const Entry&) const;        
};

#endif // ENTRY_HPP
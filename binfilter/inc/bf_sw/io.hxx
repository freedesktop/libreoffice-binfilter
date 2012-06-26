/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */
// eigene Klasse fuer IO, die die systemunabhaengige Darstellung
// uebernimmt (bytes dreht, Character konvertiert)
// das Schreiben erfolgt aus Effizienzgruenden binaer
#ifndef _IO_HXX
#define _IO_HXX

#include <bf_svtools/bf_solar.h>

#ifdef UNX
#include <unistd.h>
#else
#include <io.h>
#endif

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vcl/keycod.hxx>
#include <tools/stream.hxx>

namespace binfilter {

class SwIOin {
private:
        SvFileStream aStr; //$ ifstream
public:
    // Stream wird im entsprechenden Mode erzeugt.
    SwIOin(const String &rFilename, StreamMode nMode =
                                    STREAM_READ | STREAM_NOCREATE );

    SwIOin& operator>>(char& val);
    SwIOin& operator>>(unsigned char& val);
    SwIOin& operator>>(char* val);
    SwIOin& operator>>(unsigned char* val);
    SwIOin& operator>>(short& val);
    SwIOin& operator>>(unsigned short& val);
    SwIOin& operator>>(long& val);
    SwIOin& operator>>(unsigned long& val);
    String ReadString();
    KeyCode ReadKeyCode();
    // kann erweitert werden fuer weitere Arrays von
    // Basistypen; nLen ist die Anzahl der Elemente
    SwIOin& Read(char *buf, unsigned nLen);

    int operator!() { return aStr.GetError() != SVSTREAM_OK; }
        SvFileStream &operator()() {
        return aStr;
    }
};

class SwIOout {
private:
    void _write(const char *buf, unsigned size);
    SvFileStream aStr; //$ ofstream
public:
    // Stream wird im entsprechenden Mode erzeugt.
    SwIOout( const String &rFilename, StreamMode nMode =
                                      STREAM_WRITE | STREAM_NOCREATE );
    SwIOout& operator<<(char val);
    SwIOout& operator<<(unsigned char val);
    SwIOout& operator<<(char* val);
    SwIOout& operator<<(unsigned char* val);
    SwIOout& operator<<(short val);
    SwIOout& operator<<(unsigned short val);
    SwIOout& operator<<(long val);
    SwIOout& operator<<(unsigned long val);
    SwIOout& operator<<(const String &);
    SwIOout& operator<<(const KeyCode &);
    // kann erweitert werden fuer weitere Arrays von
    // Basistypen; nLen ist die Anzahl der Elemente
    SwIOout& Write(const char *buf, unsigned nLen);

    int operator!() { return aStr.GetError() != SVSTREAM_OK; }
    SvFileStream &operator()() {
        return aStr;
    }
};


class SwIOinout {
private:
    SvFileStream aStr; //$ fstream

public:
    // Stream wird im entsprechenden Mode erzeugt.
    SwIOinout(const String &rFilename, StreamMode nMode =
                                       STREAM_READWRITE  | STREAM_NOCREATE );

    SwIOinout& operator>>(char& val);
    SwIOinout& operator>>(unsigned char& val);
    SwIOinout& operator>>(char* val);
    SwIOinout& operator>>(unsigned char* val);
    SwIOinout& operator>>(short& val);
    SwIOinout& operator>>(unsigned short& val);
    SwIOinout& operator>>(long& val);
    SwIOinout& operator>>(unsigned long& val);
    String ReadString();
    KeyCode ReadKeyCode();
    // kann erweitert werden fuer weitere Arrays von
    // Basistypen; nLen ist die Anzahl der Elemente
    SwIOinout& Read(char *buf, unsigned nLen);
    SwIOinout& Read(unsigned short *buf, unsigned nLen );

    SwIOinout& operator<<(char val);
    SwIOinout& operator<<(unsigned char val);
    SwIOinout& operator<<(char* val);
    SwIOinout& operator<<(unsigned char* val);
    SwIOinout& operator<<(short val);
    SwIOinout& operator<<(unsigned short val);
    SwIOinout& operator<<(long val);
    SwIOinout& operator<<(unsigned long val);
    SwIOinout& operator<<(const String &);
    SwIOinout& operator<<(const KeyCode &);
    // kann erweitert werden fuer weitere Arrays von
    // Basistypen; nLen ist die Anzahl der Elemente
    SwIOinout& Write(const char *buf, unsigned nLen);

    int operator!() { return aStr.GetError() != SVSTREAM_OK; }
        SvFileStream &operator()() {
        return aStr;
    }

    BOOL Ok();
};



} //namespace binfilter
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

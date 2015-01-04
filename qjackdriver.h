///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//    This file is part of QJackAudio.                                       //
//    Copyright (C) 2015 Jacob Dawid, jacob@omg-it.works                     //
//                                                                           //
//    QJackAudio is free software: you can redistribute it and/or modify     //
//    it under the terms of the GNU General Public License as published by   //
//    the Free Software Foundation, either version 3 of the License, or      //
//    (at your option) any later version.                                    //
//                                                                           //
//    QJackAudio is distributed in the hope that it will be useful,          //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of         //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          //
//    GNU General Public License for more details.                           //
//                                                                           //
//    You should have received a copy of the GNU General Public License      //
//    along with QJackAudio. If not, see <http://www.gnu.org/licenses/>.     //
//                                                                           //
//    It is possible to obtain a closed-source license of QJackAudio.        //
//    If you're interested, contact me at: jacob@omg-it.works                //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once

// Own includes
#include <QJackParameter>

// JACK includes
typedef struct jackctl_driver jackctl_driver_t;

// Qt includes
#include <QString>
#include <QMap>

class QJackDriver
{
    friend class QJackServer;
public:
    // We need this for compatibility with QMap.
    QJackDriver() {
        _jackDriver = 0;
    }

    enum DriverType {
        DriverTypeMaster,
        DriverTypeSlave,
        DriverTypeInvalid
    };

    bool isValid() { return _jackDriver != 0; }

    QString name();
    DriverType type();
    QJackParameterMap parameters();

    int parseParameters(int argc, char* argv[]);

private:
    QJackDriver(jackctl_driver_t *driver);

    jackctl_driver_t *_jackDriver;
};

typedef QMap<QString, QJackDriver> QJackDriverMap;

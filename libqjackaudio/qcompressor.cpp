///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//    This file is part of QJackAudio.                                       //
//    Copyright (C) 2014 Jacob Dawid, jacob@omg-it.works                     //
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

// Qt includes
#include <QMutexLocker>

// Own includes
#include <QCompressor>

QCompressor::QCompressor(QObject *parent)
    : QDigitalFilter(parent)
{
    _threshold = -6.0;
    _ratio = 4.0;
    _attack = 0.0;
    _release = 0.0;
    _inputGain = 0.0;
    _makeupGain = 0.5;
}

QCompressor::~QCompressor()
{
}

void QCompressor::process(QSampleBuffer sampleBuffer)
{
    _mutex.lock();
    double threshold = _threshold;
    double ratio = _ratio;
    double attack = _attack;
    double release = _release;
    double inputGain = _inputGain;
    double makeupGain = _makeupGain;
    _mutex.unlock();

    // TODO: To be implemented.
    Q_UNUSED(attack);
    Q_UNUSED(release);

    int bufferSize = sampleBuffer.bufferSize();
    double inputGainMultiplier = QUnits::dbToLinear(inputGain);
    double makeupGainMultiplier = QUnits::dbToLinear(makeupGain);
    for(int i = 0; i < bufferSize; i++) {
        // Read audio sample
        double sample = sampleBuffer.readAudioSample(i) * inputGainMultiplier;
        // Determine peak in dB
        double peakDb = QUnits::linearToDb(QUnits::peak(sample));

        // Check if peak is over threshold
        if(peakDb > threshold) {
            double dbOverThreshold = peakDb - threshold;
            double dbOverThresholdCompressed = dbOverThreshold / ratio;
            double dbResultingPeak = threshold + dbOverThresholdCompressed;

            double resultSample = QUnits::dbToLinear(dbResultingPeak * ( sample > 0.0 ? 1.0 : -1.0 ));
            sampleBuffer.writeAudioSample(i, resultSample * makeupGainMultiplier);
        }
    }
}

void QCompressor::setThreshold(double threshold)
{
    QMutexLocker mutexLocker(&_mutex);
    _threshold = threshold;
    emit thresholdChanged(_threshold);
}

double QCompressor::threshold()
{
    return _threshold;
}

void QCompressor::setRatio(double ratio)
{
    QMutexLocker mutexLocker(&_mutex);
    _ratio = ratio;
    emit ratioChanged(_ratio);
}

double QCompressor::ratio()
{
    return _ratio;
}

void QCompressor::setAttack(double attack)
{
    QMutexLocker mutexLocker(&_mutex);
    _attack = attack;
    emit attackChanged(_attack);
}

double QCompressor::attack()
{
    return _attack;
}

void QCompressor::setRelease(double release)
{
    QMutexLocker mutexLocker(&_mutex);
    _release = release;
    emit releaseChanged(_release);
}

double QCompressor::release()
{
    return _release;
}

void QCompressor::setInputGain(double inputGain)
{
    QMutexLocker mutexLocker(&_mutex);
    _inputGain = inputGain;
    emit inputGainChanged(_inputGain);
}

double QCompressor::inputGain()
{
    return _inputGain;
}

void QCompressor::setMakeupGain(double makeupGain)
{
    QMutexLocker mutexLocker(&_mutex);
    _makeupGain = makeupGain;
    emit makeupGainChanged(_makeupGain);
}

double QCompressor::makeupGain()
{
    return _makeupGain;
}


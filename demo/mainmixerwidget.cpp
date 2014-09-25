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

// Own includes
#include "mainmixerwidget.h"
#include "ui_mainmixerwidget.h"

// Qt includes
#include <QFontDatabase>
#include <QDebug>

MainMixerWidget::MainMixerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainMixerWidget)
{
    ui->setupUi(this);

    connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(updateInterface()));
    _updateTimer.setInterval(20);
    _updateTimer.setSingleShot(false);
    _updateTimer.start();

    QFontDatabase::addApplicationFont(":/fonts/FreePixel.ttf");
    QFont font("Free Pixel", 24);
    font.setStyleStrategy(QFont::NoAntialias);
    ui->displayLabel->setFont(font);
    //

    QJackClient *jackClient = QJackClient::instance();
    _subGroup1Out = jackClient->registerAudioOutPort("subgroup1_out");
    _subGroup2Out = jackClient->registerAudioOutPort("subgroup2_out");
    _subGroup3Out = jackClient->registerAudioOutPort("subgroup3_out");
    _subGroup4Out = jackClient->registerAudioOutPort("subgroup4_out");
    _subGroup5Out = jackClient->registerAudioOutPort("subgroup5_out");
    _subGroup6Out = jackClient->registerAudioOutPort("subgroup6_out");
    _subGroup7Out = jackClient->registerAudioOutPort("subgroup7_out");
    _subGroup8Out = jackClient->registerAudioOutPort("subgroup8_out");

    _mainLeftOut = jackClient->registerAudioOutPort("main_out_1");
    _mainRightOut = jackClient->registerAudioOutPort("main_out_2");

    _subgroup1FaderStage = new QAmplifier();
    _subgroup2FaderStage = new QAmplifier();
    _subgroup3FaderStage = new QAmplifier();
    _subgroup4FaderStage = new QAmplifier();
    _subgroup5FaderStage = new QAmplifier();
    _subgroup6FaderStage = new QAmplifier();
    _subgroup7FaderStage = new QAmplifier();
    _subgroup8FaderStage = new QAmplifier();
    _main1FaderStage = new QAmplifier();
    _main2FaderStage = new QAmplifier();

    _subgroup1FaderStage->setGain(ui->subgroup1VolumeVerticalSlider->value());
    _subgroup2FaderStage->setGain(ui->subgroup2VolumeVerticalSlider->value());
    _subgroup3FaderStage->setGain(ui->subgroup3VolumeVerticalSlider->value());
    _subgroup4FaderStage->setGain(ui->subgroup4VolumeVerticalSlider->value());
    _subgroup5FaderStage->setGain(ui->subgroup5VolumeVerticalSlider->value());
    _subgroup6FaderStage->setGain(ui->subgroup6VolumeVerticalSlider->value());
    _subgroup7FaderStage->setGain(ui->subgroup7VolumeVerticalSlider->value());
    _subgroup8FaderStage->setGain(ui->subgroup8VolumeVerticalSlider->value());

    _main1FaderStage->setGain(ui->main1VolumeVerticalSlider->value());
    _main2FaderStage->setGain(ui->main2VolumeVerticalSlider->value());

    connect(ui->subgroup1VolumeVerticalSlider, SIGNAL(valueChanged(int)), _subgroup1FaderStage, SLOT(setGain(int)));
    connect(ui->subgroup2VolumeVerticalSlider, SIGNAL(valueChanged(int)), _subgroup2FaderStage, SLOT(setGain(int)));
    connect(ui->subgroup3VolumeVerticalSlider, SIGNAL(valueChanged(int)), _subgroup3FaderStage, SLOT(setGain(int)));
    connect(ui->subgroup4VolumeVerticalSlider, SIGNAL(valueChanged(int)), _subgroup4FaderStage, SLOT(setGain(int)));
    connect(ui->subgroup5VolumeVerticalSlider, SIGNAL(valueChanged(int)), _subgroup5FaderStage, SLOT(setGain(int)));
    connect(ui->subgroup6VolumeVerticalSlider, SIGNAL(valueChanged(int)), _subgroup6FaderStage, SLOT(setGain(int)));
    connect(ui->subgroup7VolumeVerticalSlider, SIGNAL(valueChanged(int)), _subgroup7FaderStage, SLOT(setGain(int)));
    connect(ui->subgroup8VolumeVerticalSlider, SIGNAL(valueChanged(int)), _subgroup8FaderStage, SLOT(setGain(int)));

    connect(ui->main1VolumeVerticalSlider, SIGNAL(valueChanged(int)), _main1FaderStage, SLOT(setGain(int)));
    connect(ui->main2VolumeVerticalSlider, SIGNAL(valueChanged(int)), _main2FaderStage, SLOT(setGain(int)));
}

MainMixerWidget::~MainMixerWidget()
{
    delete ui;
}


void MainMixerWidget::registerChannel(int i, ChannelWidget *channelWidget)
{
    _registeredChannels.insert(i, channelWidget);
}

void MainMixerWidget::process()
{
    // Obtaining sample buffers
    QSampleBuffer subgroup1SampleBuffer = _subGroup1Out->sampleBuffer();
    QSampleBuffer subgroup2SampleBuffer = _subGroup2Out->sampleBuffer();
    QSampleBuffer subgroup3SampleBuffer = _subGroup3Out->sampleBuffer();
    QSampleBuffer subgroup4SampleBuffer = _subGroup4Out->sampleBuffer();
    QSampleBuffer subgroup5SampleBuffer = _subGroup5Out->sampleBuffer();
    QSampleBuffer subgroup6SampleBuffer = _subGroup6Out->sampleBuffer();
    QSampleBuffer subgroup7SampleBuffer = _subGroup7Out->sampleBuffer();
    QSampleBuffer subgroup8SampleBuffer = _subGroup8Out->sampleBuffer();

    QSampleBuffer main1SampleBuffer = _mainLeftOut->sampleBuffer();
    QSampleBuffer main2SampleBuffer = _mainRightOut->sampleBuffer();

    // Clearing buffers (since we are going to sum up signals
    subgroup1SampleBuffer.clear();
    subgroup2SampleBuffer.clear();
    subgroup3SampleBuffer.clear();
    subgroup4SampleBuffer.clear();
    subgroup5SampleBuffer.clear();
    subgroup6SampleBuffer.clear();
    subgroup7SampleBuffer.clear();
    subgroup8SampleBuffer.clear();

    // Find out whether any of the channels is soloed
    bool soloActive = false;
    foreach(ChannelWidget *channelWidget, _registeredChannels) {
        if(channelWidget->isSoloed()) {
            soloActive = true;
        }
    }

    // Find out whether any of the subgroups are soloed
    bool subgroupSoloActive = false;
    if(ui->subgroup1SoloPushButton->isChecked()
    || ui->subgroup2SoloPushButton->isChecked()
    || ui->subgroup3SoloPushButton->isChecked()
    || ui->subgroup4SoloPushButton->isChecked()
    || ui->subgroup5SoloPushButton->isChecked()
    || ui->subgroup6SoloPushButton->isChecked()
    || ui->subgroup7SoloPushButton->isChecked()
    || ui->subgroup8SoloPushButton->isChecked()) {
        subgroupSoloActive = true;
    }

    // Routing channels to subgroups and main
    foreach(ChannelWidget *channelWidget, _registeredChannels) {
        // Create a temporary memory buffer, so we do not alter the sample in the input buffer,
        // which may effect other applications connected to the same input.
        QSampleBuffer sampleBuffer = QSampleBuffer::createMemoryAudioBuffer(QJackClient::instance()->bufferSize());

        // Do the processing for the channel
        channelWidget->process(sampleBuffer);

        // If the channel is not muted, apply to subgroups and main.
        if(!channelWidget->isMuted() && (!soloActive || channelWidget->isSoloed())) {
            double panorama = channelWidget->panorama();
            if(channelWidget->isInSubGroup12()) {
                sampleBuffer.addTo(subgroup1SampleBuffer, 1.0 - panorama);
                sampleBuffer.addTo(subgroup2SampleBuffer,       panorama);
            }

            if(channelWidget->isInSubGroup34()) {
                sampleBuffer.addTo(subgroup3SampleBuffer, 1.0 - panorama);
                sampleBuffer.addTo(subgroup4SampleBuffer,       panorama);
            }

            if(channelWidget->isInSubGroup56()) {
                sampleBuffer.addTo(subgroup5SampleBuffer, 1.0 - panorama);
                sampleBuffer.addTo(subgroup6SampleBuffer,       panorama);
            }

            if(channelWidget->isInSubGroup78()) {
                sampleBuffer.addTo(subgroup7SampleBuffer, 1.0 - panorama);
                sampleBuffer.addTo(subgroup8SampleBuffer,       panorama);
            }

            if(channelWidget->isOnMain()) {
                sampleBuffer.addTo(main1SampleBuffer, 1.0 - panorama);
                sampleBuffer.addTo(main2SampleBuffer,       panorama);
            }
        }

        sampleBuffer.releaseMemoryBuffer();
    }

    // Route subgroups through faders
    _subgroup1FaderStage->process(subgroup1SampleBuffer);
    _subgroup2FaderStage->process(subgroup2SampleBuffer);
    _subgroup3FaderStage->process(subgroup3SampleBuffer);
    _subgroup4FaderStage->process(subgroup4SampleBuffer);
    _subgroup5FaderStage->process(subgroup5SampleBuffer);
    _subgroup6FaderStage->process(subgroup6SampleBuffer);
    _subgroup7FaderStage->process(subgroup7SampleBuffer);
    _subgroup8FaderStage->process(subgroup8SampleBuffer);

    // Routing subgroups to main

    if(!ui->subgroup1MutePushButton->isChecked() && (!subgroupSoloActive || ui->subgroup1SoloPushButton->isChecked())) {
        subgroup1SampleBuffer.addTo(main1SampleBuffer);
    }
    if(!ui->subgroup2MutePushButton->isChecked() && (!subgroupSoloActive || ui->subgroup2SoloPushButton->isChecked())) {
        subgroup2SampleBuffer.addTo(main2SampleBuffer);
    }
    if(!ui->subgroup3MutePushButton->isChecked() && (!subgroupSoloActive || ui->subgroup3SoloPushButton->isChecked())) {
        subgroup3SampleBuffer.addTo(main1SampleBuffer);
    }
    if(!ui->subgroup4MutePushButton->isChecked() && (!subgroupSoloActive || ui->subgroup4SoloPushButton->isChecked())) {
        subgroup4SampleBuffer.addTo(main2SampleBuffer);
    }
    if(!ui->subgroup5MutePushButton->isChecked() && (!subgroupSoloActive || ui->subgroup5SoloPushButton->isChecked())) {
        subgroup5SampleBuffer.addTo(main1SampleBuffer);
    }
    if(!ui->subgroup6MutePushButton->isChecked() && (!subgroupSoloActive || ui->subgroup6SoloPushButton->isChecked())) {
        subgroup6SampleBuffer.addTo(main2SampleBuffer);
    }
    if(!ui->subgroup7MutePushButton->isChecked() && (!subgroupSoloActive || ui->subgroup7SoloPushButton->isChecked())) {
        subgroup7SampleBuffer.addTo(main1SampleBuffer);
    }
    if(!ui->subgroup8MutePushButton->isChecked() && (!subgroupSoloActive || ui->subgroup8SoloPushButton->isChecked())) {
        subgroup8SampleBuffer.addTo(main2SampleBuffer);
    }

    // Peak detection

    double peak1 = 0.0;
    double peak2 = 0.0;
    double peak3 = 0.0;
    double peak4 = 0.0;
    double peak5 = 0.0;
    double peak6 = 0.0;
    double peak7 = 0.0;
    double peak8 = 0.0;

    double mainPeak1 = 0.0;
    double mainPeak2 = 0.0;

    for(int i = 0; i < subgroup1SampleBuffer.size(); i++) {
        double sample1 = QUnits::peak(subgroup1SampleBuffer.readAudioSample(i));
        peak1 = sample1 > peak1 ? sample1 : peak1;
        double sample2 = QUnits::peak(subgroup2SampleBuffer.readAudioSample(i));
        peak2 = sample2 > peak2 ? sample2 : peak2;
        double sample3 = QUnits::peak(subgroup3SampleBuffer.readAudioSample(i));
        peak3 = sample3 > peak3 ? sample3 : peak3;
        double sample4 = QUnits::peak(subgroup4SampleBuffer.readAudioSample(i));
        peak4 = sample4 > peak4 ? sample4 : peak4;
        double sample5 = QUnits::peak(subgroup5SampleBuffer.readAudioSample(i));
        peak5 = sample5 > peak5 ? sample5 : peak5;
        double sample6 = QUnits::peak(subgroup6SampleBuffer.readAudioSample(i));
        peak6 = sample6 > peak6 ? sample6 : peak6;
        double sample7 = QUnits::peak(subgroup7SampleBuffer.readAudioSample(i));
        peak7 = sample7 > peak7 ? sample7 : peak7;
        double sample8 = QUnits::peak(subgroup8SampleBuffer.readAudioSample(i));
        peak8 = sample8 > peak8 ? sample8 : peak8;

        double sampleMain1 = QUnits::peak(main1SampleBuffer.readAudioSample(i));
        mainPeak1 = sampleMain1 > mainPeak1 ? sampleMain1 : mainPeak1;
        double sampleMain2 = QUnits::peak(main2SampleBuffer.readAudioSample(i));
        mainPeak2 = sampleMain2 > mainPeak2 ? sampleMain2 : mainPeak2;
    }
    _peak1 = QUnits::linearToDb(peak1);
    _peak2 = QUnits::linearToDb(peak2);
    _peak3 = QUnits::linearToDb(peak3);
    _peak4 = QUnits::linearToDb(peak4);
    _peak5 = QUnits::linearToDb(peak5);
    _peak6 = QUnits::linearToDb(peak6);
    _peak7 = QUnits::linearToDb(peak7);
    _peak8 = QUnits::linearToDb(peak8);

    _mainPeak1 = QUnits::linearToDb(mainPeak1);
    _mainPeak2 = QUnits::linearToDb(mainPeak2);

    // Check if main is muted, and clear signal if necessary
    if(ui->main1MutePushButton->isChecked()) {
        main1SampleBuffer.clear();
    } else {
        _main1FaderStage->process(main1SampleBuffer);
    }

    if(ui->main2MutePushButton->isChecked()) {
        main2SampleBuffer.clear();
    } else {
        _main2FaderStage->process(main2SampleBuffer);
    }
}

void MainMixerWidget::updateInterface()
{
    QJackClient *jackClient = QJackClient::instance();
    QString displayText;
    //displayText += QString("<table width=\"100%\"><tr><td><b>JACK Client</b></td><td></td></tr>");
    displayText += QString("<tr><td>RT processing:</td><td>%1</td></tr>").arg(jackClient->isRealtime() ? "Yes" : "No");
    displayText += QString("<tr><td>Buffers.:</td><td>%1 Samples</td></tr>").arg(jackClient->bufferSize());
    displayText += QString("<tr><td>CPU load:</td><td>%1</td></tr>").arg(jackClient->cpuLoad() < 5.0 ? "Idle" : QString("%1 %").arg((int)jackClient->cpuLoad()));
    displayText += QString("<tr><td>Samplerate:</td><td>%1 Hz</td></tr></table>").arg(jackClient->sampleRate());
    ui->displayLabel->setText(displayText);

    foreach(ChannelWidget *channelWidget, _registeredChannels) {
        channelWidget->updateInterface();
    }

    ui->subgroup1ProgressBar->setValue((int)_peak1);
    ui->subgroup2ProgressBar->setValue((int)_peak2);
    ui->subgroup3ProgressBar->setValue((int)_peak3);
    ui->subgroup4ProgressBar->setValue((int)_peak4);
    ui->subgroup5ProgressBar->setValue((int)_peak5);
    ui->subgroup6ProgressBar->setValue((int)_peak6);
    ui->subgroup7ProgressBar->setValue((int)_peak7);
    ui->subgroup8ProgressBar->setValue((int)_peak8);
}

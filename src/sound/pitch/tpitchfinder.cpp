

#include "tpitchfinder.h"
#include <QAudio>
#include <QDebug>
#include <QTimer>


/*static */
QStringList TpitchFinder::getAudioDevicesList() {
	templAudioFormat.setChannelCount(1);
    templAudioFormat.setSampleSize(16);
    templAudioFormat.setSampleRate(44100);
    templAudioFormat.setSampleType(QAudioFormat::SignedInt);
    templAudioFormat.setCodec("audio/pcm");
    QStringList devList;
    QList<QAudioDeviceInfo> dL = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for(int i = 0; i<dL.size(); i++) {
        if (dL[i].isFormatSupported(templAudioFormat))
            devList << dL[i].deviceName();
    }
    return devList;
}

QAudioFormat TpitchFinder::templAudioFormat = QAudioFormat();

//************************************************************************************/



TpitchFinder::TpitchFinder(QObject *parent) :
    QObject(parent),
    m_audioInput(0),
    m_lenght(0),
    m_maxPeak(0),
    m_deviceInfo(QAudioDeviceInfo::defaultInputDevice())
{    
	setAudioDevice(m_deviceInfo.deviceName());
// 	setAudioDevice("plughw:CARD=default,DEV=0");


}

TpitchFinder::~TpitchFinder()
{

}



void TpitchFinder::setAudioDevice(const QString &devN) {

    QList<QAudioDeviceInfo> dL = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for(int i = 0; i<dL.size(); i++) {
        if (dL[i].deviceName() == devN) {
            m_deviceInfo = dL[i];
            break;
        }
    }
	qDebug() << m_deviceInfo.deviceName();    

    if (m_audioInput)
        delete m_audioInput;
	if (!m_deviceInfo.isFormatSupported(templAudioFormat))
	  qDebug() << m_deviceInfo.deviceName() << "format unsupported !!";
    m_audioInput = new QAudioInput(m_deviceInfo, templAudioFormat, this);
}

void TpitchFinder::startSniffing() {
	
	
	m_buffer.resize(4096*2); // 2048 samples, 16 bits each
	m_buffer.fill(0);
	m_IOaudioDevice = m_audioInput->start();
	connect(m_IOaudioDevice, SIGNAL(readyRead()), this, SLOT(sniffedDataReady()));
		
}

void TpitchFinder::sniffedDataReady() {
	qint64 bytesReady = m_audioInput->bytesReady();
	qint64 bSize = m_buffer.size();
	qint64 toRead = qMin(bytesReady, bSize);
	qint64 dataRead = m_IOaudioDevice->read(m_buffer.data(), toRead);
	TsamplePeak peak;
	
	int i = 0;	
	for (i = 0; i < dataRead/2; i++) {
	  qint16 value = *reinterpret_cast<qint16*>(m_buffer.data()+i*2);
	}
	 
}
	
	






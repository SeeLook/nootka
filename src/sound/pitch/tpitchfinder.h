#ifndef TPICHFINDER_H
#define TPICHFINDER_H

#include <QObject>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioFormat>
//#include

class QTimer;

class TpitchFinder : public QObject
{
    Q_OBJECT
public:
    explicit TpitchFinder(QObject *parent = 0);
    ~TpitchFinder();

    static QStringList getAudioDevicesList();
	static QAudioFormat templAudioFormat;

    void setAudioDevice(const QString &devN);
	void startSniffing();
	qint16 maxPeak() { return m_maxPeak; }
	

signals:
	void pitchFound(float pitch);


private slots:
	void sniffedDataReady();
	void findPitch();
  
private:
    QAudioDeviceInfo m_deviceInfo;
    QAudioInput *m_audioInput;
	QIODevice *m_IOaudioDevice;
	QByteArray m_buffer;
	quint64 m_lenght;
	QTimer *m_timer;
	qint16 m_maxPeak;


};

#endif // TPICHFINDER_H

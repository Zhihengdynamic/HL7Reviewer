#ifndef QEXPORTXMLSTRUCT_H
#define QEXPORTXMLSTRUCT_H

#include <QVector>
#include <QStringList>

struct STUFilters
{
	QString m_highPass;
	QString m_lowPass;
	QString m_baselineDriftRemoval;
};

struct STUAnnotation
{
	QString m_type;
	QString m_offset;
};

struct STUWaveformSegment
{
	QString m_time;
	QString m_sampleRate;
	QString m_dataResolation;
	QString m_data;
	QVector<STUAnnotation*> m_annotation;

	STUWaveformSegment(){};
	~STUWaveformSegment()
	{
		for (QVector<STUAnnotation*>::iterator it=m_annotation.begin(); it!=m_annotation.end(); it++)
		{
			delete *it;
			*it = NULL;
		}
		m_annotation.clear();
	}
};

struct STUWaveform
{
	QString m_type;
	QString m_units;
	STUFilters m_filters;
	QVector<STUWaveformSegment*> m_waveformSegments;

	STUWaveform(){};
	~STUWaveform()
	{
		for (QVector<STUWaveformSegment*>::iterator it=m_waveformSegments.begin(); it!=m_waveformSegments.end(); it++)
		{
			delete *it;
			*it = NULL;
		}
		m_waveformSegments.clear();
	}
};

//////////////////////////////////////////////////////////////////////////
struct STUObservation
{
	QString m_type;
	QString m_channel;
	QString m_lead;
	QString m_location;
	QString m_units;
	QString m_value;

	STUObservation(){};
	~STUObservation(){};
};

#endif // QEXPORTXMLSTRUCT_H

#ifndef QSQLITEPARSER_H
#define QSQLITEPARSER_H

#include <QObject>

class QSQLiteParser : public QObject
{
	Q_OBJECT

public:
	QSQLiteParser(QObject *parent);
	~QSQLiteParser();

	bool ParsePatientList(const QString& dirName, QString& errMsg);

private:
	
};

#endif // QSQLITEPARSER_H

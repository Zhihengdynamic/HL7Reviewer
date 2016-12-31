#ifndef HL7REVIEWER_H
#define HL7REVIEWER_H

#include <QtGui/QMainWindow>
#include "ui_hl7reviewer.h"

class HL7Reviewer : public QMainWindow
{
	Q_OBJECT

public:
	HL7Reviewer(QWidget *parent = 0, Qt::WFlags flags = 0);
	~HL7Reviewer();

private:
	Ui::HL7ReviewerClass ui;
};

#endif // HL7REVIEWER_H

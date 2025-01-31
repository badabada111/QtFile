#ifndef __K_SVG_TOOL_BAR_H_
#define __K_SVG_TOOL_BAR_H_

#include <qlayout.h>
#include <kcentralwidget.h>
#include "kdirectorywatcher.h"
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QCheckBox>

class KToolBar : public QWidget
{
	Q_OBJECT

public:
	KToolBar(QWidget *parent);
	~KToolBar();
	QPushButton* m_pAddFile;
	QPushButton* m_pOptionFile;
	QLineEdit* m_pLineEdit;
	QCheckBox* m_pEnableNotifications;
signals:
	void directorySelected(const QString& directoryPath);
private:
	QHBoxLayout* m_pHLayout;
	QLabel* m_pquickAdd;
};

#endif

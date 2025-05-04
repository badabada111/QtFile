#ifndef __K_SVG_MENU_BAR_H_
#define __K_SVG_MENU_BAR_H_

#include <qlayout.h>
#include <QMenuBar>
#include <QMenu>
#include <QIcon>

class KMenuBar : public QWidget
{
	Q_OBJECT

public:
	KMenuBar(QWidget* parent);
	~KMenuBar();
	QAction* clearAction;
	QAction* stopAction;
	QAction* resumeAction;
	QAction* stopAllAction;
	QAction* resumeAllAction;
	QAction* addAction;
	QAction* moveAction;
	QAction* quitAction;
	QAction* saveAction;
	QAction* editAction;
	QAction* intoAction;
	QAction* pushAction;
	void createAction();
private:
	QHBoxLayout* m_pHLayout;
	QMenuBar* m_pMenuBar;
	
};

#endif

#include "kmenubar.h"

KMenuBar::KMenuBar(QWidget* parent)
	: QWidget(parent)
	, m_pHLayout(NULL)

{
	//ˮƽ���ֶ���
	m_pHLayout = new QHBoxLayout(this);
	m_pHLayout->setSpacing(0);
	m_pMenuBar = new QMenuBar(this);

	//// ���Զ��尴ť��ӵ���������
	m_pHLayout->addWidget(m_pMenuBar);

	// ����������ӵ� qwidget ��
	setLayout(m_pHLayout);
	createAction();
}

void KMenuBar::createAction()
{
     m_pMenuBar->setGeometry(0, 0, this->width(), 0);
     m_pMenuBar->setStyleSheet("QMenuBar {"
         "    background-color: rgba(0, 0, 0, 0);" // ��ȫ͸��
         "    border: none;" // ����ȥ���߿�
         "}");

     QMenu* fileMenu = m_pMenuBar->addMenu(u8"�ļ�");
     addAction = fileMenu->addAction(u8"����");
     addAction->setIcon(QIcon(":/icons/normal/option.png"));
     moveAction = fileMenu->addAction(u8"�Ƴ�");
     moveAction->setIcon(QIcon(":/icons/normal/move.png"));
     fileMenu->addSeparator();
     quitAction = fileMenu->addAction(u8"�˳�");
     quitAction->setIcon(QIcon(":/icons/normal/quit.png"));

     QMenu* CatalogueMenu = m_pMenuBar->addMenu(u8"Ŀ¼");
     resumeAction = CatalogueMenu->addAction(u8"�ָ�");
     resumeAction->setIcon(QIcon(":/icons/normal/play.png"));
     stopAction = CatalogueMenu->addAction(u8"��ͣ");
     stopAction->setIcon(QIcon(":/icons/normal/pause.png"));
     CatalogueMenu->addSeparator();

     resumeAllAction = CatalogueMenu->addAction(u8"ȫ���ָ�");
     resumeAllAction->setIcon(QIcon(":/icons/normal/playall.png"));
     stopAllAction = CatalogueMenu->addAction(u8"ȫ����ͣ");
     stopAllAction->setIcon(QIcon(":/icons/normal/pauseall.png"));
     CatalogueMenu->addSeparator();
     editAction = CatalogueMenu->addAction(u8"�༭");
     editAction->setIcon(QIcon(":/icons/normal/edit.png"));
     CatalogueMenu ->addSeparator();
     intoAction = CatalogueMenu->addAction(u8"���õ���");
     pushAction = CatalogueMenu->addAction(u8"���õ���");

     QMenu* logMenu = m_pMenuBar->addMenu(u8"��־");
     clearAction = logMenu->addAction(u8"���������־");
     clearAction->setIcon(QIcon(":/icons/normal/delete.png"));
     saveAction = logMenu->addAction(u8"��־����");
     saveAction->setIcon(QIcon(":/icons/normal/right.png"));

     QMenu* helpMenu = m_pMenuBar->addMenu(u8"����");
     QAction* aboutAction = helpMenu->addAction(u8"����");
     aboutAction->setIcon(QIcon(":/icons/normal/about.png"));
}

KMenuBar::~KMenuBar()
{
}

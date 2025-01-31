#include "kmenubar.h"

KMenuBar::KMenuBar(QWidget* parent)
	: QWidget(parent)
	, m_pHLayout(NULL)

{
	//水平布局对象
	m_pHLayout = new QHBoxLayout(this);
	m_pHLayout->setSpacing(0);
	m_pMenuBar = new QMenuBar(this);

	//// 将自定义按钮添加到布局器中
	m_pHLayout->addWidget(m_pMenuBar);

	// 将布局器添加到 qwidget 中
	setLayout(m_pHLayout);
	createAction();
}

void KMenuBar::createAction()
{
     m_pMenuBar->setGeometry(0, 0, this->width(), 0);
     m_pMenuBar->setStyleSheet("QMenuBar {"
         "    background-color: rgba(0, 0, 0, 0);" // 完全透明
         "    border: none;" // 可以去掉边框
         "}");

     QMenu* fileMenu = m_pMenuBar->addMenu(u8"文件");
     addAction = fileMenu->addAction(u8"新增");
     addAction->setIcon(QIcon(":/icons/normal/option.png"));
     moveAction = fileMenu->addAction(u8"移除");
     moveAction->setIcon(QIcon(":/icons/normal/move.png"));
     fileMenu->addSeparator();
     quitAction = fileMenu->addAction(u8"退出");
     quitAction->setIcon(QIcon(":/icons/normal/quit.png"));

     QMenu* CatalogueMenu = m_pMenuBar->addMenu(u8"目录");
     resumeAction = CatalogueMenu->addAction(u8"恢复");
     resumeAction->setIcon(QIcon(":/icons/normal/play.png"));
     stopAction = CatalogueMenu->addAction(u8"暂停");
     stopAction->setIcon(QIcon(":/icons/normal/pause.png"));
     CatalogueMenu->addSeparator();

     resumeAllAction = CatalogueMenu->addAction(u8"全部恢复");
     resumeAllAction->setIcon(QIcon(":/icons/normal/playall.png"));
     stopAllAction = CatalogueMenu->addAction(u8"全部暂停");
     stopAllAction->setIcon(QIcon(":/icons/normal/pauseall.png"));
     CatalogueMenu->addSeparator();
     editAction = CatalogueMenu->addAction(u8"编辑");
     editAction->setIcon(QIcon(":/icons/normal/edit.png"));
     CatalogueMenu ->addSeparator();
     intoAction = CatalogueMenu->addAction(u8"配置导入");
     pushAction = CatalogueMenu->addAction(u8"配置导出");

     QMenu* logMenu = m_pMenuBar->addMenu(u8"日志");
     clearAction = logMenu->addAction(u8"清除所有日志");
     clearAction->setIcon(QIcon(":/icons/normal/delete.png"));
     saveAction = logMenu->addAction(u8"日志保存");
     saveAction->setIcon(QIcon(":/icons/normal/right.png"));

     QMenu* helpMenu = m_pMenuBar->addMenu(u8"帮助");
     QAction* aboutAction = helpMenu->addAction(u8"关于");
     aboutAction->setIcon(QIcon(":/icons/normal/about.png"));
}

KMenuBar::~KMenuBar()
{
}

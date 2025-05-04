#include "ktoolbar.h"
#include <qDebug>

KToolBar::KToolBar(QWidget *parent)
	: QWidget(parent)
	, m_pHLayout(NULL)

{
	//水平布局对象
	m_pHLayout = new QHBoxLayout(this);
	m_pHLayout->setSpacing(0);
	m_pquickAdd = new QLabel(u8"快速新增：",this);
	m_pLineEdit = new QLineEdit(this);
	m_pAddFile = new QPushButton(u8"新增文件",this);
    m_pOptionFile = new QPushButton("...",this);
	m_pEnableNotifications = new QCheckBox(u8"启用消息弹窗", this);

	// 将自定义按钮添加到布局器中
	m_pHLayout->addWidget(m_pquickAdd);
	m_pHLayout->addWidget(m_pLineEdit);
	m_pHLayout->addWidget(m_pAddFile);
	m_pHLayout->addWidget(m_pOptionFile);
	m_pHLayout->addWidget(m_pEnableNotifications);

	// 将布局器添加到 qwidget 中
	setLayout(m_pHLayout);
}

KToolBar::~KToolBar()
{
}



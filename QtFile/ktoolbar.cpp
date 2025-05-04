#include "ktoolbar.h"
#include <qDebug>

KToolBar::KToolBar(QWidget *parent)
	: QWidget(parent)
	, m_pHLayout(NULL)

{
	//ˮƽ���ֶ���
	m_pHLayout = new QHBoxLayout(this);
	m_pHLayout->setSpacing(0);
	m_pquickAdd = new QLabel(u8"����������",this);
	m_pLineEdit = new QLineEdit(this);
	m_pAddFile = new QPushButton(u8"�����ļ�",this);
    m_pOptionFile = new QPushButton("...",this);
	m_pEnableNotifications = new QCheckBox(u8"������Ϣ����", this);

	// ���Զ��尴ť��ӵ���������
	m_pHLayout->addWidget(m_pquickAdd);
	m_pHLayout->addWidget(m_pLineEdit);
	m_pHLayout->addWidget(m_pAddFile);
	m_pHLayout->addWidget(m_pOptionFile);
	m_pHLayout->addWidget(m_pEnableNotifications);

	// ����������ӵ� qwidget ��
	setLayout(m_pHLayout);
}

KToolBar::~KToolBar()
{
}



#ifndef __K_SVG_CENTRAL_BAR_H_
#define __K_SVG_CENTRAL_BAR_H_

#include <QtSvg/qsvgrenderer.h>
#include <qlayout.h>
#include <kdirectorywatcher.h>
#include <QTableView>
#include <QTableWidget>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QLabel>
#include <QFileDialog>
#include <QDateTime>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QDateTimeEdit>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QQueue>
#include <QTimer>
#include <kcustomsortfilterproxymodel.h>

class KCentralWidget : public QWidget
{
	Q_OBJECT

public:
	KCentralWidget(QWidget* parent);
	~KCentralWidget();
	QTableWidget* m_pTableView1;
	QTableView* m_pTableView2;
	QTableView* m_pTableView3;
	QSqlTableModel* m_pTableViewModel2;
	QSqlTableModel* m_pTableViewModel3;
	QLineEdit* m_pSearchLineEdit;
	QPushButton* m_pSearchButton;
	QPushButton* m_pSortButton;
	QDateTimeEdit* m_pDateTimeBeginEdit;
	QDateTimeEdit* m_pDateTimeEndEdit;
	QPushButton* m_pInquireButton;
	QPushButton* m_pInquireRemuseButton;

	QLineEdit* m_pSearchLineEditTwo;
	QPushButton* m_pSearchButtonTwo;
	QPushButton* m_pSortButtonTwo;
	QDateTimeEdit* m_pDateTimeBeginEditTwo;
	QDateTimeEdit* m_pDateTimeEndEditTwo;
	QPushButton* m_pInquireButtonTwo;
	QPushButton* m_pInquireRemuseButtonTwo;

	void insertData(const QString& action, const QString& time, const QString& filepath);
	void insertDataTwo(const QString& action, const QString& time, const QString& filepath);

	void onRowDoubleClicked(const QModelIndex& index);
	void performRollback(const QString& directory, const QString& timestamp);
	void backupFile(const QString& filePath, const QString& timestamp);
	static void createFileBackup(const QString& file);
private:
	QVBoxLayout* m_pVLayout;
	QLabel* m_pquickAdd;
	QLineEdit* m_pLineEdit;
	KDirectoryWatcher* m_pDirectoryWatcher;
	QTabWidget* m_pLogTavWidget;
	QLabel* m_pSearchLabel;
	QLabel* m_pDateTimeBegin;
	QLabel* m_pDateTimeEnd;
	QLabel* m_pSearchLabelTwo;
	QLabel* m_pDateTimeBeginTwo;
	QLabel* m_pDateTimeEndTwo;
	bool m_isSorted; // 标记是否已排序
	QSqlDatabase m_db;
	QQueue<QList<QString>> m_dataBuffer;
	QTimer m_bufferTimer;  // 定时器，用于定期处理缓存区
	const int BUFFER_PROCESS_SIZE = 50;  // 每次处理的最大数据量
	void onHeaderClicked(int index);
	void onHeaderTwoClicked(int index);
	void processBuffer();
	void logDatabaseError(const QString& errorMsg);
};

class CustomItemDelegate : public QStyledItemDelegate
{
public:
	CustomItemDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

class RevertButtonDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit RevertButtonDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
	{
		if (index.column() == 4)  // 假设回溯按钮在第4列
		{
			QPushButton button("回溯");
			button.setStyleSheet("background-color: lightblue; border-radius: 5px;");
			QPixmap pixmap = button.grab();  // 将按钮绘制为图片
			painter->drawPixmap(option.rect.x() + (option.rect.width() - pixmap.width()) / 2,
				option.rect.y() + (option.rect.height() - pixmap.height()) / 2,
				pixmap);
		}
		else
		{
			QStyledItemDelegate::paint(painter, option, index);
		}
	}

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const override
	{
		QPushButton* button = new QPushButton("回溯", parent);
		connect(button, &QPushButton::clicked, this, &RevertButtonDelegate::handleRevert);
		return button;
	}

signals:
	void revertClicked(const QModelIndex& index) const;

private slots:
	void handleRevert()
	{
		QPushButton* button = qobject_cast<QPushButton*>(sender());
		if (button)
		{
			emit revertClicked(QModelIndex()); // 需要在外部绑定槽函数处理
		}
	}
};

#endif

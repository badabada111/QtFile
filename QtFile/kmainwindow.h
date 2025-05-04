#ifndef __K_MAINWINDOW_H_
#define __K_MAINWINDOW_H_

#include <QtWidgets/QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "ui_kmainwindow.h"
#include "ktoolbar.h"
#include "kcentralwidget.h"
#include "kmenubar.h"
#include <kcentralwidget.h>
#include <kselectedbar.h>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QLineEdit>
#include <QPushButton>
#include <QStatusBar>
#include <QSystemTrayIcon>
#include <QMap>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QTimer>
#include <QQueue>
#include <QListWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>

class KMainWindow : public QWidget
{
    Q_OBJECT

public:
    KMainWindow(QWidget *parent = Q_NULLPTR);
    void initWindows();
    void onAddFileButtonClicked();
    void onOptionFileButtonClicked();
    void onEditFileButtonClicked();
    void onDirectoryChanged(const QString& directory, const QString& action);
    void onStopClicked();
    void onResumeClicked();
    void onStopAllClicked();
    void onResumeAllClicked();
    void onRemoveFileButtonClicked();
    void onRemoveAllFileButtonClicked();
    void onSaveLogButtonClicked();
    void updateStatusBar();
    void initContextMenu();
    const QList<QString>& getWatchedFiles() const;
    QList<QString> watchedFiles;

protected:
    virtual void contextMenuEvent(QContextMenuEvent* event) override;
    virtual void closeEvent(QCloseEvent* event) override;
    virtual void dragEnterEvent(QDragEnterEvent* event) override;
    virtual void dropEvent(QDropEvent* event) override;

private slots:
    void createAction();
    void clearLog();

private:
    Ui::KMainWindowClass ui;
    QVBoxLayout *m_pVLayout;
    QHBoxLayout *m_pHLayout;
    KMenuBar* m_pMenuBar;
    KToolBar* m_pToolBar;
    KCentralWidget* m_pCentralWidget;
    QStatusBar* m_pStatusBar;
    KDirectoryWatcher* m_pDirectoryWatcher;
    QMap<QString, KDirectoryWatcher*> m_directoryWatchers;
    QString m_Directory;
    QMenu* m_pContextMenu;
    QSystemTrayIcon* m_SystemTray;//ϵͳ����
    void init_systemTrayIcon();//ϵͳ���̳�ʼ��
    QTimer* m_pTimer; // ������ QTimer ����
    QQueue<QString> m_fileChangeQueue; // ���ڴ洢�ļ��仯��Ϣ�Ķ���
    QListWidget* m_pSubDirListWidget;  // ������ʾ��Ŀ¼
    QString m_previousFilter; // ����֮ǰ�Ĺ�������
    QString m_previousTwoFilter; // ����֮ǰ�Ĺ�������
    void systemTrayIcon_activated(QSystemTrayIcon::ActivationReason reason);//��Ӧϵͳ���̵Ķ�����˫���ȣ�
    void quitMusicPlayer();
    void onTimerTimeout();
    void onDirectoryTextChanged(const QString& directory);
    void onSubDirItemClicked(QListWidgetItem* item);
    void onSearchButtonClicked();
    void onResumeSearchClicked();
    void onInquireButtonClicked();
    void onInquireResumeButtonClicked();
    void onSearchButtonTwoClicked();
    void onResumeSearchTwoClicked();
    void onInquireButtonTwoClicked();
    void onInquireResumeButtonTwoClicked();
    void clearDatabase();
    void exportLogs(const QString& filePath);
    void addUserOperationLog(const QString& operation, const QString& time, const QString& details);
    void onCheckboxStateChanged(int row, int col);
    void onImportButtonClicked();
    void onExportButtonClicked();
    void importData(const QString& filePath);
    void exportData(const QString& filePath);
    void onOpenFileButtonClicked();
    void addWatchDirectory(const QString& directory);

};

#endif

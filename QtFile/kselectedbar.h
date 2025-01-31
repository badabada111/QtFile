#ifndef KSELECTEDBAR_H
#define KSELECTEDBAR_H

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QSet>
#include <QMessageBox>
#include "kmainwindow.h"
#include <kdirectorywatcher.h>

class CustomDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CustomDialog(QWidget* parent = nullptr);
    QStringList getWatchExtensions() const;
    QStringList getIgnoreExtensions() const;
    QStringList getWatchFileExtensions() const;
    QStringList getIgnoreFileExtensions() const;
    void setFilePath(const QString& filePath);
    void setWatchExtensions(const QStringList& extensions);
    void setIgnoreExtensions(const QStringList& extensions);
    void setWatchFiles(const QStringList& files);
    void setIgnoreFiles(const QStringList& files);
    QString getFilePath() const;
    QList<QCheckBox*> getCheckBoxes() const;
    QLineEdit* m_pFilePathLineEdit;
    QLineEdit* m_pWatchExtensionsLineEdit;
    QLineEdit* m_pIgnoreExtensionsLineEdit;
    QLineEdit* m_pWatchExtensionsFileLineEdit;
    QLineEdit* m_pIgnoreExtensionsFileLineEdit;
    void openFileButtonClicked();
    QSet<int> getAllowedActions() const;
    bool isSubdirectoryWatched() const;
    void setSubdirectoryWatched(bool watched);
    void setDirectoryLineEditReadOnly(bool readOnly);

private:
    QList<QCheckBox*> checkBoxes;
    QPushButton* m_pOpenFileButton;
    KDirectoryWatcher* m_pDirectoryWatcher;
    QPushButton* m_pWatchFileButton;
    QPushButton* m_pIgnoreFileButton;
    void openWatchFileButtonClicked();
    void openIgnoreFileButtonClicked();
    bool isFileBeingWatched(const QString& filePath) const;
};

#endif // KSELECTEDBAR_H

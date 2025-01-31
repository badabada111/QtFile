#ifndef CUSTOMSORTFILTERPROXYMODEL_H
#define CUSTOMSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QString>
#include <QDateTime>

class KCustomSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit KCustomSortFilterProxyModel(QObject* parent = nullptr);

    void setFilterText(const QString& text);
    void setDateTimeRange(const QDateTime& begin, const QDateTime& end);
    void clearFilters();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;

private:
    QString m_filterText;
    QDateTime m_dateTimeBegin;
    QDateTime m_dateTimeEnd;
};

#endif // CUSTOMSORTFILTERPROXYMODEL_H
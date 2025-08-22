#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QString>
#include <QSortFilterProxyModel>
#include <filesystem>

class FileModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString currentDirectory READ currentDirectory WRITE setCurrentDirectory NOTIFY currentDirectoryChanged)

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        PathRole,
        SizeRole,
        TypeRole,
        IsDirRole
    };

    explicit FileModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void loadDirectory(const QString& path);
    Q_INVOKABLE void sortByType();
    Q_INVOKABLE void sortByName();
    Q_INVOKABLE void sortBySize();

    QString currentDirectory() const;
    void setCurrentDirectory(const QString& directory);

signals:
    void currentDirectoryChanged();

private:
    struct FileInfo {
        QString name;
        QString path;
        qint64 size;
        QString type;
        bool isDirectory;
        QString extension; // Добавляем для сортировки
    };

    QVector<FileInfo> m_files;
    QString m_currentDirectory;
};
#endif // FILEMODEL_H

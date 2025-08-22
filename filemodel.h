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
    Q_PROPERTY(bool showFullPath READ showFullPath WRITE setShowFullPath NOTIFY showFullPathChanged)

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        PathRole,
        RelativePathRole,
        SizeRole,
        TypeRole,
        ExtensionRole,
        DisplayNameRole
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

    bool showFullPath() const;
    void setShowFullPath(bool show);

signals:
    void currentDirectoryChanged();
    void showFullPathChanged();

private:
    struct FileInfo {
        QString name;
        QString path;
        QString relativePath;
        qint64 size;
        QString type;
        QString extension;
    };

    QVector<FileInfo> m_files;
    QString m_currentDirectory;
    bool m_showFullPath = false;
};
#endif // FILEMODEL_H

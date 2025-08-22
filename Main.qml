import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    visible: true
    width: 1200
    height: 800
    title: "File Browser - " + (fileModel.currentDirectory || "Current Directory")

    ColumnLayout {
        anchors.fill: parent
        spacing: 5

        // Панель управления
        RowLayout {
            Layout.fillWidth: true
            Layout.margins: 5

            Button {
                text: "Sort by Name"
                onClicked: fileModel.sortByName()
            }

            Button {
                text: "Sort by Type"
                onClicked: fileModel.sortByType()
            }

            Button {
                text: "Sort by Size"
                onClicked: fileModel.sortBySize()
            }

            Text {
                text: "Total files: " + listView.count
                color: "gray"
            }

            Text {
                text: fileModel.currentDirectory
                Layout.fillWidth: true
                elide: Text.ElideLeft
                color: "gray"
            }

            Button {
                text: "Refresh"
                onClicked: fileModel.loadDirectory(fileModel.currentDirectory)
            }
        }

        // Таблица файлов
        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: fileModel
            clip: true

            // Заголовок таблицы
            header: RowLayout {
                width: listView.width
                height: 40
                spacing: 5

                Text {
                    text: "Path";
                    Layout.preferredWidth: 600
                    font.bold: true
                    horizontalAlignment: Text.AlignLeft
                }
                Text {
                    text: "Size";
                    Layout.preferredWidth: 150
                    font.bold: true
                    horizontalAlignment: Text.AlignRight
                }
                Text {
                    text: "Type";
                    Layout.preferredWidth: 200
                    font.bold: true
                    horizontalAlignment: Text.AlignLeft
                }
            }

            // Элемент списка
            delegate: RowLayout {
                width: listView.width
                height: 30
                spacing: 5

                Text {
                    text: model.relativePath  // Показываем относительный путь
                    Layout.preferredWidth: 600
                    elide: Text.ElideRight
                    color: model.isDirectory ? "blue" : "black"
                    font.bold: model.isDirectory
                }
                Text {
                    text: model.isDirectory ? "" : formatSize(model.size)
                    Layout.preferredWidth: 150
                    horizontalAlignment: Text.AlignRight
                    color: "gray"
                }
                Text {
                    text: model.type
                    Layout.preferredWidth: 200
                    elide: Text.ElideRight
                    color: model.isDirectory ? "blue" : "darkgreen"
                }
            }

            ScrollBar.vertical: ScrollBar {}
        }
    }

    // Функция форматирования размера файла
    function formatSize(bytes) {
        if (bytes === 0) return "0 B";
        const sizes = ['B', 'KB', 'MB', 'GB'];
        const i = Math.floor(Math.log(bytes) / Math.log(1024));
        return Math.round(bytes / Math.pow(1024, i) * 100) / 100 + ' ' + sizes[i];
    }
}

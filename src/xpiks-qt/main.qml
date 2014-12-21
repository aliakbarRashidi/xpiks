/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.4
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import xpiks 1.0

ApplicationWindow {
    id: applicationWindow
    visible: true
    width: 900
    height: 640
    minimumHeight: 640
    minimumWidth: 800
    title: qsTr("Hello World")

    onClosing: {
        if (artItemsModel.modifiedArtworksCount > 0) {
            close.accepted = false
            configExitDialog.open()
        }
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("&Settings")
                onTriggered: {
                    console.log("Settings action triggered");
                    var component = Qt.createComponent("SettingsWindow.qml");
                    var window = component.createObject(applicationWindow);
                    window.show();
                }
            }
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
    }

    MessageDialog {
        id: configExitDialog

        title: "Confirmation"
        text: qsTr("You have some artworks modified. Really exit?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            Qt.quit()
        }
    }

    MessageDialog {
        id: confirmRemoveSelectedDialog
        property int itemsCount
        title: "Confirmation"
        text: qsTr("Are you sure you want to remove %1 item(s)?").arg(itemsCount)
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            artItemsModel.removeSelectedArtworks()
        }
    }

    MessageDialog {
        id: confirmRemoveDirectoryDialog
        property int directoryIndex
        title: "Confirmation"
        text: qsTr("Are you sure you want to remove this directory?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            artItemsModel.removeArtworksDirectory(directoryIndex)
        }
    }

    FileDialog {
        id: chooseArtworksDialog
        title: "Please choose artworks"
        selectExisting: true
        selectMultiple: true
        nameFilters: [ "Jpeg images (*.jpg), All files (*)" ]

        onAccepted: {
            console.log("You chose: " + chooseArtworksDialog.fileUrls)
            artItemsModel.addLocalArtworks(chooseArtworksDialog.fileUrls)
            Qt.createComponent("ImportMetadata.qml").createObject(applicationWindow, {})
        }

        onRejected: {
            console.log("Artworks dialog canceled")
        }
    }

    FileDialog {
        id: chooseDirectoryDialog
        title: "Please choose artworks"
        selectExisting: true
        selectMultiple: false
        selectFolder: true

        onAccepted: {
            console.log("You chose: " + chooseDirectoryDialog.fileUrl)
            artItemsModel.addLocalDirectory(chooseDirectoryDialog.fileUrl)
            Qt.createComponent("ImportMetadata.qml").createObject(applicationWindow, {})
        }

        onRejected: {
            console.log("Directory dialog canceled")
        }
    }

    MessageDialog {
        id: mustSaveWarning
        title: "Warning"
        text: "Please, save selected items before upload"
    }

    SplitView {
        id: mainGrid
        anchors.fill: parent
        orientation: Qt.Horizontal

        ColumnLayout {
            Layout.minimumWidth: 250
            Layout.preferredWidth: 250
            Layout.maximumWidth: 350

            spacing: 5

            Rectangle {
                Layout.fillWidth: true
                height: 30
                color: "#dddddd"

                RowLayout {
                    spacing: 5
                    anchors.fill: parent

                    Item {
                        width: 1
                    }

                    Button {
                        text: qsTr("Add directory")
                        onClicked: chooseDirectoryDialog.open()
                    }

                    Button {
                        text: qsTr("Add files")
                        onClicked: chooseArtworksDialog.open()
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }

            }

            ListView {
                id: sourcesListView
                model: artworkRepository
                boundsBehavior: Flickable.StopAtBounds
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.minimumWidth: 250
                Layout.maximumWidth: 350

                spacing: 10

                delegate: Rectangle {
                    id: wrapperRect
                    property int indexOfThisDelegate: index
                    color: "white"
                    width: parent.width
                    height: 20
                    Layout.minimumWidth: 250

                    RowLayout {
                        spacing: 10
                        anchors.fill: parent

                        Item {
                            id: placeholder1
                            width: 1
                        }

                        Text {
                            id: directoryPath
                            Layout.fillWidth: true
                            anchors.verticalCenter: parent.verticalCenter
                            height: 20
                            text: path + "(" + usedimagescount + ")"
                            elide: Text.ElideMiddle
                        }

                        Item {
                            width: 14
                            height: 14

                            Rectangle {
                                anchors.fill: parent
                                radius: 7
                                border.width: 1
                                color: "#dddddd"
                                border.color: "black"

                                Rectangle {
                                    width: parent.width - 4
                                    anchors.centerIn: parent
                                    height: 2
                                    radius: 1
                                    border.color: "black"
                                    border.width: 1
                                }
                            }

                            opacity: removeDirectoryMouseArea.containsMouse ? 1 : 0.5
                            scale: removeDirectoryMouseArea.pressed ? 0.8 : 1

                            MouseArea {
                                id: removeDirectoryMouseArea
                                anchors.fill: parent
                                hoverEnabled: true

                                onClicked: {
                                    confirmRemoveDirectoryDialog.directoryIndex = wrapperRect.indexOfThisDelegate
                                    confirmRemoveDirectoryDialog.open()

                                }
                            }
                        }

                        Item {
                            id: placeholder2
                            width: 1
                        }
                    }
                }
            }
        }

        ColumnLayout {
            spacing: 5

            Rectangle {
                color: "#dddddd"
                z: 10000
                Layout.fillWidth: true
                height: 30

                RowLayout {
                    spacing: 5
                    anchors.fill: parent

                    Item {
                        width: 3
                    }

                    CheckBox {
                        id: selectAllCheckbox
                        checked: artItemsModel.selectedArtworksCount > 0

                        onClicked: {
                            if (checked) {
                                artItemsModel.selectAllArtworks();
                            }
                            else {
                                artItemsModel.unselectAllArtworks();
                            }
                        }

                        Connections {
                            target: artItemsModel
                            onSelectedArtworksCountChanged: {
                                selectAllCheckbox.checked = artItemsModel.selectedArtworksCount > 0
                            }
                        }
                    }

                    Button {
                        text: qsTr("Remove Selected")
                        onClicked: {
                            var itemsCount = artItemsModel.selectedArtworksCount
                            if (itemsCount > 0) {
                                confirmRemoveSelectedDialog.itemsCount = itemsCount
                                confirmRemoveSelectedDialog.open()
                            }
                        }
                    }

                    Button {
                        text: qsTr("Edit Selected")
                        onClicked: {
                            if (artItemsModel.selectedArtworksCount > 0) {
                                combinedArtworks.resetModelData();
                                artItemsModel.combineSelectedArtworks();
                                Qt.createComponent("CombinedArtworksDialog.qml").createObject(applicationWindow, {});
                            }
                        }
                    }

                    // TODO: status line like reshaper (X items modified)
                    Item {
                        Layout.fillWidth: true
                    }

                    Button {
                        text: qsTr("Save Selected")
                        onClicked: {
                            if (artItemsModel.selectedArtworksCount > 0) {
                                iptcProvider.resetModel()
                                artItemsModel.patchSelectedArtworks()
                                Qt.createComponent("ExportMetadata.qml").createObject(applicationWindow, {})
                            }
                        }
                    }

                    Button {
                        text: qsTr("Upload Selected")
                        onClicked: {
                            if (artItemsModel.areSelectedArtworksSaved()) {
                                artworkUploader.resetModel()
                                artItemsModel.uploadSelectedArtworks()
                                Qt.createComponent("UploadArtworks.qml").createObject(applicationWindow, {})
                            } else {
                                mustSaveWarning.open()
                            }
                        }
                    }

                    Item {
                        width: 1
                    }
                }
            }

            ListView {
                id: imagesListView
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: artItemsModel
                boundsBehavior: Flickable.StopAtBounds
                spacing: 2

                delegate: Rectangle {
                    id: wrapperRectangle
                    color: "#dddddd"
                    property int indexOfThisDelegate: index

                    width: parent.width
                    height: 200

                    RowLayout {
                        anchors.fill: parent
                        anchors.rightMargin: 5
                        spacing: 5

                        Rectangle {
                            id: isModifiedRectangle
                            color: ismodified ? "orange" : "green"
                            width: 3
                            Layout.fillHeight: true
                        }

                        CheckBox {
                            id: itemCheckedCheckbox
                            checked: isselected
                            onClicked: editisselected = checked
                            Component.onCompleted: itemCheckedCheckbox.checked = isselected
                            Connections {
                                target: artItemsModel
                                onSelectedArtworksCountChanged: {
                                    itemCheckedCheckbox.checked = isselected
                                }
                            }
                        }

                        ColumnLayout {
                            width: 150
                            spacing: 5

                            Item {
                                Layout.fillHeight: true
                            }

                            Rectangle {
                                width: 150
                                Layout.minimumHeight: 100
                                Layout.maximumHeight: 150
                                anchors.horizontalCenter: parent.horizontalCenter
                                color: "transparent"
                                Image {
                                    anchors.fill: parent
                                    source: "image://global/" + filename
                                    sourceSize.width: 150
                                    sourceSize.height: 150
                                    fillMode: Image.PreserveAspectFit
                                    asynchronous: true
                                }
                            }

                            Text {
                                Layout.fillWidth: true
                                elide: Text.ElideMiddle
                                horizontalAlignment: Text.AlignHCenter
                                text: filename.split(/[\\/]/).pop()
                                renderType: Text.NativeRendering
                            }

                            Item {
                                Layout.fillHeight: true
                            }
                        }

                        ColumnLayout {
                            id: columnLayout
                            spacing: 2

                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            Text {
                                text: qsTr("Description:")
                                anchors.left: parent.left
                                renderType: Text.NativeRendering
                            }

                            Rectangle {
                                id: rect
                                width: 300
                                height: 25
                                color: "white"
                                anchors.left: parent.left

                                TextInput {
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.leftMargin: 5
                                    anchors.rightMargin: 5
                                    anchors.verticalCenter: parent.verticalCenter
                                    maximumLength: 250
                                    clip: true
                                    text: description
                                    onTextChanged: model.editdescription = text
                                    renderType: TextInput.NativeRendering
                                    Keys.onTabPressed: {
                                        flv.activateEdit()
                                    }
                                }
                            }

                            RowLayout {
                                Layout.fillWidth: true

                                Text {
                                    id: keywordsLabel
                                    anchors.left: parent.left
                                    text: qsTr("Keywords:")
                                }

                                Item {
                                    Layout.fillWidth: true
                                }

                                Text {
                                    text: "<a href=\"#\">" + qsTr("Copy keywords") + "</a>"
                                    linkColor: "blue"

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: clipboard.setText(keywordsstring)
                                    }
                                }
                            }

                            Rectangle {
                                id: keywordsWrapper
                                color: "#adadad"
                                Layout.fillHeight: true
                                Layout.fillWidth: true

                                function removeKeyword(index) {
                                    artItemsModel.removeKeywordAt(wrapperRectangle.indexOfThisDelegate, index)
                                }

                                function removeLastKeyword() {
                                    artItemsModel.removeLastKeyword(wrapperRectangle.indexOfThisDelegate)
                                }

                                function appendKeyword(keyword) {
                                    artItemsModel.appendKeyword(wrapperRectangle.indexOfThisDelegate, keyword)
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    propagateComposedEvents: true
                                    onClicked: {
                                        flv.activateEdit()
                                        mouse.accepted = false
                                    }
                                }

                                ScrollView {
                                    id: scroller
                                    anchors.fill: parent
                                    highlightOnFocus: true

                                    EditableTags {
                                        id: flv
                                        anchors.margins: 5
                                        model: keywords

                                        delegate: Rectangle {
                                            id: itemWrapper
                                            property int indexOfThisDelegate: index
                                            property string keyword: modelData
                                            border.width: 1
                                            border.color: "black"
                                            color: "#cccccc"

                                            width: childrenRect.width
                                            height: childrenRect.height

                                            RowLayout {
                                                Rectangle {
                                                    color: "transparent"
                                                    width: childrenRect.width + 15
                                                    height: 30

                                                    Text {
                                                        anchors.left: parent.left
                                                        anchors.leftMargin: 10
                                                        anchors.top: parent.top
                                                        anchors.bottom: parent.bottom
                                                        verticalAlignment: Text.AlignVCenter
                                                        text: modelData
                                                        renderType: Text.NativeRendering
                                                    }
                                                }

                                                CloseIcon {
                                                    onItemClicked: keywordsWrapper.removeKeyword(itemWrapper.indexOfThisDelegate)
                                                }

                                                Item {
                                                    width: 5
                                                }
                                            }
                                        }

                                        onTagAdded: {
                                            keywordsWrapper.appendKeyword(text)
                                        }

                                        onRemoveLast: {
                                            keywordsWrapper.removeLastKeyword()
                                        }
                                    }
                                }
                            }

                            Item {
                                height: 1
                            }
                        }
                    }
                }
            }

            ClipboardHelper {
                id: clipboard
            }
        }
    }

    statusBar: StatusBar {
        RowLayout {
            implicitHeight: 13
            width: parent.width
            spacing: 5

            Item {
                Layout.fillWidth: true
            }

            Text {
                text: qsTr("(%1) item(s) selected").arg(artItemsModel.selectedArtworksCount)
            }

            Text {
                text: "|"
            }

            Text {
                text: qsTr("(%1) item(s) modified").arg(artItemsModel.modifiedArtworksCount)
            }
        }
    }
}

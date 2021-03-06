/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2019 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.1
import QtQuick.Controls.Styles 1.1
import QtGraphicalEffects 1.0
import xpiks 1.0
import "../Constants"
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"
import "../Constants/UIConfig.js" as UIConfig

StaticDialogBase {
    id: plainTextKeywordsComponent
    canEscapeClose: false
    property string keywordsText
    property var callbackObject
    property var keywordsModel
    anchors.fill: parent

    function closePopup() {
        callbackObject.onClose()
        plainTextKeywordsComponent.destroy()
    }

    Component.onCompleted: {
        focus = true
        textEdit.forceActiveFocus()
    }

    Connections {
        target: keywordsModel
        onAboutToBeRemoved: {
            closePopup()
        }
    }

    Keys.onEscapePressed: closePopup()

    function scrollToBottom() {
        var flickable = flick
        if (flickable.contentHeight > flickable.height) {
            flickable.contentY = flickable.contentHeight - flickable.height
        } else {
            flickable.contentY = 0
        }
    }

    function submitKeywords() {
        callbackObject.onSuccess(textEdit.text, spaceAsSeparatorCheckBox.checked)
        closePopup()
    }

    contentsWidth: 600
    contentsHeight: 400

    contents: Item {
        anchors.fill: parent

        StyledText {
            id: header
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 20
            anchors.leftMargin: 20
            text: i18.n + qsTr("Keywords")
        }

        Rectangle {
            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            anchors.topMargin: 10
            anchors.bottom: footer.top
            anchors.bottomMargin: 20
            color: uiColors.popupDarkInputBackground

            Flickable {
                id: flick
                anchors.fill: parent
                anchors.margins: 10
                clip: true
                contentWidth: textEdit.paintedWidth
                contentHeight: textEdit.paintedHeight

                function ensureVisible(r) {
                    if (contentX >= r.x)
                        contentX = r.x;
                    else if (contentX+width <= r.x+r.width)
                        contentX = r.x+r.width-width;
                    if (contentY >= r.y)
                        contentY = r.y;
                    else if (contentY+height <= r.y+r.height)
                        contentY = r.y+r.height-height;
                }

                StyledTextEdit {
                    id: textEdit
                    anchors.top: parent.top
                    width: flick.width - 10
                    height: flick.height
                    focus: true
                    text: plainTextKeywordsComponent.keywordsText
                    font.pixelSize: UIConfig.fontPixelSize*settingsModel.keywordSizeScale
                    selectionColor: uiColors.inputBackgroundColor
                    wrapMode: TextEdit.Wrap
                    horizontalAlignment: TextEdit.AlignLeft
                    verticalAlignment: TextEdit.AlignTop
                    textFormat: TextEdit.PlainText

                    Component.onCompleted: {
                        scrollToBottom()
                        textEdit.cursorPosition += plainTextKeywordsComponent.keywordsText.length
                    }

                    Keys.onPressed: {
                        if(event.matches(StandardKey.Paste)) {
                            var clipboardText = clipboard.getText();
                            if (Common.safeInsert(textEdit, clipboardText)) {
                                event.accepted = true
                            }
                        } else if ((event.key === Qt.Key_Return) || (event.key === Qt.Key_Enter)) {
                            if (event.modifiers === Qt.ControlModifier) {
                                submitKeywords()
                            } else {
                                event.accepted = true
                            }
                        }
                    }

                    Keys.onBacktabPressed: {
                        event.accepted = true
                    }

                    Keys.onTabPressed: {
                        event.accepted = true
                    }

                    onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
                }
            }

            CustomScrollbar {
                anchors.topMargin: -5
                anchors.bottomMargin: -5
                anchors.rightMargin: -5
                flickable: flick
            }
        }

        RowLayout {
            id: footer
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.right: parent.right
            anchors.rightMargin: 20
            height: 24
            spacing: 20

            StyledCheckbox {
                id: spaceAsSeparatorCheckBox
                text: i18.n + qsTr("Treat space as separator")
                checked: false
            }

            Item {
                Layout.fillWidth: true
            }

            StyledButton {
                id: okButton
                text: i18.n + qsTr("Save")
                width: 100
                onClicked: submitKeywords()
            }

            StyledButton {
                text: i18.n + qsTr("Cancel")
                width: 100
                onClicked: closePopup()
            }
        }
    }

    ClipboardHelper {
        id: clipboard
    }
}

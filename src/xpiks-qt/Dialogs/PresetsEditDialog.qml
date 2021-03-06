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

StaticDialogBase {
    id: presetEditComponent
    anchors.fill: parent

    property variant componentParent
    property var autoCompleteBox

    function onAutoCompleteClose() {
        autoCompleteBox = undefined
    }

    MessageDialog {
        id: confirmRemoveItemDialog
        property int itemIndex
        title: i18.n + qsTr("Confirmation")
        text: i18.n + qsTr("Are you sure you want to remove this item?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            presetsModel.removeItem(itemIndex);

            if (presetNamesListView.count == 0) {
                addPresetButton.forceActiveFocus()
            }
        }
    }

    ClipboardHelper {
        id: clipboard
    }

    contentsWidth: 700
    contentsHeight: 450

    contents: Item {
        anchors.fill: parent

        Rectangle {
            id: leftPanel
            color: uiColors.defaultControlColor
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 250

            ListView {
                id: presetNamesListView
                objectName: "presetNamesListView"
                model: presetsModel
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: leftFooter.top
                anchors.topMargin: 30
                anchors.bottomMargin: 10
                clip: true
                boundsBehavior: Flickable.StopAtBounds

                displaced: Transition {
                    NumberAnimation { properties: "x,y"; duration: 230 }
                }

                addDisplaced: Transition {
                    NumberAnimation { properties: "x,y"; duration: 230 }
                }

                removeDisplaced: Transition {
                    NumberAnimation { properties: "x,y"; duration: 230 }
                }

                delegate: Rectangle {
                    id: sourceWrapper
                    objectName: "presetDelegate"
                    property variant myData: model
                    property int delegateIndex: index
                    property bool isCurrent: ListView.isCurrentItem
                    color: ListView.isCurrentItem ? uiColors.popupBackgroundColor : (exportPlanMA.containsMouse ? uiColors.panelColor :  leftPanel.color)
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 50

                    MouseArea {
                        id: exportPlanMA
                        hoverEnabled: true
                        anchors.fill: parent
                        //propagateComposedEvents: true
                        //preventStealing: false

                        onClicked: {
                            if (presetNamesListView.currentIndex != sourceWrapper.delegateIndex) {
                                presetNamesListView.currentIndex = sourceWrapper.delegateIndex

                                groupsCombobox.updateSelectedGroup()
                                //uploadInfos.updateProperties(sourceWrapper.delegateIndex)
                            }
                        }
                    }

                    RowLayout {
                        spacing: 10
                        anchors.fill: parent

                        Item {
                            width: 10
                        }

                        StyledText {
                            id: infoTitle
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignVCenter
                            height: 31
                            text: name
                            elide: Text.ElideMiddle
                        }

                        CloseIcon {
                            id: closeIcon
                            width: 14
                            height: 14
                            anchors.verticalCenterOffset: 1
                            isActive: false

                            onItemClicked: {
                                confirmRemoveItemDialog.itemIndex = sourceWrapper.delegateIndex
                                confirmRemoveItemDialog.open()
                            }
                        }

                        Item {
                            id: placeholder2
                            width: 15
                        }
                    }
                }

                Component.onCompleted: {
                    if (count > 0) {
                        titleText.forceActiveFocus()
                        titleText.cursorPosition = titleText.text.length
                    }
                }
            }

            CustomScrollbar {
                id: exportPlansScroll
                anchors.topMargin: 0
                anchors.bottomMargin: 0
                anchors.rightMargin: 0
                flickable: presetNamesListView
                canShow: !rightPanelMA.containsMouse
            }

            Item {
                id: leftFooter
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 50

                StyledBlackButton {
                    id: addPresetButton
                    objectName: "addPresetButton"
                    width: 210
                    height: 30
                    anchors.centerIn: parent
                    text: i18.n + qsTr("Add new", "preset")
                    onClicked: {
                        presetsModel.addItem()
                        presetNamesListView.currentIndex = presetNamesListView.count - 1

                        // fake "default" group
                        groupsCombobox.selectedIndex = 0

                        titleText.forceActiveFocus()
                        titleText.cursorPosition = titleText.text.length
                    }
                }
            }
        }

        Item {
            id: rightPanel
            anchors.left: leftPanel.right
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: footer.top

            Connections {
                target: acSource

                onCompletionsAvailable: {
                    acSource.initializeCompletions()

                    if (typeof presetEditComponent.autoCompleteBox !== "undefined") {
                        if (presetEditComponent.autoCompleteBox.isBelowEdit) {
                            // update completion
                            return;
                        }
                    }

                    var directParent = presetEditComponent;
                    var currWordStartRect = flv.editControl.getCurrentWordStartRect()

                    var tmp = flv.editControl.mapToItem(directParent,
                                                        currWordStartRect.x - 17,
                                                        flv.editControl.height + 1)

                    var visibleItemsCount = Math.min(acSource.getCount(), 5);
                    var popupHeight = visibleItemsCount * (25 + 1) + 10

                    if (typeof presetEditComponent.autoCompleteBox !== "undefined") {
                        if (!presetEditComponent.autoCompleteBox.isBelowEdit) {
                            presetEditComponent.autoCompleteBox.anchors.topMargin = tmp.y - popupHeight - flv.editControl.height - 2
                        }
                        // update completion
                        return
                    }

                    var isBelow = (tmp.y + popupHeight) < directParent.height;

                    var options = {
                        model: acSource.getCompletionsModelObject(),
                        autoCompleteSource: acSource,
                        isBelowEdit: isBelow,
                        withPresets: false,
                        "anchors.left": directParent.left,
                        "anchors.leftMargin": Math.min(tmp.x, directParent.width - 200),
                        "anchors.top": directParent.top
                    }

                    if (isBelow) {
                        options["anchors.topMargin"] = tmp.y
                    } else {
                        options["anchors.topMargin"] = tmp.y - popupHeight - flv.editControl.height - 2
                    }

                    var component = Qt.createComponent("../Components/CompletionBox.qml");
                    if (component.status !== Component.Ready) {
                        console.warn("Component Error: " + component.errorString());
                    } else {
                        var instance = component.createObject(directParent, options);

                        instance.boxDestruction.connect(presetEditComponent.onAutoCompleteClose)
                        instance.itemSelected.connect(flv.acceptCompletion)
                        presetEditComponent.autoCompleteBox = instance

                        instance.openPopup()
                    }
                }
            }

            MouseArea {
                id: rightPanelMA
                anchors.fill: parent
                hoverEnabled: true
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                anchors.topMargin: 20
                spacing: 4

                StyledText {
                    text: i18.n + qsTr("Title:")
                    isActive: presetNamesListView.count > 0
                }

                Rectangle {
                    id: titleWrapper
                    border.width: titleText.activeFocus ? 1 : 0
                    property bool isValid: presetNamesListView.currentItem ? presetNamesListView.currentItem.myData.isnamevalid : false
                    border.color: isValid ? uiColors.artworkActiveColor : uiColors.artworkModifiedColor
                    Layout.fillWidth: true
                    color: enabled ? uiColors.inputBackgroundColor : uiColors.inputInactiveBackground
                    height: 30

                    StyledTextInput {
                        id: titleText
                        objectName: "titleEdit"
                        height: parent.height
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.rightMargin: 5
                        text: presetNamesListView.currentItem ? presetNamesListView.currentItem.myData.name : ""
                        anchors.leftMargin: 5
                        enabled: presetNamesListView.count > 0

                        onTextChanged: {
                            if (presetNamesListView.currentItem) {
                                presetNamesListView.currentItem.myData.editname = text
                            }
                        }

                        Keys.onTabPressed: {
                            flv.activateEdit()
                        }

                        onEditingFinished: {
                            if (titleText.length == 0) {
                                if (presetNamesListView.currentItem) {
                                    presetNamesListView.currentItem.myData.editname = qsTr("Untitled")
                                }
                            }
                            presetsModel.makeTitleValid(presetNamesListView.currentIndex)
                        }

                        onActiveFocusChanged: {
                            if (!activeFocus) {
                                //ftpListAC.cancelCompletion()
                            }
                        }

                        validator: RegExpValidator {
                            // copy paste in keys.onpressed Paste
                            regExp: /[a-zA-Z0-9 _-]*$/
                        }

                        Keys.onPressed: {
                            if (event.matches(StandardKey.Paste)) {
                                var clipboardText = clipboard.getText();
                                clipboardText = clipboardText.replace(/(\r\n|\n|\r)/gm, '');
                                // same regexp as in validator
                                var sanitizedText = clipboardText.replace(/[^a-zA-Z0-9 _-]/g, '');
                                titleText.paste(sanitizedText)
                                event.accepted = true
                            }
                        }
                    }
                }

                Item {
                    height: 12
                }

                StyledText {
                    text: i18.n + qsTr("Group:")
                    isActive: presetNamesListView.count > 0
                }

                ComboBoxPopup {
                    id: groupsCombobox
                    objectName: "groupsCombobox"
                    model: presetsGroups.groupNames
                    hasLastItemAction: true
                    enabled: presetNamesListView.count > 0
                    lastActionText: i18.n + qsTr("Add group...", "preset group")
                    width: 200
                    height: 24
                    itemHeight: 28
                    showColorSign: false
                    dropDownWidth: width
                    glowEnabled: true
                    glowTopMargin: 2
                    globalParent: presetEditComponent

                    onComboItemSelected: {
                        if (presetNamesListView.currentItem) {
                            var groupID = presetsGroups.findGroupIdByIndex(selectedIndex - 1)
                            presetNamesListView.currentItem.myData.editgroup = groupID
                        }
                    }

                    onLastItemActionInvoked: {
                        var callbackObject = {
                            onSuccess: function(groupName) {
                                var groupID = presetsGroups.addGroup(groupName)
                                if (presetNamesListView.currentItem) {
                                    presetNamesListView.currentItem.myData.editgroup = groupID
                                }
                                groupsCombobox.closePopup()
                                groupsCombobox.selectedIndex = (presetsGroups.getGroupsCount() - 1) + 1
                            },
                            onClose: function() {
                                groupsCombobox.closePopup()
                            }
                        }

                        Common.launchDialog("Dialogs/AddPresetGroupDialog.qml",
                                            componentParent,
                                            {
                                                callbackObject: callbackObject
                                            })
                    }

                    function updateSelectedGroup() {
                        if (presetNamesListView.currentItem) {
                            var groupID = presetNamesListView.currentItem.myData.group
                            var groupIndex = presetsGroups.findGroupIndexById(groupID);
                            if (groupIndex !== -1) {
                                // take into account empty group
                                groupsCombobox.selectedIndex = (groupIndex + 1)
                            } else {
                                groupsCombobox.selectedIndex = 0
                            }
                        } else {
                            groupsCombobox.selectedIndex = 0
                        }
                    }

                    Component.onCompleted: {
                        groupsCombobox.updateSelectedGroup()
                    }
                }

                Item {
                    height: 12
                }

                RowLayout {
                    spacing: 5

                    StyledText {
                        text: i18.n + qsTr("Keywords:")
                        isActive: presetNamesListView.count > 0
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledText {
                        text: presetNamesListView.currentItem ? presetNamesListView.currentItem.myData.keywordscount : ""
                    }
                }

                Rectangle {
                    id: keywordsWrapper
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    border.color: uiColors.artworkActiveColor
                    border.width: flv.isFocused ? 1 : 0
                    color: uiColors.inputBackgroundColor
                    property var keywordsModel: presetNamesListView.currentItem ? presetsModel.getKeywordsModelObject(presetNamesListView.currentIndex) : []
                    state: ""

                    function removeKeyword(index) {
                        presetsModel.removeKeywordAt(presetNamesListView.currentIndex, index)
                    }

                    function removeLastKeyword() {
                        presetsModel.removeLastKeyword(presetNamesListView.currentIndex)
                    }

                    function appendKeyword(keyword) {
                        var added = presetsModel.appendKeyword(presetNamesListView.currentIndex, keyword)
                        if (!added) {
                            keywordsWrapper.state = "blinked"
                            blinkTimer.start()
                        }
                    }

                    function pasteKeywords(keywordsList) {
                        presetsModel.pasteKeywords(presetNamesListView.currentIndex, keywordsList)
                    }

                    EditableTags {
                        id: flv
                        objectName: "editableTags"
                        anchors.fill: parent
                        enabled: presetNamesListView.currentIndex >= 0
                        model: keywordsWrapper.keywordsModel
                        property int keywordHeight: uiManager.keywordHeight
                        scrollStep: keywordHeight

                        function acceptCompletion(completionID) {
                            // do not handle preset insertion here
                            var completion = acSource.getCompletion(completionID)
                            flv.editControl.acceptCompletion(completion)
                        }

                        delegate: KeywordWrapper {
                            id: kw
                            isHighlighted: true
                            keywordText: keyword
                            hasSpellCheckError: !iscorrect
                            hasDuplicate: hasduplicate
                            delegateIndex: index
                            itemHeight: flv.keywordHeight
                            onRemoveClicked: presetsModel.removeKeywordAt(presetNamesListView.currentIndex, index)
                            onActionDoubleClicked: {
                                var callbackObject = {
                                    onSuccess: function(replacement) {
                                        presetsModel.editKeyword(presetNamesListView.currentIndex, kw.delegateIndex, replacement)
                                    },
                                    onClose: function() {
                                        flv.activateEdit()
                                    }
                                }

                                Common.launchDialog("Dialogs/EditKeywordDialog.qml",
                                                    componentParent,
                                                    {
                                                        callbackObject: callbackObject,
                                                        previousKeyword: keyword,
                                                        keywordIndex: kw.delegateIndex,
                                                        keywordsModel: keywordsWrapper.keywordsModel
                                                    })
                            }
                        }

                        onTagAdded: {
                            keywordsWrapper.appendKeyword(text)
                        }

                        onRemoveLast: {
                            keywordsWrapper.removeLastKeyword()
                        }

                        onTagsPasted: {
                            keywordsWrapper.pasteKeywords(tagsList)
                        }

                        onBackTabPressed: {
                            titleText.forceActiveFocus()
                            titleText.cursorPosition = titleText.text.length
                        }

                        onCompletionRequested: {
                            dispatcher.dispatch(UICommand.GenerateCompletions, prefix)
                        }
                    }

                    CustomScrollbar {
                        anchors.topMargin: -5
                        anchors.bottomMargin: -5
                        anchors.rightMargin: -15
                        flickable: flv
                    }

                    Timer {
                        id: blinkTimer
                        repeat: false
                        interval: 400
                        triggeredOnStart: false
                        onTriggered: keywordsWrapper.state = ""
                    }

                    states: State {
                        name: "blinked";
                        PropertyChanges {
                            target: keywordsWrapper;
                            border.width: 0
                        }
                    }
                }
            }
        }

        Item {
            id: footer
            anchors.left: leftPanel.right
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 50
            //color: uiColors.defaultDarkColor

            RowLayout {
                id: footerRow
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                spacing: 20

                StyledLink {
                    objectName: "plainTextLink"
                    text: i18.n + qsTr("<u>edit in plain text</u>")
                    normalLinkColor: uiColors.labelActiveForeground
                    enabled: presetNamesListView.currentItem ? true : false
                    visible: presetNamesListView.count > 0
                    Layout.alignment: Qt.AlignVCenter
                    onClicked: {
                        if (!presetNamesListView.currentItem) { return; }

                        var callbackObject = {
                            onSuccess: function(text, spaceIsSeparator) {
                                presetsModel.plainTextEdit(presetNamesListView.currentIndex, text, spaceIsSeparator)
                            },
                            onClose: function() {
                                flv.activateEdit()
                            }
                        }

                        Common.launchDialog("Dialogs/PlainTextKeywordsDialog.qml",
                                            componentParent,
                                            {
                                                callbackObject: callbackObject,
                                                keywordsText: presetNamesListView.currentItem.myData.keywordsstring,
                                                keywordsModel: keywordsWrapper.keywordsModel
                                            });
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                StyledButton {
                    text: i18.n + qsTr("Close")
                    Layout.alignment: Qt.AlignVCenter
                    width: 100
                    onClicked: {
                        closePopup();
                    }
                }
            }
        }
    }
}

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
    id: uploadArtworksComponent
    canEscapeClose: false
    anchors.fill: parent

    // if MasterPassword wasn't entered do not show passwords
    property bool emptyPasswords: false
    property bool skipUploadItems: false
    property variant componentParent
    property bool uploadEnabled: (artworksRepository.artworksSourcesCount > 0) && (filteredArtworksListModel.selectedArtworksCount > 0)
    property var artworksUploader: dispatcher.getCommandTarget(UICommand.SetupUpload)
    property var uploadWatcher: artworksUploader.getUploadWatcherObject()
    property var uploadInfos: artworksUploader.getUploadInfosObject();
    property var ftpListAC: uploadInfos.getStocksCompletionObject()

    Component.onCompleted: {
        ftpListAC.searchTerm = ''
    }

    Connections {
        target: xpiksApp
        onGlobalBeforeDestruction: {
            console.debug("UI:UploadArtworks # globalBeforeDestruction")
            closePopup()
        }
    }

    Connections {
        target: dispatcher.getCommandTarget(UICommand.SetupCreatingArchives)
        onFinishedProcessing: doStartUpload()
    }

    UICommandListener {
        commandDispatcher: dispatcher
        commandIDs: [UICommand.SetupCreatingArchives]
        onDispatched: {
            Common.launchDialog("Dialogs/ZipArtworksDialog.qml",
                                uploadArtworksComponent.componentParent,
                                {
                                    immediateProcessing: true
                                });
        }
    }

    function closePopup() {
        secretsManager.purgeMasterPassword()
        warningsModel.resetShowSelected()
        uploadInfos.finalizeAccounts()
        uploadArtworksComponent.destroy()
        uploadWatcher.resetModel()
    }

    function doStartUpload() {
        artworksUploader.resetProgress()
        uploadWatcher.resetModel()
        artworksUploader.uploadArtworks()
    }

    function startUpload() {
        if (artworksUploader.needCreateArchives()) {
            dispatcher.dispatch(UICommand.SetupCreatingArchives, {})
        } else {
            doStartUpload();
        }
    }

    MessageDialog {
        id: confirmRemoveItemDialog
        property int itemIndex
        title: i18.n + qsTr("Confirmation")
        text: i18.n + qsTr("Are you sure you want to remove this item?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            uploadInfos.removeItem(itemIndex)
        }
    }

    MessageDialog {
        id: noPasswordDialog
        property string agenciesList
        title: i18.n + qsTr("Warning")
        text: i18.n + qsTr("Some agencies (%1) miss FTP credentials. Start upload anyway?").arg(agenciesList)
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            startUpload()
        }
    }

    MessageDialog {
        id: abortConfirmationDialog
        title: i18.n + qsTr("Confirmation")
        text: i18.n + qsTr("There is upload in progress. Do you want to abort it?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            console.log("UI:UploadArtworks # About to cancel upload...")
            uploadButton.enabled = false
            artworksUploader.cancelOperation()
        }
    }

    MessageDialog {
        id: selectHostsMessageBox
        title: i18.n + qsTr("Warning")
        text: i18.n + qsTr("Please, select some hosts first")
    }

    ClipboardHelper {
        id: clipboard
    }

    contentsWidth: 700
    contentsHeight: 500

    contents: Item {
        anchors.fill: parent

        Rectangle {
            id: leftPanel
            color: uiColors.defaultControlColor
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: progress.top
            width: 250
            enabled: !artworksUploader.inProgress

            ListView {
                id: uploadHostsListView
                objectName: "uploadHostsListView"
                model: uploadInfos
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: leftFooter.top
                anchors.topMargin: 50
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

                onCurrentIndexChanged: { uploadInfos.setCurrentIndex(uploadHostsListView.currentIndex) }

                delegate: Rectangle {
                    id: sourceWrapper
                    objectName: "sourceWrapper"
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
                            if (uploadHostsListView.currentIndex != sourceWrapper.delegateIndex) {
                                // "if" is needed for tabControl
                                if (typeof credentialsStatus !== "undefined") {
                                    credentialsStatus.enabled = false
                                    credentialsStatus.isGreen = false
                                    credentialsStatus.stopAnimation()
                                }

                                uploadHostsListView.currentIndex = sourceWrapper.delegateIndex
                                uploadInfos.updateProperties(sourceWrapper.delegateIndex)
                            }
                        }

                        onDoubleClicked: {
                            myData.editisselected = !myData.isselected
                            itemCheckedCheckbox.checked = isselected
                        }
                    }

                    RowLayout {
                        spacing: 10
                        anchors.fill: parent

                        Item {
                            width: 10
                        }

                        StyledCheckbox {
                            id: itemCheckedCheckbox
                            objectName: "itemCheckedCheckbox"
                            isContrast: !sourceWrapper.isCurrent
                            onClicked: editisselected = checked
                            Component.onCompleted: itemCheckedCheckbox.checked = isselected
                        }

                        StyledText {
                            id: infoTitle
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignVCenter
                            height: 31
                            text: title
                            elide: Text.ElideMiddle
                            font.bold: sourceWrapper.isCurrent
                        }

                        StyledText {
                            id: percentText
                            text: percent + '%'
                            visible: artworksUploader.inProgress && isselected
                            color: uiColors.artworkActiveColor
                            font.bold: true
                        }

                        CloseIcon {
                            width: 14
                            height: 14
                            // anchors.verticalCenterOffset: 1
                            isActive: false
                            disabledColor: uiColors.labelInactiveForeground

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
                        generalTab.focusTitleField()
                    }
                }
            }

            CustomScrollbar {
                id: uploadHostsScroll
                anchors.topMargin: 0
                anchors.bottomMargin: 0
                anchors.rightMargin: 0
                flickable: uploadHostsListView
                canShow: !rightPanelMA.containsMouse
            }

            Item {
                id: leftFooter
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 50

                StyledBlackButton {
                    id: addExportPlanButton
                    objectName: "addExportPlanButton"
                    text: i18.n + qsTr("Add new", "upload host")
                    width: 210
                    height: 30
                    anchors.centerIn: parent
                    onClicked: {
                        uploadInfos.addItem()
                        uploadHostsListView.currentIndex = uploadHostsListView.count - 1
                        generalTab.focusTitleField()
                    }
                }
            }
        }

        Item {
            id: rightPanel
            anchors.left: leftPanel.right
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: progress.top
            enabled: !artworksUploader.inProgress && (uploadHostsListView.count > 0)

            MouseArea {
                id: rightPanelMA
                anchors.fill: parent
                hoverEnabled: true
            }

            Row {
                id: tabsHeader
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: 50
                spacing: 0

                Repeater {
                    objectName: "tabsRepeater"
                    model: [i18.n + qsTr("General"), i18.n + qsTr("Advanced")]
                    delegate: CustomTab {
                        width: tabsHeader.width/2
                        objectName: 'tabDelegate'
                        property int delegateIndex: index
                        tabIndex: delegateIndex
                        isSelected: tabIndex === optionsTabView.currentIndex
                        color: isSelected ? uiColors.selectedArtworkBackground : uiColors.inputInactiveBackground
                        hovered: tabMA.containsMouse

                        StyledText {
                            color: parent.isSelected ? uiColors.artworkActiveColor : (parent.hovered ? uiColors.labelActiveForeground : uiColors.labelInactiveForeground)
                            text: modelData
                            anchors.centerIn: parent
                        }

                        MouseArea {
                            id: tabMA
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: optionsTabView.setCurrentIndex(parent.tabIndex)
                        }
                    }
                }
            }

            Item {
                id: optionsTabView
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: tabsHeader.bottom
                anchors.bottom: parent.bottom
                property int currentIndex: 0

                function setCurrentIndex(index) {
                    if (index === 0) {
                        advancedTab.visible = false
                        generalTab.visible = true
                    } else if (index === 1) {
                        advancedTab.visible = true
                        generalTab.visible = false
                    }

                    optionsTabView.currentIndex = index
                }

                Rectangle {
                    id: generalTab
                    objectName: "generalTab"
                    color: uiColors.selectedArtworkBackground
                    anchors.fill: parent
                    property var autoCompleteBox

                    function onAutoCompleteClose() {
                        autoCompleteBox = undefined
                    }

                    function focusTitleField() {
                        titleText.forceActiveFocus()
                        titleText.cursorPosition = titleText.text.length
                    }

                    function showStockCompletion(textField) {
                        ftpListAC.selectedIndex = -1

                        if (ftpListAC.getCount() === 0) {
                            ftpListAC.cancelCompletion()
                            return;
                        }

                        if (typeof generalTab.autoCompleteBox !== "undefined") {
                            // update completion
                            return
                        }

                        var directParent = generalTab;
                        var tmp = textField.parent.mapToItem(directParent, 0, textField.parent.height + 1)
                        var isBelow = true

                        var options = {
                            model: ftpListAC.getCompletionsModelObject(),
                            autoCompleteSource: ftpListAC,
                            isBelowEdit: isBelow,
                            "anchors.left": directParent.left,
                            "anchors.leftMargin": tmp.x,
                            "anchors.top": directParent.top,
                            "anchors.topMargin": tmp.y
                        }

                        var component = Qt.createComponent("../Components/CompletionBox.qml");
                        if (component.status !== Component.Ready) {
                            console.warn("Component Error: " + component.errorString());
                        } else {
                            var instance = component.createObject(directParent, options);

                            instance.boxDestruction.connect(generalTab.onAutoCompleteClose)
                            generalTab.autoCompleteBox = instance

                            ftpListAC.isActive = true
                            instance.openPopup()
                        }
                    }

                    ColumnLayout {
                        id: infoColumn
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 4
                        property int mySpacing: 16

                        StyledText {
                            text: i18.n + qsTr("Title:")
                            isActive: false
                        }

                        Rectangle {
                            id: titleWrapper
                            border.width: titleText.activeFocus ? 1 : 0
                            border.color: uiColors.artworkActiveColor
                            Layout.fillWidth: true
                            color: enabled ? uiColors.inputBackgroundColor : uiColors.inputInactiveBackground
                            height: 30

                            StyledTextInput {
                                id: titleText
                                objectName: "titleTextInput"
                                height: parent.height
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.rightMargin: 5
                                text: uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.title : ""
                                anchors.leftMargin: 5

                                property bool autoCompleteActive: ftpListAC.isActive

                                onTextChanged: {
                                    if (uploadHostsListView.currentItem) {
                                        uploadHostsListView.currentItem.myData.edittitle = text
                                    }
                                }

                                KeyNavigation.tab: ftpHost

                                onEditingFinished: {
                                    if (text.length == 0) {
                                        uploadHostsListView.currentItem.myData.edittitle = qsTr("Untitled")
                                    }
                                }

                                onActiveFocusChanged: {
                                    if (!activeFocus) {
                                        ftpListAC.cancelCompletion()
                                    }
                                }

                                validator: RegExpValidator {
                                    // copy paste in keys.onpressed Paste
                                    regExp: /[a-zA-Z0-9 _-]*$/
                                }

                                Keys.onPressed: {
                                    if (event.matches(StandardKey.Paste)) {
                                        var clipboardText = clipboard.getText();
                                        // same regexp as in validator
                                        var sanitizedText = clipboardText.replace(/[^a-zA-Z0-9 _-]/g, '');
                                        titleText.paste(sanitizedText)
                                        event.accepted = true
                                    } else if (autoCompleteActive && (event.key === Qt.Key_Return)) {
                                        ftpListAC.acceptSelected()
                                        if (ftpListAC.hasSelectedCompletion()) {
                                            event.accepted = true
                                        }
                                    } else if (autoCompleteActive && (event.key === Qt.Key_Up)) {
                                        ftpListAC.moveSelectionUp()
                                        event.accepted = true
                                    } else if (autoCompleteActive && (event.key === Qt.Key_Down)) {
                                        ftpListAC.moveSelectionDown()
                                        event.accepted = true;
                                    } else if (autoCompleteActive && (event.key === Qt.Key_Escape)) {
                                        ftpListAC.cancelCompletion()
                                        event.accepted = true;
                                    }
                                }

                                Keys.onReleased: {
                                    if (event.key === Qt.Key_Escape) {
                                        return
                                    }

                                    if ((Qt.Key_A <= event.key && event.key <= Qt.Key_Z) ||
                                            (Qt.Key_0 <= event.key && event.key <= Qt.Key_9) ||
                                            (text.length === 0 && !ftpListAC.isActive)) {
                                        ftpListAC.searchTerm = text
                                        generalTab.showStockCompletion(titleText)
                                    } else if (event.key === Qt.Key_Backspace) {
                                        ftpListAC.searchTerm = text
                                        generalTab.showStockCompletion(titleText)
                                    }
                                }
                            }
                        }

                        Item {
                            height: infoColumn.mySpacing
                        }

                        StyledText {
                            text: i18.n + qsTr("Host:")
                            isActive: false
                        }

                        Rectangle {
                            border.width: ftpHost.activeFocus ? 1 : 0
                            border.color: uiColors.artworkActiveColor
                            Layout.fillWidth: true
                            color: enabled ? uiColors.inputBackgroundColor : uiColors.inputInactiveBackground
                            height: 30

                            StyledTextInput {
                                id: ftpHost
                                objectName: "ftpAddressInput"
                                height: parent.height
                                anchors.left: parent.left
                                anchors.leftMargin: 5
                                anchors.right: parent.right
                                anchors.rightMargin: 5
                                text: uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.host : ""
                                onTextChanged: {
                                    if (uploadHostsListView.currentItem) {
                                        uploadHostsListView.currentItem.myData.edithost = text
                                    }
                                    credentialsStatus.enabled = false
                                    credentialsStatus.stopAnimation()
                                }
                                KeyNavigation.tab: ftpUsername
                                KeyNavigation.backtab: titleText
                            }
                        }

                        Item {
                            height: infoColumn.mySpacing
                        }

                        StyledText {
                            text: i18.n + qsTr("Username:")
                            isActive: false
                        }

                        Rectangle {
                            border.width: ftpUsername.activeFocus ? 1 : 0
                            border.color: uiColors.artworkActiveColor
                            Layout.fillWidth: true
                            color: enabled ? uiColors.inputBackgroundColor : uiColors.inputInactiveBackground
                            height: 30

                            StyledTextInput {
                                id: ftpUsername
                                height: parent.height
                                anchors.left: parent.left
                                anchors.leftMargin: 5
                                anchors.right: parent.right
                                anchors.rightMargin: 5
                                text: uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.username : ""
                                onTextChanged: {
                                    if (uploadHostsListView.currentItem) {
                                        uploadHostsListView.currentItem.myData.editusername = text
                                    }
                                }
                                KeyNavigation.tab: ftpPassword
                                KeyNavigation.backtab: ftpHost
                            }
                        }

                        Item {
                            height: infoColumn.mySpacing
                        }

                        StyledText {
                            text: i18.n + qsTr("Password:")
                            isActive: false
                        }

                        Rectangle {
                            border.width: ftpPassword.activeFocus ? 1 : 0
                            border.color: uiColors.artworkActiveColor
                            Layout.fillWidth: true
                            height: 30
                            color: enabled ? uiColors.inputBackgroundColor : uiColors.inputInactiveBackground

                            StyledTextInput {
                                id: ftpPassword
                                height: parent.height
                                anchors.left: parent.left
                                anchors.leftMargin: 5
                                anchors.right: parent.right
                                anchors.rightMargin: 5
                                horizontalAlignment: TextInput.AlignLeft
                                echoMode: showPasswordCheckBox.checked ? TextInput.Normal : TextInput.Password
                                text: uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.password : ""
                                onTextChanged: {
                                    if (uploadHostsListView.currentItem) {
                                        uploadHostsListView.currentItem.myData.editpassword = text
                                    }
                                    credentialsStatus.enabled = false
                                    credentialsStatus.stopAnimation()
                                }
                                KeyNavigation.backtab: ftpUsername
                            }
                        }

                        Item {
                            height: infoColumn.mySpacing / 2
                        }

                        RowLayout {
                            height: 25
                            Layout.fillWidth: true
                            spacing: 0

                            StyledCheckbox {
                                id: showPasswordCheckBox
                                text: i18.n + qsTr("Show password")
                                checked: false

                                onCheckedChanged: {
                                    if (checked) {
                                        passwordTimer.start()
                                    }
                                }
                            }

                            Timer {
                                id: passwordTimer
                                repeat: false
                                triggeredOnStart: false
                                interval: 5000
                                onTriggered: {
                                    showPasswordCheckBox.checked = false
                                }
                            }

                            Item {
                                Layout.fillWidth: true
                            }

                            CheckedComponent {
                                id: credentialsStatus
                                width: 18
                                height: 18
                                enabled: false
                                isGreen: false
                            }

                            Item {
                                width: 5
                            }

                            StyledButton {
                                id: testButton
                                text: i18.n + qsTr("Test connection")
                                height: 24
                                width: 130
                                onClicked: {
                                    testButton.enabled = false
                                    credentialsStatus.enabled = false
                                    credentialsStatus.startAnimation()
                                    var disablePassiveMode = uploadHostsListView.currentItem.myData.disablepassivemode
                                    var disableEPSV = uploadHostsListView.currentItem.myData.disableEPSV
                                    artworksUploader.checkCredentials(ftpHost.text, ftpUsername.text, ftpPassword.text, disablePassiveMode, disableEPSV)
                                }

                                Connections {
                                    target: artworksUploader
                                    onCredentialsChecked: {
                                        var currHost = ftpHost.text
                                        if (url.indexOf(currHost) > -1) {
                                            credentialsStatus.enabled = true
                                            credentialsStatus.isGreen = result
                                            credentialsStatus.stopAnimation()
                                        }

                                        testButton.enabled = true
                                    }
                                }
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }

                Rectangle {
                    id: advancedTab
                    color: uiColors.selectedArtworkBackground
                    anchors.fill: parent
                    visible: false

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 25

                        StyledCheckbox {
                            id: zipBeforeUploadCheckBox
                            text: i18.n + qsTr("Zip vector with preview")
                            objectName: 'zipBeforeUploadCheckBox'
                            Component.onCompleted: checked = uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.zipbeforeupload : false

                            onClicked: {
                                if (uploadHostsListView.currentItem) {
                                    uploadHostsListView.currentItem.myData.editzipbeforeupload = checked
                                }
                            }

                            Connections {
                                target: uploadInfos
                                onDataChanged: {
                                    zipBeforeUploadCheckBox.checked = uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.zipbeforeupload : false
                                }
                            }
                        }

                        StyledCheckbox {
                            id: ftpPassiveModeCheckBox
                            text: i18.n + qsTr("Disable FTP passive mode")
                            Component.onCompleted: checked = uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.disablepassivemode : false

                            onClicked: {
                                if (uploadHostsListView.currentItem) {
                                    uploadHostsListView.currentItem.myData.editdisablepassivemode = checked
                                }
                            }

                            Connections {
                                target: uploadInfos
                                onDataChanged: {
                                    ftpPassiveModeCheckBox.checked = uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.disablepassivemode : false
                                }
                            }
                        }

                        StyledCheckbox {
                            id: epsvCheckBox
                            text: i18.n + qsTr("Disable extended passive mode")
                            Component.onCompleted: checked = uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.disableEPSV : false

                            onClicked: {
                                if (uploadHostsListView.currentItem) {
                                    uploadHostsListView.currentItem.myData.editdisableEPSV = checked
                                }
                            }

                            Connections {
                                target: uploadInfos
                                onDataChanged: {
                                    epsvCheckBox.checked = uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.disableEPSV : false
                                }
                            }
                        }

                        StyledCheckbox {
                            id: vectorsFirstCheckBox
                            text: i18.n + qsTr("Upload vectors before previews")
                            Component.onCompleted: checked = uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.vectorfirst : false

                            onClicked: {
                                if (uploadHostsListView.currentItem) {
                                    uploadHostsListView.currentItem.myData.editvectorfirst = checked
                                }
                            }

                            Connections {
                                target: uploadInfos
                                onDataChanged: {
                                    vectorsFirstCheckBox.checked = uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.vectorfirst : false
                                }
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }
            }

            Rectangle {
                id: overlayRectangle
                anchors.fill: parent
                color: uiColors.selectedArtworkBackground
                opacity: 0.6
                visible: (uploadInfos.infosCount === 0) || artworksUploader.inProgress
            }
        }

        SimpleProgressBar {
            id: progress
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: footer.top
            height: 5
            isRounded: false
            color: artworksUploader.isError ? uiColors.destructiveColor : uiColors.artworkActiveColor
            value: artworksUploader.percent
        }

        Rectangle {
            id: footer
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 50
            color: uiColors.defaultDarkColor

            RowLayout {
                id: footerRow
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                spacing: 20

                StyledLink {
                    color: isPressed ? uiColors.linkClickedColor : (warningsModel.warningsCount > 0 ? uiColors.artworkModifiedColor : uiColors.labelInactiveForeground)
                    text: i18.n + getOriginalText()
                    Layout.alignment: Qt.AlignVCenter
                    enabled: uploadArtworksComponent.uploadEnabled && !skipUploadItems
                    visible: !skipUploadItems && (!artworksUploader.inProgress) && (uploadWatcher.failedImagesCount === 0)

                    function getOriginalText() {
                        return warningsModel.warningsCount == 1 ? qsTr("1 warning") : qsTr("%1 warnings").arg(warningsModel.warningsCount)
                    }

                    onClicked: {
                        if (warningsModel.warningsCount > 0) {
                            Common.launchDialog("Dialogs/WarningsDialog.qml",
                                                uploadArtworksComponent.componentParent,
                                                {
                                                    componentParent: uploadArtworksComponent.componentParent,
                                                    isRestricted: true
                                                });
                        }
                    }
                }

                StyledLink {
                    id: failedArtworksStatus
                    objectName: "failedArtworksStatus"
                    visible: !skipUploadItems && (uploadWatcher.failedImagesCount > 0)
                    enabled: uploadArtworksComponent.uploadEnabled && !skipUploadItems && (uploadWatcher.failedImagesCount > 0)
                    text: i18.n + getOriginalText()
                    color: isPressed ? uiColors.linkClickedColor : uiColors.artworkModifiedColor
                    Layout.alignment: Qt.AlignVCenter

                    function getOriginalText() {
                        return uploadWatcher.failedImagesCount === 1 ?
                                    qsTr("1 failed upload") :
                                    qsTr("%1 failed uploads").arg(uploadWatcher.failedImagesCount)
                    }

                    onClicked: {
                        Common.launchDialog("Dialogs/FailedUploadArtworks.qml",
                                            uploadArtworksComponent.componentParent,
                                            {})
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                StyledButton {
                    id: uploadButton
                    objectName: "uploadButton"
                    enabled: uploadArtworksComponent.uploadEnabled && !skipUploadItems
                    text: i18.n + (artworksUploader.inProgress ? qsTr("Stop") : qsTr("Start Upload"))
                    width: 130
                    Layout.alignment: Qt.AlignVCenter
                    onClicked: {
                        if (!artworksUploader.inProgress) {
                            if (uploadInfos.getSelectedInfosCount() === 0) {
                                selectHostsMessageBox.open()
                            } else {
                                var agencies = uploadInfos.getAgenciesWithMissingDetails();
                                if (agencies.length !== 0) {
                                    noPasswordDialog.agenciesList = agencies
                                    noPasswordDialog.open()
                                } else {
                                    startUpload()
                                }
                            }
                        } else {
                            abortConfirmationDialog.open()
                        }
                    }

                    Connections {
                        target: artworksUploader
                        onStartedProcessing: {
                            helpersWrapper.turnTaskbarProgressOn()
                        }
                        onPercentChanged: {
                            helpersWrapper.setTaskbarProgress(artworksUploader.percent)
                        }
                        onFinishedProcessing: {
                            uploadButton.enabled = true
                            helpersWrapper.turnTaskbarProgressOff()
                        }
                        onRequestCloseWindow: {
                            closePopup();
                        }
                    }
                }

                StyledButton {
                    text: i18.n + qsTr("Close")
                    width: 120
                    enabled: !artworksUploader.inProgress
                    onClicked: {
                        closePopup()
                    }
                }
            }
        }
    }
}

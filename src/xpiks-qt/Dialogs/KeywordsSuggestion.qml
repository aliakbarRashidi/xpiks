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
import "../Constants"
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"
import "../Constants/UIConfig.js" as UIConfig
import xpiks 1.0

StaticDialogBase {
    id: keywordsSuggestionComponent
    canEscapeClose: false
    objectName: "keywordsSuggestionComponent"
    anchors.fill: parent
    property var keywordsSuggestor: dispatcher.getCommandTarget(UICommand.InitSuggestionArtwork)
    property bool initialized: false

    Keys.onEscapePressed: {
        if (!keywordsSuggestor.isInProgress) {
            closePopup()
        }

        event.accepted = true
    }

    function closePopup() {
        keywordsSuggestor.cancelSearch()
        keywordsSuggestor.close();
        keywordsSuggestionComponent.destroy();
    }

    Menu {
        id: contextMenu
        property string externalUrl
        property int delegateIndex

        MenuItem {
            text: i18.n + qsTr("Open in browser")
            onTriggered: {
                if (contextMenu.externalUrl != "") {
                    Qt.openUrlExternally(contextMenu.externalUrl)
                } else {
                    console.log("Got no URL")
                }
            }
        }

        MenuItem {
            text: i18.n + qsTr("Copy to Quick Buffer");
            onTriggered: {
                keywordsSuggestor.copyToQuickBuffer(contextMenu.delegateIndex)
            }
        }
    }

    Menu {
        id: suggestedKeywordsMenu

        MenuItem {
            text: i18.n + qsTr("Clear")
            onTriggered: {
                keywordsSuggestor.clearSuggested()
            }
        }

        MenuItem {
            text: i18.n + qsTr("Copy")
            onTriggered: {
                clipboard.setText(keywordsSuggestor.getSuggestedKeywordsString())
            }
        }
    }

    property bool isTooNarrow: keywordsSuggestionComponent.width <= 1150
    contentsWidth: Math.max(900, Math.min(1100, keywordsSuggestionComponent.width - 200))
    contentsHeight: Math.min(keywordsSuggestionComponent.height - 40, 700)

    contents: Item {
        anchors.fill: parent

        Item {
            id: previewsPanel
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.leftMargin: 20
            anchors.topMargin: 20
            anchors.bottomMargin: 20
            width: keywordsSuggestionComponent.isTooNarrow ? 460 : 615

            Item {
                id: searchRow
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.right: parent.right
                height: childrenRect.height

                Rectangle {
                    id: searchRect
                    anchors.left: parent.left
                    color: enabled ? uiColors.inputBackgroundColor : uiColors.inputInactiveBackground
                    border.width: queryText.activeFocus ? 1 : 0
                    border.color: uiColors.artworkActiveColor
                    width: keywordsSuggestionComponent.isTooNarrow ? 200 : 250
                    height: UIConfig.textInputHeight
                    clip: true

                    StyledTextInput {
                        id: queryText
                        objectName: "queryTextInput"
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 5
                        anchors.rightMargin: 5
                        anchors.verticalCenter: parent.verticalCenter
                        onAccepted: keywordsSuggestor.searchArtworks(queryText.text, searchTypeCombobox.selectedIndex)
                    }
                }

                ComboBoxPopup {
                    id: searchTypeCombobox
                    anchors.left: searchRect.right
                    model: [i18.n + qsTr("All Images"),
                        i18.n + qsTr("Photos"),
                        i18.n + qsTr("Vectors"),
                        i18.n + qsTr("Illustrations")]
                    width: 150
                    height: 24
                    itemHeight: 28
                    showColorSign: false
                    dropDownWidth: 150
                    glowEnabled: true
                    glowTopMargin: 2
                    globalParent: keywordsSuggestionComponent
                    onComboItemSelected: {
                        keywordsSuggestor.searchTypeIndex = searchTypeCombobox.selectedIndex
                    }

                    Component.onCompleted: searchTypeCombobox.selectedIndex = keywordsSuggestor.searchTypeIndex
                }

                StyledButton {
                    anchors.right: parent.right
                    objectName: "searchButton"
                    text: i18.n + qsTr("Search")
                    width: 100
                    activeFocusOnPress: true
                    enabled: !keywordsSuggestor.isInProgress
                    onClicked: keywordsSuggestor.searchArtworks(queryText.text, searchTypeCombobox.selectedIndex)
                }
            }

            Rectangle {
                id: resultsRect
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: searchRow.bottom
                anchors.topMargin: 10
                anchors.bottom: parent.bottom
                color: uiColors.defaultControlColor

                Flickable {
                    clip: true
                    id: suggestionsWrapper
                    anchors.fill: parent
                    contentHeight: flow.childrenRect.height + 20
                    contentWidth: parent.width
                    enabled: !keywordsSuggestor.isInProgress
                    flickableDirection: Flickable.VerticalFlick
                    boundsBehavior: Flickable.StopAtBounds

                    Flow {
                        id: flow
                        objectName: "suggestionsFlow"
                        spacing: 10
                        anchors.fill: parent
                        anchors.margins: 10
                        clip: true

                        Repeater {
                            id: suggestionsRepeater
                            objectName: "suggestionsRepeater"
                            model: keywordsSuggestor

                            delegate: Item {
                                id: imageWrapper
                                property int delegateIndex: index
                                property string realUrl: externalurl
                                height: 140
                                width: 140

                                Image {
                                    anchors.fill: parent
                                    anchors.margins: 1
                                    source: url
                                    sourceSize.width: 150
                                    sourceSize.height: 150
                                    fillMode: Image.PreserveAspectCrop
                                    asynchronous: true
                                }

                                Rectangle {
                                    anchors.fill: parent
                                    color: uiColors.defaultControlColor
                                    opacity: isselected ? (mouseArea.containsMouse ? 0.6 : 0.7) : (mouseArea.containsMouse ? 0.4 : 0)
                                }

                                LargeAddIcon {
                                    opacity: isselected ? (mouseArea.containsMouse ? 0.85 : 1) : (mouseArea.containsMouse ? 0.7 : 0)
                                    width: parent.width
                                    height: parent.height
                                    rotation: isselected ? 45 : 0
                                }

                                MouseArea {
                                    id: mouseArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                                    onClicked: {
                                        if (mouse.button == Qt.RightButton) {
                                            contextMenu.externalUrl = imageWrapper.realUrl
                                            contextMenu.delegateIndex = imageWrapper.delegateIndex
                                            contextMenu.popup()
                                        } else {
                                            console.log('KeywordsSuggestion # selectSuggestion ' + delegateIndex)
                                            keywordsSuggestor.setArtworkSelected(delegateIndex, !isselected)
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                CustomScrollbar {
                    anchors.topMargin: 0
                    anchors.bottomMargin: 0
                    anchors.rightMargin: -15
                    flickable: suggestionsWrapper
                }

                Rectangle {
                    anchors.fill: parent
                    color: uiColors.selectedArtworkBackground
                    opacity: 0.4
                    visible: keywordsSuggestor.isInProgress
                }

                Item {
                    anchors.fill: parent
                    visible: !keywordsSuggestor.isInProgress && (suggestionsRepeater.count == 0)

                    StyledText {
                        anchors.centerIn: parent
                        text: keywordsSuggestor.lastErrorString
                        color: uiColors.selectedArtworkBackground
                    }
                }

                LoaderIcon {
                    width: parent.width/2
                    height: parent.width/2
                    anchors.centerIn: parent
                    running: keywordsSuggestor.isInProgress
                }
            }
        }

        Item {
            id: keywordsPane
            anchors.top: parent.top
            anchors.left: previewsPanel.right
            anchors.right: parent.right
            anchors.bottom: actionsPane.top
            anchors.topMargin: 20
            anchors.rightMargin: 20
            anchors.bottomMargin: 20
            anchors.leftMargin: 30

            Item {
                id: headerRow
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: childrenRect.height

                ComboBoxPopup {
                    id: sourceComboBox
                    objectName: "engineComboBox"
                    model: keywordsSuggestor.engineNames
                    anchors.right: parent.right
                    width: 200
                    height: 24
                    itemHeight: 28
                    enabled: !keywordsSuggestor.isInProgress
                    dropDownWidth: 200
                    glowEnabled: true
                    glowTopMargin: 2
                    globalParent: keywordsSuggestionComponent

                    onComboItemSelected: {
                        keywordsSuggestor.selectedSourceIndex = sourceComboBox.selectedIndex
                    }

                    Component.onCompleted: selectedIndex = keywordsSuggestor.selectedSourceIndex
                }
            }

            Item {
                id: suggestedPart
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: headerRow.bottom
                height: (keywordsPane.height - headerRow.height)/2

                RowLayout {
                    id: captionSuggested
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.topMargin: 15
                    height: childrenRect.height

                    StyledText {
                        text: i18.n + qsTr("Suggested keywords:")
                        color: uiColors.artworkActiveColor
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledText {
                        text: keywordsSuggestor.suggestedKeywordsCount
                    }
                }

                Rectangle {
                    id: suggestedKeywordsWrapper
                    border.color: uiColors.artworkActiveColor
                    border.width: suggestedFlv.isFocused ? 1 : 0
                    anchors.top: captionSuggested.bottom
                    anchors.topMargin: 5
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    color: uiColors.inputBackgroundColor

                    function removeKeyword(index) {
                        var keyword = keywordsSuggestor.removeSuggestedKeywordAt(index)
                        keywordsSuggestor.appendKeywordToOther(keyword)
                    }

                    EditableTags {
                        id: suggestedFlv
                        objectName: "suggestedEditableTags"
                        anchors.fill: parent
                        model: keywordsSuggestor.getSuggestedKeywordsModel()
                        property int keywordHeight: uiManager.keywordHeight
                        scrollStep: keywordHeight
                        editEnabled: false

                        delegate: KeywordWrapper {
                            isHighlighted: true
                            delegateIndex: index
                            keywordText: keyword
                            itemHeight: suggestedFlv.keywordHeight
                            onRemoveClicked: suggestedKeywordsWrapper.removeKeyword(delegateIndex)
                        }

                        onTagAdded: {
                            //suggestedKeywordsWrapper.appendKeyword(text)
                        }

                        onRemoveLast: {
                            //suggestedKeywordsWrapper.removeLastKeyword()
                        }

                        onTagsPasted: {
                            //suggestedKeywordsWrapper.pasteKeywords(tagsList)
                        }
                    }

                    CustomScrollbar {
                        anchors.topMargin: -5
                        anchors.bottomMargin: -5
                        anchors.rightMargin: -15
                        flickable: suggestedFlv
                    }

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.RightButton
                        propagateComposedEvents: true
                        preventStealing: true
                        onClicked: {
                            if (mouse.button == Qt.RightButton) {
                                if (keywordsSuggestor.suggestedKeywordsCount > 0) {
                                    suggestedKeywordsMenu.popup()
                                }
                            }
                        }
                    }
                }
            }

            Item {
                id: otherPart
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: suggestedPart.bottom
                height: suggestedPart.height

                RowLayout {
                    id: captionOther
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.topMargin: 20
                    height: childrenRect.height

                    StyledText {
                        text: i18.n + qsTr("Other keywords:")
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledText {
                        text: keywordsSuggestor.otherKeywordsCount
                    }
                }

                Rectangle {
                    id: otherKeywordsWrapper
                    anchors.top: captionOther.bottom
                    anchors.topMargin: 5
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    color: uiColors.inputInactiveBackground

                    function removeKeyword(index) {
                        var keyword = keywordsSuggestor.removeOtherKeywordAt(index)
                        keywordsSuggestor.appendKeywordToSuggested(keyword)
                    }

                    EditableTags {
                        id: otherFlv
                        objectName: "otherEditableTags"
                        anchors.fill: parent
                        model: keywordsSuggestor.getAllOtherKeywordsModel()
                        property int keywordHeight: uiManager.keywordHeight
                        scrollStep: keywordHeight
                        editEnabled: false

                        delegate: KeywordWrapper {
                            delegateIndex: index
                            keywordText: keyword
                            isHighlighted: false
                            hasPlusSign: true
                            itemHeight: otherFlv.keywordHeight
                            onRemoveClicked: otherKeywordsWrapper.removeKeyword(delegateIndex)
                        }

                        onTagAdded: {
                            //eywordsWrapper.appendKeyword(text)
                        }

                        onRemoveLast: {
                            //keywordsWrapper.removeLastKeyword()
                        }

                        onTagsPasted: {
                            //keywordsWrapper.pasteKeywords(tagsList)
                        }
                    }

                    CustomScrollbar {
                        anchors.topMargin: -5
                        anchors.bottomMargin: -5
                        anchors.rightMargin: -15
                        flickable: otherFlv
                    }
                }
            }
        }

        Item {
            id: actionsPane
            anchors.left: previewsPanel.right
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.rightMargin: 20
            anchors.bottomMargin: 20
            anchors.leftMargin: 20
            height: childrenRect.height

            RowLayout {
                spacing: 0
                anchors.left: parent.left
                anchors.right: parent.right

                /*StyledText {
                        text: i18.n + (keywordsSuggestor.selectedArtworksCount !== 1 ?
                                           qsTr("%1 selected items").arg(keywordsSuggestor.selectedArtworksCount) :
                                           qsTr("1 selected item"))
                    }*/

                Item {
                    Layout.fillWidth: true
                }

                Timer {
                    id: suggestedAddedTimer
                    property int iterations: 0
                    interval: 1000
                    repeat: false
                    running: false
                    onTriggered: {
                        keywordsSuggestor.resetSelection()
                        addKeywordsButton.enabled = true
                        console.log("Add suggested reenabled")
                    }
                }

                StyledButton {
                    width: 100
                    objectName: "closeButton"
                    text: i18.n + qsTr("Close")
                    onClicked: closePopup()
                }

                Item {
                    width: 20
                }

                StyledButton {
                    id: addKeywordsButton
                    isDefault: true
                    objectName: "addSuggestedButton"
                    text: i18.n + qsTr("Add suggested")
                    enabled: !keywordsSuggestor.isInProgress && (keywordsSuggestor.suggestedKeywordsCount > 0)
                    width: 150
                    onClicked: {
                        console.log("Add suggested clicked")
                        addKeywordsButton.enabled = false
                        dispatcher.dispatch(UICommand.AppendSuggestedKeywords, keywordsSuggestor.getSuggestedKeywords())
                        suggestedAddedTimer.start()
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        focus = true
        // for some reason onCompleted was called twice
        if (!initialized) {
            queryText.forceActiveFocus()
            initialized = true
        }
    }

    ClipboardHelper {
        id: clipboard
    }
}

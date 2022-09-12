/*
  ==============================================================================

    PlaylistComponent.h
    Created: 20 Mar 2022 2:49:22am
    Author:  Aaron Lee

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>
#include <fstream>


//==============================================================================
/*
*/
class PlaylistComponent  : public juce::Component,
                           public TableListBoxModel,
                           public AudioSource,
                           public Button::Listener,
                           public FileDragAndDropTarget,
                           public TextEditor::Listener
{
public:
    PlaylistComponent(AudioFormatManager& formatManager);
    ~PlaylistComponent() override;

    //customisation for input graphics
    void paint (juce::Graphics&) override;

    //Scaling of components when adjusting windows
    void resized() override;

    //returning number of rows currently in the table; Override of TableListBoxModel pure virtual
    int getNumRows() override;

    //drawing the background of a row in the table; Override of TableListBoxModel pure virtual
    void paintRowBackground(Graphics &,
                            int rowNumber, 
                            int width, 
                            int height, 
                            bool rowIsSelected) override;
    
    //drawing a cell that is used to input details; Override of TableListBoxModel pure virtual
    void paintCell (Graphics &, 
                    int rowNumber, 
                    int columnId, 
                    int width, 
                    int height,
                    bool rowIsSelected) override;

    //To create and update custom components such as buttons to a cell; Override of TableListBoxModel pure virtual
    Component* refreshComponentForCell (int rowNumber,
                                   int columnId,
                                   bool isRowSelected,
                                   Component *existingComponentToUpdate) override;

    /**Placeholder function override for Audio Source component to tell the source to prepare for playing*/
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    /**Placeholder function override for Audio Source component to fetch subsequent blocks of audio data*/
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill);
    /**Placeholder function override for Audio Source component to release anything it no longer needs*/
    void releaseResources() override;


    //Called when the button is clicked, allowing users to load files to deck
    void  buttonClicked(Button* button) override;
    //Callback to check whether this target is interested in the set of files being offered. 
    //If set to true, allows files to be dragged and dropped into the area*/
    bool isInterestedInFileDrag(const StringArray& files) override;
    //Callback to indicate that the user has dropped the files onto this component. 
    //Processes file once it has been dropped (mouse released)
    void filesDropped(const StringArray& files, int x, int y) override;
    /**Override of TextEditor::Listener function to be called whenever the user changes 
    the text in the object in some way*/
    void textEditorTextChanged(TextEditor&) override;


    /**Vector of songs to be added to the Left Channel Player, utilised by DeckGUI*/
    std::vector<std::string> playListLeft;
    /**Vector of songs to be added to the Right Channel Player, utilised by DeckGUI*/
    std::vector<std::string> playListRight;
  


private:

    AudioFormatManager& formatManager;
    AudioTransportSource transportSource;
    std::unique_ptr<AudioFormatReaderSource> readerSource;

    //playlist displayed as a table list
    TableListBox tableComponent;

    //vectors to store music files metadata
    std::vector<std::string> inputFiles;
    std::vector<std::string> interestedFiles;
    std::vector<std::string> trackTitles; 
    std::vector<std::string> interestedTitles;
    std::vector<std::string> trackDurations;
    std::vector<std::string> interestedDuration;

    std::vector<URL> path;
    TextButton saveLibButton{ "SAVE LIBRARY" };

    // Search bar and label to allow for searching functionality 
    TextEditor searchBar;
    Label searchLabel;

    //user defined variables to process data
    void addToChannelList(std::string filepath, int channel);
    void deleteTrack(int id);
    void getAudioLength(URL audioURL);

    void writingIntoFile();
    void readingFile();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};

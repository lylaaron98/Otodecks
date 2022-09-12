

#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include "PlaylistComponent.h"

//==============================================================================
/*
*/
class DeckGUI    : public Component,
                   public Button::Listener, 
                   public Slider::Listener,
                   public TableListBoxModel, 
                   public Timer
{
public:
    DeckGUI(DJAudioPlayer* player,
           PlaylistComponent* playlistComponent, 
           AudioFormatManager & formatManagerToUse,
           AudioThumbnailCache & cacheToUse,
           int channeToUse );

    ~DeckGUI();

    //Customise graphics on DeckGUI
    void paint (Graphics&) override;
    //Scaling of components in the application when resizing windows
    void resized() override;

     /** implement Button::Listener */
     //called upon when button is clicked, interacting with the play and stop buttons, with the player
    void buttonClicked (Button *) override;

    /** implement Slider::Listener */
    //called upon when the slider's value is changed, interacting with the volume, speed and playback sliders, with the player
    void sliderValueChanged (Slider *slider) override;

    //returns the number of rows currently in the table
    int getNumRows() override;

    /**Override of TableListBoxModel pure virtual.For up next table in GUI
    Draws the background behind one of the rows in the table*/
    void paintRowBackground(Graphics&,
        int rowNumber,
        int width,
        int height,
        bool rowIsSelected) override;
    
    /**Override of TableListBoxModel pure virtual.For up next table in GUI
    Draws one of the cells, used to input details in cells*/
    void paintCell(Graphics&,
        int rowNumber,
        int columnId,
        int width,
        int height,
        bool rowIsSelected) override;

    //to allow callback for updating the waveform's visuals
    void timerCallback() override; 

private:

    //creating the buttons
    TextButton playButton{"PLAY"};
    TextButton stopButton{"STOP"};
    TextButton nextButton{"LOAD"};
    
    //creating the sliders
    Slider volSlider; 
    Slider speedSlider;
    Slider posSlider;

    //adding labels to the sliders
    Label volLabel;
    Label speedLabel;

    //controlling the visual theme
    LookAndFeel_V4 lookAndFeel;

    //creating the playlist component associated with the GUI
    PlaylistComponent* playlistComponent;

    //creating the waveform display (visual)
    WaveformDisplay waveformDisplay;

    //creating a table containing a list of upcoming songs in the playlist
    TableListBox upNext;

    //variable indicating the channel that's associated with the GUI (Left & Right)
    int channel;

    //creating the player associated with the GUI
    DJAudioPlayer* player; 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};

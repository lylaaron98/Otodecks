

#include <JuceHeader.h>
#include <cmath>
#include "DeckGUI.h"
#include "PlaylistComponent.h"

//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer* _player,
                PlaylistComponent* _playlistComponent,
                AudioFormatManager & 	formatManagerToUse,
                AudioThumbnailCache & 	cacheToUse,
                int channelToUse
                ) : player(_player),
                    playlistComponent(_playlistComponent),
                    waveformDisplay(formatManagerToUse, cacheToUse),
                    channel(channelToUse)
{
    
    //add buttons for each GUI
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(nextButton);
    
    //add button listeners for each GUI
    playButton.addListener(this);
    stopButton.addListener(this);
    nextButton.addListener(this);
    
    //add sliders to each GUI
    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);

    //add sliders listeners to each GUI
    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);

    //set range of each sider on GUI
    volSlider.setRange(0.0, 1.0);
    speedSlider.setRange(0.0, 100.0);
    posSlider.setRange(0.0, 1.0);

    //posSlider customisation
    posSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    posSlider.setRange(0.0, 1.0);
    posSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);

    //volSlider customisation
    volSlider.setRange(0.0, 1.0);
    volSlider.setValue(0.5); //default volume half of max vol
    volSlider.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    volSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(volLabel);
    volLabel.setText("Volume", juce::dontSendNotification);
    volLabel.attachToComponent(&volSlider, false);
    volLabel.setJustificationType(juce::Justification::centred);

    //speedSlider customisation
    speedSlider.setRange(0.5, 2, 0); //min half speed, max speed 2x
    speedSlider.setValue(1); //default speed at 1x
    speedSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalDrag);
    speedSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    speedSlider.setMouseDragSensitivity(80);
    addAndMakeVisible(speedLabel);
    speedLabel.setText("Speed X", juce::dontSendNotification);
    speedLabel.attachToComponent(&speedSlider, false);
    speedLabel.setJustificationType(juce::Justification::centred);

    //adding a waveform to each track on GUI
    addAndMakeVisible(waveformDisplay);

    //set colour scheme for sliders 
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::mediumspringgreen); //dial
    getLookAndFeel().setColour(juce::Slider::trackColourId, juce::Colours::lightslategrey); //body
    getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::lightslategrey); //body

    //add list of songs to be played next
    upNext.getHeader().addColumn("Up Next", 1, 100);
    upNext.setModel(this);
    addAndMakeVisible(upNext);

    //start thread calling 50 times per second
    startTimer(500);


}

DeckGUI::~DeckGUI()
{
    //stop timer
    stopTimer();
}

void DeckGUI::paint (Graphics& g)
{


    // g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    // g.setColour (Colours::black);
    // g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    // g.setColour (Colours::white);
    // g.setFont (14.0f);
    // g.drawText ("DeckGUI", getLocalBounds(),
    //             Justification::centred, true);   // draw some placeholder text
}

void DeckGUI::resized()
{
    double rowH = getHeight() / 6;
    double colW = getWidth() / 4;

    waveformDisplay.setBounds(0, 0, getWidth(), rowH * 2);

    posSlider.setBounds(0, rowH * 2, getWidth(), rowH);

    volSlider.setBounds(0, rowH * 3 +20, colW, rowH*3 -30);    
    speedSlider.setBounds(colW, rowH * 3 +20, colW*1.5, rowH*2 - 30);
    upNext.setBounds(colW * 2.5, rowH * 3, colW * 1.5 - 20, rowH * 2);

    playButton.setBounds(colW+10, rowH * 5 + 10, colW-20, rowH-20);
    stopButton.setBounds(colW*2+10, rowH * 5 + 10, colW-20, rowH-20);
    nextButton.setBounds(colW * 3 + 10, rowH * 5 + 10, colW - 20, rowH - 20);
}

void DeckGUI::buttonClicked(Button* button)
{
    if (button == &playButton)
    {
        //requires to load a music file first, by pressing load button
        player->start();
    }
     if (button == &stopButton)
    {
        player->stop();
    }
    if (button == &nextButton)
    {
       //handling next button for left channel
        if (channel == 0 && playlistComponent->playListLeft.size() > 0) //handle only if there are songs added 
        {
            //get URL to first song of Left playlist
            URL fileURL = URL{ File{playlistComponent->playListLeft[0]} }; 
            //load the first URL 
            player->loadURL(fileURL);
            //display the waveforms
            waveformDisplay.loadURL(fileURL);
            //pop the first URL of the Left playlist so it doesn't replay
            playlistComponent->playListLeft.erase(playlistComponent->playListLeft.begin()); 
        }

        //handling next button for right channel
        if (channel == 1 && playlistComponent->playListRight.size() > 0)
        {
            //get URL to first song of playlist
            URL fileURL = URL{ File{playlistComponent->playListRight[0]} }; 
            //load the first URL 
            player->loadURL(fileURL);
            //display the waveforms
            waveformDisplay.loadURL(fileURL);
            //pop the first URL of the Right playlist so it doesn't replay
            playlistComponent->playListRight.erase(playlistComponent->playListRight.begin()); //remove first element
        }

        //Buttons starts with indicating load. Once first songs have been loaded, we can change it to next 
        if (nextButton.getButtonText() == "LOAD")
        {
            nextButton.setButtonText("NEXT");
        }
        else
        {
            player->start(); //starts player each time button labeled next is clicks
        }
    }

    //refresh up next table whenever a button is clicked
    upNext.updateContent();
}

void DeckGUI::sliderValueChanged (Slider *slider)
{
    if (slider == &volSlider)
    {
        player->setGain(slider->getValue());
    }

    if (slider == &speedSlider)
    {
        player->setSpeed(slider->getValue());
    }
    
    if (slider == &posSlider)
    {
        player->setPositionRelative(slider->getValue());
    }
    
}

int DeckGUI::getNumRows()
{
    //number of rows in the table depends on the number of songs loaded to the respective channels
    if (channel == 0)// left channel
    {
        return playlistComponent->playListLeft.size();
    }
    if (channel == 1)// right channel
    {
        return playlistComponent->playListRight.size();
    }
}

void DeckGUI::paintRowBackground(Graphics & g,
                                int rowNumber,
                                int width,
                                int height,
                                bool rowIsSelected)
{
    g.fillAll(juce::Colours::lavenderblush);
}

void DeckGUI::paintCell(Graphics & g,
                        int rowNumber,
                        int columnId,
                        int width,
                        int height,
                        bool rowIsSelected)
{
    std::string filepath = "";

    //get file path with reference to the channel
    if (channel == 0) // left channel
    {
        filepath = playlistComponent->playListLeft[rowNumber];
    }

    if (channel == 1)// right channel
    {
        filepath = playlistComponent->playListRight[rowNumber];
    }

    // extracting the file name from the path
    std::size_t startFilePos = filepath.find_last_of("/");
    std::size_t startExtPos = filepath.find_last_of(".");
    std::string extn = filepath.substr(startExtPos + 1, filepath.length() - startExtPos);
    std::string file = filepath.substr(startFilePos + 1, filepath.length() - startFilePos - extn.size() - 2);

    //drawing the name of each cell
    g.drawText(file,
                1, rowNumber,
                width - 4, height,
                Justification::centredLeft,
                true);

}

    // bool DeckGUI::isInterestedInFileDrag (const StringArray &files)
    // {
    // std::cout << "DeckGUI::isInterestedInFileDrag" << std::endl;
    // return true; 
    // }

    // void DeckGUI::filesDropped (const StringArray &files, int x, int y)
    // {
    // std::cout << "DeckGUI::filesDropped" << std::endl;
    // if (files.size() == 1)
    // {
    //     player->loadURL(URL{File{files[0]}});
    // }
    // }

void DeckGUI::timerCallback()
{
    waveformDisplay.setPositionRelative(
            player->getPositionRelative());
}


    


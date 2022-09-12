

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (2, 2); });
    }  
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);
    }  

    // add application components and make them visible
    addAndMakeVisible(deckGUILeft); 
    addAndMakeVisible(deckGUIRight);  
    addAndMakeVisible(playlistComponent);
    

    // Add Labels and customize visuals for labels 
    addAndMakeVisible(waveformLabel);
    waveformLabel.setText("Waveforms", juce::dontSendNotification);
    waveformLabel.setColour(juce::Label::textColourId, juce::Colours::whitesmoke);
    waveformLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(posLabel);
    posLabel.setText("Playback", juce::dontSendNotification);
    posLabel.setColour(juce::Label::textColourId, juce::Colours::whitesmoke);
    posLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(widgetLabel);
    widgetLabel.setText("Widgets Controls", juce::dontSendNotification);
    widgetLabel.setColour(juce::Label::textColourId, juce::Colours::whitesmoke);
    widgetLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(playlistLabel);
    playlistLabel.setText("Drag Files here to add to Library", juce::dontSendNotification);
    playlistLabel.setColour(juce::Label::textColourId, juce::Colours::whitesmoke);
    playlistLabel.setJustificationType(juce::Justification::centred);
    
    // to register file formats enabled by JUCE
    formatManager.registerBasicFormats();
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    playlistComponent.prepareToPlay(samplesPerBlockExpected, sampleRate);

    playerLeft.prepareToPlay(samplesPerBlockExpected, sampleRate);
    playerRight.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    mixerSource.addInputSource(&playerLeft, false);
    mixerSource.addInputSource(&playerRight, false);

 }
void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    mixerSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.
    playlistComponent.releaseResources();

    playerLeft.releaseResources();
    playerRight.releaseResources();

}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{   
    double rowH = getHeight() / 10;
    double colW = getWidth() / 7;

    //position labels to left side of screen
    waveformLabel.setBounds(0, 0, colW, rowH*2);
    posLabel.setBounds(0, rowH*2, colW, rowH);
    widgetLabel.setBounds(0, rowH*3, colW, rowH*3);
    playlistLabel.setBounds(0, rowH*6, colW, rowH*3);

    //add GUIs
    deckGUILeft.setBounds(colW, 0, colW * 3, rowH*6);
    deckGUIRight.setBounds(colW * 4, 0, colW * 3, rowH * 6);

    //add playlistComponent
    playlistComponent.setBounds(colW, rowH *6, colW * 6, rowH * 4);
}


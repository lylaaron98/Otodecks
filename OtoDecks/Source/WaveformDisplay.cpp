

#include <JuceHeader.h>
#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(AudioFormatManager & 	formatManagerToUse,
                                 AudioThumbnailCache & 	cacheToUse) :
                                 audioThumb(1000, formatManagerToUse, cacheToUse), 
                                 fileLoaded(false), 
                                 position(0)
                          
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

  audioThumb.addChangeListener(this);
}

WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint (juce::Graphics& g)
{

    g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);   // draw an outline around the component

    //draw waveforms when file is loaded into deck
    if(fileLoaded)
    { 
      g.setColour(juce::Colours::blue);
      audioThumb.drawChannel(g, 
        getLocalBounds(), //initialising area
        0, //start time
        audioThumb.getTotalLength(), //retrieving the length of track as end time
        0, 
        1.0f
      );

      //setting colour to playerhead
      g.setColour(juce::Colours::mediumspringgreen);
      g.fillRect(position * getWidth(), 0, 2, getHeight());

      //display name of currently playing track on the waveform in white
      g.setColour(juce::Colours::floralwhite);
      g.setFont(16.0f);
      g.drawText(nowPlaying, getLocalBounds(),
        juce::Justification::centred, true);

    }
    else 
    {
      //when the track could not be loaded
      g.setColour(juce::Colours::mediumspringgreen);
      g.setFont (20.0f);
      g.drawText ("File not loaded...", getLocalBounds(),
                  Justification::centred, true);   // draw some placeholder text

    }
}

void WaveformDisplay::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void WaveformDisplay::loadURL(URL audioURL)
{
  audioThumb.clear();
  fileLoaded  = audioThumb.setSource(new URLInputSource(audioURL));
  if (fileLoaded)
  {
        std::string justFile = audioURL.toString(false).toStdString();
        std::size_t startFilePos = justFile.find_last_of("/");
        std::size_t startExtPos = justFile.find_last_of(".");
        std::string extn = justFile.substr(startExtPos + 1, justFile.length() - startExtPos);
        std::string file = justFile.substr(startFilePos + 1, justFile.length() - startFilePos - extn.size() - 2);

        nowPlaying = file;
        repaint();
  }
  else {
    std::cout << "wfd: not loaded! " << std::endl;
  }

}

void WaveformDisplay::changeListenerCallback (ChangeBroadcaster *source)
{
    std::cout << "wfd: change received! " << std::endl;

    repaint();

}

void WaveformDisplay::setPositionRelative(double pos)
{
  if (pos != position && !isnan(pos))

  {
    position = pos;
    repaint();
  }

  
}





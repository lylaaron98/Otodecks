/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 20 Mar 2022 2:49:22am
    Author:  Aaron Lee

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistComponent.h"


//==============================================================================
PlaylistComponent::PlaylistComponent(AudioFormatManager& _formatManager)
                  : formatManager(_formatManager)
{
    // In your constructor, you should add any child components, and

    //set up playlist library
    tableComponent.getHeader().addColumn("Track Title", 1, 250);
    tableComponent.getHeader().addColumn("Duration", 2, 100);
    tableComponent.getHeader().addColumn("Add file to Left Channel", 3, 250);
    tableComponent.getHeader().addColumn("Add file to Right channel", 4, 250);
    tableComponent.getHeader().addColumn("Delete file", 5, 100);
    tableComponent.setModel(this);
    addAndMakeVisible(tableComponent);
    
    //to save playlist of tracks
    addAndMakeVisible(saveLibButton);
    saveLibButton.addListener(this);

    readingFile();

    //add search bar and listener
    addAndMakeVisible(searchBar);
    searchBar.addListener(this);

    //add label for search bar
    addAndMakeVisible(searchLabel);
    searchLabel.setText("Search Track: ", juce::dontSendNotification);
    
}

PlaylistComponent::~PlaylistComponent()
{
}

void PlaylistComponent::paint (juce::Graphics& g)
{

    //to set the colour of the buttons
    saveLibButton.setColour(TextButton::buttonColourId, Colours::lightblue);
    saveLibButton.setColour(TextButton::textColourOffId, Colours::black);

}
//==============================================================================
void PlaylistComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    double rowH = getHeight() / 8;
    double colW = getWidth() / 6;

    //set position of search bar functionality
    searchLabel.setBounds(0, 0, colW, rowH);
    searchBar.setBounds(colW, 0, colW * 4, rowH);


    //setting the position of the table
    tableComponent.setBounds(0, rowH, getWidth(), rowH*7);

    //set position of save playlist button
    saveLibButton.setBounds(colW * 5, 1, colW, rowH);

}

//==============================================================================
int PlaylistComponent::getNumRows()
{
  return interestedTitles.size(); //the length of filtered vector of file
}

void PlaylistComponent::paintRowBackground(Graphics & g,
                        int rowNumber, 
                        int width, 
                        int height, 
                        bool rowIsSelected)
{
    if (rowIsSelected)
    {
      g.fillAll(Colours::orange); //colour background of row orange when selected
    }
    else{
      g.fillAll(Colours::darkgrey); //colour of background of rows will be grey when unselected
    }
}

//==============================================================================

void PlaylistComponent::paintCell (Graphics & g, 
                                    int rowNumber, 
                                    int columnId, 
                                    int width, 
                                    int height,
                                    bool rowIsSelected)
{
    //Draw track's title to first column
    if (columnId == 1)
    {
        g.drawText (interestedTitles[rowNumber],
            1, rowNumber,
            width - 4, height,
            Justification::centredLeft,
            true);
    }
    //Draw duration of track in seconds to second column
    if (columnId == 2)
    {
         g.drawText ((interestedDuration[rowNumber]) + "s",
            1, rowNumber,
            width - 4, height,
            Justification::centredLeft,
            true);       
    }
}

//==============================================================================

Component* PlaylistComponent::refreshComponentForCell (int rowNumber,
                                   int columnId,
                                   bool isRowSelected,
                                   Component *existingComponentToUpdate)
{
    // Create buttons for each line in the 3rd column to add to the Left channel deck
    if (columnId == 3)
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* btn = new TextButton{"Add to Left Channel"};
              String id{ std::to_string(rowNumber) };
              btn->setComponentID(id);
              btn->addListener(this);
              existingComponentToUpdate = btn;
              btn->setColour(TextButton::buttonColourId, juce::Colours::darkslategrey);
        }
    }
    // Create buttons for each line in the 4th column to add to the Right channel deck
    if (columnId == 4)
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* btn = new TextButton{"Add to Right Channel"};
            // add 100 to id to allow buttons to be processed differently between channels
              String id{ std::to_string(rowNumber + 100) };
              btn->setComponentID(id);
              btn->addListener(this);
              existingComponentToUpdate = btn;
              btn->setColour(TextButton::buttonColourId, juce::Colours::darkslategrey);
        }
    }
    // Create buttons for each line in the 5th column to delete file
    if (columnId == 5)
    {
        if (existingComponentToUpdate == nullptr)
        {
              TextButton* btn = new TextButton{"X"};
              // add 200 to id to allow buttons to be processed differently between channels
              String id{ std::to_string(rowNumber + 200) };
              btn->setComponentID(id);
              btn->addListener(this);
              existingComponentToUpdate = btn;
              btn->setColour(TextButton::buttonColourId, juce::Colours::darksalmon);
        }
    }

    // Create buttons for each line in the 5th column to delete file
    if (columnId == 5)
    {
        if (existingComponentToUpdate == nullptr)
        {
              TextButton* btn = new TextButton{"X"};
              String id{ std::to_string(rowNumber + 200) };
              btn->setComponentID(id);
              btn->addListener(this);
              existingComponentToUpdate = btn;
              btn->setColour(TextButton::buttonColourId, juce::Colours::darksalmon);
        }
    }
    return existingComponentToUpdate;

}
//==============================================================================

//AudioSource pure virtual functions
void PlaylistComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate){}
void PlaylistComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill){}
void PlaylistComponent::releaseResources(){}

//==============================================================================

void PlaylistComponent::buttonClicked(Button* button)
{
    //get id of button 
    int id = std::stoi(button->getComponentID().toStdString()); 
    //if id is less than 100, it should be allocated to the left channel GUI player. 
    if (id < 100)
    {
        addToChannelList(interestedFiles[id], 0);
    }
    //if id between 100 and 200, it should be allocated to the right chanel GUI player
    if ( id > 99 && id < 200)
    {
        addToChannelList(interestedFiles[id - 100], 1);
    }
    
     //if id is above 200, it should invoked the delete function to delete the track
    if (id > 199)
    {
        deleteTrack(id);
    }

    if (button == &saveLibButton) 
    {
        DBG("Save Button");
        DBG("Writing into file");
        writingIntoFile(); 
    }
}

//==============================================================================
bool PlaylistComponent::isInterestedInFileDrag(const StringArray& files)
{
    return true; // allows files to be dragged and dropped 
}

void PlaylistComponent::filesDropped(const StringArray& files, int x, int y)
{
    //perform if files have been dropped (mouse released with files) 
    for (String filename : files)
    {
        //for each file URL, get filepath and file name 
        std::string filepath = String(filename).toStdString();
        std::size_t startFilePos = filepath.find_last_of("/");
        std::size_t startExtPos = filepath.find_last_of(".");
        std::string extn = filepath.substr(startExtPos + 1, filepath.length() - startExtPos);
        std::string file = filepath.substr(startFilePos + 1, filepath.length() - startFilePos - extn.size() - 2);

        //update vectors for file details
        inputFiles.push_back(filepath);
        trackTitles.push_back(file);

        //compute adudio length of the file and update vectors for file details 
        getAudioLength(URL{ File{filepath} });

    }
    //Initialise interested titles as the full list. 
    //This will be updated when text is entered in the search bar
    interestedTitles = trackTitles;
    interestedFiles = inputFiles;

    //update the music library table to include added files 
    tableComponent.updateContent();
    tableComponent.repaint();
}

//==============================================================================
void PlaylistComponent::textEditorTextChanged(TextEditor& textEditor)
{
    //whenever the search box is modified, clear the vectors that will be used for the table 
    interestedTitles.clear(); 
    interestedDuration.clear();
    interestedFiles.clear();

    // start at position 0 of the original library list and increment until the last element of the list
    int pos = 0;
    for (std::string track : trackTitles)
    {   
        //check if the texts typed in the search box is a substring of the track title we are currently processing
        if (track.find(searchBar.getText().toStdString()) != std::string::npos)
        {
            //if there is a match, push the item to the vector used for displaying values in the table
            interestedTitles.push_back(trackTitles[pos]);
            interestedDuration.push_back(trackDurations[pos]);
            interestedFiles.push_back(inputFiles[pos]);
        }
        ++pos; 
    }
    //update the contents of the table after looping
    tableComponent.updateContent();
    tableComponent.repaint();

}


//==============================================================================
// Add music file to list of the respective Left/Right channel's playlist
void PlaylistComponent::addToChannelList(std::string filepath, int channel)
{
    if (channel == 0) //left
    {
        playListLeft.push_back(filepath);
        AlertWindow::showMessageBox(juce::AlertWindow::AlertIconType::InfoIcon,
            "Add to Deck Information:",
            "Track added to left playlist on Deck",
            "OK",
            nullptr);
        
        tableComponent.updateContent();
    }
    if (channel == 1) //right
    {
        playListRight.push_back(filepath);
        AlertWindow::showMessageBox(juce::AlertWindow::AlertIconType::InfoIcon,
            "Add to Deck Information:",
            "Track added to right playlist on Deck",
            "OK",
            nullptr);

        tableComponent.updateContent();

    }
}

//Delete music file from playlist
void PlaylistComponent::deleteTrack(int id)
{ 
    auto fileIt = std::find(inputFiles.begin(), inputFiles.end(), interestedFiles[id]);
    auto titleIt = std::find(trackTitles.begin(), trackTitles.end(), interestedTitles[id]);

    auto fileID = std::distance(inputFiles.begin(), fileIt);
    auto titleID = std::distance(trackTitles.begin(), titleIt);

    inputFiles.erase(inputFiles.begin() + fileID);
    trackTitles.erase(trackTitles.begin() + titleID);

    interestedTitles.erase(interestedTitles.begin() + id);
    interestedFiles.erase(interestedFiles.begin() + id);
    interestedDuration.erase(interestedDuration.begin() + id);

    trackDurations.erase(trackDurations.begin() + id);

    AlertWindow::showMessageBox(juce::AlertWindow::AlertIconType::InfoIcon,
            "Information:",
            "Track has been deleted from playlist",
            "OK",
            nullptr);
    
    tableComponent.updateContent();
}

// get audio length metadata
void PlaylistComponent::getAudioLength(URL audioURL)
{
    double trackLen = 0.0;

    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));

    if (reader != nullptr) // good file!
    {
        std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader,
            true));
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
        double trackLen = transportSource.getLengthInSeconds(); // get length of audio
        std::string trackLength = std::to_string(trackLen);
        trackDurations.push_back(trackLength); // add audio length to vector
    }

    //Initialise interested durations as the full list. 
    //This will be updated when text is entered in the search bar
    interestedDuration = trackDurations;
}

/*======================================================*/


//writing in name, duration and path to a txt file
void PlaylistComponent::writingIntoFile()
{
   File dataFile = File::getCurrentWorkingDirectory().getChildFile("songData.txt");

    if (!dataFile.existsAsFile())
    {
        DBG("File doesn't exist ...");
    }
    else
    {
        DBG("File exists");
    }
   
    dataFile.replaceWithText("", false, false, "\n");
    FileOutputStream _file(dataFile);

    if (!_file.openedOk())
    {
        DBG("not opened");
       
    }

    _file.setNewLineString("\n");
    if (trackTitles.size() >0) {
        for (int i = 0; i < trackTitles.size(); i++) {
            //DBG("is inside");
            _file.writeText(path[i].getFileName()+"\n", false, false, nullptr);
            _file.writeText(trackDurations[i] + "\n", false, false, nullptr);
            _file.writeText(path[i].toString(false)+"\n", false, false, nullptr);
           
        }
    }
    _file.flush(); 
}

//reading in name, duration and path from txt file and then pushing it back to the vectors
void PlaylistComponent::readingFile() 
{
   File dataFile = File::getCurrentWorkingDirectory().getChildFile("songData.txt");
   FileInputStream _file(dataFile);

    if (!_file.openedOk())
    {
       DBG("Failed to open file");
            
    }
    else 
    {
          while (!_file.isExhausted())
          {
                String singleLine = _file.readNextLine();
                DBG(singleLine);
                trackTitles.push_back(singleLine.toStdString());
                singleLine = _file.readNextLine();
                DBG(singleLine);
                trackDurations.push_back(singleLine.toStdString());
                singleLine = _file.readNextLine();
                DBG(singleLine);
                path.push_back(singleLine);  
          }
    }
}

/***************************************************************
 * Name:      PhotoCommenterMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Christopher Wallace (cjwallacework@gmail.com)
 * Created:   2015-02-11
 * Copyright: Christopher Wallace (2015)
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "PhotoCommenterMain.h"

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild = _T("Image Commenter built for bobblek04 using ");

    wxbuild<<wxVERSION_STRING;

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-Mac");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    wxbuild<<_T("\n\nCreated by Christopher Wallace, contact: cjwallacework@gmail.com");

    return wxbuild;
}

BEGIN_EVENT_TABLE(PhotoCommenterFrame, wxFrame)
    EVT_BUTTON(idButtonDirectory,PhotoCommenterFrame::OnButtonDirectory)
    EVT_BUTTON(idButtonNext,PhotoCommenterFrame::OnButtonNext)
    EVT_BUTTON(idButtonPrev,PhotoCommenterFrame::OnButtonPrev)
    EVT_BUTTON(idButtonSave,PhotoCommenterFrame::OnButtonExit)

    EVT_CLOSE(PhotoCommenterFrame::OnClose)
    EVT_MENU(idMenuAbout, PhotoCommenterFrame::OnAbout)
    EVT_MENU(idMenuLocation,PhotoCommenterFrame::OnLocation)
END_EVENT_TABLE()

PhotoCommenterFrame::PhotoCommenterFrame(wxFrame *frame, const wxString& title)
    : wxFrame(frame, -1, title)
{
    //correct seperator
    sep = _T("/");
    #if defined(WXMSW)
        sep = _T("\\");
    #endif
    #if defined(UNIX)
        sep = _T("/");
    #endif


#if wxUSE_MENUS
    // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu(_T(""));
    fileMenu->Append(idMenuLocation,_T("Location"));
    mbar->Append(fileMenu,_T("&File"));
    wxMenu* helpMenu = new wxMenu(_T(""));
    helpMenu->Append(idMenuAbout, _("&About\tF1"), _("Show info about this application"));
    mbar->Append(helpMenu, _("&Help"));

    SetMenuBar(mbar);
#endif // wxUSE_MENUS

    //check for save data
    wxTextFile loadData(wxGetCwd()+sep+_T("Prefs.txt"));
    if (loadData.Exists())
    {
        loadData.Open();
        if (loadData.GetLineCount() > 0)
        {
            pathToLocation = loadData.GetFirstLine();
        }
        else
        {
            pathToLocation = wxGetCwd()+sep+_T("MasterList.txt");
        }
    }
    else
    {
        pathToLocation = wxGetCwd()+sep+_T("MasterList.txt");
    }
    //update the menu text
    locationItem = fileMenu->FindItemByPosition(0);
    locationItem->SetText(pathToLocation);

    //initialise handlers
    wxInitAllImageHandlers();

    //setup the image viewing/editing screen
    ImagePanel = new wxPanel(this);
    imagesizer = new wxBoxSizer(wxVERTICAL);

    //create objects
    DirectoryButton = new wxButton(ImagePanel,idButtonDirectory,wxT("Open Directory"));

    CurrentFolderText = new wxStaticText(ImagePanel,-1,_T("No Folder Selected"));

    FolderDescription = new wxTextCtrl(ImagePanel,-1,_T("Folder Description"));
    //staticboxsizer so we know what we're entering text into
    wxStaticBoxSizer *foldersizer = new wxStaticBoxSizer(wxVERTICAL,ImagePanel,_T("Folder Description"));
    foldersizer->Add(FolderDescription,0,wxALL|wxEXPAND,5);

    //create a blank bitmap the size of 1/2 of the screen
    screenHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
    screenWidth = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
    screenHeight = (screenHeight)/2;
    screenWidth = (screenWidth)/2;
    loadedBitmap = wxBitmap(screenWidth, screenHeight,-1);
    ImageViewer = new wxStaticBitmap(ImagePanel,-1,loadedBitmap);

    CurrentImageText = new wxStaticText(ImagePanel,-1,_T("No Image Selected"));

    //setup buttons on the bottom
    NextImage = new wxButton(ImagePanel,idButtonNext,_T("Next Image"));
    PrevImage = new wxButton(ImagePanel,idButtonPrev,_T("Previous Image"));
    wxBoxSizer *middlesizer = new wxBoxSizer(wxHORIZONTAL);
    middlesizer->Add(PrevImage,1,wxALL|wxEXPAND,5);
    middlesizer->Add(ImageViewer,0,wxALL|wxEXPAND,5);
    middlesizer->Add(NextImage,1,wxALL|wxEXPAND,5);

    //description box
    wxStaticBoxSizer *descriptionsizer = new wxStaticBoxSizer(wxVERTICAL,ImagePanel,_T("Image Description"));
    ImageDescription = new wxTextCtrl(ImagePanel,-1,_T("No Description"));
    descriptionsizer->Add(ImageDescription,0,wxALL|wxEXPAND,5);

    //And the save button
    SaveText = new wxButton(ImagePanel,idButtonSave,_T("Exit"));

    imagesizer->Add(DirectoryButton,0,wxALL|wxEXPAND,5);
    imagesizer->Add(foldersizer,1,wxALL|wxEXPAND,5);
    imagesizer->Add(CurrentFolderText,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
    imagesizer->Add(middlesizer,7,wxALL|wxEXPAND,5);
    imagesizer->Add(CurrentImageText,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,5);
    imagesizer->Add(descriptionsizer,1,wxALL|wxEXPAND,5);
    imagesizer->Add(SaveText,1,wxALL|wxEXPAND,5);
    ImagePanel->SetSizer(imagesizer);
    //ImagePanel->Hide();

    //add it all together
    wxBoxSizer *windowsizer = new wxBoxSizer(wxVERTICAL);
    windowsizer->Add(ImagePanel,1,wxALL|wxEXPAND,0);
    SetSizer(windowsizer);
    Maximize();
}


PhotoCommenterFrame::~PhotoCommenterFrame()
{
}

bool PhotoCommenterFrame::ProcessDirectory()
{

    //clear any stored values
    filesGet.Empty();
    filesDescription.Empty();
    currentImage = 0;

    //update text field
    CurrentFolderText->SetLabel(pathToProcess);
    CurrentFolderText->Refresh();

    wxDir dir(pathToProcess);

    if (!dir.IsOpened())
    {
        //couldn't open directory
        return false;
    }

    wxArrayString getFiles;
    size_t numFiles = dir.GetAllFiles(pathToProcess,&getFiles,wxEmptyString,wxDIR_DEFAULT);

    //data for saved records internally for program
    bool saved = false;
    int saveDate = 0;
    wxString savePath;

    //we check each file for the specified filetypes
    for (int i=0;i<numFiles;i++)
    {
        //convert to wxFileName for easy access to file extension
        wxFileName current(getFiles[i]);
        if (current.GetExt() == _T("jpg"))
        {
            filesGet.Add(getFiles[i]);
            filesDescription.Add(_T("No Description"));
        }
        else if (current.GetExt() == _T("txt") && current.GetName().AfterFirst('-') == _T("SaveData"))
        {
            if (wxAtoi(current.GetName().BeforeFirst('-')) > saveDate)
            {
                saved = true;
                saveDate = wxAtoi(current.GetName().BeforeFirst('-'));
                savePath = getFiles[i];
            }
        }
    }
    wxString message = _T("Found ");
    message<<filesGet.Count()<<_T(" files in the directory specified.");
    wxMessageDialog *notify = new wxMessageDialog(this,message,_T("Action complete"),wxOK);
    //notify->ShowModal();

    //handle no images found
    if (filesGet.Count() < 1)
    {
        return false;
    }

    //go to phase 2
    //MainPanel->Hide();
    //Refresh();

    if (saved)
    {
        LoadSave(savePath);
    }

    //setup image display
    currentImage = 0;
    DisplayImage();

}

void PhotoCommenterFrame::LoadSave(wxString path)
{
    wxTextFile loadFile(path);
    if (!loadFile.Exists())
    {
        return;
    }
    loadFile.Open();
    if (!loadFile.IsOpened())
    {
        return;
    }
    //ensure file exists and is opened
    wxString line = loadFile.GetFirstLine();
    FolderDescription->SetValue(line.AfterFirst('-').Trim(false));
    for (line=loadFile.GetNextLine();!loadFile.Eof();line = loadFile.GetNextLine())
    {
        //check each line of the file
        for (int x=0;x<filesGet.Count();x++)
        {
            //we need to compare filenames again
            wxFileName current(filesGet[x]);
            if (line.BeforeFirst('-').Trim(true) == current.GetName())
            {
                //if the line we're on refers to this entry
                filesDescription[x] = line.AfterFirst('-').Trim(false);
            }
        }
    }

    loadFile.Close();
}

void PhotoCommenterFrame::DisplayImage()
{
    if (currentImage < 0 || currentImage+1 > filesGet.Count())
    {
        return;
    }
    ImageDescription->SetValue(filesDescription[currentImage]);
    if (!loadedImage.LoadFile(filesGet[currentImage]))
    {
        return;
    }
    //check for resize
    if (loadedImage.GetHeight() > screenHeight || loadedImage.GetWidth() > screenWidth)
    {
        //get aspect ratio
        double aspectRatio = (double)loadedImage.GetWidth() / (double)loadedImage.GetHeight();
        int newHeight, newWidth;
        wxString str = _T("oldheight: ");
        str<<loadedImage.GetHeight()<<_T(", oldwidth: ")<<loadedImage.GetWidth();
        if ((loadedImage.GetHeight()-screenHeight) * (double)aspectRatio > (loadedImage.GetWidth() - screenWidth))
        {
            //if height difference is greater than width difference
            newHeight = screenHeight;
            newWidth = screenHeight * (double)aspectRatio;
        }
        else
        {
            //if width difference is greater
            newWidth = screenWidth;
            newHeight = screenWidth / (double)aspectRatio;
        }
        str<<_T(", newheight: ")<<newHeight<<_T(", newwidth: ")<<newWidth;
        str<<_T(", aspectratio: ")<<aspectRatio;
        wxMessageDialog *dlg = new wxMessageDialog(this,str,str,wxOK);
        //dlg->ShowModal();
        loadedImage.Rescale(newWidth,newHeight,wxIMAGE_QUALITY_HIGH);
    }
    CurrentImageText->SetLabel(wxFileName(filesGet[currentImage]).GetName());
    CurrentImageText->Refresh();
    loadedBitmap = wxBitmap(loadedImage);
    ImageViewer->SetBitmap(loadedBitmap);
    ImageViewer->Refresh();
    imagesizer->Layout();

}

void PhotoCommenterFrame::OnButtonDirectory(wxCommandEvent &event)
{
    //open a file dialog
    wxDirDialog *dd = new wxDirDialog(this,_T("Choose a directory"),_T(""),wxDD_DEFAULT_STYLE|wxDD_DIR_MUST_EXIST|wxDD_CHANGE_DIR);
    if (dd->ShowModal() == wxID_OK)
    {
        if (filesGet.Count() > 0)
        {
            SaveCurrent();
            SaveData();
        }
        //user selected a valid directory
        pathToProcess = dd->GetPath();
        ProcessDirectory();
    }
}

void PhotoCommenterFrame::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void PhotoCommenterFrame::OnAbout(wxCommandEvent &event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void PhotoCommenterFrame::OnButtonNext(wxCommandEvent &event)
{
    SaveCurrent();
    currentImage++;
    if (currentImage >= filesGet.Count())
    {
        currentImage = 0;
    }
    DisplayImage();
}

void PhotoCommenterFrame::OnButtonPrev(wxCommandEvent &event)
{
    SaveCurrent();
    currentImage-=1;
    if (currentImage < 0)
    {
        currentImage = filesGet.Count() -1;
    }
    DisplayImage();
}

void PhotoCommenterFrame::OnButtonExit(wxCommandEvent &event)
{
    SaveCurrent();
    SaveData();
    Destroy();
}

void PhotoCommenterFrame::SaveCurrent()
{
    if (filesGet.Count() >= currentImage)
    {
        filesDescription[currentImage] = ImageDescription->GetValue();
    }
}

void PhotoCommenterFrame::SaveData()
{
//save preferences
    wxTextFile prefsFile(wxGetCwd()+sep+_T("Prefs.txt"));
    if (prefsFile.Exists())
    {
        prefsFile.Open();
        prefsFile.Clear();
    }
    else
    {
        prefsFile.Create(wxGetCwd()+sep+_T("Prefs.txt"));
        prefsFile.Open();
    }
    if (!prefsFile.IsOpened())
    {
        //error opening
    wxMessageDialog *prefsErrorDlg = new wxMessageDialog(this,_T("Error saving preferences to ")+wxGetCwd()+sep+_T("Prefs.txt"),_T("Error"),wxOK);
        prefsErrorDlg->ShowModal();
        return;
    }
    prefsFile.AddLine(pathToLocation);
    prefsFile.Write();
    prefsFile.Close();
    if (filesGet.Count() > 0)
    {
        //save data to the appropriate file
        wxDateTime currentDate = wxDateTime::Today();
        wxString filenameSave = sep+currentDate.Format(_T("%y%m%d-SaveData.txt"));
        wxTextFile saveFile(pathToProcess+filenameSave);
        if (saveFile.Exists())
        {
            saveFile.Open();
            saveFile.Clear();
        }
        else
        {
            saveFile.Create(pathToProcess+filenameSave);
            saveFile.Open();
        }
        if (!saveFile.IsOpened())
        {
            //error opening
            wxMessageDialog *errorDlg = new wxMessageDialog(this,_T("Error saving data to ")+pathToProcess+filenameSave,_T("Error"),wxOK);
            errorDlg->ShowModal();
            return;
        }

        wxString line = pathToProcess;
        line<<_T(" - ")<<FolderDescription->GetValue();
        saveFile.AddLine(line);
        for (int i=0;i<filesGet.Count();i++)
        {
            //again convert to wxFileName to get just the filename easily
            wxFileName current(filesGet[i]);
            line = current.GetName()<<_T(" - ")<<filesDescription[i];
            saveFile.AddLine(line);
        }
        saveFile.Write();
        saveFile.Close();

        //save data to the master list
        wxTextFile masterFile(pathToLocation);
        if (masterFile.Exists())
        {
            masterFile.Open();
        }
        else
        {
            masterFile.Create(pathToLocation);
            masterFile.Open();
        }
        if (!masterFile.IsOpened())
        {
            //error opening
            wxMessageDialog *masterErrorDlg = new wxMessageDialog(this,_T("Error saving master list to ")+pathToLocation,_T("Error"),wxOK);
            masterErrorDlg->ShowModal();
            return;
        }
        //date first for identification
        line = currentDate.Format(_T("%y%m%d"));
        masterFile.AddLine(line);
        line = pathToProcess;
        line<<_T(" - ")+FolderDescription->GetValue();
        masterFile.AddLine(line);
        for (int i=0;i<filesGet.Count();i++)
        {
            wxFileName current(filesGet[i]);
            line = current.GetName()+_T(" - ")+filesDescription[i];
            masterFile.AddLine(line);
        }
        masterFile.Write();
        masterFile.Close();
    }
    //done saving, now exit
}

void PhotoCommenterFrame::OnLocation(wxCommandEvent &event)
{
    //first we gather the filename and directory components to use
    wxFileName current(pathToLocation);
    //allow for saving in a new location
    wxFileDialog *fd = new wxFileDialog(this,_T("Save the master list..."),current.GetPath(),current.GetFullName(),_T("*.txt"),wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (fd->ShowModal() == wxID_OK)
    {
        pathToLocation = fd->GetPath();
        locationItem->SetText(pathToLocation);
    }
}

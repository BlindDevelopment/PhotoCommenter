/***************************************************************
 * Name:      PhotoCommenterMain.h
 * Purpose:   Defines Application Frame
 * Author:    Christopher Wallace (cjwallacework@gmail.com)
 * Created:   2015-02-11
 * Copyright: Christopher Wallace ()
 **************************************************************/

#ifndef PHOTOCOMMENTERMAIN_H
#define PHOTOCOMMENTERMAIN_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/dir.h>
    #include <wx/filename.h>
    #include <wx/image.h>
    #include <wx/datetime.h>
    #include <wx/textfile.h>
#endif

#include "PhotoCommenterApp.h"

class PhotoCommenterFrame: public wxFrame
{
    public:
        PhotoCommenterFrame(wxFrame *frame, const wxString& title);
        ~PhotoCommenterFrame();
    private:
        wxButton *DirectoryButton;
        wxStaticText *CurrentFolderText;

        wxPanel *ImagePanel;
        wxTextCtrl *FolderDescription;
        wxButton *NextImage, *PrevImage, *SaveText;
        wxStaticBitmap *ImageViewer;
        wxImage loadedImage;
        wxBitmap loadedBitmap;
        wxTextCtrl *ImageDescription;
        wxStaticText *CurrentImageText;
        //we put imagesizer here so we can refresh it later
        wxBoxSizer *imagesizer;

        //data storage values
        int currentImage;
        wxArrayString filesGet;
        wxArrayString filesDescription;
        int screenHeight, screenWidth;
        wxString pathToProcess;
        wxMenuItem *locationItem;
        wxString pathToLocation;
        wxString sep;


        enum
        {
            idMenuAbout = 1000,
            idMenuLocation,
            idButtonDirectory,
            idButtonSave,
            idButtonNext,
            idButtonPrev,
        };
        bool ProcessDirectory();
        void DisplayImage();
        void LoadSave(wxString path);
        void SaveData();
        void SaveCurrent();

        void OnButtonDirectory(wxCommandEvent &event);
        void OnClose(wxCloseEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnLocation(wxCommandEvent &event);
        void OnButtonNext(wxCommandEvent &event);
        void OnButtonPrev(wxCommandEvent &event);
        void OnButtonExit(wxCommandEvent &event);
        DECLARE_EVENT_TABLE()
};


#endif // PHOTOCOMMENTERMAIN_H

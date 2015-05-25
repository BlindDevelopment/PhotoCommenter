/***************************************************************
 * Name:      PhotoCommenterApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Christopher Wallace (cjwallacework@gmail.com)
 * Created:   2015-02-11
 * Copyright: Christopher Wallace ()
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "PhotoCommenterApp.h"
#include "PhotoCommenterMain.h"

IMPLEMENT_APP(PhotoCommenterApp);

bool PhotoCommenterApp::OnInit()
{
    PhotoCommenterFrame* frame = new PhotoCommenterFrame(0L, _("Photo Commenter"));

    frame->Show();

    return true;
}

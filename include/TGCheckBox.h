//-----------------------------------------------------------------------------
// This source file is part of TGUI (Tiny GUI)
//
// Copyright (c) 2006-2007 Tubras Software, Ltd
// Also see acknowledgements in Readme.html
//
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal 
// in the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
// of the Software, and to permit persons to whom the Software is furnished to 
// do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
// THE SOFTWARE.
//-----------------------------------------------------------------------------
#ifndef __TGCHECKBOX_H__
#define __TGCHECKBOX_H__

namespace TGUI
{

    class TGCheckBox : public TGControl
    {
        TGString        m_caption;
        bool			m_hover;
        bool			m_pushed;
        TGDataManager<bool>	m_checked;

    public:
        TGCheckBox(TGControl *parent, TGString name="", TGString caption="");
        virtual ~TGCheckBox();

        virtual void setCaption(TGString newCaption);

        virtual void render();
        virtual TGString getControlType() {return "TGCheckBox";};

        virtual void setState(bool checked);

        virtual void onMouseDown(int x, int y, int b);
        virtual void onMouseUp(int x, int y, int b);
        virtual void onMouseEnter();
        virtual void onMouseExit(int x, int y);
    };
}
#endif

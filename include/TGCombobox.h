//-----------------------------------------------------------------------------
// This source file is part of TGUI (Tiny GUI)
//
// Copyright (c) 2006 Tubras Software, Ltd
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
#ifndef __TGCOMBOBOX_H__
#define __TGCOMBOBOX_H__

namespace TGUI
{

    enum TGComboboxStyle
    {
        CBS_NORMAL,             // allow text editing
        CBS_DROPDOWN_LIST       // pick from drop down only
    };

    class TGCombobox : public TGControl
    {
    private:
        int             m_height;
        TGEditbox*      m_inputbox;
        TGListbox*      m_listbox;
        TGComboboxStyle m_style;

    private:
        bool itemSelected(const TGEventArgs& args);

    public:

        TGCombobox(TGControl *parent, int x1=0, int y1=0, int x2=5, int y2=5);
        virtual ~TGCombobox();

        void setStyle(TGComboboxStyle value);
        TGComboboxStyle getStyle()  { return m_style; };

        virtual void render();
        virtual TGString getControlType() {return "TGCombobox";};
        virtual void setBounds(int x1, int y1, int x2, int y2);
        virtual TGControl *childAt(TGReal x, TGReal y);
        virtual bool pointInControl(TGReal x, TGReal y);
        virtual void setTheme(TGTheme theme,bool updateChildren=false);
        virtual void onFocusExit();

        virtual bool focused();

        virtual void addItem(TGString text);
        virtual void onMouseDown(int x, int y, int b);
        virtual void onMouseMoved(int x, int y);
        virtual void onMouseEnter();
        virtual void onMouseExit(int x, int y);

    };
}
#endif

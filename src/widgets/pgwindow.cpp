/*
    ParaGUI - crossplatform widgetset
    Copyright (C) 2000,2001,2002  Alexander Pipelka
 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
 
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
 
    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
    Alexander Pipelka
    pipelka@teleweb.at
 
    Last Update:      $Author: braindead $
    Update Date:      $Date: 2003/11/24 09:17:22 $
    Source File:      $Source: /sources/paragui/paragui/src/widgets/pgwindow.cpp,v $
    CVS/RCS Revision: $Revision: 1.3.6.9.2.2 $
    Status:           $State: Exp $
*/

#include "pgwindow.h"
#include "pgapplication.h"
#include <cstring>

PG_Window::PG_Window(PG_Widget* parent, const PG_Rect& r, const char* windowtext, Uint32 flags, const char* style, int heightTitlebar) : PG_ThemeWidget(parent, r) {

	my_moveMode = false;
	my_heightTitlebar = heightTitlebar;
	my_showCloseButton = flags & WF_SHOW_CLOSE;
	my_showMinimizeButton = flags & WF_SHOW_MINIMIZE;

	PG_Rect rb;

	my_titlebar = new PG_ThemeWidget(this, PG_Rect(0, 0, my_width, my_heightTitlebar), style);
	my_titlebar->EnableReceiver(false);

	my_labelTitle = new PG_Label(my_titlebar, PG_Rect(0, 0, my_width, my_heightTitlebar), windowtext, style);
	my_labelTitle->SetAlignment(PG_TA_CENTER);

	my_buttonClose = new PG_Button(my_titlebar, PG_WINDOW_CLOSE, rb, NULL);
	my_buttonClose->sigClick.connect(slot(*this, &PG_Window::handleButtonClick));
	
	my_buttonMinimize = new PG_Button(my_titlebar, PG_WINDOW_MINIMIZE, rb, NULL);
	my_buttonMinimize->sigClick.connect(slot(*this, &PG_Window::handleButtonClick));

	LoadThemeStyle(style);

	if(!my_showCloseButton) {
		my_buttonClose->Hide();
	}
	if(!my_showMinimizeButton) {
		my_buttonMinimize->Hide();
	}
}

PG_Window::~PG_Window() {
}

void PG_Window::SetTitle(const char* title, int alignment) {
	my_labelTitle->SetAlignment(alignment);
	my_labelTitle->SetText(title);

	/*if (my_showCloseButton)
		my_buttonClose->Update();

	if (my_showMinimizeButton)
		my_buttonMinimize->Update();*/
}

const char* PG_Window::GetTitle() {
	return my_labelTitle->GetText();
}

void PG_Window::LoadThemeStyle(const char* widgettype) {
	PG_Theme* t = PG_Application::GetTheme();

	PG_ThemeWidget::LoadThemeStyle(widgettype, "Window");

	my_titlebar->LoadThemeStyle(widgettype, "Titlebar");

	t->GetProperty(widgettype, "Titlebar", "height", my_heightTitlebar);
	my_titlebar->SizeWidget(my_width, my_heightTitlebar);
	my_labelTitle->SizeWidget(my_width, my_heightTitlebar);

	PG_Color c = my_labelTitle->GetFontColor();
	t->GetColor(widgettype, "Titlebar", "textcolor", c);
	SetColorTitlebar(c);
	
	t->GetProperty(widgettype, "Titlebar", "showclosebutton", my_showCloseButton);
	my_buttonClose->LoadThemeStyle(widgettype, "CloseButton");

	t->GetProperty(widgettype, "Titlebar", "showminimizebutton", my_showMinimizeButton);
	my_buttonMinimize->LoadThemeStyle(widgettype, "MinimizeButton");

	my_buttonClose->MoveWidget(PG_Rect(my_width - my_heightTitlebar, 0, my_heightTitlebar, my_heightTitlebar));
	my_buttonMinimize->MoveWidget(PG_Rect(0, 0, my_heightTitlebar, my_heightTitlebar));
}

void PG_Window::eventSizeWidget(Uint16 w, Uint16 h) {
	my_titlebar->SizeWidget(w, my_heightTitlebar);

	my_buttonClose->MoveWidget(PG_Rect(w - my_heightTitlebar, 0, my_heightTitlebar, my_heightTitlebar));
	my_buttonMinimize->MoveWidget(PG_Rect(0, 0, my_heightTitlebar, my_heightTitlebar));

	int lw = w;
	int lx = 0;
	if(my_showMinimizeButton) {
		lw -= my_buttonMinimize->w;
		lx += my_buttonMinimize->w;
	}
	if(my_showCloseButton) {
		lw -= my_buttonMinimize->w;
	}
	my_labelTitle->MoveWidget(PG_Rect(lx, 0, lw, my_heightTitlebar));

	PG_ThemeWidget::eventSizeWidget(w, h);
}

void PG_Window::eventBlit(SDL_Surface* srf, const PG_Rect& src, const PG_Rect& dst) {
	PG_Rect my_src;
	PG_Rect my_dst;

	PG_ThemeWidget::eventBlit(srf, src, dst);

	PG_Rect client(0, my_heightTitlebar, my_width, my_height-my_heightTitlebar);
	DrawBorder(client, my_bordersize);

	client.my_xpos++;
	client.my_ypos++;
	client.my_width -= 2;
	client.my_height -= 2;

	DrawBorder(client, my_bordersize, false);
}

bool PG_Window::eventMouseButtonDown(const SDL_MouseButtonEvent* button) {
	PG_Rect ta(*this);
	ta.my_width = my_titlebar->w;
	ta.my_height = my_titlebar->h;

	int x,y;
	x = button->x;
	y = button->y;

	if((ta.my_xpos <= x) && (x <= ta.my_xpos + ta.my_width)) {
		if((ta.my_ypos <= y) && (y <= ta.my_ypos + ta.my_height)) {
			my_moveMode = true;
			my_moveDelta.x = ta.my_xpos - x;
			my_moveDelta.y = ta.my_ypos - y;
			Show();
			SetCapture();
			return true;
		}
	}

	Show();
	return false; //PG_Widget::eventMouseButtonDown(button);
}

bool PG_Window::eventMouseButtonUp(const SDL_MouseButtonEvent* button) {
	int x,y;

	x = button->x;
	y = button->y;

	x += my_moveDelta.x;
	y += my_moveDelta.y;

	if(x < 0)
		x=0;
	if(y < 0)
		y=0;
	if(x+my_width > (my_srfScreen->w - my_width))
		x = (my_srfScreen->w - my_width);
	if(y+my_height > (my_srfScreen->h - my_height))
		y = (my_srfScreen->h - my_height);

	if(my_moveMode) {
		my_moveMode = false;
		ReleaseCapture();
		return true;
	}

	return false; //PG_Widget::eventMouseButtonUp(button);
}

bool PG_Window::eventMouseMotion(const SDL_MouseMotionEvent* motion) {
	if(!my_moveMode) {
		return PG_Widget::eventMouseMotion(motion);
	}

	int x,y;
	x = motion->x;
	y = motion->y;

	x += my_moveDelta.x;
	y += my_moveDelta.y;

	if(GetParent() != NULL) {  // Not a top-level-widget
		PG_Point pos = GetParent()->ScreenToClient(x,y);
		x = pos.x;
		y = pos.y;         // Should not be moved out of the parent:
		if(x+my_width > GetParent()->Width())
			x = GetParent()->Width() - my_width;
		if(y+my_height > GetParent()->Height())
			y = GetParent()->Height() - my_height;
	}

	if(x+my_width > my_srfScreen->w)
		x = (my_srfScreen->w - my_width);
	if(y+my_height > my_srfScreen->h)
		y = (my_srfScreen->h - my_height);

	if(x < 0)
		x=0;
	if(y < 0)
		y=0;

	MoveWidget(x,y);
	return true;
}

bool PG_Window::handleButtonClick(PG_Button* button) {
	switch(button->GetID()) {
		// close window
		case PG_WINDOW_CLOSE:
			Hide();
			sigClose(this);

		// minimize window
		case PG_WINDOW_MINIMIZE:
			Hide();
			sigMinimize(this);
	}

	// just in case we're modal
	QuitModal();

	return true;
}

void PG_Window::SetColorTitlebar(const PG_Color& c) {
	my_labelTitle->SetFontColor(c);
}

void PG_Window::SetIcon(const char* filename) {
	if (my_buttonMinimize)
		my_labelTitle->SetIndent(my_buttonMinimize->my_width);
	my_labelTitle->SetIcon(filename);
}
	
void PG_Window::SetIcon(SDL_Surface* icon) {
	if (my_buttonMinimize)
		my_labelTitle->SetIndent(my_buttonMinimize->my_width);
	my_labelTitle->SetIcon(icon);
}

void PG_Window::eventShow() {
	// don't ask me why
	my_buttonClose->Update();
	my_buttonMinimize->Update();
}

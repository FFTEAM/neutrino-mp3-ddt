/*
	Based up Neutrino-GUI - Tuxbox-Project 
	Copyright (C) 2001 by Steffen Hehn 'McClean'

	Classes for generic GUI-related components.
	Copyright (C) 2012-2014, Thilo Graf 'dbt'

	License: GPL

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	General Public License for more details.

	You should have received a copy of the GNU General Public
	License along with this program; if not, write to the
	Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
	Boston, MA  02110-1301, USA.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <global.h>
#include <neutrino.h>
#include <system/debug.h>
#include "cc_frm_button.h"


using namespace std;

CComponentsButton::CComponentsButton( 	const int& x_pos, const int& y_pos, const int& w, const int& h,
					const std::string& caption, const std::string& icon_name,
					CComponentsForm* parent,
					bool selected,
					bool enabled,
					bool has_shadow,
					fb_pixel_t color_frame, fb_pixel_t color_body, fb_pixel_t color_shadow)
{
	cc_btn_capt_locale = NONEXISTANT_LOCALE;
	initVarButton(x_pos, y_pos, w, h,  caption, icon_name, parent, selected, enabled, has_shadow, color_frame, color_body, color_shadow);
}

CComponentsButton::CComponentsButton( 	const int& x_pos, const int& y_pos, const int& w, const int& h,
					const neutrino_locale_t& caption_locale, const std::string& icon_name,
					CComponentsForm* parent,
					bool selected,
					bool enabled,
					bool has_shadow,
					fb_pixel_t color_frame, fb_pixel_t color_body, fb_pixel_t color_shadow)
{
	cc_btn_capt_locale = caption_locale;
	initVarButton(x_pos, y_pos, w, h, g_Locale->getText(cc_btn_capt_locale), icon_name, parent, selected, enabled, has_shadow, color_frame, color_body, color_shadow);
}

CComponentsButton::CComponentsButton( 	const int& x_pos, const int& y_pos, const int& w, const int& h,
					const std::string& caption, const char* icon_name,
					CComponentsForm* parent,
					bool selected,
					bool enabled,
					bool has_shadow,
					fb_pixel_t color_frame, fb_pixel_t color_body, fb_pixel_t color_shadow)
{
	string _icon_name = icon_name == NULL ? "" : string(icon_name);
	initVarButton(x_pos, y_pos, w, h,  caption, _icon_name, parent, selected, enabled, has_shadow, color_frame, color_body, color_shadow);
}

CComponentsButton::CComponentsButton( 	const int& x_pos, const int& y_pos, const int& w, const int& h,
					const neutrino_locale_t& caption_locale, const char* icon_name,
					CComponentsForm* parent,
					bool selected,
					bool enabled,
					bool has_shadow,
					fb_pixel_t color_frame, fb_pixel_t color_body, fb_pixel_t color_shadow)
{
	string _icon_name = icon_name == NULL ? "" : string(icon_name);
	cc_btn_capt_locale = caption_locale;
	initVarButton(x_pos, y_pos, w, h,  g_Locale->getText(cc_btn_capt_locale), _icon_name, parent, selected, enabled, has_shadow, color_frame, color_body, color_shadow);
}

void CComponentsButton::initVarButton(	const int& x_pos, const int& y_pos, const int& w, const int& h,
					const std::string& caption,
					const std::string& icon_name,
					CComponentsForm* parent,
					bool selected,
					bool enabled,
					bool has_shadow,
					fb_pixel_t color_frame, fb_pixel_t color_body, fb_pixel_t color_shadow)
{
	cc_item_type 	= CC_ITEMTYPE_BUTTON;

	x 		= x_pos;
	y 		= y_pos;
	width 		= w;
	height	 	= h;
	shadow		= has_shadow;
	shadow_w	= SHADOW_OFFSET;

	col_body_gradient = false/*g_settings.gradiant*/; //gradient is prepared for use but disabled at the moment till some other parts of gui parts are provide gradient
	setColBodyGradient(CColorGradient::gradientLight2Dark, CFrameBuffer::gradientVertical, CColorGradient::light);
	col_frame 	= color_frame;
	col_body	= col_body_gradient? COL_DARK_GRAY : color_body;
	col_shadow	= color_shadow;

	cc_item_enabled  = enabled;
	cc_item_selected = selected;
	fr_thickness 	= 3;
	append_x_offset = 6;
	append_y_offset = 0;
	corner_rad	= 0;
	
	cc_btn_capt_col		= col_body_gradient ? COL_BUTTON_TEXT_ENABLED : COL_INFOBAR_SHADOW_TEXT;
	cc_btn_capt_disable_col = col_body_gradient ? COL_BUTTON_TEXT_DISABLED : COL_MENUCONTENTINACTIVE_TEXT;
	cc_btn_icon_obj	= NULL;
	cc_btn_capt_obj = NULL;
	cc_btn_dy_font  = CNeutrinoFonts::getInstance();
	cc_btn_font	= NULL;
	cc_btn_icon	= icon_name;
	cc_btn_capt	= caption;
	cc_btn_msg	= CRCInput::RC_nokey;
	cc_btn_result	= -1;
	cc_btn_alias	= -1;

	initParent(parent);
	initCCBtnItems();
}

void CComponentsButton::initIcon()
{
	//init cch_icon_obj only if an icon available
	if (cc_btn_icon.empty()) {
		if (cc_btn_icon_obj)
			delete cc_btn_icon_obj;
		cc_btn_icon_obj = NULL;
		return;
	}

	//initialize icon object
	if (cc_btn_icon_obj == NULL){
		int w_icon = 0;
		int h_icon = 0;
		int y_icon = 0;

		string::size_type pos = cc_btn_icon.find("/", 0);
		if (pos == string::npos)
			cc_btn_icon = frameBuffer->getIconBasePath() + "/" + cc_btn_icon + ".png";

		cc_btn_icon_obj = new CComponentsPicture(fr_thickness, y_icon, cc_btn_icon, this);
		h_icon = cc_btn_icon_obj->getHeight();

		if (h_icon > (height-2*fr_thickness)){
			cc_btn_icon_obj->setHeight(height*80/100);
			uint8_t h_ratio = uint8_t(height*100/h_icon);
			w_icon = h_ratio*cc_btn_icon_obj->getWidth()/100;
			cc_btn_icon_obj->setWidth(w_icon);
		}

		y_icon = height/2 - cc_btn_icon_obj->getHeight()/2;

		cc_btn_icon_obj->setYPos(y_icon);
		cc_btn_icon_obj->doPaintBg(false);
	}
}

void CComponentsButton::initCaption()
{
	//init label as caption object and add to container
	if (!cc_btn_capt.empty()){
		if (cc_btn_capt_obj == NULL){
			cc_btn_capt_obj = new CComponentsLabel();
			cc_btn_capt_obj->doPaintBg(false);
			cc_btn_capt_obj->enableTboxSaveScreen(save_tbox_screen);
			addCCItem(cc_btn_capt_obj);
		}
	}else{
		if (cc_btn_capt_obj){
			delete cc_btn_capt_obj;
			cc_btn_capt_obj = NULL;
		}
	}

	//set basic properties
	if (cc_btn_capt_obj){
		//position and size
		int x_cap = fr_thickness;
		x_cap += cc_btn_icon_obj ? cc_btn_icon_obj->getWidth() : 0;

		int w_cap = width - fr_thickness - append_x_offset - x_cap - fr_thickness;
		int h_cap = height*80/100/* - 2*fr_thickness*/;

		/*NOTE:
			paint of centered text in y direction without y_offset
			looks unlovely displaced in y direction especially besides small icons and inside small areas,
			but text render isn't wrong here, because capitalized chars or long chars like e. 'q', 'y' are considered!
			Therefore we here need other icons or a hack, that considers some different height values.
		*/
		int y_cap = height/2 - h_cap/2 + fr_thickness/2;

		cc_btn_capt_obj->setDimensionsAll(x_cap, y_cap, w_cap, h_cap);

		//text and font
		if (cc_btn_font == NULL)
			cc_btn_font = *cc_btn_dy_font->getDynFont(w_cap, h_cap, cc_btn_capt);

		cc_btn_capt_obj->setText(cc_btn_capt, CTextBox::NO_AUTO_LINEBREAK, cc_btn_font);
		cc_btn_capt_obj->forceTextPaint(); //here required;

		//set color
		cc_btn_capt_obj->setTextColor(this->cc_item_enabled ? cc_btn_capt_col : cc_btn_capt_disable_col);

		//corner of text item
		cc_btn_capt_obj->setCorner(corner_rad-fr_thickness, corner_type);
	}

	//handle common position of icon and text inside container required for alignment
	int w_required 	= fr_thickness + append_x_offset;
	w_required 	+= cc_btn_icon_obj ? cc_btn_icon_obj->getWidth() + append_x_offset : 0;
	w_required 	+= cc_btn_font ? cc_btn_font->getRenderWidth(cc_btn_capt) : 0;
	w_required 	+= append_x_offset + fr_thickness;

	//dynamic width
	if (w_required > width){
		dprintf(DEBUG_INFO, "[CComponentsButton]   [%s - %d] width of button (%s) will be changed: defined width=%d, required width=%d\n", __func__, __LINE__, cc_btn_capt.c_str(), width, w_required);
		width = max(w_required, width);
	}

	//do center
	int x_icon = width/2 - w_required/2 /*+ fr_thickness + append_x_offset*/;
	int w_icon = 0;
	if (cc_btn_icon_obj){
		x_icon += fr_thickness + append_x_offset;
		cc_btn_icon_obj->setXPos(x_icon);
		w_icon = cc_btn_icon_obj->getWidth();
	}
	if (cc_btn_capt_obj){
		cc_btn_capt_obj->setXPos(x_icon + w_icon + append_x_offset);
		cc_btn_capt_obj->setWidth(width - cc_btn_capt_obj->getXPos());
	}
}

void CComponentsButton::setCaption(const std::string& text)
{
	cc_btn_capt = text;
	initCCBtnItems();
}

void CComponentsButton::setCaption(const neutrino_locale_t locale_text)
{
	cc_btn_capt_locale = locale_text;
	setCaption(g_Locale->getText(cc_btn_capt_locale));
}

void CComponentsButton::initCCBtnItems()
{
	initIcon();

	initCaption();
}


void CComponentsButton::paint(bool do_save_bg)
{
	//prepare items before paint
	initCCBtnItems();

	//paint form contents
	paintForm(do_save_bg);
}

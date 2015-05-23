/*
	ShairPlay Setup

	(c) 2013 by martii


	License: GPL

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#define __USE_FILE_OFFSET64 1
#include "filebrowser.h"
#include <stdio.h>
#include <global.h>
#include <libgen.h>
#include <neutrino.h>
#include <gui/widget/keyboard_input.h>
#include <driver/screen_max.h>
#include <driver/shairplay.h>
#include "mymenu.h"
#include "shairplay_setup.h"

CShairPlaySetup::CShairPlaySetup()
{
	width = 40;
	selected = -1;
}


int CShairPlaySetup::exec(CMenuTarget* parent, const std::string & /*actionKey*/)
{
	int res = menu_return::RETURN_REPAINT;

	if (parent)
		parent->hide();

	Show();

	return res;
}

void CShairPlaySetup::Show()
{
	int shortcut = 1;
	bool shairplay_enabled_old = g_settings.shairplay_enabled;
	int shairplay_port_old = g_settings.shairplay_port;
	int shairplay_bufsize_old = g_settings.shairplay_bufsize;
	std::string shairplay_apname_old = g_settings.shairplay_apname;
	std::string shairplay_password_old = g_settings.shairplay_password;

	CKeyboardInput si_apname(LOCALE_SHAIRPLAY_APNAME, &g_settings.shairplay_apname);
	CKeyboardInput si_password(LOCALE_SHAIRPLAY_PASSWORD, &g_settings.shairplay_password);

	CMenuWidget m(LOCALE_SHAIRPLAY_HEAD, NEUTRINO_ICON_AUDIO, width);
	m.addIntroItems(NONEXISTANT_LOCALE);
	m.setSelected(selected);
	m.addItem(new CMenuOptionChooser(LOCALE_SHAIRPLAY_ENABLE, &g_settings.shairplay_enabled,
				OPTIONS_OFF0_ON1_OPTIONS, OPTIONS_OFF0_ON1_OPTION_COUNT, true, NULL,
				CRCInput::convertDigitToKey(shortcut++)));
	m.addItem(new CMenuForwarder(LOCALE_SHAIRPLAY_APNAME, true, g_settings.shairplay_apname, &si_apname));
	m.addItem(new CMenuForwarder(LOCALE_SHAIRPLAY_PASSWORD, true, g_settings.shairplay_password, &si_password));
	m.addItem(new CMenuOptionNumberChooser(LOCALE_SHAIRPLAY_PORT, &g_settings.shairplay_port, true, 1024, 65535));
	m.addItem(new CMenuOptionNumberChooser(LOCALE_SHAIRPLAY_BUFSIZE, &g_settings.shairplay_bufsize, true, 10, 999));
	m.exec(NULL, "");
	m.hide();

	if (g_settings.shairplay_enabled != shairplay_enabled_old
	 || shairplay_port_old != g_settings.shairplay_port
	 || shairplay_bufsize_old != g_settings.shairplay_bufsize
	 || shairplay_apname_old != g_settings.shairplay_apname
	 || shairplay_password_old != g_settings.shairplay_password) {
			delete CNeutrinoApp::getInstance()->shairPlay;
			CNeutrinoApp::getInstance()->shairPlay = NULL;
			shairplay_enabled_old = false;
	}

	if (shairplay_enabled_old != g_settings.shairplay_enabled) {
		CNeutrinoApp::getInstance()->shairplay_enabled_cur = g_settings.shairplay_enabled;
		if (g_settings.shairplay_enabled)
				CNeutrinoApp::getInstance()->shairPlay =
					new CShairPlay(&CNeutrinoApp::getInstance()->shairplay_enabled_cur,
								   &CNeutrinoApp::getInstance()->shairplay_active);
		else {
			delete CNeutrinoApp::getInstance()->shairPlay;
			CNeutrinoApp::getInstance()->shairPlay = NULL;
		}
	}
}
// vim:ts=4

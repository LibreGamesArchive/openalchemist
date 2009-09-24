/********************************************************************
                          OpenAlchemist

  File : Application.cpp
  Description : implements the begining of the software
  License : GNU General Public License 2 or +
  Author : Guillaume Delhumeau <guillaume.delhumeau@gmail.com>


*********************************************************************/

#include "memory.h"
#include <ClanLib/core.h>
#include <ClanLib/application.h>
#include <ClanLib/display.h>
#include <ClanLib/gl1.h>
#include <ClanLib/gl.h>
#include <ClanLib/gdi.h>
#include <ClanLib/sdl.h>

#include "Preferences.h"
#include "GameEngine.h"
#include "CommonResources.h"
#include "misc.h"
#include "AudioManager.h"

/**
 * Main application
 */
class Application
{
public:

  /**
   * Check command lines arguments
   */
  static bool check_args(const std::vector<CL_String> &args)
  {
    bool run_game = true;
    Preferences *pref = pref_get_instance();

    // Checking parameters
    for(unsigned int i = 0; i < args.size(); ++i)
    {
			// Render target
			if((args[i] =="--sdl") && pref->render_target != Preferences::SDL)
			{
				pref -> render_target = Preferences::SDL;
				pref -> write();

			}
			if((args[i] =="--gdi") && pref->render_target != Preferences::GDI)
			{
				pref -> render_target = Preferences::GDI;
				pref -> write();

			}
			if((args[i] =="--opengl" || args[i] =="--opengl1") && pref->render_target != Preferences::OPENGL_1)
			{
				pref -> render_target = Preferences::OPENGL_1;
				pref -> write();

			}
			if((args[i] =="--opengl2") && pref->render_target != Preferences::OPENGL_2)
			{
				pref -> render_target = Preferences::OPENGL_2;
				pref -> write();

			}

			// Help
			if((args[i] =="--help"))
			{
				help();
				run_game = false;
			}

			// License
			if((args[i] =="--license"))
			{
				license();
				run_game = false;
			}

			// FPS limit
			if((args[i] =="--maxfps") && i < args.size()-1)
			{
				int maxfps = CL_StringHelp::text_to_int(args[i+1]);
				if(maxfps)
				{
					pref -> maxfps = maxfps;
				}
				else
				{
					run_game = false;
					std::cout << "Option maxfps must be followed by a number." << std::endl;
				}

			}

			// Colorblind mode
			if((args[i] =="--cb"))
			{
				pref -> colorblind = true;
				pref -> write();

			}
			if((args[i] =="--nocb"))
			{
				pref -> colorblind = false;
				pref -> write();

			}

    }

    return run_game;
  }

  /**
   * Application main function
   */
  static int main(const std::vector<CL_String> &args)
  {
		CL_SetupCore setup_core;
		init_memory();
		common_resources_init();
		g_audio_manager.init();		
    Preferences *pref = pref_get_instance();
    bool run_game = check_args(args);

    if(run_game)
    {
			try
			{
				CL_SetupDisplay setup_display;

				CL_SetupGL target_GL2;
				CL_SetupGL1 target_GL1;
				CL_SetupSDL target_SDL;
				CL_SetupGDI target_GDI;

				switch (pref->render_target)
				{
				case (Preferences::OPENGL_1):
					target_GL1.set_current();
					break;

				case (Preferences::OPENGL_2):
					target_GL2.set_current();
					break;

				case (Preferences::GDI):
					target_GDI.set_current();
					break;

				case (Preferences::SDL):
					target_SDL.set_current();
					break;
				}

				GameEngine game;
				game.init();
				game.run();
			}
			// Catch any errors from ClanLib
			catch(CL_Exception& exception)
			{
				CL_ConsoleWindow console("Console", 80, 160);
				CL_Console::write_line("Exception caught:");
				CL_Console::write_line(exception.message);

				// Display the stack trace (if available)
				std::vector<CL_String> stacktrace = exception.get_stack_trace();
				int size = stacktrace.size();
				if (size > 0)
				{
					CL_Console::write_line("Stack Trace:");
					for (int cnt=0; cnt < size; cnt++)
					{
						CL_Console::write_line(stacktrace[cnt]);
					}
				}
				console.display_close_message();
				return -1;
			}
			
		}
		common_resources_term();		
		g_audio_manager.term();
		my_delete(pref_get_instance());
		term_memory();
    return 0;
  }

  /**
   * Display the help informations
   */
  void static help()
    {
      std::cout << "OpenAlchemist " << get_version() << " -- Help" << std::endl
				<< "Usage: openalchemist [OPTIONS]"  << std::endl  << std::endl
				<< "Options:"  << std::endl
				<< "\t--cb       : Active colorblind mode"  << std::endl
				<< "\t--help     : Show this message"  << std::endl
				<< "\t--license  : Show the license of this program"  << std::endl
				<< "\t--maxfps X : Limit framerate to X"  << std::endl
				<< "\t--nocb     : Disable colorblind mode"  << std::endl
				<< "\t--nowide   : Disable 16:9 wide screen mode"  << std::endl
				<< "\t--gdi      : Use GDI as render target (default)" << std::endl
				            << "\t--opengl   : Use OpenGL as render target" << std::endl
				            << "\t--opengl2  : Use OpenGL 2 as render target" << std::endl
				<< "\t--sdl      : Use SDL as render target" << std::endl
				<< "\t--wide     : Special mode for 16:9 wide screen (only supported with OpenGL)" << std::endl;				
    }

  /**
   * Display the license
   */
  void static license()
    {
      std::cout << " * OpenAlchemist "<< get_version() << " -- License\n" << std::endl
				<< " * Copyright (C) 2006,2009 Guillaume Delhumeau <guillaume.delhumeau at gmail.com>" << std::endl
				<< " *"  << std::endl
				<< " * This program is free software; you can redistribute it and/or modify " << std::endl
				<< " * it under the terms of the GNU General Public License as published by" << std::endl
				<< " * the Free Software Foundation; either version 2 of the License, or" << std::endl
				<< " * (at your option) any later version." << std::endl
				<< " *" << std::endl
				<< " * This program is distributed in the hope that it will be useful," << std::endl
				<< " * but WITHOUT ANY WARRANTY; without even the implied warranty of" << std::endl
				<< " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the" << std::endl
				<< " * GNU General Public License for more details." << std::endl
				<< " *" << std::endl
				<< " * You should have received a copy of the GNU General Public License" << std::endl
				<< " * along with this program; if not, write to the Free Software" << std::endl
				<< " * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA." <<std::endl
				<< " *" << std::endl << std::endl;
    }

};

CL_ClanApplication app(&Application::main);

/* main.cpp: parses options, runs slop, prints results.
 *
 * Copyright (C) 2014: Dalton Nell, Slop Contributors (https://github.com/naelstrof/slop/graphs/contributors).
 *
 * This file is part of Slop.
 *
 * Slop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Slop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Slop.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "cxxopts.hpp"
#include "slop.hpp"

using namespace slop;

template<typename Out>
static void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

static std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

glm::vec4 parseColor( std::string value ) {
    std::string valuecopy = value;
    glm::vec4 found;
    std::string::size_type sz;
    try {
        found[0] = std::stof(value,&sz);
        value = value.substr(sz+1);
        found[1] = std::stof(value,&sz);
        value = value.substr(sz+1);
        found[2] = std::stof(value,&sz);
        if ( value.size() != sz ) {
            value = value.substr(sz+1);
            found[3] = std::stof(value,&sz);
            if ( value.size() != sz ) {
                throw "dur";
            }
        } else {
            found[3] = 1;
        }
    } catch ( ... ) {
        throw new std::invalid_argument("Unable to parse value `" + valuecopy + "` as a color. Should be in the format r,g,b or r,g,b,a. Like 1,1,1,1.");
    }
    return found;
}

SlopOptions* getOptions( cxxopts::Options& options ) {
    slop::SlopOptions* foo = new slop::SlopOptions();
    if ( options.count( "bordersize" ) > 0 ) {
        foo->borderSize = options["bordersize"].as<float>();
    }
    if ( options.count( "padding" ) > 0 ) {
        foo->padding = options["padding"].as<float>();
    }
    if ( options.count( "tolerance" ) > 0 ) {
        foo->tolerance = options["tolerance"].as<float>();
    }
    glm::vec4 color = glm::vec4( foo->r, foo->g, foo->b, foo->a );
    if ( options.count( "color" ) > 0 ) {
        color = parseColor( options["color"].as<std::string>() );
    }
    foo->r = color.r;
    foo->g = color.g;
    foo->b = color.b;
    foo->a = color.a;
    if ( options.count( "nokeyboard" ) > 0 ) {
        foo->nokeyboard = options["nokeyboard"].as<bool>();
    }
    if ( options.count( "xdisplay" ) > 0 ) {
        foo->xdisplay = options["xdisplay"].as<std::string>();
    }
    std::string shaders = "textured";
    if ( options.count( "shader" ) > 0 ) {
        shaders = options["shader"].as<std::string>();
    }
    foo->shaders = split( shaders, ',' );
    if ( options.count( "noopengl" ) > 0 ) {
        foo->noopengl = options["noopengl"].as<bool>();
    }
    if ( options.count( "highlight" ) > 0 ) {
        foo->highlight = options["highlight"].as<bool>();
    }
    if ( options.count( "nodecorations" ) > 0 ) {
        foo->nodecorations = options["nodecorations"].as<int>();
        if ( foo->nodecorations < 0 || foo->nodecorations > 2 ) {
            throw new std::invalid_argument( "--nodecorations must be between 0 and 2. Or be used as a flag." );
        }
    }
    return foo;
}

std::string formatOutput( std::string input, SlopSelection selection, bool cancelled ) {
    std::stringstream output;
    for( unsigned int i=0;i<input.length();i++) {
        if ( input[i] == '%' ) {
            if ( input.length() <= i+1 ) {
                throw new std::invalid_argument( "Expected character after `%`, got END." );
            }
            switch( input[i+1] ) {
                case 'x':
                case 'X': output << round(selection.x); break;
                case 'y':
                case 'Y': output << round(selection.y); break;
                case 'w':
                case 'W': output << round(selection.w); break;
                case 'c':
                case 'C': output << cancelled; break;
                case 'h':
                case 'H': output << round(selection.h); break;
                case 'g':
                case 'G': output << round(selection.w) << "x" << round(selection.h)
                          << "+" << round(selection.x) << "+" << round(selection.y); break;
                case 'i':
                case 'I': output << selection.id; break;
                case '%': output << "%"; break;
                default: throw new std::invalid_argument( std::string()+"Expected x, y, w, h, g, i, c, or % after % in format. Got `" + input[i+1] + "`." );
             }
            i++;
            continue;
        }
        output << input[i];
    }
    return output.str();
}

void printHelp() {
    std::cout << "slop v5.3.21\n";
    std::cout << "\n";
    std::cout << "Copyright (C) 2017 Dalton Nell, Slop Contributors\n";
    std::cout << "(https://github.com/naelstrof/slop/graphs/contributors)\n";
    std::cout << "Usage: slop [options]\n";
    std::cout << "\n";
    std::cout << "slop (Select Operation) is an application that queries for a selection from the\n";
    std::cout << "user and prints the region to stdout.\n";
    std::cout << "\n";
    std::cout << "-h, --help                    Print help and exit\n";
    std::cout << "-v, --version                 Print version and exit\n";
    std::cout << "Options\n";
    std::cout << "  -x, --xdisplay=hostname:number.screen_number\n";
    std::cout << "                                Sets the x display.\n";
	std::cout << "  -k, --nokeyboard              Disables the ability to cancel selections with\n";
	std::cout << "                                  the keyboard.  (default=off)\n";
	std::cout << "  -b, --bordersize=FLOAT        Set the selection rectangle's thickness.\n";
	std::cout << "                                  (default=`1')\n";
	std::cout << "  -p, --padding=FLOAT           Set the padding size of the selection. Can be\n";
	std::cout << "                                  negative.  (default=`0')\n";
	std::cout << "  -t, --tolerance=FLOAT         How far in pixels the mouse can move after\n";
	std::cout << "                                  clicking and still be detected as a normal\n";
	std::cout << "                                  click instead of a click and drag. Setting\n";
	std::cout << "                                  this to 0 will disable window selections.\n";
	std::cout << "                                  Alternatively setting it to 999999 would.\n";
	std::cout << "                                  only allow for window selections.\n";
	std::cout << "                                  (default=`2')\n";
	std::cout << "  -c, --color=FLOAT,FLOAT,FLOAT,FLOAT\n";
	std::cout << "                                Set the selection rectangle's color. Supports\n";
	std::cout << "                                  RGB or RGBA values.\n";
	std::cout << "                                  (default=`0.5,0.5,0.5,1')\n";
	std::cout << "  -n, --nodecorations=INT       Attempt to select child windows in order to\n";
	std::cout << "                                  avoid window decorations. Setting this to\n";
    std::cout << "                                  1 will enable a light attempt to\n";
    std::cout << "                                  remove decorations. Setting this to 2 will\n";
    std::cout << "                                  enable aggressive decoration removal.\n";
    std::cout << "                                  Supplying slop with just `-n` is\n";
    std::cout << "                                  equivalent to supplying `-n1`.\n";
    std::cout << "                                  (default=`0')\n";
	std::cout << "  -q, --quiet                   Disable any unnecessary cerr output. Any\n";
	std::cout << "                                  warnings simply won't print.\n";
	std::cout << "  -l, --highlight               Instead of outlining selections, slop\n";
	std::cout << "                                  highlights it. This is only useful when\n";
	std::cout << "                                  --color is set to a transparent color.\n";
	std::cout << "                                  (default=off)\n";
	std::cout << "  -r, --shader=STRING           Sets the shader to load and use from\n";
	std::cout << "                                  ~/.config/slop/\n";
	std::cout << "  -f, --format=STRING           Set the output format string. Format specifiers\n";
	std::cout << "                                  are %x, %y, %w, %h, %i, %g, and %c.\n";
	std::cout << "                                  (default=`%g\n')\n";
	std::cout << "  -o, --noopengl                Disable graphics acceleration.\n";
	std::cout << "Examples\n";
	std::cout << "    $ # Gray, thick, transparent border for maximum visiblity.\n";
	std::cout << "    $ slop -b 20 -c 0.5,0.5,0.5,0.8\n";
	std::cout << "\n";
	std::cout << "    $ # Remove window decorations.\n";
	std::cout << "    $ slop --nodecorations\n";
	std::cout << "\n";
	std::cout << "    $ # Disable window selections. Useful for selecting individual pixels.\n";
	std::cout << "    $ slop -t 0\n";
	std::cout << "\n";
	std::cout << "    $ # Classic Windows XP selection.\n";
	std::cout << "    $ slop -l -c 0.3,0.4,0.6,0.4\n";
	std::cout << "\n";
	std::cout << "    $ # Read slop output for use in scripts.\n";
	std::cout << "    $ read -r X Y W H G ID < <(slop -f '%x %y %w %h %g %i')\n";
	std::cout << "\n";
	std::cout << "Tips\n";
	std::cout << "    * If you don't like a selection: you can cancel it by right-clicking\n";
	std::cout << "regardless of which options are enabled or disabled for slop.\n";
	std::cout << "    * If slop doesn't seem to select a window accurately, the problem could be\n";
	std::cout << "because of decorations getting in the way. Try enabling the --nodecorations\n";
	std::cout << "flag.\n";
}

int app( int argc, char** argv ) {
    cxxopts::Options options("maim", "Screenshot application.");
    options.add_options()
    ("h,help", "Print help and exit.")
    ("v,version", "Print version and exit.")
    ("x,xdisplay", "Sets the xdisplay to use", cxxopts::value<std::string>())
    ("f,format", "Sets the output format for slop. Format specifiers are  %x, %y, %w, %h, %i, %c, and %g. If actual percentage signs are desired in output, use a double percentage sign like so `%%`.", cxxopts::value<std::string>())
    ("b,bordersize", "Sets the selection rectangle's thickness.", cxxopts::value<float>())
    ("p,padding", "Sets the padding size for the selection, this can be negative.", cxxopts::value<float>())
    ("t,tolerance", "How far in pixels the mouse can move after clicking, and still be detected as a normal click instead of a click-and-drag. Setting this to 0 will disable window selections. Alternatively setting it to 9999999 would force a window selection.", cxxopts::value<float>())
    ("c,color", "Sets  the  selection  rectangle's  color.  Supports  RGB or RGBA input. Depending on the system's window manager/OpenGL  support, the opacity may be ignored.", cxxopts::value<std::string>())
    ("r,shader", "This  sets  the  vertex shader, and fragment shader combo to use when drawing the final framebuffer to the screen. This obviously only  works  when OpenGL is enabled. The shaders are loaded from ~/.config/maim. See https://github.com/naelstrof/slop for more information on how to create your own shaders.", cxxopts::value<std::string>())
    ("n,nodecorations", "Sets the level of aggressiveness when trying to remove window decroations. `0' is off, `1' will try lightly to remove decorations, and `2' will recursively descend into the root tree until it gets the deepest available visible child under the mouse. Defaults to `0'.", cxxopts::value<int>()->implicit_value("1"))
    ("l,highlight", "Instead of outlining a selection, maim will highlight it instead. This is particularly useful if the color is set to an opacity lower than 1.")
    ("q,quiet", "Disable any unnecessary cerr output. Any warnings or info simply won't print.")
    ("k,nokeyboard", "Disables the ability to cancel selections with the keyboard.")
    ("o,noopengl", "Disables graphics hardware acceleration.")
    ("positional", "Positional parameters", cxxopts::value<std::vector<std::string>>())
    ;
    options.parse_positional("positional");
    options.parse(argc, argv);
    // Options just validates all of our input from argv
    bool quiet = false;
    if ( options.count( "quiet" ) > 0 ) {
        quiet = options["quiet"].as<bool>();
    }
    auto& positional = options["positional"].as<std::vector<std::string>>();
    if ( positional.size() > 0 ) {
        throw new std::invalid_argument("Unexpected positional argument: " + positional[0]);
    }
    bool help = false;
    if ( options.count( "help" ) > 0 ) {
        help = options["help"].as<bool>();
    }
    if ( help ) {
        printHelp();
        return 0;
    }
    bool version = false;
    if ( options.count( "version" ) > 0 ) {
        version = options["version"].as<bool>();
    }
    if ( version ) {
        std::cout << SLOP_VERSION << "\n";
        return 0;
    }
    // We then parse the options into something slop can understand.
    SlopOptions* parsedOptions = getOptions( options );

    // We want to validate our format option if we got one, we do that by just doing a dry run
    // on a fake selection.
    SlopSelection selection(0,0,0,0,0);
    std::string format;
    bool gotFormat = options.count( "format" ) > 0;
    if ( gotFormat ) {
        format = options["format"].as<std::string>();
        formatOutput( format, selection, false );
    }

    // Finally we do the real selection.
    bool cancelled = false;
    selection = SlopSelect(parsedOptions, &cancelled, quiet);
    
    // Here we're done with the parsed option data.
    delete parsedOptions;
    // We know if we cancelled or not
    if ( cancelled ) {
        if ( !quiet ) {
            std::cerr << "Selection was cancelled by keystroke or right-click.\n";
        }
        return 1;
    }
    // If we recieved a format option, we output the specified output.
    if ( gotFormat ) {
        std::cout << formatOutput( format, selection, cancelled );
        return 0;
    }
    std::cout << formatOutput( "%g\n", selection, cancelled );
    return 0;
}

int main( int argc, char** argv ) {
    try {
        return app( argc, argv );
    } catch( std::exception* e ) {
        std::cerr << "Slop encountered an error:\n" << e->what() << "\n";
        return 1;
    } // let the operating system handle any other kind of exception.
    return 1;
}

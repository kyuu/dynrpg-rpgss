# RPGSS

The Role Playing Game Scripting System ([RPGSS][1]) is a plugin for Cherry's
[DynRPG][2]. RPGSS is a scripting environment for the [RPG Maker][3] 2003,
based on the programming language [Lua][4]. It's most comparable to the RGSS
(Ruby Game Scripting System), the scripting environment for the [RPG Maker][3] XP.

RPGSS offers the following features:

- [MIT Licensed][5]
- Lua instead of C++. Lua is very easy to pick up, dynamic, powerful, extendable,
  widely used (even in commercial video games) and very fast ([LuaJIT][6] compiles
  Lua code to machine code at run time).
- Rapid extension development and painless maintenance. An extension in RPGSS
  is simply a human-readable Lua source code file.
- Meaningful error messages and a detailed stack traceback, when something
  goes wrong, assist the developer.
- Most of the powerful C++ DynRPG API available to Lua scripts.
- Rich set of powerful standard libraries. RPGSS provides a lot of essential
  libraries out of the box with an easy to pick up API.
- Fast and powerful graphics routines. Unfortunately the graphics routines
  available to a DynRPG plugin developer are slow and simplistic. RPGSS
  provides a rich set of blazing fast graphics routines that open up new
  possibilities.

Please read the [RPGSS documentation][7] for more details.

## Official Repository

RPGSS is published under the terms of the [MIT License][5] and is being
developed by [Anatoli Steinmark][8].

For questions, comments, or bug reports feel free to open a Github issue
or contact Anatoli Steinmark directly at the email address indicated below.

Copyright 2014, [Anatoli Steinmark][8] (<[solid.snaq@gmail.com][9]>)<br>

Older versions of RPGSS up to and including 0.5.2 are distributed under the
GPL v3 License. See the corresponding license file in those versions for more
details.

[1]:  https://github.com/kyuu/dynrpg-rpgss "RPGSS"
[2]:  http://rpg-maker.cherrytree.at/dynrpg "DynRPG"
[3]:  http://en.wikipedia.org/wiki/RPG_Maker "RPG Maker"
[4]:  http://lua.org "The Lua Programming Language"
[5]:  http://www.opensource.org/licenses/mit-license.html "The MIT License"
[6]:  http://luajit.org/ "The LuaJIT Probject"
[7]:  https://github.com/kyuu/dynrpg-rpgss/tree/master/docs "RPGSS documentation"
[8]:  https://github.com/kyuu "Anatoli Steinmark's Github"
[9]:  mailto:solid.snaq@gmail.com "Anatoli Steinmark (Email)"

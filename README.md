# Project Carrot Level Converter

An utility program to convert a Jazz Jackrabbit 2 level (`.j2l`) into
the format recognized by [Project Carrot](https://github.com/soulweaver91/project-carrot).
Events and other features that are currently not supported in Project Carrot are ignored,
as the event mapping in Project Carrot is different, but not yet finalized.
Expect new versions to be released whenever new features are added to Project Carrot.

You can download prebuilt Windows binaries over at the 
[Project Carrot homepage](https://carrot.soulweaver.fi/).

## Usage

```
pclc name path\to\Level.j2l
```

You can pick any name you want; the level will be built into a folder by that name.
The same name is used by other levels in their next and secret level data.

## Development

Compiling requires [Qt 5.x](http://www.qt.io/download/) and Microsoft Visual Studio 2015.
Configure the Qt settings with the MSVS Qt 5 plugin to be able to compile it.

MSVS is not strictly required as long as you know how to set up a build
system manually, though.

## License
This software is licensed under the [MIT License](https://opensource.org/licenses/MIT).
See the included `LICENSE` file for the licenses of the third-party libraries used.

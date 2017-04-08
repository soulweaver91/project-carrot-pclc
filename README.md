# Project Carrot Level Converter

An utility program to convert a Jazz Jackrabbit 2 level (`.j2l`) into
the format recognized by [Project Carrot](https://github.com/soulweaver91/project-carrot).
Most events and other features that are currently not supported in Project Carrot are ignored,
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
If the name is omitted, the filename will be used as the name.

## Development

Compiling requires [Qt 5.x](http://www.qt.io/download/).

### Windows

To build with Visual Studio 2015, configure the Qt settings with the MSVS Qt 5 plugin first.

MSVS is not strictly required for building, but you're on your own if you prefer using some other
compiler for building on Windows.


### Linux

First set up the environment as instructed in the repository for
[Project Carrot](https://github.com/soulweaver91/project-carrot), clone this project
to a folder, navigate to it, and:

```shell
qmake -spec linux-clang
make release
```

This will build the executable to `Release/PCLevelConverter`.

### macOS

Follow the Linux instructions, but use `macx-clang` as the spec instead.

## License
This software is licensed under the [MIT License](https://opensource.org/licenses/MIT).
See the included `LICENSE` file for the licenses of the third-party libraries used.

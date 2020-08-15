<!-- PROJECT SHIELDS -->
<!-- [![Contributors][contributors-shield]]() -->

<!-- PROJECT LOGO -->
<p align="center">
  <a href="https://github.com/Plutoberth/SonyHeadphonesClient">
    <!-- img src="" alt="Logo" width="80" height="80"-->
  </a>

  <h1 align="center">Sony Headphones Client</h1>

  <p align="center">
    This project features a PC alternative for the mobile-only Sony Headphones app.
    <br />
  </p>
</p>

![Program Showcase](https://github.com/Plutoberth/SonyHeadphonesClient/blob/master/static/showcase.gif)

<!-- TABLE OF CONTENTS -->
## Table of Contents

* [Legal Disclaimer](#legal-disclaimer)
* [Download](#download)
* [Motivation](#motivation)
* [Features](#features)
* [Supported Platforms](#supported-platforms-and-headsets)
* [For Developers](#for-developers)
* [Contact](#contact)
* [License](#license)

<!-- Legal disclaimer -->
## Legal Disclaimer

### THIS PROGRAM IS NOT AFFILIATED IN ANY WAY, SHAPE, OR FORM WITH THE SONY CORPORATION. 
### YOU ARE RESPONSIBLE FOR ANY DAMAGE THAT MAY OCCUR WHILE USING THIS PROGRAM.

## Download

You can download compiled versions of the client from the [releases page](https://github.com/Plutoberth/SonyHeadphonesClient/releases).

## Motivation

I recently bought the WH-1000-XM3s, and I was annoyed by the fact that I couldn't change their settings while using my PC. 
So I reverse-engineered the application (for intercompatibility purposes, of course), defined the protocol, and created with an alternative application with [Mr-M33533K5](https://github.com/Mr-M33533K5]).

## Features

- [x] Ambient Sound Control
- [ ] Display battery life and fetch existing settings from device
- [ ] Equalizer

#### If you'd like to implement the equalizer (I can implement the GUI, you'll need to do the rest), please contact me privately via Twitter at `@Plutoberth`. 

### TODO: ADD SCREENSHOTS

## Supported Platforms And Headsets

For now, only the WH-1000-XM3 is supported. Other headsets may work, but I haven't tested them.

It shouldn't be too much work to add support for a new platform. Open a PR if you'd like to do so. See [For Developers - Adding a new platform](#adding-a-new-platform).

- [x] Windows
- [ ] Linux
- [ ] OSX


## For Developers

`git clone --recurse-submodules https://github.com/Plutoberth/SonyHeadphonesClient.git`

### Compiling

#### Windows

Use the provided solution file.

### Adding a new platform

There are two platform dependent parts in the code - the GUI and Bluetooth communication. 

For the GUI, you need to copy the relevant parts from ImGui. 

For Bluetooth, you need to implement `IBluetoothConnector` for your desired platform.

## Contact

I prefer to get contacted via issues on this repository. If you have a private question, you may contact me via Twitter at `@Plutoberth`.

<!-- LICENSE -->
## License

Distributed under the [MIT License](https://github.com/Plutoberth/SonyHeadphonesClient/blob/master/LICENSE). See LICENSE for more information.

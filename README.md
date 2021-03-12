<p class="aligncenter">
  <a href="https://github.com/Plutoberth/SonyHeadphonesClient">
    <!-- img src="" alt="Logo" width="80" height="80"-->
  </a>

  <h1 align="center">Sony Headphones Client</h1>

  This project features a PC alternative for the mobile-only Sony Headphones app.
  <br/><br/>
  <img width="650" src="static/showcase.gif" alt="Program Showcase"><p> <br/>
  <a href="https://paypal.me/plutoberth"><img width="110" src="static/badge.svg" alt="Donate" class="center"/></a>
  <br/>
</p>

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

### THIS PROGRAM IS NOT AFFILIATED WITH SONY. YOU ARE RESPONSIBLE FOR ANY DAMAGE THAT MAY OCCUR WHILE USING THIS PROGRAM.

## Download

You can download compiled versions of the client from the [releases page](https://github.com/Plutoberth/SonyHeadphonesClient/releases).

**Note:** If you're getting an error like `VCRUNTIME140_1.dll was not found`, you need to install the `Microsoft VC++ Redistributable`.

## Motivation

I recently bought the WH-1000-XM3s, and I was annoyed by the fact that I couldn't change their settings while using my PC.
So I reverse-engineered the application (for intercompatibility purposes, of course), defined the protocol, and created with an alternative application with [Mr-M33533K5](https://github.com/Mr-M33533K5).

## Features

- [x] Ambient Sound Control
- [ ] Disabling noise cancelling
- [ ] Display battery life and fetch existing settings from device
- [ ] Equalizer

### TODO: ADD SCREENSHOTS

## Supported Platforms And Headsets

For now, only the WH-1000-XM3 is supported. Other headsets may work, but I haven't tested them. 

#### **Please report about your experiences using other Sony headset in the [Headset Reports](https://github.com/Plutoberth/SonyHeadphonesClient/issues/29) issue.**

- [x] Windows
- [x] Linux
- [x] macOS
- [ ] ~~TempleOS~~

## For Developers

`git clone --recurse-submodules https://github.com/Plutoberth/SonyHeadphonesClient.git`

### Compiling

#### Windows

Use the provided solution file.

#### Linux

First install the dependencies:

```bash
# For Debian based systems
sudo apt install libbluetooth-dev libglew-dev libglfw3-dev libdbus-1-dev
```

Then navigate to `Client/linux` and `make`.

#### macOS

Use the provided xcodeproj file.

### Adding a new platform

**This is no longer relevant, we support all major platforms :)**

There are two platform dependent parts in the code - the GUI and Bluetooth communication.

For the GUI, you need to copy the relevant parts from Dear ImGui.

For Bluetooth, you need to implement `IBluetoothConnector` for your desired platform.

<!-- LICENSE -->
## License

Distributed under the [MIT License](https://github.com/Plutoberth/SonyHeadphonesClient/blob/master/LICENSE). See LICENSE for more information.

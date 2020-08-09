<!-- PROJECT SHIELDS -->
<!-- [![Contributors][contributors-shield]]() -->

<!-- PROJECT LOGO -->
<br />
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



<!-- TABLE OF CONTENTS -->
## Table of Contents

* [Legal Disclaimer](#legal-disclaimer)
* [Motivation](#motivation)
* [Usage](#usage)
* [Supported Platforms](#supported-platforms)
* [License](#license)
* [For Developers](#for-developers)


<!-- Legal disclaimer -->
## Legal Disclaimer

### THIS PROGRAM IS NOT AFFILIATED IN ANY WAY, SHAPE, OR FORM WITH THE SONY CORPORATION. 
### YOU ARE RESPONSIBLE FOR ANY DAMAGE THAT MAY OCCUR WHILE USING THIS PROGRAM.

## Motivation

I recently bought the WH-1000-XM3s, and I was frustrated that I couldn't change their settings while using my PC. 
So I reverse-engineered the application (for intercompatibility purposes, of course), and created with an alternative application with [Mr-M33533K5](https://github.com/Mr-M33533K5]).

## Usage

### TODO: FILL IN

## Supported Platforms & Headsets

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

<!-- LICENSE -->
## License

Distributed under the [MIT License](https://github.com/Plutoberth/SonyHeadphonesClient/blob/master/LICENSE). See LICENSE for more information.

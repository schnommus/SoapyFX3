# SoapyFX3

WIP attempt to get the Cypress FX3 USB3 development kit to work as an SDR using SoapySDR.

## Prerequisites

Install SoapySDR from source.

## Building

    mkdir build
    cd build
    cmake ..
    make

## Testing

    sudo make install
    SoapySDRUtil --info # See if the driver module loaded
    SoapySDRUtil --find # Probes for devices

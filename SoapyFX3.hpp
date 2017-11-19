#pragma once

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Registry.hpp>
#include <SoapySDR/Formats.hpp>

#include <libusb-1.0/libusb.h>

#include <string>
#include <iomanip>

#include "SoapyFX3Util.hpp"

/***********************************************************************
 * Device interface
 **********************************************************************/
class SoapyFX3 : public SoapySDR::Device
{
public:
    SoapyFX3( const SoapySDR::Kwargs & args );

    ~SoapyFX3( void );

    /*******************************************************************
     * Identification API
     ******************************************************************/

    std::string getDriverKey( void ) const;


    std::string getHardwareKey( void ) const;


    /*******************************************************************
     * Channels API
     ******************************************************************/

    size_t getNumChannels( const int ) const;


    bool getFullDuplex( const int direction, const size_t channel ) const;


    /*******************************************************************
     * Stream API
     ******************************************************************/

    std::vector<std::string> getStreamFormats(const int direction, const size_t channel) const;

    std::string getNativeStreamFormat(const int direction, const size_t channel, double &fullScale) const;

    SoapySDR::Stream *setupStream(
            const int direction,
            const std::string &format,
            const std::vector<size_t> &channels = std::vector<size_t>(),
            const SoapySDR::Kwargs &args = SoapySDR::Kwargs() );


    void closeStream( SoapySDR::Stream *stream );


    size_t getStreamMTU( SoapySDR::Stream *stream ) const;


    int activateStream(
            SoapySDR::Stream *stream,
            const int flags = 0,
            const long long timeNs = 0,
            const size_t numElems = 0 );


    int deactivateStream(
            SoapySDR::Stream *stream,
            const int flags = 0,
            const long long timeNs = 0 );


    int readStream(
            SoapySDR::Stream *stream,
            void * const *buffs,
            const size_t numElems,
            int &flags,
            long long &timeNs,
            const long timeoutUs = 100000 );


    /*******************************************************************
     * Gain API
     ******************************************************************/

    std::vector<std::string> listGains( const int direction, const size_t channel ) const;


    void setGain( const int direction, const size_t channel, const std::string &name, const double value );

    SoapySDR::Range getGainRange( const int direction, const size_t channel, const std::string &name ) const;


    /*******************************************************************
     * Frequency API
     ******************************************************************/

    void setFrequency( const int direction, const size_t channel, const std::string &name, const double frequency, const SoapySDR::Kwargs &args = SoapySDR::Kwargs() );


    std::vector<std::string> listFrequencies( const int direction, const size_t channel ) const;


    SoapySDR::RangeList getFrequencyRange( const int direction, const size_t channel, const std::string &name ) const;


    /*******************************************************************
     * Sample Rate API
     ******************************************************************/

    void setSampleRate( const int direction, const size_t channel, const double rate );


    std::vector<double> listSampleRates( const int direction, const size_t channel ) const;

private:
    SoapySDR::Kwargs _creation_args;
    struct libusb_device_descriptor _libusb_desc;
    libusb_device_handle *_libusb_handle;
};

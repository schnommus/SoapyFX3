#include "SoapyFX3.hpp"

static const uint32_t fx3_vid = 0x04B4;
static const uint32_t fx3_pid = 0x00F1;

/***********************************************************************
 * Find available devices
 **********************************************************************/

SoapySDR::KwargsList findSoapyFX3(const SoapySDR::Kwargs &args)
{
    //locate the device on the system...
    //return a list of 0, 1, or more argument maps that each identify a device

    SoapySDR::KwargsList results;

    libusb_init(NULL);

    // Get list of USB devices currently connected
    libusb_device **device_list;
    size_t count = libusb_get_device_list(NULL, &device_list);

    for(size_t i = 0; i < count; i++) {
        struct libusb_device_descriptor desc;

        libusb_get_device_descriptor(device_list[i], &desc);

        // Is a valid FX3 device?
        if(desc.idVendor == fx3_vid && desc.idProduct == fx3_pid) {
            SoapySDR::Kwargs device_args;

            device_args["idVendor"] = int_to_hex(desc.idVendor);
            device_args["idProduct"] = int_to_hex(desc.idProduct);
            device_args["idBus"] =
                std::to_string(libusb_get_bus_number(device_list[i]));
            device_args["idBusAddress"] =
                std::to_string(libusb_get_device_address(device_list[i]));

            results.push_back(device_args);
        }
    }

    libusb_free_device_list(device_list, 1);

    return results;
}

/***********************************************************************
 * Make device instance
 **********************************************************************/

SoapySDR::Device *makeSoapyFX3(const SoapySDR::Kwargs &args)
{
    //create an instance of the device object given the args
    //here we will translate args into something used in the constructor
    return new SoapyFX3(args);
}

/***********************************************************************
 * Registration
 **********************************************************************/

static SoapySDR::Registry registerSoapyFX3("cypress_fx3", &findSoapyFX3, &makeSoapyFX3, SOAPY_SDR_ABI_VERSION);

/*******************************************************************
 * Instance Construction
 ******************************************************************/

SoapyFX3::SoapyFX3( const SoapySDR::Kwargs & args ) {
    _creation_args = args;
}

SoapyFX3::~SoapyFX3( void ) {
}

/*******************************************************************
 * Identification API
 ******************************************************************/

std::string SoapyFX3::getDriverKey(void) const
{
    return "CypressFX3";
}

std::string SoapyFX3::getHardwareKey(void) const
{
    const std::string bus = _creation_args.at("idBus");
    const std::string addr = _creation_args.at("idBusAddress");
    return std::string("[BUS:") + bus + std::string("][ADDR:") + addr + std::string("]");
}

/*******************************************************************
 * Channels API
 ******************************************************************/

size_t SoapyFX3::getNumChannels(const int) const
{
    return 1;
}

bool SoapyFX3::getFullDuplex(const int, const size_t) const
{
    return false;
}

/*******************************************************************
 * Stream API
 ******************************************************************/

std::vector<std::string> SoapyFX3::getStreamFormats(const int, const size_t) const
{
    return std::vector<std::string>();
}

std::string SoapyFX3::getNativeStreamFormat(const int, const size_t, double &fullScale) const
{
    fullScale = double(1 << 15);
    return SOAPY_SDR_CS16;
}

SoapySDR::Stream *SoapyFX3::setupStream(const int, const std::string &, const std::vector<size_t> &, const SoapySDR::Kwargs &)
{
    return nullptr;
}

void SoapyFX3::closeStream(SoapySDR::Stream *)
{
    return;
}

size_t SoapyFX3::getStreamMTU(SoapySDR::Stream *) const
{
    //provide a non-zero default when the implementation does not overload the MTU
    return 1024;
}

int SoapyFX3::activateStream(SoapySDR::Stream *, const int flags, const long long, const size_t)
{
    return (flags == 0)? 0 : SOAPY_SDR_NOT_SUPPORTED;
}

int SoapyFX3::deactivateStream(SoapySDR::Stream *, const int flags, const long long)
{
    return (flags == 0)? 0 : SOAPY_SDR_NOT_SUPPORTED;
}

int SoapyFX3::readStream(SoapySDR::Stream *, void * const *, const size_t, int &, long long &, const long)
{
    return SOAPY_SDR_NOT_SUPPORTED;
}

/*******************************************************************
 * Gain API
 ******************************************************************/

std::vector<std::string> SoapyFX3::listGains(const int, const size_t) const
{
    return std::vector<std::string>();
}

void SoapyFX3::setGain(const int, const size_t, const std::string &, const double)
{
    return;
}

SoapySDR::Range SoapyFX3::getGainRange(const int, const size_t, const std::string &) const
{
    return SoapySDR::Range(0.0, 0.0);
}

/*******************************************************************
 * Frequency API
 ******************************************************************/

std::vector<std::string> SoapyFX3::listFrequencies(const int, const size_t) const
{
    return std::vector<std::string>();
}

void SoapyFX3::setFrequency(const int, const size_t, const std::string &, const double, const SoapySDR::Kwargs &)
{
    return;
}

SoapySDR::RangeList SoapyFX3::getFrequencyRange(const int, const size_t, const std::string &) const
{
    return SoapySDR::RangeList();
}

/*******************************************************************
 * Sample Rate API
 ******************************************************************/

std::vector<double> SoapyFX3::listSampleRates(const int, const size_t) const
{
    std::vector<double> rates;

    rates.push_back(10E6);
    rates.push_back(20E6);
    rates.push_back(30E6);

    return rates;
}

void SoapyFX3::setSampleRate(const int, const size_t, const double)
{
    return;
}

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
    _libusb_handle = NULL;

    libusb_init(NULL);

    // Get list of USB devices currently connected
    libusb_device **device_list;
    size_t count = libusb_get_device_list(NULL, &device_list);

    for(size_t i = 0; i < count; i++) {
        struct libusb_device_descriptor desc;

        libusb_get_device_descriptor(device_list[i], &desc);

        // Is a valid FX3 device?
        if(desc.idVendor == fx3_vid && desc.idProduct == fx3_pid) {
            // Does it match the device we actually want to open?
            if(_creation_args.at("idBus") ==
                    std::to_string(libusb_get_bus_number(device_list[i])) &&
               _creation_args.at("idBusAddress") ==
                    std::to_string(libusb_get_device_address(device_list[i])) ) {
                _libusb_desc = desc;
                libusb_open(device_list[i], &_libusb_handle);
                break;
            }
        }
    }

    libusb_free_device_list(device_list, 1);

    if(_libusb_handle == NULL) {
		throw std::runtime_error( "libusb failed to open Cypress FX3 device" );
    }
}

SoapyFX3::~SoapyFX3( void ) {
    libusb_close(_libusb_handle);
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

size_t SoapyFX3::getNumChannels(const int direction) const
{
    if ( direction == SOAPY_SDR_RX ) {
        return 1;
    } else {
        return 0;
    }
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
	std::vector<std::string> formats;

	formats.push_back(SOAPY_SDR_U32);

	return formats;
}

std::string SoapyFX3::getNativeStreamFormat(const int, const size_t, double &fullScale) const
{
    fullScale = double((uint32_t)0xFFFFFFFF);
    return SOAPY_SDR_U32;
}

SoapySDR::Stream *SoapyFX3::setupStream(
	const int direction,
	const std::string &format,
	const std::vector<size_t> &channels,
	const SoapySDR::Kwargs &args )
{
    return RX_STREAM;
}

void SoapyFX3::closeStream(SoapySDR::Stream *stream)
{
    return;
}

size_t SoapyFX3::getStreamMTU(SoapySDR::Stream *) const
{
    //provide a non-zero default when the implementation does not overload the MTU
    return 1024;
}

int SoapyFX3::activateStream(SoapySDR::Stream *stream,
	const int flags,
	const long long timeNs,
	const size_t numElems )
{
    return (flags == 0)? 0 : SOAPY_SDR_NOT_SUPPORTED;
}

int SoapyFX3::deactivateStream(
	SoapySDR::Stream *stream,
	const int flags,
	const long long timeNs )
{
    return (flags == 0)? 0 : SOAPY_SDR_NOT_SUPPORTED;
}

int SoapyFX3::readStream(
	SoapySDR::Stream *stream,
	void * const *buffs,
	const size_t numElems,
	int &flags,
	long long &timeNs,
	const long timeoutUs )
{

    int numBytes = numElems*4;
    int received = 0;

    if( libusb_bulk_transfer(_libusb_handle,
                             0x81,
                             ((uint8_t*)(buffs[0])),
                             numBytes,
                             &received,
                             timeoutUs/1000) == 0 ) {
        return received;
    } else {
        return 0;
    }

    return numElems;
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
    std::vector<std::string> freqs;
    freqs.push_back("RF");
    return freqs;
}

void SoapyFX3::setFrequency(const int, const size_t, const std::string &, const double, const SoapySDR::Kwargs &)
{
    return;
}

SoapySDR::RangeList SoapyFX3::getFrequencyRange(const int, const size_t, const std::string &name) const
{
	if ( name != "RF" )
		throw std::runtime_error( "getFrequencyRange(" + name + ") unknown name" );

    return(SoapySDR::RangeList( 1, SoapySDR::Range( 0, 50000000ull ) ) );
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

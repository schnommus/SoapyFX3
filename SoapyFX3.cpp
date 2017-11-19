#include <SoapySDR/Device.hpp>
#include <SoapySDR/Registry.hpp>

#include <libusb-1.0/libusb.h>

#include <string>
#include <iomanip>

template< typename T >
std::string int_to_hex( T i )
{
  std::stringstream stream;
  stream << "0x" 
         << std::setfill ('0') << std::setw(sizeof(T)*2) 
         << std::hex << i;
  return stream.str();
}

/***********************************************************************
 * Device interface
 **********************************************************************/
class SoapyFX3 : public SoapySDR::Device
{
    //Implement constructor with device specific arguments...

    //Implement all applicable virtual methods from SoapySDR::Device
};

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
    return new SoapyFX3;
}

/***********************************************************************
 * Registration
 **********************************************************************/
static SoapySDR::Registry registerSoapyFX3("cypress_fx3", &findSoapyFX3, &makeSoapyFX3, SOAPY_SDR_ABI_VERSION);

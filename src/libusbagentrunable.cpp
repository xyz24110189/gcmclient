#include "libusbagentrunable.h"
#include "wrappkiagent.h"
#include "utils.h"

#ifdef _WIN32

#pragma comment(lib, "legacy_stdio_definitions.lib")

#ifdef __cplusplus
FILE iob[] = { *stdin, *stdout, *stderr };
extern "C" {
	FILE * __cdecl _iob(void) { return iob; }
}
#endif

#endif

libusb_device_handle *handle = NULL;

LibUsbAgentRunable::LibUsbAgentRunable()
	: m_exit(false)
{

}

LibUsbAgentRunable::~LibUsbAgentRunable()
{
	LOG_INFO << "LibUsbAgentRunable::~LibUsbAgentRunable() begin...";
	ShutDown();
	LOG_INFO << "LibUsbAgentRunable::~LibUsbAgentRunable() end...";
}

void LibUsbAgentRunable::run()
{
	LOG_INFO << "LibUsbAgentRunable::run() begin...";
	while (!m_exit)
	{
		LOG_INFO << "new loop";
		InitUsbAgent();
	}
	LOG_INFO << "LibUsbAgentRunable::run() end...";
}

void LibUsbAgentRunable::ShutDown()
{
	m_exit = true;
}

int LibUsbAgentRunable::InitUsbAgent()
{
	LOG_INFO << "LibUsbAgentRunable::InitUsbAgent() begin...";
    libusb_hotplug_callback_handle hp[2];
    int class_id;
    int rc;
    class_id   = LIBUSB_HOTPLUG_MATCH_ANY;

    rc = libusb_init (NULL);
    if (rc < 0)
    {
		LOG_ERROR << "failed to initialise libusb: " << libusb_error_name(rc);
        return EXIT_FAILURE;
    }

    /*if (!libusb_has_capability (LIBUSB_CAP_HAS_HOTPLUG)) {
        printf ("Hotplug capabilites are not supported on this platform\n");
        libusb_exit (NULL);
        return EXIT_FAILURE;
    }*/

    rc = libusb_hotplug_register_callback (NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, 
		LIBUSB_HOTPLUG_NO_FLAGS, LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY, 
		class_id, hotplug_callback, NULL, &hp[0]);
    if (LIBUSB_SUCCESS != rc) {
		LOG_ERROR <<  "Error registering callback 0";
        libusb_exit (NULL);
        return EXIT_FAILURE;
    }

    rc = libusb_hotplug_register_callback (NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, 
		LIBUSB_HOTPLUG_NO_FLAGS, LIBUSB_HOTPLUG_MATCH_ANY ,LIBUSB_HOTPLUG_MATCH_ANY,
		class_id, hotplug_callback_detach, NULL, &hp[1]);
    if (LIBUSB_SUCCESS != rc) {
		LOG_ERROR << "Error registering callback 1";
        libusb_exit (NULL);
        return EXIT_FAILURE;
    }

    LOG_INFO << "init libusb success!";

    while (!m_exit) {
        rc = libusb_handle_events (NULL);
        if (rc < 0)
			LOG_ERROR << "libusb_handle_events() failed: " << libusb_error_name(rc);
    }

    if (handle) {
        libusb_close (handle);
    }

    libusb_exit (NULL);

	LOG_INFO << "LibUsbAgentRunable::InitUsbAgent() end...";
    return EXIT_SUCCESS;
}

int LIBUSB_CALL LibUsbAgentRunable::hotplug_callback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data)
{
    struct libusb_device_descriptor desc;
    int rc;

    (void)ctx;
    (void)dev;
    (void)event;
    (void)user_data;

    rc = libusb_get_device_descriptor(dev, &desc);
    if (LIBUSB_SUCCESS != rc) {
		LOG_ERROR << "Error getting device descriptor";
    }

	LOG_INFO << "Device attached: desc.idVendor = " << desc.idVendor << " desc.idProduct = " << desc.idProduct;

    GCM::Utils::Sleep(1000);
    //WrapPkiAgent::GetInstance()->AgentNotify(UkeyOperation::INSERT);

    if (handle) {
        libusb_close (handle);
        handle = NULL;
    }

    /*rc = libusb_open (dev, &handle);
    if (LIBUSB_SUCCESS != rc) {
		LOG_ERROR << "Error opening device\n";
    }*/

    return 0;
}

int LIBUSB_CALL LibUsbAgentRunable::hotplug_callback_detach(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data)
{
    (void)ctx;
    (void)dev;
    (void)event;
	(void)user_data;

    LOG_INFO << "Device detached";

    //WrapPkiAgent::GetInstance()->AgentNotify(UkeyOperation::EXTRACT);

    if (handle) {
        libusb_close (handle);
        handle = NULL;
    }

    return 0;
}

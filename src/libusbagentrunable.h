#ifndef LIBUSBAGENT_H
#define LIBUSBAGENT_H

#include <libusb.h>
#include <QtCore/QRunnable>

class LibUsbAgentRunable : public QRunnable
{
public:
	LibUsbAgentRunable();
	~LibUsbAgentRunable();

    void run();

	void ShutDown();

protected:
	int InitUsbAgent();

    static int LIBUSB_CALL hotplug_callback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data);
    static int LIBUSB_CALL hotplug_callback_detach(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data);

private:
	bool m_exit;
};

#endif // LIBUSBAGENT_H

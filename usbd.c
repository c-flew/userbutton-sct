#include <stdio.h>
#include <usb.h>
#include <stdbool.h>

static const int usb_num = 6;

int main() {
    int count = 0;
    bool prev = false;
    while (true) {
        struct usb_bus *bus;
        struct usb_device *dev;
        usb_init();
        usb_find_busses();
        usb_find_devices();
        for (bus = usb_busses; bus; bus = bus->next) {
            for (dev = bus->devices; dev; dev = dev->next){
                count++;
                /* printf("Trying device %s/%s\n", bus->dirname, dev->filename); */
                /* printf("\tID_VENDOR = 0x%04x\n", dev->descriptor.idVendor); */
                /* printf("\tID_PRODUCT = 0x%04x\n", dev->descriptor.idProduct); */
            }
        }
        /* printf("%d\n", count); */
        if (count > usb_num && !prev) {
            printf("usb device plugged in\n");
            printf("running: sct -s t\n");
            printf("------------\n");

            system("./sct -s t");
            printf("\n");

            prev = true;
        }

        count = 0;
        sleep(1);

        // There is probably a better way to do this
        usb_busses = NULL;
    }

    return 0;
}

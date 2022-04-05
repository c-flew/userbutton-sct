#include <stdio.h>
#include <usb.h>
#include <stdbool.h>


int main() {
    struct usb_bus *bus;
    struct usb_device *dev;
    usb_init();

    int count = 0;
    int prev_count = -1;
    while (true) {
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
        /* printf("%d\n", prev_count); */
        if (prev_count != -1 && count > prev_count) {
            printf("usb device plugged in\n");
            printf("running: sct -s t\n");
            printf("------------\n");

            system("./sct -s t");
            printf("\n");
        }

        prev_count = count;
        count = 0;

        // There is probably a better way to do this
        usb_busses = NULL;

        sleep(1);
    }

    return 0;
}

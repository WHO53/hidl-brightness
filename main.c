#include <stdio.h>
#include <stdlib.h>
#include <gbinder.h>

#define BINDER_LIGHT_HIDL_DEVICE "/dev/hwbinder"
#define BINDER_LIGHT_HIDL_IFACE "android.hardware.light@2.0::ILight"
#define BINDER_LIGHT_HIDL_SLOT "default"
#define BINDER_LIGHT_HIDL_2_0_IFACE BINDER_LIGHT_HIDL_IFACE
#define BINDER_LIGHT_HIDL_SET_LIGHT 1

typedef struct {
    int32_t color;
    int32_t flashMode;
    int32_t flashOnMs;
    int32_t flashOffMs;
    int32_t brightnessMode;
} LightState;

static gboolean set_display_brightness(guint brightness) {
    GBinderServiceManager *service_manager;
    GBinderRemoteObject *remote;
    GBinderClient *client;
    GBinderLocalRequest *req;
    GBinderRemoteReply *reply;
    GBinderWriter writer;
    LightState *light_state;
    int status;

    
    service_manager = gbinder_servicemanager_new(BINDER_LIGHT_HIDL_DEVICE);
    remote = gbinder_servicemanager_get_service_sync(service_manager, BINDER_LIGHT_HIDL_2_0_IFACE "/" BINDER_LIGHT_HIDL_SLOT, NULL);
    
    if (!remote) {
        g_warning("Failed to get light service.");
        return FALSE;
    }

    client = gbinder_client_new(remote, BINDER_LIGHT_HIDL_2_0_IFACE);

    
    req = gbinder_client_new_request(client);
    gbinder_local_request_init_writer(req, &writer);
    
    
    light_state = gbinder_writer_new0(&writer, LightState);
	light_state->color = (brightness << 16) | (brightness << 8) | brightness; 
    light_state->brightnessMode = 1; 

    
    gbinder_writer_append_int32(&writer, 0); 
    gbinder_writer_append_buffer_object(&writer, light_state, sizeof(*light_state));

    
    reply = gbinder_client_transact_sync_reply(client, BINDER_LIGHT_HIDL_SET_LIGHT, req, &status);
    gbinder_local_request_unref(req);

    
    if (status != GBINDER_STATUS_OK) {
        g_warning("Failed to set display brightness.");
        return FALSE;
    }

    return TRUE;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <brightness (0-255)>\n", argv[0]);
        return 1;
    }

    int brightness = atoi(argv[1]);
    
    if (brightness < 0 || brightness > 255) {
        printf("Brightness value must be between 0 and 255.\n");
        return 1;
    }

    if (set_display_brightness(brightness)) {
        printf("Display brightness set to %d%%\n", brightness);
    } else {
        printf("Failed to set display brightness.\n");
    }

    return 0;
}

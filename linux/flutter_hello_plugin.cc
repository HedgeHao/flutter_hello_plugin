#include "include/flutter_hello_plugin/flutter_hello_plugin.h"

#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>
#include <sys/utsname.h>

#include <cstring>
#include <vector>

#define FLUTTER_HELLO_PLUGIN(obj)                                     \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), flutter_hello_plugin_get_type(), \
                              FlutterHelloPlugin))

struct _MyTextureClass
{
  FlPixelBufferTextureClass parent_class;
  int64_t texture_id = 0;
  std::vector<uint8_t> buffer{};
  int32_t video_width = 0;
  int32_t video_height = 0;
};

G_DECLARE_DERIVABLE_TYPE(MyTexture,
                         my_texture,
                         MY,
                         TEXTURE,
                         FlPixelBufferTexture)

G_DEFINE_TYPE(MyTexture,
              my_texture,
              fl_pixel_buffer_texture_get_type())

struct _FlutterHelloPlugin
{
  GObject parent_instance;
  FlTextureRegistrar *texture_registrar;
  MyTexture *myTextxure;
};

static gboolean my_texture_copy_pixels(
    FlPixelBufferTexture *texture,
    const uint8_t **out_buffer,
    uint32_t *width,
    uint32_t *height,
    GError **error)
{
  *out_buffer = MY_TEXTURE_GET_CLASS(texture)->buffer.data();
  *width = 300;
  *height = 300;

  return TRUE;
}

static void my_texture_class_init(
    MyTextureClass *klass)
{
  FL_PIXEL_BUFFER_TEXTURE_CLASS(klass)->copy_pixels =
      my_texture_copy_pixels;
}

static void my_texture_init(MyTexture *self) {}

G_DEFINE_TYPE(FlutterHelloPlugin, flutter_hello_plugin, g_object_get_type())

// Called when a method call is received from Flutter.
static void flutter_hello_plugin_handle_method_call(
    FlutterHelloPlugin *self,
    FlMethodCall *method_call)
{
  g_autoptr(FlMethodResponse) response = nullptr;

  const gchar *method = fl_method_call_get_name(method_call);

  if (strcmp(method, "getPlatformVersion") == 0)
  {
    struct utsname uname_data = {};
    uname(&uname_data);
    g_autofree gchar *version = g_strdup_printf("Linux %s", uname_data.version);
    g_autoptr(FlValue) result = fl_value_new_string(version);
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
  }
  else if (strcmp(method, "registerTexture") == 0)
  {
    self->myTextxure = MY_TEXTURE(g_object_new(my_texture_get_type(), nullptr));
    FL_PIXEL_BUFFER_TEXTURE_GET_CLASS(self->myTextxure)->copy_pixels = my_texture_copy_pixels;
    fl_texture_registrar_register_texture(self->texture_registrar, FL_TEXTURE(self->myTextxure));
    MY_TEXTURE_GET_CLASS(self->myTextxure)->texture_id = reinterpret_cast<int64_t>(FL_TEXTURE(self->myTextxure));
    fl_texture_registrar_mark_texture_frame_available(self->texture_registrar, FL_TEXTURE(self->myTextxure));
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_int(MY_TEXTURE_GET_CLASS(self->myTextxure)->texture_id)));
  }
  else if (strcmp(method, "nativeClick") == 0)
  {
    auto & buffer = MY_TEXTURE_GET_CLASS(self->myTextxure)->buffer;
    int len = 300 * 300 * 4;
    buffer.resize(len);
    for (int i = 0; i < len; i++)
    {
      buffer[i] = 127;
    }
    fl_texture_registrar_mark_texture_frame_available(self->texture_registrar, FL_TEXTURE(self->myTextxure));
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(nullptr));
  }
  else
  {
    response = FL_METHOD_RESPONSE(fl_method_not_implemented_response_new());
  }

  fl_method_call_respond(method_call, response, nullptr);
}

static void flutter_hello_plugin_dispose(GObject *object)
{
  G_OBJECT_CLASS(flutter_hello_plugin_parent_class)->dispose(object);
}

static void flutter_hello_plugin_class_init(FlutterHelloPluginClass *klass)
{
  G_OBJECT_CLASS(klass)->dispose = flutter_hello_plugin_dispose;
}

static void flutter_hello_plugin_init(FlutterHelloPlugin *self) {}

static void method_call_cb(FlMethodChannel *channel, FlMethodCall *method_call,
                           gpointer user_data)
{
  FlutterHelloPlugin *plugin = FLUTTER_HELLO_PLUGIN(user_data);
  flutter_hello_plugin_handle_method_call(plugin, method_call);
}

void flutter_hello_plugin_register_with_registrar(FlPluginRegistrar *registrar)
{
  FlutterHelloPlugin *plugin = FLUTTER_HELLO_PLUGIN(
      g_object_new(flutter_hello_plugin_get_type(), nullptr));

  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  g_autoptr(FlMethodChannel) channel =
      fl_method_channel_new(fl_plugin_registrar_get_messenger(registrar),
                            "flutter_hello_plugin",
                            FL_METHOD_CODEC(codec));
  fl_method_channel_set_method_call_handler(channel, method_call_cb,
                                            g_object_ref(plugin),
                                            g_object_unref);

  plugin->texture_registrar = fl_plugin_registrar_get_texture_registrar(registrar);
  g_object_unref(plugin);
}

import 'dart:async';

import 'package:flutter/services.dart';

class FlutterHelloPlugin {
  static const MethodChannel _channel = MethodChannel('flutter_hello_plugin');

  static Future<String?> get platformVersion async {
    final String? version = await _channel.invokeMethod('getPlatformVersion');
    return version;
  }

  static Future<int?> registerTexture() async {
    final int? id = await _channel.invokeMethod('registerTexture');
    return id;
  }

  static Future<void> nativeClick() async {
    print('Click');
    await _channel.invokeMethod('nativeClick');
  }
}

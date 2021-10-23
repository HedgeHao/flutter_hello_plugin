import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:flutter_hello_plugin/flutter_hello_plugin.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  String _platformVersion = 'Unknown';
  int? textureId;

  @override
  void initState() {
    super.initState();
    initPlatformState();
    test();
  }

  Future<void> test() async {
    try {
      textureId = await FlutterHelloPlugin.registerTexture();
      print('texture id: $textureId');
    } catch (e) {
      print('error:$e');
    }
  }

  // Platform messages are asynchronous, so we initialize in an async method.
  Future<void> initPlatformState() async {
    String platformVersion;
    // Platform messages may fail, so we use a try/catch PlatformException.
    // We also handle the message potentially returning null.
    try {
      platformVersion = await FlutterHelloPlugin.platformVersion ?? 'Unknown platform version';
    } on PlatformException {
      platformVersion = 'Failed to get platform version.';
    }

    // If the widget was removed from the tree while the asynchronous platform
    // message was in flight, we want to discard the reply rather than calling
    // setState to update our non-existent appearance.
    if (!mounted) return;

    setState(() {
      _platformVersion = platformVersion;
    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
        home: Scaffold(
      appBar: AppBar(
        title: const Text('Plugin example app'),
      ),
      body: Container(
        color: Colors.black,
        child: Center(
          child: Column(children: [
            textureId == null
                ? const SizedBox()
                : SizedBox(
                    height: 300,
                    width: 300,
                    child: Texture(
                      textureId: textureId!,
                      filterQuality: FilterQuality.high,
                    )),
            Text(
              'Running on: $_platformVersion\n',
              style: const TextStyle(color: Colors.white),
            ),
            TextButton(
                onPressed: () {
                  FlutterHelloPlugin.nativeClick();
                },
                child: const Text('Click'))
          ]),
        ),
      ),
    ));
  }
}

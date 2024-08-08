import 'package:flutter/material.dart';
import 'dart:async';

import 'package:flutter/services.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  String result = "Result will be shown here";

  /// The method channel used to interact with the native platform.
  final methodChannel = const MethodChannel('test/get_platform_name');

  Future<String?> getPlatformString() async {
    final str = await methodChannel.invokeMethod<String>('getPlatformString');
    return str;
  }

  Future<String> concat(List<String> args) async {
    final str = await methodChannel.invokeMethod<String>('concat', args);
    return str ?? "???";
  }

  Future<int> napiBenchmark(
      int numStrs, int lengthPerStr, int numTrials) async {
    final args = <String>[];
    for (var i = 0; i < numStrs; i++) {
      args.add('x' * lengthPerStr);
    }

    final stopwatch = Stopwatch()..start();
    for (var i = 0; i < numTrials; i++) {
      await concat(args);
    }
    stopwatch.stop();

    return stopwatch.elapsedMicroseconds ~/ numTrials;
  }

  Future<void> runOnce(int numStrs, int lengthPerStr, int numTrials) async {
    final duration = await napiBenchmark(numStrs, lengthPerStr, numTrials);
    final s = 'tbench:dart-napi,$numStrs,$lengthPerStr,$duration';
    debugPrint(s);
    setState(() {
      result = '$result$s\n';
    });
  }

  void _handleButtonClick() async {
    String? name = await getPlatformString();
    String concatRet = await concat(["x", "y", "z"]);
    setState(() {
      result = '$name: $concatRet:\n';
    });

    runOnce(1, 1, 1);
    for (var i = 0; i < 5000; i += 200) {
      runOnce(1500, i, 3);
    }
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Platform Channel Benchmark'),
        ),
        body: Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: <Widget>[
              Text(
                result,
                style: Theme.of(context).textTheme.bodyLarge,
              ),
              const Spacer(),
              ElevatedButton(
                onPressed: _handleButtonClick,
                child: const Text('Run benchmark!'),
              ),
              const SizedBox(height: 20),
            ],
          ),
        ),
      ),
    );
  }
}

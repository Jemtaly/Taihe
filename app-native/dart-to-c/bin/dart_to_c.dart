import 'package:dart_to_c/wrapper_taihe.dart' as taihe;
import 'package:dart_to_c/wrapper.dart' as capi;

int taiheBenchmark(int numStrs, int lengthPerStr, int numTrials) {
  final args = <String>[];
  for (var i = 0; i < numStrs; i++) {
    args.add('x' * lengthPerStr);
  }

  final stopwatch = Stopwatch()..start();
  for (var i = 0; i < numTrials; i++) {
    taihe.concat(args);
  }
  stopwatch.stop();

  return stopwatch.elapsedMicroseconds ~/ numTrials;
}

int cApiBenchmark(int numStrs, int lengthPerStr, int numTrials) {
  final args = <String>[];
  for (var i = 0; i < numStrs; i++) {
    args.add('x' * lengthPerStr);
  }

  final stopwatch = Stopwatch()..start();
  for (var i = 0; i < numTrials; i++) {
    capi.concat(args);
  }
  stopwatch.stop();

  return stopwatch.elapsedMicroseconds ~/ numTrials;
}

void runOnce(int numStrs, int lengthPerStr, int numTrials, String name,
    int Function(int, int, int) fn) {
  final duration = fn(numStrs, lengthPerStr, numTrials);
  print('tbench:$name,$numStrs,$lengthPerStr,$duration');
}

void main() {
  capi.concat([]);
  taihe.concat([]);

  runOnce(1, 1, 3, 'x86-dart-taihe', taiheBenchmark);
  for (var i = 0; i < 5000; i += 200) {
    runOnce(1500, i, 3, 'x86-dart-taihe', taiheBenchmark);
  }

  runOnce(1, 1, 3, 'x86-dart-capi', cApiBenchmark);
  for (var i = 0; i < 5000; i += 200) {
    runOnce(1500, i, 3, 'x86-dart-capi', cApiBenchmark);
  }
}

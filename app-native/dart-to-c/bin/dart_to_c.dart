import 'package:dart_to_c/wrapper.dart';

int benchmark(int numStrs, int lengthPerStr, int numTrials) {
  final args = <String>[];
  for (var i = 0; i < numStrs; i++) {
    args.add('x' * lengthPerStr);
  }

  final stopwatch = Stopwatch()..start();
  for (var i = 0; i < numTrials; i++) {
    concat(args);
  }
  stopwatch.stop();

  return stopwatch.elapsedMicroseconds ~/ numTrials;
}

void main() {
  for (var i = 0; i < 5000; i += 200) {
    final duration = benchmark(1500, i, 3);
    print('${i.toString().padLeft(8)},${duration.toString().padLeft(8)}');
  }
}

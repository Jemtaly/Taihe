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

void main() {
  taiheBenchmark(1, 1, 1);
  for (var i = 0; i < 5000; i += 200) {
    final duration = taiheBenchmark(1500, i, 3);
    print('taihe,${i.toString().padLeft(8)},${duration.toString().padLeft(8)}');
  }

  cApiBenchmark(1, 1, 1);
  for (var i = 0; i < 5000; i += 200) {
    final duration = cApiBenchmark(1500, i, 3);
    print('capi,${i.toString().padLeft(8)},${duration.toString().padLeft(8)}');
  }
}

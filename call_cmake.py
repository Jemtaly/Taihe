import os

def main():
  if not os.path.exists("/root/TaiheCompiler/out/runtime"):
    os.makedirs("/root/TaiheCompiler/out/runtime")
  os.system("cd /root/TaiheCompiler/out/runtime && cmake -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ /root/TaiheCompiler/runtime && make")
  return 0

if __name__ == "__main__":
  main()

# echo "" > log.txt
# echo ${@}
# exit 0
if [ "$1" = "debug" ];then
  if [ "$2" = "preprocess" ];then
    echo preprocessing
    g++ -E -std="c++20" -I./src -g ./src/main.cc -o ./debug/debug.i 2>log.txt
    if [ $(stat -c%s ./log.txt) -gt 1 ];then
      echo preprocessing failure
      exit 1
    fi
    echo preprocessing success
  elif [ "$2" = "build" ] || [ "$2" = "core" ];then
    g++ -std="c++20" -I./src -g ./src/main.cc -o ./debug/debug.out 2>log.txt #change something maybe
    if test $(stat -c%s ./log.txt) -gt 1; then
      echo debug didn\'t compile \:\(
      cat log.txt
      exit 1
    fi
    rm log.txt
    echo debug compiled \:3
    if [ "$2" = "core" ];then
      xzcat /var/lib/systemd/coredump/core.debug*.xz > ./debug/core
      gdb ./debug/debug.out ./debug/core
    elif [ "$2" = "build" ];then
      gdb ./debug/debug.out
    else
      ./debug/debug.out
    fi
  fi
  exit 0
fi
g++ -std="c++20" -I./src ./src/main.cc 2>log.txt
if test $(stat -c%s ./log.txt) -gt 1; then
  echo didn\'t compile \:\(
  cat log.txt
  exit 1
fi
rm log.txt
echo compiled \:3
./a.out
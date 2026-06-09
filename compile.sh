# echo "" > log.txt
# echo ${@}
# exit 0
mkdir debug 2>/dev/null
if [ "$1" = "debug" ];then
  if [ "$2" = "preprocess" ];then
    echo preprocessing
    gcc -std="c++20" -I./src ./src/main.cc -g -E -lstdc++ -lm -o ./debug/debug.i 2>log.txt
    if [ $(stat -c%s ./log.txt) -gt 1 ];then
      echo preprocessing failure
      exit 1
    fi
    echo preprocessing success
  else
    if [ "$2" = "core" ];then
      xzcat /var/lib/systemd/coredump/core.debug*.xz > ./debug/core
      gdb ./debug/debug.out ./debug/core
    elif [ "$2" = "build" ];then
      gcc -std="c++20" -I./src -g ./src/main.cc -lstdc++ -lm -o ./debug/debug.out 2>log.txt
      if test $(stat -c%s ./log.txt) -gt 1; then
        echo debug didn\'t compile \:\(
        cat log.txt
        exit 1
      fi
      rm log.txt
      echo debug compiled \:3
      gdb ./debug/debug.out
    else
      gcc -std="c++20" -I./src -g ./src/debug.cc -lstdc++ -lm -o ./debug/debug.out 2>log.txt
      if test $(stat -c%s ./log.txt) -gt 1; then
        echo debug didn\'t compile \:\(
        cat log.txt
        exit 1
      fi
      rm log.txt
      echo debug compiled \:3
      ./debug/debug.out
      cat ./debug/debug.log
    fi
  fi
  exit 0
fi
gcc -std="c++20" -Wall -I./src ./src/main.cc -lstdc++ -lm 2>log.txt
if test $(stat -c%s ./log.txt) -gt 1; then
  echo didn\'t compile \:\(
  cat log.txt
  exit 1
fi
rm log.txt
echo compiled \:3
./a.out
cat ./debug/debug.log

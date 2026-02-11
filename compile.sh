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
  elif [ "$2" = "build" ];then
    g++ -std="c++20" -I./src -g ./src/main.cc -o ./debug/debug.out 2>log.txt #change something maybe
    if test $(stat -c%s ./log.txt) -gt 1; then
      echo debug didn\'t compile \:\(
      cat log.txt
      exit 1
    fi
    rm log.txt
    echo debug compiled \:3
    ./debug/debug.out
  fi
  exit 0
fi
g++ -std="c++20" -I./src -g ./src/main.cc 2>log.txt
if test $(stat -c%s ./log.txt) -gt 1; then
  echo didn\'t compile \:\(
  cat log.txt
  exit 1
fi
rm log.txt
echo compiled \:3
./a.out
# echo "" > log.txt
NCURSES_PATH="/opt/c9/ncurses-6.3/"
g++ -std="c++20" -I./src -g ./src/main.cc 2>log.txt
if test $(stat -c%s ./log.txt) -gt 1; then
  echo didn\'t compile \:\(
  cat log.txt
  exit 1
fi
rm log.txt
echo compiled \:3
./a.out
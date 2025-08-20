# Yutovo project
Yutovo is a powerful calculator with graphical representation of mathematics operations inside a text editor.

Yutovo editor combines the capabilities of editing text, mathematical formulas and performing calculations.

## Building for Ubuntu

If you haven't yet, build [yutovo-logger](https://github.com/denprog/yutovo-logger), [yutovo-calculator](https://github.com/denprog/yutovo-calculator) and [yutovo-solver](https://github.com/denprog/yutovo-solver).
Install the dependencies:

```
sudo update && sudo apt install -y qtbase5-dev qtchooser qt5-qmake
```
This variable should be set to the yutovo directory:

```
export YUTOVO_DEPLOY=~/yutovo/deploy
```
Clone the project in the yutovo dir (select another branch if you want):

```
cd yutovo
git clone -b develop https://github.com/denprog/yutovo-editor.git
```
Create the build directories and build the debug version:

```
mkdir -p build/debug
cd build/debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
make -sj && make install
```
Run the tests:

```
./test/yutovo-editor_tests
```

./compile
./build/main -r 10 -o example1.txt
rm regret.pdf
python simpleplot.py out/example1.txt 

# Monopoly
### A simulator of play on a Monopoly board, to analyze good buys

In the board game Monopoly, all monopolies are good, but some are better than others.
This is true not just because they have different costs and payouts,
but because different sections of the board have a surprisingly large difference
in how likely other players are to land on them.

This is a simulator which marches players around a monopoly board,
counting how often they land on each square.
It simulates going to jail,
and all Chance and Community Chest cards that affect movement.

As an end result, it displays a "bargain rating" for each square and each monopoly.
This indicates the rate of payout, relative to the cost of fully monopolizing it.

My finding: the orange monopoly (St James, Tennessee, New York) has by far the best
payout to cost ratio, as many experienced players are already well aware.
Light blue (Oriental, Vermont, Connecticut) is next best, well ahead of the rest.
Third through sixth place are nearly a four-way tie,
with red, dark blue, magenta, and yellow all rating about the same,.
The railroads manage to beat two of the monopolies:
green (overpriced) and purple (underpowered).
The utilities, of course, are at the bottom of the heap.

The program was originally written in Fortran back around 1980,
then translated to portable C around 1990.
I have sometimes used it as a speed benchmark as I have gone through different computers.
It still works but may need a warning suppressed for old deprecated stdio functions.

No license, do whatever you want with it.

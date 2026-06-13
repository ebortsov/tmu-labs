# Sheet 04 - Pointers, references, arrays

## Sport

You're implementing a ranking system for a very serious sport organisation.
What kind of sport it is does not matter &ndash; what matters is that it's _fast_.

The results of a competition in this sport consist of two arrays &ndash; points and times.
Points are more important &ndash; a competitor with more points is better than one with fewer.
Times are used as a tie-breaker &ndash; if two competitors score the same number of points the faster one wins.

Your task is to order the results &ndash; the best result first, second best second, etc.
However, we don't want to move the actual results, as their order tells us which competitor had which result.
Instead, you are given arrays of _pointers_ to the points and times, and need to sort those pointers based on the results they point to.

The signature of the `sport::sort` function is provided in `sport.hpp`. Your implementation should be located in `sport.cpp`.
The sort need not be stable.

You can assume all pointers are valid to dereference, in particular they are all non-null.

You __may not__ modify the header file. 

## Stockpiles

A local company is distributing apfelschorle bottles to thirsty university students, but they're struggling with managing their stockpiles.
Each stockpile has an ID and access to some finite number of bottles.
They would like to serve requests for some number of bottles according to a simple strategy: always pick the stockpile with the largest number of available bottles.
In case of a tie, the stockpile with the lower ID should be chosen.
All bottles for a single request must come from a single stockpile.

The company owner knows a little computer science, so he recommended you use some sort of a heap to serve requests quickly.

First the `stockpiles::init` function will be called on the array of stockpiles so you can build the heap at the start.
Then a number of calls to `stockpiles::serve` will occur.
The function should return a boolean value indicating if the request was possible to serve.
If successful, the passed-in reference should be filled with the stockpile that was used to fulfill the request.
Remember to actually update the number of available bottles in the used stockpile!

You can assume all pointers are valid to dereference and non-null, and serve request amounts are greater than zero.

You __may not__ modify the header file.
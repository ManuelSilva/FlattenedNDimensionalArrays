# FlattenedNDimensionalArrays
A C++ implementation of Flattened N Dimensional Arrays using Variadic Templates


 
You're probably familiar with the common case
The common case is normally 2-D to 1-D
Looks like this
```
int arr[XSize * YSize];
int index = XSize * x + y;
x == index % XSize
y == index / XSize
```

One day I wondered what about X-D to 1-D

I found a solution for the 3-D to 1-D case online

https://stackoverflow.com/questions/7367770/how-to-flatten-or-index-3d-array-in-1d-array
```
public int to1D( int x, int y, int z ) 
{
	return (z * xMax * yMax) + (y * xMax) + x;
}

public int[] to3D( int idx ) 
{
	final int z = idx / (xMax * yMax);
	idx -= (z * xMax * yMax);
	final int y = idx / xMax;
	final int x = idx % xMax;
	return new int[]{ x, y, z };
}
```

And started seeing a pattern from it I derived the GetIndex and GetCoordinate functions bellow

```
// Pseudo code

// N Coordinates to 1 Index
int GetIndex(int[N] coords)
{
	int index = 0;
	int cumulativeSize = 1;

   for (int i = 0; i < N; ++i)
   {
      int coordinates = coords[i];
      index += coordinates * cumulativeSize;
      cumulativeSize *= c_dimensions[i]; // variable holding size of each dimension
   }

		return index;
}

// 1 index to N Coordinates
constexpr int[N] GetCoordinates(int index)
{
	int[N] output;
	int cumulativeSize = 1;

	for (int i = 0; i < N; ++i)
	{
		int size = c_dimensions[i];
		int coord = (index / cumulativeSize) % size;
		output[i] = coord;
		cumulativeSize *= c_dimensions[i]; // variable holding size of each dimension
	}

	return output;
}
```

Then I implemented this into a data structure... in C# 

Unfortunately Generics made the resulting code not pretty due to a lack of the ability to write
a Variable number of elements in Generics (example MyFunction<T1,T2, TX...>)
which led to a verbose syntax, with special constructors and helper info prepared only at runtime. :/
It left a sour taste in my mouth.

Then I learned about Variadic Templates in C++

That, plus the new-ish syntax sugar added in C++17 were perfect to express this data structure in a clean and mostly cost free manner
Recommended Reading: https://en.cppreference.com/w/cpp/language/parameter_pack

// MIT License
// 
// Copyright(c) 2023 Manuel Correia Neves Tomás da Silva
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// Flattened N Dimensional Arrays 
// or
// Variadic Flattened Arrays
// 
// by Manuel Silva
// 
// You're probably familiar with the common case
// The common case is normally 2-D to 1-D
// Looks like this
//	int arr[XSize * YSize];
//	int index = XSize * x + y;
//	x == index % XSize
//	y == index / XSize
// 
// One day I wondered what about X-D to 1-D
// I found a solution for the 3-D to 1-D case online
/*
	// https://stackoverflow.com/questions/7367770/how-to-flatten-or-index-3d-array-in-1d-array
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
*/
// and started seeing a pattern
// 
// from it I derived the GetIndex and GetCoordinate functions bellow
/*  
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
*/
// Then I implemented this into a data structure... in C# 
// unfortunately Generics made the resulting code not pretty due to a lack of the ability to write
// a Variable number of elements in Generics (example MyFunction<T1,T2, TX...>)
// which led to a verbose syntax, with special constructors and helper info prepared only at runtime. :/
// It left a sour taste in my mouth.
// Then I learned about Variadic Templates in C++
// That, plus the new-ish syntax sugar added in C++17 were perfect to express this data structure in a clean and mostly cost free manner
// Recommended Reading: https://en.cppreference.com/w/cpp/language/parameter_pack
//

#include <iostream>

// These Helper Array templates help FlattenedArray 
// at providing multiple brackets functionality to our Flattened N-Dimensional array [][][]...
// making it behave more like a typical array
template <typename T, int FirstDimension, int... Dimensions>
struct HelperArray
{
	static constexpr int c_size = FirstDimension;

	// notice usage of recursion
	HelperArray<T, Dimensions...> data[c_size];

	HelperArray<T, Dimensions...>& operator[](int index)
	{
		return data[index];
	}
};

template <typename T, int Dimension>
struct HelperArray<T, Dimension> // Required syntax for Template Specialization for termination of recursion
{
	static constexpr int c_size = Dimension;
	T data[c_size];

	T& operator[](int index)
	{
		return data[index];
	}
};

// This struct helps at cleaning the array coordinates interface
template<int Size>
struct Coordinates
{
	static constexpr int c_size = Size;
	int data[Size];

	int& operator[](int index)
	{
		return data[index];
	}
};	

// FlattenedArray -> Flattened N Dimensional Array
// Using Variadic Templates's new-ish syntax (C++ 17)
// https://en.cppreference.com/w/cpp/language/parameter_pack
template <typename T, int FirstDimension, int... Dimensions>
struct FlattenedArray
{
	static constexpr int c_size = FirstDimension;
	
	// (Dimensions * ...) //all elements of Dimensions get multiplied together
	static constexpr int c_fullSize = FirstDimension * (Dimensions * ...); 

	// sizeof...(Dimensions) //gives the number of elements contained in Dimensions
	static constexpr int c_numberOfDimensions = sizeof...(Dimensions) + 1; 

	static constexpr int c_dimensions[c_numberOfDimensions] = { FirstDimension, Dimensions... };

	// using type punning (technically UB) make sure the data is aligned
	union alignas(T)
	{
		T data[c_fullSize];
		HelperArray<T, Dimensions...> matrix[c_size];
	};

	// N Coordinates to 1 Index
	constexpr int GetIndex(Coordinates<c_numberOfDimensions> coords)
	{
		int index = 0;

		int cumulativeSize = 1;
		for (int i = 0; i < c_numberOfDimensions; ++i)
		{
			int coordinates = coords[i];
			index += coordinates * cumulativeSize;
			cumulativeSize *= c_dimensions[i];
		}

		return index;
	}

	// 1 index to N Coordinates
	constexpr Coordinates<c_numberOfDimensions> GetCoordinates(int index)
	{
		Coordinates<c_numberOfDimensions> output;

		int cumulativeSize = 1;
		for (int i = 0; i < c_numberOfDimensions; ++i)
		{
			int size = c_dimensions[i];
			int coord = (index / cumulativeSize) % size;
			output[i] = coord;
			cumulativeSize *= c_dimensions[i];
		}

		return output;
	}

	HelperArray<T, Dimensions...>& operator[](int index)
	{
		return matrix[index];
	}
};


//Example Usage

using myFlattened5dIntArray = FlattenedArray<int, 3, 2, 1, 1, 2>;
//using myFlattened3DChessBoardArray = FlattenedArray<char, 8, 8, 8>;


int main()
{
    std::cout << "My Flattened Array\n";

	myFlattened5dIntArray myFlattenedArray;
	int myReal5dArray[3][2][1][1][2];

	//c_fullSize = 8
	for (int index = 0, lenght = myFlattened5dIntArray::c_fullSize; index < lenght; ++index)
	{
		myFlattenedArray.data[index] = index * 3;

		//auto -> Coordinates<5>
		auto coord = myFlattenedArray.GetCoordinates(index);
		std::cout << "(" 
			<< coord[0] << "," 
			<< coord[1] << "," 
			<< coord[2] << "," 
			<< coord[3] << "," 
			<< coord[4] << ") Index ->" << index 
			<< " Value -> " << myFlattenedArray.data[index] << std::endl;
	}

	std::cout << "---------------------------\n";

	// reversing loop order to traverse elements in their natural order 0...n
	//c_dimensions[5] = {2,2,1,1,2}
	for (int i = 0, size1 = myFlattened5dIntArray::c_dimensions[4]; i < size1; ++i)
	{
		for (int j = 0, size2 = myFlattened5dIntArray::c_dimensions[3]; j < size2; ++j)
		{
			for (int k = 0, size3 = myFlattened5dIntArray::c_dimensions[2]; k < size3; ++k)
			{
				for (int l = 0, size4 = myFlattened5dIntArray::c_dimensions[1]; l < size4; ++l)
				{
					for (int m = 0, size5 = myFlattened5dIntArray::c_dimensions[0]; m < size5; ++m)
					{
						// { m, l, k, j, i } -> Gets interpreted as Coordinates<5>
						int index = myFlattenedArray.GetIndex({ m, l, k, j, i });
						std::cout << "(" << m << "," 
							<< l << ","
							<< k << ","
							<< j << "," 
							<< i 
							<< ") Index ->" << index 
							<< " Value -> " << myFlattenedArray.data[index] << std::endl;

						// initializing real array with same value
						myReal5dArray[m][l][k][j][i] = index * 3;
					}
				}
			}
		}
	}

	int x1 = myFlattenedArray[0][2][1][1][0];
	int x2 = myFlattenedArray.data[myFlattenedArray.GetIndex({0,2,1,1,0})];

	std::cout << "---------------------------\n";
	// To show size is the same if we disregard static variables created
	std::cout << "fake array size -> " << sizeof(myFlattenedArray) << "\n";
	std::cout << "real array size -> " << sizeof(myReal5dArray) << "\n";
	std::cout << "---------------------------\n";
}


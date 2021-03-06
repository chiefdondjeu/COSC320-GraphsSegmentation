/*
	Florent Dondjeu Tschoufack
	COSC 320 - 002
	Project 3
	Graph.cpp
	May 10, 2020
*/

#include "Graph.h"

/*
	constructor	
*/

template <class T>
Graph<T>::Graph()
{}

/*
	deconstructor	
*/

template <class T>
Graph<T>::~Graph()
{}

/*
	(private)
	returns a positive difference of two numbers	
*/

template <class T>
double Graph<T>::difference(double& a, double& b)
{
	if((a-b) < 0)
		return -(a-b);
	else return (a-b);
}

/*
	adds a pixel to the graph
*/

template <class T>
void Graph<T>::addVertex(T v, double w)
{
	if(V.find(v) == V.end())
	{
		std::vector<Pixel> vect;
		V[v] = vect;
		W[v] = w;
	}
}

/*
	adds an undirected edge between 2 pixels
*/

template <class T>
void Graph<T>::addEdge(T u, T v)
{
	//pixel have already been added and no self edges
	if(V.find(u) != V.end() && V.find(v) != V.end() && u != v)
	{
		typename std::map<T,double>::iterator iteru = W.find(u);
		typename std::map<T,double>::iterator iterv = W.find(v);

		V[u].push_back(Pixel(v,difference(iteru->second,iterv->second)));
		V[v].push_back(Pixel(u,difference(iteru->second,iterv->second)));

		std::pair<T,T> p = std::make_pair(u,v);
		pi[difference(iteru->second,iterv->second)] = p;
	}
}

/*
	(private)
	helper function to print content stored 
	in a vector
*/

template <class T>
void Graph<T>::printVect(std::vector<Pixel>& vect)
{
	for(int i = 0; i < vect.size(); i++)
	{
		std::cout << vect.at(i).data << "(" << vect.at(i).weight << ") ";
	}
}

/*
	prints all pixels along with their intensity 
	and their adjacency list, and edge weight
*/

template <class T>
void Graph<T>::print()
{
	std::cout << "========================================" << std::endl;
	std::cout << "\t\tGraph" << std::endl;
	typename std::map<T, std::vector<Pixel> >::iterator iter;
	typename std::map<T,double>::iterator iterW;
	std::cout << "========================================";
	for(iter = V.begin(), iterW = W.begin(); iter != V.end(); iter++,iterW++)
	{
		std::cout << "\nPixel: " << iter->first << "-" << iterW->second << std::endl;
		std::cout << "Edges: ";
		printVect(iter->second);
		std::cout << std::endl;
	}
}

/*
	(private)
	helper function to search for an element in a vector
*/

template <class T>
bool Graph<T>::isPresent(std::vector<T> vect, T x)
{
	for(int i = 0; i < vect.size(); i++)
	{
		if(vect.at(i) == x)
			return true;
	}
	return false;
}

/*
	(private)
	determines if two elements are in different components
	return true if element are in different component
	otherwise false
*/

template <class T>
bool Graph<T>::isDisjoint(std::vector<std::vector<T> >S, T x,T y)
{
	for(int i = 0; i < S.size(); i++)
	{
		if(!isPresent(S.at(i),x))
		{	
			if(!isPresent(S.at(i),y))
				return true;
		}
	}
	return false;
}

/*
	(private)
	controls the degree of difference between two components
*/

template <class T>
double Graph<T>::tau(int x)
{
	return 50/x;
}

/*
	(private)
	adds all the intensity of each element present in 2 components
	returns the minimun of the two
*/

template <class T>
double Graph<T>::internal_difference(std::vector<std::vector<T> >S, T x, T y)
{
	int min, sum1 = 0, sum2 = 0;

	for(int i = 0; i < S.size(); i++)
	{
		if(isPresent(S.at(i),x))
		{
			for(int j = 0; j < S.at(i).size(); j++)
			{
				for(auto l = W.begin(); l != W.end(); l++)
				{
					if(l->first == S.at(i).at(j))
						sum1 += l->second;
					sum1+= tau(S.at(i).at(j));
				}
			}
		}
		if(isPresent(S.at(i),y))
		{
			for(int j = 0; j < S.at(i).size(); j++)
			{
				for(auto l = W.begin(); l != W.end(); l++)
				{
					if(l->first == S.at(i).at(j))
						sum2 += l->second;
					sum2+= tau(S.at(i).at(j));
				}
			}
		}
	}

	if(sum1 < sum2)
		min = sum1;
	else min = sum2;

	return min;
}

/*
	(private)
	combines the pixel from two 
	components into one 
*/

template <class T>
void Graph<T>::merge(std::vector<std::vector<T> >& S, T x,T y)
{	
	//search
	int merge_index, del_index;
	for(int i = 0; i < S.size(); i++)
	{	
		if(isPresent(S.at(i),x))
			merge_index = i;

		if(isPresent(S.at(i),y))
			del_index = i;
	}

	//copy
	std::vector<T> tmp = S.at(del_index);;

	//delete
	S.at(del_index).clear();

	//merge
	for(int i = 0; i < tmp.size(); i++)
		S.at(merge_index).push_back(tmp.at(i));
}

/*
	segements the graph by putting each pixel in a 
	component and merges pixels with similar 
	intensity thus creating sub-regions of the graphs
*/

template <class T>
void Graph<T>::Segment()
{
	//step 1
	//put E into pi, and sort pi non-decreasing order
	//std::map<double,std::pair<T,T> > pi;

	//step 2
	//put each vertex is in its own component
	std::vector<std::vector<T> > S;
	std::vector<T> tmp;
	for(auto i = W.begin(); i != W.end(); i++)
	{
		tmp.push_back(i->first);
		S.push_back(tmp);
		tmp.clear();
	}

	//step 3 & 4
	//group by region
	for(auto q = pi.begin(); q != pi.end(); q++)
	{
		//if disjoint and edge weight < internal component weight
		if(isDisjoint(S,q->second.first,q->second.second) && q->first < internal_difference(S,q->second.first,q->second.second))
			merge(S,q->second.first,q->second.second);
	}

	std::cout << "\n========================================\n";
	std::cout << "\tSegmentation Result";
	std::cout << "\n========================================\n";
	for(int i = 0; i < S.size(); i++)
	{
		if(S.at(i).size() != 0)
		{
			std::cout << "Region " << i+1 << ": ";
			for(int j = 0; j < S.at(i).size(); j++)
			{
				std::cout << S.at(i).at(j) << "-";
				auto iter = W.find(S.at(i).at(j));
				std::cout << iter->second << "   ";

			}
			std::cout << "\n\n";
		}
	}
}
#include <deque>
#include <iostream>
int main(){
	std::deque<int> a;
	a.push_back(1);
	a.push_back(2);
	a.push_back(3);
	std::deque<int>::iterator it = std::find(a.begin(), a.end(), 1);
	if (it != a.end())
	{
		a.erase(it);
	}
	std::cout << a[4] << std::endl;
}
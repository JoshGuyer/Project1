#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::string;
using std::list;
using namespace std;

#include <math.h>
#include <stdlib.h>

class nz {	    // one instance for every non-zero entry in matrix
public:
  nz(int i, double v) {	// constructor when given the two items of data
    ind = i;
    val = v;
  }
  int getInd() const { return ind; } // access to ind class member
  double getVal() const { return val; } // access to val class member
private:
  int ind;
  double val;
};

bool mycomparison (nz first, nz second) {
  return (first.getInd()<second.getInd());
}
bool same_integral_part (nz first, nz second) {
  return (first.getInd()==second.getInd());
}

typedef list<nz> sparseRow;

double epsilon = 0.0;

int main(int argc, char *argv[])
{

  string basename, graphname, xyzname;
  cin>> basename;

  graphname = xyzname = basename;
  graphname += ".osm.graph";
  xyzname +=  ".osm.xyz";

  ifstream graph(graphname);
  ifstream xyz(xyzname);
  string line; 
  while (std::getline(graph, line)) {
    if (line[0] == '%') {	// read past initial comments, if any
      //cout<< "Ignoring '"<< line<< "'..."<< endl;
      continue;
    }
    break;
  }
  std::istringstream lstream(line);
  int n, m;
  lstream>> n>> m;

  // with n and m read we can now enter main loop
  vector<list<nz>> vertices;
  int ind; 
  double val;
  string cords;
  vector<vector<double>> coordinates;
  while(getline(xyz, cords)) {
    std::istringstream lstream(cords);
    double c;
    vector<double> xy;
    while(lstream>> c) {
      xy.push_back(c);
    }
    coordinates.push_back(xy);
  }
  int v = 0;
  while (getline(graph, line))	// get next vertex and neighbors
  {
    
    list<nz> neighbors;
    std::istringstream lstream(line);
    int u;
    while (lstream>> u) {//the neighboring vertex and distance to the vertex
      ind = u;
      val = sqrt(pow((coordinates.at(u-1).at(0)-coordinates.at(v).at(0)), 2.0)+pow((coordinates.at(u-1).at(1)-coordinates.at(v).at(1)),2.0));
      neighbors.push_back(nz(ind, val));
    }
    v++;
    vertices.push_back(neighbors);
  }

  // now with all the graph read in respond to query
  int query;
  cin>> query;
  if (query == 1) { //Order and Size
    cout<< "n= "<< n<< "; m= "<< m<<"."<<endl;
  } else if (query == 2) { //Largest Degree
    int total = 0, largeVertex = 0, largeDegree = 0;
    for(int i = 0; i < vertices.size(); i++) {
      total += vertices[i].size();
      if(vertices[i].size() > largeDegree) {
        largeDegree = vertices.at(i).size();
        largeVertex = i+1;
      }
    }
    cout<< "v= "<< largeVertex<< "; |N(v)|= "<< largeDegree<<"."<<endl;
  } else if (query == 3) { //Average Degree
    int total = 0; 
    float avgDegree;
    for(int i = 0; i < vertices.size(); i++) {
      total += vertices[i].size();
    }
    avgDegree = total/(float)vertices.size();
    printf("avg |N(v)|= %1.6f.\n", avgDegree);
  } else if (query == 4) { //Neighbor Edge distance 1
    int vertex;
    cin>> vertex;
    printf("N(%d)=", vertex);
    int size = vertices.at(vertex-1).size();
    sparseRow cpy = vertices.at(vertex-1);
    for(int i = 0; i < size; i++) {
      nz tmp = cpy.front();;
      printf(" %d", tmp.getInd());
      cpy.pop_front();
    }
    printf(".\n");
  } else if (query == 5) { //Neighbor Edge distance k
    int vertex, k;
    cin>> vertex >> k;
    list<nz> neighbors;
    list<nz> current;
    neighbors=vertices.at(vertex-1);
    for(int i = 1; i < k; i++) {
      int size = neighbors.size();
      for(int j = 0; j < size; j++) {
        int v = neighbors.back().getInd()-1;
        list<nz> cpy = vertices.at(v);
        current.merge(cpy, mycomparison);
        neighbors.pop_back();
      }
      neighbors = current;
      neighbors.unique(same_integral_part);
      current.clear();
    }
    printf("N(%d,%d)=", vertex,k);
    int size = neighbors.size();
    for(int i = 0; i < size; i++) {
      printf(" %d", neighbors.front().getInd());
      neighbors.pop_front();
    }
    printf(".\n");
  } else if (query == 6) { //Shortest path 
    int v1,v2;
    cin>> v1 >> v2;
    vector<list<nz>> path;
    list<nz> level;
    double distance;
    bool found = false;
    level.push_back(nz(v1, 0));
    path.push_back(level);
    level=vertices.at(v1-1);
    path.push_back(level);
    while(!found) {
      int size = level.size();
      for(int i = 0; i < size; i++) {
        double dis = level.front().getVal();
        int ind = level.front().getInd();
        if(v2 == ind) {
          found = true;
          distance = dis;
          break;
        } else if(path.size() > 20) {
          found = true;
          distance = -1;
        }
        list<nz> cpy = vertices.at(ind-1);
        int sz = cpy.size();
        for(int j = 0; j < sz; j++) {
          level.push_back(nz(cpy.front().getInd(), cpy.front().getVal() + dis));
          if(cpy.front().getInd() == v2) {
            found = true;
            distance = cpy.front().getVal() + dis;
            break;
          }
          cpy.pop_front();
        }
        level.pop_front();
      }
      path.push_back(level);
    }
    cout.precision(6);
    cout << "d(" << v1 << "," << v2 << ")= " << distance << "." << endl;
  } else if (query == 7) { //Shortest Edge count
    int v1,v2;
    cin>> v1 >> v2;
    vector<list<nz>> path;
    list<nz> level;
    bool found = false;
    level.push_back(nz(v1, 0));
    path.push_back(level);
    level=vertices.at(v1-1);
    path.push_back(level);
    int edges;
    while(!found) {
      int size = level.size();
      for(int i = 0; i < size; i++) {
        double dis = level.front().getVal();
        int ind = level.front().getInd();
        if(v2 == ind) {
          found = true;
          edges = 1;
        } else if(path.size() > 20) {
          found = true;
        }
        list<nz> cpy = vertices.at(ind-1);
        int sz = cpy.size();
        for(int j = 0; j < sz; j++) {
          level.push_back(nz(cpy.front().getInd(), cpy.front().getVal() + dis));
          if(cpy.front().getInd() == v2) {
            found = true;
            edges = path.size();
          }
          cpy.pop_front();
        }
        level.pop_front();
      }
      path.push_back(level);
    }
    cout.precision(6);
    printf("ed(%d,%d)= %d.", v1, v2, edges);
  }
}

// STL:  http://www.csci.csusb.edu/dick/samples/stl.html

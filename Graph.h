#ifndef _GRAPH_H
#define _GRAPH_H
#include "Common.h"

#include <map>
#include <set>
using namespace std;

struct _Word
{ 
	string word;
	double weight;
};

//≈≈–ÚÀ„∑®
static bool SortByWord( const _Word &v1, const _Word &v2)
{  
	return v1.weight > v2.weight; //Ωµ–Ú≈≈¡–  
}  

typedef std::map<string, _Word> WordMap;
    
class WordGraph
{
private:
    typedef double Score;
    typedef string Node;
    typedef std::set<Node> NodeSet;

    typedef std::map<Node,double> Edges;
    typedef std::map<Node,Edges> Graph;

    double d;
    Graph graph;
    NodeSet nodeSet;
public:
    WordGraph(): d(0.85) {};
    WordGraph(double in_d): d(in_d) {};

    void addEdge(Node start,Node end,double weight)
    {
      Edges temp;
      Edges::iterator gotEdges;
      nodeSet.insert(start);
      nodeSet.insert(end);
      graph[start][end]+=weight;
      graph[end][start]+=weight;
    }

    void rank(WordMap &ws,size_t rankTime=10)
    {
      WordMap outSum;
      Score wsdef, min_rank, max_rank;

      if( graph.size() == 0)
        return;

      wsdef = 1.0 / graph.size();

      for(Graph::iterator edges=graph.begin();edges!=graph.end();++edges){
        ws[edges->first].word=edges->first;
        ws[edges->first].weight=wsdef;
        outSum[edges->first].weight=0;
        for(Edges::iterator edge=edges->second.begin();edge!=edges->second.end();++edge){
          outSum[edges->first].weight+=edge->second;
        }
      }

      for( size_t i=0; i<rankTime; i++ ){
        for(NodeSet::iterator node = nodeSet.begin(); node != nodeSet.end(); node++ ){
          double s = 0;
          for( Edges::iterator edge= graph[*node].begin(); edge != graph[*node].end(); edge++ )
            s += edge->second / outSum[edge->first].weight * ws[edge->first].weight;
          ws[*node].weight = (1 - d) + d * s;
        }
      }

      min_rank=max_rank=ws.begin()->second.weight;
      for(WordMap::iterator i = ws.begin(); i != ws.end(); i ++){
        if( i->second.weight < min_rank ){
          min_rank = i->second.weight;
        }
        if( i->second.weight > max_rank ){
          max_rank = i->second.weight;
        }
      }
      for(WordMap::iterator i = ws.begin(); i != ws.end(); i ++){
        ws[i->first].weight = (i->second.weight - min_rank / 10.0) / (max_rank - min_rank / 10.0);
      }
    }
};
#endif

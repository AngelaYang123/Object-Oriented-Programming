#include<bits/stdc++.h>
using namespace std;

int node;
vector<int> graph[50];

class Node{
  public:
    void sett(int num); //initial setup
    void BFS(); //BFS
    void buildTable(int dstID); //build the routing table
    int checkTable(int cur,int d); //table lookup

  private:
    int id;
    int level[50];
    int mark[50];
    int countt;
    queue<int> giveLevelOrder;
    int routingTable[50][50];
    void giveLevel(); //give each node level by BFS
    int smallestID(int num); //return smallest ID when tracing back
};

void Node::sett(int num){
  id=num;
  countt=0;
  for(int i=0;i<node;i++){
    level[i]=0;
    mark[i]=0;
  }
  while(!giveLevelOrder.empty())
    giveLevelOrder.pop();
}

void Node::BFS(){
  mark[id]=1;
  countt++;
  giveLevelOrder.push(id);
  giveLevel();
}

void Node::giveLevel(){
  for(int i=0;i<graph[giveLevelOrder.front()].size();i++){
    if(mark[graph[giveLevelOrder.front()][i]]==0){
      mark[graph[giveLevelOrder.front()][i]]=1;
      countt++;
      level[graph[giveLevelOrder.front()][i]]=level[giveLevelOrder.front()]+1;
      giveLevelOrder.push(graph[giveLevelOrder.front()][i]);
    }
  }
  giveLevelOrder.pop();

  if(countt!=node)
    giveLevel();
}

void Node::buildTable(int dstID){
  int temp=dstID;
  while(smallestID(temp)!=id)
    temp=smallestID(temp);
  routingTable[id][dstID]=temp;
}

int Node::smallestID(int num){
  for(int i=0;i<graph[num].size();i++)
    if(level[graph[num][i]]==level[num]-1)
      return graph[num][i];
}

int Node::checkTable(int cur,int d){
  return routingTable[cur][d];
}

int main(){
  int i,j,a,b,c;
  int link,flow,cur;
  Node Node;

  //input
  cin>>node>>link;
  for(i=0;i<link;i++){
    cin>>a>>b>>c;
    graph[b].push_back(c);
    graph[c].push_back(b);
  }

  cin>>flow;
  int source[flow],destination[flow];
  for(i=0;i<flow;i++)
    cin>>a>>source[i]>>destination[i];

  //process
  for(i=0;i<node;i++){
    Node.sett(i);
    Node.BFS();
    for(j=0;j<node;j++)
      if(j!=i)
        Node.buildTable(j);
  }

  //output
  for(i=0;i<flow;i++){
    cout<<i<<" "<<source[i]<<" ";
    cur=source[i];
    while(Node.checkTable(cur,destination[i])!=destination[i]){
      cout<<Node.checkTable(cur,destination[i])<<" ";
      cur=Node.checkTable(cur,destination[i]);
    }
    cout<<destination[i]<<endl;
  }

  return 0;
}

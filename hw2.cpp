#include<bits/stdc++.h>
using namespace std;

class Node{
  public:
    Node(){};
    void getGraph(int num);
    vector<int> graph;
    int active = -1; //MIS
    int CDS = 0;
    int level = -1; //BFS
    int marked = 0; //BFS
    int* table; //routing table for CDS
};

void Node::getGraph(int num){
    graph.push_back(num);
}

void MIS(Node* node, int node_num){
    int i, j;

    for(i=0;i<node_num;i++){
        if(node[i].active != -1)
            continue;
        //check the neighbors
        for(j=0;j<node[i].graph.size();j++){
            if(node[node[i].graph[j]].active == 0)
                continue;
            if(i > node[i].graph[j])
                break;
        }
        //smaller ID than its neighbors
        if(j == node[i].graph.size()){
            node[i].active = 1;
            node[i].CDS = 1;
            for(j=0;j<node[i].graph.size();j++)
                node[node[i].graph[j]].active = 0;
        }
    }

    for(i=0;i<node_num;i++)
        if(node[i].active == -1)
            MIS(node, node_num);

    return;
}

void giveLevel(Node* node, int node_num, queue<int> giveLevelOrder, int countt){
    int i;

    for(i=0;i<node[giveLevelOrder.front()].graph.size();i++){
        if(node[node[giveLevelOrder.front()].graph[i]].marked == 0){
            node[node[giveLevelOrder.front()].graph[i]].marked = 1;
            countt++;
            node[node[giveLevelOrder.front()].graph[i]].level = node[giveLevelOrder.front()].level+1;
            giveLevelOrder.push(node[giveLevelOrder.front()].graph[i]);
        }
    }
    giveLevelOrder.pop();

    if(countt != node_num)
        giveLevel(node, node_num, giveLevelOrder, countt);

    return;
}

void BFS(Node* node, int node_num, int start){
    queue<int> giveLevelOrder;
    int i, countt = 0;
    for(i=0;i<node_num;i++){
        node[i].level = -1;
        node[i].marked = 0;
    }
    node[start].level = 0;
    node[start].marked = 1;
    countt++;
    while(!giveLevelOrder.empty())
        giveLevelOrder.pop();

    giveLevelOrder.push(start);
    giveLevel(node, node_num, giveLevelOrder, countt);

    return;
}

void AODV_trackback(Node* node, int s, int d){
    int i;
    //the visited nodes join the CDS
    for(i=0;i<node[d].graph.size();i++)
        if(node[node[d].graph[i]].level == node[d].level-1){
            node[node[d].graph[i]].CDS = 1;
            break;
        }
    if(s != node[d].graph[i])
        AODV_trackback(node, s, node[d].graph[i]);

    return;
}

void AODV(Node* node, int node_num){
    int i, j;

    for(i=0;i<node_num;i++){
        if(node[i].active == 0)
            continue;
        BFS(node, node_num, i);
        //connect two nodes in the CDS with distance less than 3
        for(j=0;j<node_num;j++)
            if(node[j].level > 1 && node[j].level <= 3 && node[j].active == 1)
                AODV_trackback(node, i, j);
    }

    return;
}

int smallestID(Node* node, int temp){
    int i, smallest;

    for(i=0;i<node[temp].graph.size();i++)
        if(node[node[temp].graph[i]].level == node[temp].level-1 && node[node[temp].graph[i]].CDS == 1){
            smallest = node[temp].graph[i];
            return smallest;
        }

    return 0;
}

void buildTable(Node* node, int node_num){
    int i, j, temp;

    for(i=0;i<node_num;i++)
        if(node[i].CDS == 1){
            BFS(node, node_num, i);
            node[i].table = new int[node_num];
            for(j=0;j<node_num;j++){
                node[i].table[j] = -1;
                if(node[j].CDS == 0)
                    continue;
                if(i == j){
                    node[i].table[j] = j;
                    continue;
                }
                //track back
                temp = j;
                while(smallestID(node, temp) != i)
                    temp = smallestID(node, temp);
                node[i].table[j] = temp;
            }
        }

    return;
}

int find_proxy(Node* node, int num){
    int i, ans;

    for(i=0;i<node[num].graph.size();i++)
        if(node[node[num].graph[i]].CDS == 1){
            ans = node[num].graph[i];
            return ans;
        }

    return 0;
}

void output(Node* node, int i, int s, int d){
    int proxy_s, proxy_d, cur;

    cout<<i<<" ";
    proxy_s=s;proxy_d=d;
    if(node[s].CDS == 0)
        proxy_s = find_proxy(node, s);
    if(node[d].CDS == 0)
        proxy_d = find_proxy(node, d);

    if(proxy_s == proxy_d){
        if(node[s].CDS == 0 && node[d].CDS != 0)
            cout<<s<<" "<<proxy_s<<endl;
        else if(node[s].CDS != 0 && node[d].CDS == 0)
            cout<<proxy_d<<" "<<d<<endl;
        else
            cout<<s<<" "<<proxy_s<<" "<<d<<endl;
        return;
    }

    cur = proxy_s;
    if(node[s].CDS == 0)
        cout<<s<<" ";
    cout<<cur<<" ";
    while(node[cur].table[proxy_d]!=proxy_d){
        cur = node[cur].table[proxy_d];
        cout<<cur<<" ";
    }
    cout<<proxy_d;
    if(node[d].CDS == 0)
        cout<<" "<<d;
    cout<<endl;

    return;
}

int main(){
    int i, a, b, c;
    int node_num, link, flow;

    /*fstream inputStream;
    inputStream.open("input1.txt");*/

    //input
    cin>>node_num>>link;
    Node node[node_num];
    for(i=0;i<link;i++){
        cin>>a>>b>>c;
        node[b].getGraph(c);
        node[c].getGraph(b);
    }

    cin>>flow;
    int source[flow],destination[flow];
    for(i=0;i<flow;i++)
        cin>>a>>source[i]>>destination[i];

    //process
    MIS(node, node_num);
    AODV(node, node_num);
    buildTable(node, node_num);

    //output
    for(i=0;i<flow;i++)
        output(node, i, source[i], destination[i]);

    for(i=0;i<node_num;i++)
        if(node[i].CDS == 1)
            delete[] node[i].table;

    return 0;
}

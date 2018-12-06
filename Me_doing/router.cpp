#include <bits/stdc++.h>
#include <string>

using namespace std; // @suppress("Symbol is not resolved")

struct table_entry
{
    string dest;
    string nexthop;
    int cost;
    table_entry();

};

table_entry::table_entry()
{
    dest="";
    nexthop="";
    cost=0;
}
string self_ip;
set<string>total;
set<string>conn;
set<string>disconn;
vector<table_entry>routing_table;

int sockfd;
int bind_flag;
int bytes_received;
socklen_t addrlen;
char receive_buffer[1024];
char sending_buffer[1024];
struct sockaddr_in receiving_address;
struct sockaddr_in sending_address;
struct sockaddr not_sure;
socklen_t not_sure_len;
void print_table()
{
    cout<<setw(15)<<"destination"<<setw(15)<<"next hop"<<setw(10)<<"cost"<<endl;
    cout<<setw(15)<<"-----------"<<setw(15)<<"--------"<<setw(10)<<"-----"<<endl;
    //routing_table[0].dest="";routing_table[0].nexthop=" ";routing_table[0].cost=1;
    for(int i=0; i<routing_table.size(); i++)
    {
        if(routing_table[i].dest==self_ip)continue;
        cout<<setw(15)<<routing_table[i].dest<<setw(15)<<routing_table[i].nexthop<<setw(10)<<routing_table[i].cost<<"\n";
    }
}


int main(int argc, char *argv[])
{

    if(argc!=3)
    {
        printf("Give <topology><space><router_address>");
        return 0;
    }
    //while(true)
    ifstream infile(argv[1]);
    self_ip=string(argv[2]);
    string src,dest;
    int cst;
    //cout<<self_ip<<endl;
    table_entry temp;
    conn.insert(self_ip);





    receiving_address.sin_family = AF_INET; //
    receiving_address.sin_port = htons(4747);
    receiving_address.sin_addr.s_addr = inet_addr(argv[2]);

    sockfd=socket(AF_INET,SOCK_GRAM,0);
    bind_flag=bind(sockfd,(struct sockaddr*) &receiving_address,sizeof(struct sockaddr_in));
    while(infile>>src>>dest>>cst)
    {
        //cout<<src <<" "<<dest<<" "<<cst<<endl;
        total.insert(src);
        total.insert(dest);
        if(src==self_ip)
        {
            //cout<<1<<endl;
            temp.dest=dest;
            temp.nexthop=dest;
            temp.cost=cst;
            //cout<<temp.dest<<temp.nexthop<<temp.cost<<endl;
            routing_table.push_back(temp);
            //conn.insert(src);
            conn.insert(dest);
        }
        else if(dest==self_ip)
        {
            temp.dest=src;
            temp.nexthop=src;
            temp.cost=cst;
            routing_table.push_back(temp);
            conn.insert(src);
            //conn.insert(dest);
        }

    }
    temp.dest=self_ip;
    temp.nexthop=self_ip;
    temp.cost=0;
    routing_table.push_back(temp);

    set_difference(total.begin(),total.end(),conn.begin(),conn.end(), inserter(disconn,disconn.end()));

    std::set<string>::iterator it;
    for (it = disconn.begin(); it !=disconn.end(); ++it)
    {
        //cout<<*it<<endl;
        temp.dest=*it;
        temp.nexthop="-";
        temp.cost=10000;
        routing_table.push_back(temp); // Note the "*" here
        //if();
    }

    print_table();
    //cout<<total.size()<<endl;
    while(true)
    {
        bytes_received=recvfrom(sockfd,recieve_buffer,1024,0,(struct sockaddr*) &not_sure,not_sure_len);
    }
    //total.size();
    return 0;
}




/*#include <bits/stdc++.h>
#include <string>

using namespace std; // @suppress("Symbol is not resolved")

struct table_entry
{
    string dest;
    string nexthop;
    int cost;
    table_entry();

};

table_entry::table_entry()
{
    dest="";
    nexthop="";
    cost=0;
}
string self_ip;
set<string>total;
set<string>conn;
set<string>disconn;
vector<table_entry>routing_table;
int main(int argc, char *argv[])
{
    if(argc!=3)
    {
        printf("Give <topology><space><router_address>");
        return 0;
    }
    //while(true)
    ifstream infile(argv[1]);
    self_ip=string(argv[2]);
    string src,dest;
    int cst;
    //cout<<self_ip<<endl;
    table_entry temp;
    conn.insert(self_ip);
    while(infile>>src>>dest>>cst)
    {
        //cout<<src <<" "<<dest<<" "<<cst<<endl;
        total.insert(src);
        total.insert(dest);
        if(src==self_ip)
        {
            //cout<<1<<endl;
            temp.dest=dest;
            temp.nexthop=dest;
            temp.cost=cst;
            //cout<<temp.dest<<temp.nexthop<<temp.cost<<endl;
            routing_table.push_back(temp);
            //conn.insert(src);
            conn.insert(dest);
        }
        else if(dest==self_ip)
        {
            temp.dest=src;
            temp.nexthop=src;
            temp.cost=cst;
            routing_table.push_back(temp);
            conn.insert(src);
            //conn.insert(dest);
        }

    }
    temp.dest=self_ip;
    temp.nexthop=self_ip;
    temp.cost=0;
    routing_table.push_back(temp);

    set_difference(total.begin(),total.end(),conn.begin(),conn.end(), inserter(disconn,disconn.end()));

    std::set<string>::iterator it;
    for (it = disconn.begin(); it !=disconn.end(); ++it)
    {
       //cout<<*it<<endl;
       temp.dest=*it;
       temp.nexthop="-";
       temp.cost=10000;
       routing_table.push_back(temp); // Note the "*" here
       //if();
    }
    cout<<setw(15)<<"destination"<<setw(15)<<"next hop"<<setw(10)<<"cost"<<endl;
    cout<<setw(15)<<"-----------"<<setw(15)<<"--------"<<setw(10)<<"-----"<<endl;
    //routing_table[0].dest="";routing_table[0].nexthop=" ";routing_table[0].cost=1;
    for(int i=0; i<routing_table.size(); i++)
    {
        if(routing_table[i].dest==self_ip)continue;
        cout<<setw(15)<<routing_table[i].dest<<setw(15)<<routing_table[i].nexthop<<setw(10)<<routing_table[i].cost<<"\n";
    }

    cout<<total.size()<<endl;
    //total.size();
    return 0;
}
*/

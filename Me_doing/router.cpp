#include <bits/stdc++.h>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
using namespace std; // @suppress("Symbol is not resolved")

struct table_entry
{
    string dest;
    string nexthop;
    int cost;
    table_entry();

};
int clocklast[256];
table_entry::table_entry()
{
    dest="";
    nexthop="";
    cost=0;
}

int cost_array[256][256];
table_entry hop_array[256][256];
string self_ip;
set<string>total;
set<string>conn;
set<string>disconn;
vector<table_entry>routing_table;
vector<table_entry>neighbours;
//vector<routing_table>neighbours_tables;
vector<table_entry>neighbours_tables(256);
int * buff;
short * sh;
int sockfd;
int bind_flag;
int bytes_received;
socklen_t addrlen;
char receive_buffer[1024];
char sending_buffer[1024];
struct sockaddr_in receiving_address;
struct sockaddr_in sending_address;
struct sockaddr_in not_sure;
struct sockaddr_in temporary_socket;
socklen_t not_sure_len;
char router_1_address[1024];
char router_2_address[1024];
char *pointing;
int n,temp_cost=0;
string sender_ip;
int self_id;
int getid(string ip)
{
    string h;
    stringstream ch(ip);
    while(getline(ch,h,'.'))
    {

    }
    //cout<<h<<endl;
    int res=stoi(h);
    return res;
}
void print_table()
{
    cout<<setw(15)<<"destination"<<setw(15)<<"next hop"<<setw(10)<<"cst"<<endl;
    cout<<setw(15)<<"-----------"<<setw(15)<<"--------"<<setw(10)<<"-----"<<endl;
    //routing_table[0].dest="";routing_table[0].nexthop=" ";routing_table[0].cost=1;
    for(int i=0; i<routing_table.size(); i++)
    {
        if(routing_table[i].dest==self_ip)continue;
        cout<<setw(15)<<routing_table[i].dest<<setw(15)<<routing_table[i].nexthop<<setw(10)<<routing_table[i].cost<<"\n";
    }
}

int clockno=0;
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

    for(int i=0;i<256;i++)
    {
        for(int j=0;j<256;j++)
        {
            cost_array[i][j]=10000;
        }
        cost_array[i][i]=0;
    }



    receiving_address.sin_family = AF_INET; //
    receiving_address.sin_port = htons(4747);
    receiving_address.sin_addr.s_addr = inet_addr(argv[2]);

    sockfd=socket(AF_INET,SOCK_DGRAM,0);
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
            neighbours.push_back(temp);
        }
        else if(dest==self_ip)
        {
            temp.dest=src;
            temp.nexthop=src;
            temp.cost=cst;
            routing_table.push_back(temp);
            neighbours.push_back(temp);
            conn.insert(src);
            //conn.insert(dest);
        }

    }
    temp.dest=self_ip;
    temp.nexthop=self_ip;
    temp.cost=0;
    routing_table.push_back(temp);
    neighbours.push_back(temp);
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
    n=total.size();
    self_id=getid(self_ip);
    for(int i=0;i<neighbours.size();i++)
    {
        int te=getid(neighbours[i].dest);
        cost_array[self_id][te]=neighbours[i].cost;
        //cost_array[te][self_id]=neighbours[i].cost;

    }


    char header[4];
    while(true)
    {
        bytes_received=recvfrom(sockfd,receive_buffer,1024,0,(struct sockaddr*) &not_sure,&not_sure_len);
        string receive_from(inet_ntoa(not_sure.sin_addr));
        cout<<receive_from<<endl;
        strncpy(header, receive_buffer,4);
        string action(header);
        //printf("%s\n", header);
        cout<<action<<endl;
        if(action=="show")
        {
            print_table();
        }
        else if(action=="cost")
        {
            in_addr r1,r2;
            buff=static_cast<int *>(static_cast<void *>(receive_buffer+4));
            r1.s_addr=*buff;
            pointing=inet_ntoa(r1);
            strcpy(router_1_address,pointing);
            //printf("%s\n", router_1_address);
            buff=static_cast<int *>(static_cast<void *>(receive_buffer+8));
            r2.s_addr=*buff;
            pointing=inet_ntoa(r2);
            printf("%d\n", temp_cost);
            strcpy(router_2_address,pointing);
            printf("%s\n", router_2_address);
            printf("%s\n", router_1_address);
            sh=static_cast<short *>(static_cast<void *>(receive_buffer+12));
            //printf("%d\n", *buff);
            temp_cost=*sh;
            printf("%d\n", temp_cost);
            string go;
            if(string(router_1_address)==self_ip)
            {
                go=string(router_2_address);
            }
            else
            {
                go=string(router_1_address);
            }
            cout<<"go "<<go<<endl;
            for(int i=0;i<neighbours.size();i++)
            {
                if(neighbours[i].dest==go)
                {
                    neighbours[i].cost=temp_cost;
                }

            }




        }
        else if(action=="frwd")         ///SEMI TESTED
        {
            char message[1024];
            in_addr r1,r2;
            buff=static_cast<int *>(static_cast<void *>(receive_buffer+4));
            r1.s_addr=*buff;
            pointing=inet_ntoa(r1);
            strcpy(router_1_address,pointing);
            //printf("%s\n", router_1_address);
            printf("%s\n", router_1_address);
            sh=static_cast<short *>(static_cast<void *>(receive_buffer+8));
            //printf("%d\n", *buff);
            temp_cost=*sh;
            printf("%dH\n", temp_cost);
            buff=static_cast<int *>(static_cast<void *>(receive_buffer+10));
            pointing=(char *)buff;
            strcpy(message,pointing);
            printf("%s\n", message);
            string go,msgstr;
            go=string(router_1_address);
            msgstr=string(message);
            if(go==self_ip)
            {
                cout<<msgstr<<" packet reached destination"<<endl;
            }
            else
            {
                for(int i=0;i<routing_table.size();i++)
                {
                    if(routing_table[i].dest==go)
                    {
                        char msgchar[1024];
                        memcpy(msgchar,"frwd",4);
                        char ipad[1024];
                        strcpy(ipad,routing_table[i].dest.c_str());
                        printf("%s\n", ipad);
                        temporary_socket.sin_addr.s_addr=inet_addr(ipad);
                        memcpy(msgchar+4,&temporary_socket.sin_addr.s_addr,4);
                        memcpy(msgchar+8,sh,2);
                        memcpy(msgchar+10,message,temp_cost);

                        strcpy(ipad,routing_table[i].nexthop.c_str());
                        //printf("%s\n", ipad);
                        temporary_socket.sin_addr.s_addr=inet_addr(ipad);
                        temporary_socket.sin_port=htons(4747);
                        temporary_socket.sin_family=AF_INET;
                        sendto(sockfd,msgchar,1024,0,(struct sockaddr*) &temporary_socket,sizeof(sockaddr_in));
                        cout<<msgstr<<" packet forwarded to "<<routing_table[i].nexthop<<endl;

                    }
                }
            }


        }
        else if(action=="send") //TESTED
        {
            char message[1024];
            in_addr r1,r2;
            buff=static_cast<int *>(static_cast<void *>(receive_buffer+4));
            r1.s_addr=*buff;
            pointing=inet_ntoa(r1);
            strcpy(router_1_address,pointing);
            //printf("%s\n", router_1_address);
            buff=static_cast<int *>(static_cast<void *>(receive_buffer+8));
            r2.s_addr=*buff;
            pointing=inet_ntoa(r2);
            printf("%d\n", temp_cost);
            strcpy(router_2_address,pointing);
            printf("%s\n", router_2_address);
            printf("%s\n", router_1_address);
            sh=static_cast<short *>(static_cast<void *>(receive_buffer+12));
            //printf("%d\n", *buff);
            temp_cost=*sh;
            printf("%dH\n", temp_cost);
            buff=static_cast<int *>(static_cast<void *>(receive_buffer+14));
            pointing=(char *)buff;
            strcpy(message,pointing);
            printf("%s\n", message); 
            /* -----------------------
            */
            string go,msgstr;
            go=string(router_2_address);
            msgstr=string(message);
            for(int i=0;i<routing_table.size();i++)
            {
                if(routing_table[i].dest==go)
                {
                        char msgchar[1024];
                        memcpy(msgchar,"frwd",4);
                        char ipad[1024];
                        strcpy(ipad,routing_table[i].dest.c_str());
                        //printf("ipad %s\n", ipad);
                        temporary_socket.sin_addr.s_addr=inet_addr(ipad);
                        memcpy(msgchar+4,&temporary_socket.sin_addr.s_addr,4);
                        memcpy(msgchar+8,sh,2);
                        memcpy(msgchar+10,message,temp_cost);

                        strcpy(ipad,routing_table[i].nexthop.c_str());
                        temporary_socket.sin_addr.s_addr=inet_addr(ipad);
                        temporary_socket.sin_port=htons(4747);
                        temporary_socket.sin_family=AF_INET;
                        sendto(sockfd,msgchar,1024,0,(struct sockaddr*) &temporary_socket,sizeof(sockaddr_in));
                        cout<<msgstr<<" packet forwarded to "<<routing_table[i].nexthop<<endl;

                }
            }




        }
        else if(action=="rout")
        {
            sender_ip=receive_from;
            int x=getid(sender_ip);
            clocklast[x]=clockno;
            for(int i=0;i<n;i++)
            {
                in_addr r1,r2;
                buff=static_cast<int *>(static_cast<void *>(receive_buffer+4+8*i));
                r1.s_addr=*buff;
                pointing=inet_ntoa(r1);
                strcpy(router_1_address,pointing);
                buff=static_cast<int *>(static_cast<void *>(receive_buffer+8+8*i));
                //printf("%d\n", *buff);
                temp_cost=*buff;
                //printf("%d\n", temp_cost);
                int y=getid(string(router_1_address));
                cost_array[x][y]=temp_cost;
            }
            printf("JUST RECEIVED PACKET FROM %d. ME %d\n", x,self_id);
/*            for(int i=1;i<=n;i++)
            {
                for(int j=1;j<=n;j++)
                {
                    printf("%d ", cost_array[i][j]);
                }
                printf("\n");
            }
*/

        }
        else if(action=="clk ")
        {

            for(int i=1;i<=n;i++)
            {
                for(int j=1;j<=n;j++)
                {
                    printf("%d ", cost_array[i][j]);
                }
                printf("\n");
            }

            //buff=static_cast<int *>(static_cast<void *>(receive_buffer+4));
                //printf("%d\n", *buff);
            clockno++;
            printf("%d\n", clockno);
            printf("Khelbo\n");
            for(int i=0;i<neighbours.size();i++)
            {
                if(neighbours[i].dest==self_ip)
                {
                    continue;
                }
                int idx=getid(neighbours[i].dest);
                printf("%d\n", clocklast[idx]);
                int period=clocklast[idx];
                if(clockno - clocklast[idx] > 3)
                {
                    printf("clockless for 3\n");
                    neighbours[i].cost=10000;
                }

            }

            /*
            for(int i=1;i<=n;i++)
            {
                cost_array[self_id][i]=10000;
                int maxv=10000;
                for(int j=0;j<neighbours.size();j++)
                {
                    int v=getid(neighbours[j].dest);
                    cout<<neighbours[j].dest<<" "<<neighbours[j].cost<<" "<<v<<endl;
                    cost_array[self_id][i]=min(neighbours[j].cost+cost_array[v][i],cost_array[self_id][i]);
                }
            }
            */
            for(int i=0;i<neighbours.size();i++)
            {
                cout<<"neigh "<<neighbours[i].dest<<neighbours[i].cost<<endl;
            }
            
            for(int i=0;i<routing_table.size();i++)
            {

                int maxv=10000;
                int x=self_id;
                string nh="";
                int y=getid(routing_table[i].dest);
                for(int j=0;j<neighbours.size();j++)
                {
                    int v=getid(neighbours[j].dest);
                    if((neighbours[j].cost + cost_array[v][y]) < maxv)
                    {
                        //cout<<"j "<<j<<v<<i<<endl;
                        maxv=neighbours[j].cost+cost_array[v][y];
                        nh=neighbours[j].dest;
                        routing_table[i].nexthop=nh;
                        routing_table[i].cost=maxv;
                    }

                }

            }
            print_table();
            printf("--------------------------------\n");
            for(int i=1;i<=n;i++)
            {
                for(int j=1;j<=n;j++)
                {
                    printf("%d ", cost_array[i][j]);
                }
                printf("\n");
            }
            printf("--------------------------------\n");

            /*------------------------------------------------ YET WORK BAKI FOR SENDING TABLE*/

            for(int t=0;t<neighbours.size();t++)
            {
                if(neighbours[t].dest==self_ip)
                {
                    continue;
                }
                char msgchar[2048];
                char ipad[1024];
                memcpy(msgchar,"rout",4);
                for(int i=0;i<routing_table.size();i++)
                {
                        strcpy(ipad,routing_table[i].dest.c_str());
                        int ca=routing_table[i].cost;
                        if(routing_table[i].nexthop==neighbours[t].dest)
                        {
                            ca=10000;
                        }
                        //printf("ipad %s\n", ipad);
                        temporary_socket.sin_addr.s_addr=inet_addr(ipad);
                        memcpy(msgchar+4+8*i,&temporary_socket.sin_addr.s_addr,4);
                        memcpy(msgchar+8+8*i,&ca,4);

                }
                strcpy(ipad,neighbours[t].dest.c_str());
                temporary_socket.sin_addr.s_addr=inet_addr(ipad);
                temporary_socket.sin_port=htons(4747);
                temporary_socket.sin_family=AF_INET;
                sendto(sockfd,msgchar,2048,0,(struct sockaddr*) &temporary_socket,sizeof(sockaddr_in));
                cout<<"sending table to "<<neighbours[t].dest<<" from "<<self_ip<<endl;

            }




        }
        
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

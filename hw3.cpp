#include <iostream>
#include <map>
#include <queue>
#include <utility>
#include <climits>
#include <functional>
#include <iomanip>
#include <stack>
#include <fstream>
#include <string>

using namespace std;

#define SET(func_name,type,var_name,_var_name) void func_name(type _var_name) { var_name = _var_name ;} 
#define GET(func_name,type,var_name) type func_name() const { return var_name ;}

class header; //the cover of packet
class payload; //the content of packet
class packet; //composed of header and payload
class node;
class event;

// for simplicity, we use a const int to simulate the delay
// if you want to simulate the more details, you should revise it to be a class
const unsigned int ONE_HOP_DELAY = 10;
const unsigned int BROCAST_ID = UINT_MAX;

// BROCAST_ID means that all neighbors are receivers; UINT_MAX is the maximum value of unsigned int

class header {
    public:
        virtual ~header() {}

        SET(setSrcID, unsigned int , srcID, _srcID);
        SET(setDstID, unsigned int , dstID, _dstID);
        SET(setPreID, unsigned int , preID, _preID);
        SET(setNexID, unsigned int , nexID, _nexID);
        GET(getSrcID, unsigned int , srcID);
        GET(getDstID, unsigned int , dstID);
        GET(getPreID, unsigned int , preID);
        GET(getNexID, unsigned int , nexID);
        
        virtual string type() = 0;
        
        // factory concept: generate a header
        class header_generator {
                // lock the copy constructor
                header_generator(header_generator &){}
                // store all possible types of header
                static map<string,header_generator*> prototypes;
            protected:
                // allow derived class to use it
                header_generator() {}
                // after you create a new header type, please register the factory of this header type by this function
                void register_header_type(header_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your header
                virtual header* generate() = 0 ;
            public:
                // you have to implement your own type() to return your header type
        	    virtual string type() = 0 ;
        	    // this function is used to generate any type of header derived
        	    static header * generate (string type) {
            		if(prototypes.find(type) != prototypes.end()){ // if this type derived exists 
            			return prototypes[type]->generate(); // generate it!!
            		}
            		std::cerr << "no such header type" << std::endl; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered header types: " << endl;
            	    for (map<string,header::header_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++)
            	        cout << it->second->type() << endl;
            	}
            	virtual ~header_generator(){};
        };
        
    protected:
        header():srcID(BROCAST_ID),dstID(BROCAST_ID),preID(BROCAST_ID),nexID(BROCAST_ID){} // this constructor cannot be directly called by users

    private:
        unsigned int srcID;
        unsigned int dstID;
        unsigned int preID;
        unsigned int nexID;
        header(header&){} // this constructor cannot be directly called by users
};
map<string,header::header_generator*> header::header_generator::prototypes;

class LS3D_header : public header{
        bool isPub; // is this header for pub?
        
        stack<unsigned int> DFS_path;
        map<unsigned int,bool> isVisited;
        bool isHilltopOnce; // does the packet visit a hilltop once?
        LS3D_header(LS3D_header&){}
        bool hilltopMode;

    protected:
        LS3D_header(){ isHilltopOnce = false; hilltopMode = true; } // this constructor cannot be directly called by users

    public:
        ~LS3D_header(){}
        
        SET(setIsPub,bool,isPub,_isPub);
        GET(getIsPub,bool,isPub);
        
        SET(setIsHilltopOnce,bool,isHilltopOnce,_isHilltopOnce);
        GET(getIsHilltopOnce,bool,isHilltopOnce);

        SET(setIsHilltopMode,bool,hilltopMode,_hilltopMode);
        GET(getIsHilltopMode,bool,hilltopMode);

        void push_visited_node (unsigned int n_id) { DFS_path.push(n_id); }
        unsigned int pop_visited_node () {
            if ( DFS_path.empty() ) // if the stack is empty, then return BROCAST_ID
                return BROCAST_ID;
            else {
                DFS_path.pop();
                if(DFS_path.empty()){return BROCAST_ID;}
                unsigned int temp = DFS_path.top ();
                DFS_path.pop();
                return temp; 
            }
        }
        void mark_visited_node (unsigned int n_id) { isVisited[n_id] = true; }
        bool check_visited_node (unsigned int n_id) { return (isVisited.find(n_id) != isVisited.end()) ; }
        

        string type() { return "LS3D_header"; }

        class LS3D_header_generator;
        friend class LS3D_header_generator;
        // LS3D_header is derived from header_generator to generate a header
        class LS3D_header_generator : public header_generator{
                static LS3D_header_generator sample;
                // this constructor is only for sample to register this header type
                LS3D_header_generator() { /*cout << "LS3D_header registered" << endl;*/ register_header_type(&sample); }
            protected:
                virtual header * generate(){
                    // cout << "LS3D_header generated" << endl;
                    return new LS3D_header; 
                }
            public:
                virtual string type() { return "LS3D_header";}
                ~LS3D_header_generator(){}
        
        };
};
LS3D_header::LS3D_header_generator LS3D_header::LS3D_header_generator::sample;

/***********************************************************************************/

class payload {
        payload(payload&){} // this constructor cannot be directly called by users
    protected:
        payload(){}
    public:
        virtual ~payload(){}
        virtual string type() = 0;
        
        class payload_generator {
                // lock the copy constructor
                payload_generator(payload_generator &){}
                // store all possible types of header
                static map<string,payload_generator*> prototypes;
            protected:
                // allow derived class to use it
                payload_generator() {}
                // after you create a new payload type, please register the factory of this payload type by this function
                void register_payload_type(payload_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your payload
                virtual payload* generate() = 0;
            public:
                // you have to implement your own type() to return your header type
        	    virtual string type() = 0;
        	    // this function is used to generate any type of header derived
        	    static payload * generate (string type) {
            		if(prototypes.find(type) != prototypes.end()){ // if this type derived exists 
            			return prototypes[type]->generate(); // generate it!!
            		}
            		std::cerr << "no such payload type" << std::endl; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered payload types: " << endl;
            	    for (map<string,payload::payload_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++)
            	        cout << it->second->type() << endl;
            	}
            	virtual ~payload_generator(){};
        };
};
map<string,payload::payload_generator*> payload::payload_generator::prototypes;


class LS3D_payload : public payload {
        LS3D_payload(LS3D_payload&){}
        unsigned int hostID;
        unsigned int proxyID;
    protected:
        LS3D_payload(){} // this constructor cannot be directly called by users
    public:
        ~LS3D_payload(){}
        
        SET(setHostID,unsigned int,hostID,_hostID);
        GET(getHostID,unsigned int,hostID);
        SET(setProxyID,unsigned int,proxyID,_proxyID);
        GET(getProxyID,unsigned int,proxyID);
        
        string type() { return "LS3D_payload"; }
        
        class LS3D_payload_generator;
        friend class LS3D_payload_generator;
        // LS3D_payload is derived from payload_generator to generate a payload
        class LS3D_payload_generator : public payload_generator{
                static LS3D_payload_generator sample;
                // this constructor is only for sample to register this payload type
                LS3D_payload_generator() { /*cout << "LS3D_payload registered" << endl;*/ register_payload_type(&sample); }
            protected:
                virtual payload * generate(){ 
                    // cout << "LS3D_payload generated" << endl;
                    return new LS3D_payload; 
                }
            public:
                virtual string type() { return "LS3D_payload";}
                ~LS3D_payload_generator(){}
        };
};
LS3D_payload::LS3D_payload_generator LS3D_payload::LS3D_payload_generator::sample;

/***********************************************************************************/

class packet{
        // a packet usually contains a header and a payload
        header *hdr;
        payload *pld;
        unsigned int p_id;
        static unsigned int last_packet_id ; 
        
        packet(packet &) {}
        static int live_packet_num ;
    protected:
        // these constructors cannot be directly called by users
        packet(): hdr(nullptr), pld(nullptr) { p_id=last_packet_id++; live_packet_num ++; }
        packet(string _hdr, string _pld, bool rep = false, unsigned int rep_id = 0) {
            if (! rep ) // a duplicated packet does not have a new packet id
                p_id = last_packet_id ++;
            else
                p_id = rep_id;
            hdr = header::header_generator::generate(_hdr); 
            pld = payload::payload_generator::generate(_pld); 
            live_packet_num ++;
        }
    public:
        virtual ~packet(){ 
            // cout << "packet destructor begin" << endl;
            if (hdr != nullptr) 
                delete hdr; 
            if (pld != nullptr) 
                delete pld; 
            live_packet_num --;
            // cout << "packet destructor end" << endl;
        }
        
        SET(setHeader,header*,hdr,_hdr);
        GET(getHeader,header*,hdr);
        SET(setPayload,payload*,pld,_pld);
        GET(getPayload,payload*,pld);
        GET(getPacketID,unsigned int,p_id);
        
        static void discard ( packet* &p ) {
            // cout << "checking" << endl;
            if (p != nullptr) {
                // cout << "discarding" << endl;
                // cout << p->type() << endl;
                delete p;
                // cout << "discarded" << endl;
            }
            p = nullptr;
            // cout << "checked" << endl;
        }
        virtual string type () = 0;
        
        static int getLivePacketNum () { return live_packet_num; }
        
        class packet_generator;
        friend class packet_generator;
        class packet_generator {
                // lock the copy constructor
                packet_generator(packet_generator &){}
                // store all possible types of packet
                static map<string,packet_generator*> prototypes;
            protected:
                // allow derived class to use it
                packet_generator() {}
                // after you create a new packet type, please register the factory of this payload type by this function
                void register_packet_type(packet_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your payload
                virtual packet* generate ( packet *p = nullptr) = 0;
            public:
                // you have to implement your own type() to return your packet type
        	    virtual string type() = 0;
        	    // this function is used to generate any type of packet derived
        	    static packet * generate (string type) {
            		if(prototypes.find(type) != prototypes.end()){ // if this type derived exists 
            			return prototypes[type]->generate(); // generate it!!
            		}
            		std::cerr << "no such packet type" << std::endl; // otherwise
            		return nullptr;
            	}
            	static packet * replicate (packet *p) {
            	    if(prototypes.find(p->type()) != prototypes.end()){ // if this type derived exists 
            			return prototypes[p->type()]->generate(p); // generate it!!
            		}
            		std::cerr << "no such packet type" << std::endl; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered packet types: " << endl;
            	    for (map<string,packet::packet_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++)
            	        cout << it->second->type() << endl;
            	}
            	virtual ~packet_generator(){};
        };
};
map<string,packet::packet_generator*> packet::packet_generator::prototypes;
unsigned int packet::last_packet_id = 0 ;
int packet::live_packet_num = 0;


// this packet is used to tell the storage node the proxy id of the node with hostID
class LS3D_packet: public packet {
        LS3D_packet(LS3D_packet &) {}
        LS3D_header *hdr;

    protected:
        LS3D_packet(){} // this constructor cannot be directly called by users
        LS3D_packet(packet*p): packet(p->getHeader()->type(), p->getPayload()->type(), true, p->getPacketID()) {
            *(dynamic_cast<LS3D_header*>(this->getHeader())) = *(dynamic_cast<LS3D_header*> (p->getHeader()));
            *(dynamic_cast<LS3D_payload*>(this->getPayload())) = *(dynamic_cast<LS3D_payload*> (p->getPayload()));
            //DFS_path = (dynamic_cast<LS3D_header*>(p))->DFS_path;
            //isVisited = (dynamic_cast<LS3D_header*>(p))->isVisited;
        } // for duplicate
        LS3D_packet(string _h, string _p): packet(_h,_p) {}
        
    public:
        virtual ~LS3D_packet(){}
        string type() { return "LS3D_packet"; }
        
        class LS3D_packet_generator;
        friend class LS3D_packet_generator;
        // LS3D_packet is derived from packet_generator to generate a pub packet
        class LS3D_packet_generator : public packet_generator{
                static LS3D_packet_generator sample;
                // this constructor is only for sample to register this packet type
                LS3D_packet_generator() { /*cout << "LS3D_packet registered" << endl;*/ register_packet_type(&sample); }
            protected:
                virtual packet *generate (packet *p = nullptr){
                    // cout << "LS3D_packet generated" << endl;
                    if ( nullptr == p )
                        return new LS3D_packet("LS3D_header","LS3D_payload"); 
                    else
                        return new LS3D_packet(p); // duplicate
                }
            public:
                virtual string type() { return "LS3D_packet";}
                ~LS3D_packet_generator(){}
        };

        SET(setLS3D_Header,LS3D_header*,hdr,_hdr);
        GET(getLS3D_Header,LS3D_header*,hdr);
};
LS3D_packet::LS3D_packet_generator LS3D_packet::LS3D_packet_generator::sample;

/***********************************************************************************/

class node {
        // all nodes created in the program
        static map<unsigned int, node*> id_node_table;
        
        unsigned int id;
        map<unsigned int,bool> phy_neighbors;
        
    protected:
        node(node&){} // this constructor should not be used
        node(){} // this constructor should not be used
        node(unsigned int _id): id(_id) { id_node_table[_id] = this; }
    public:
        virtual ~node() { id_node_table.erase (id) ;} // erase the node
        
        void add_phy_neighbor (unsigned int _id); // we only add a directed link from id to _id
        void del_phy_neighbor (unsigned int _id); // we only delete a directed link from id to _id
        
        // you can use the function to get the node's neigbhors
        // if you don't use the following function and obtain the neighbor information by broadcast, then you will earn extra credit
        const map<unsigned int,bool> & getPhyNeighbors () { 
            return phy_neighbors;
        }   
        
        void recv (packet *p) {
            packet *tp = p;
            recv_handler(tp); //handle the receive event
            packet::discard(p); 
        } // the packet will be directly delted after the handler
        void send (packet *p);
        
        // receive the packet and do something; this is a pure virtual function
        virtual void recv_handler(packet *p) = 0;
        void send_handler(packet *p);
        
        static node * id_to_node (unsigned int id) { return ((id_node_table.find(id)!=id_node_table.end()) ? id_node_table[id]: nullptr) ; }
        GET(getNodeID,unsigned int,id);
        
        static unsigned int getNodeNum () { return id_node_table.size(); }

        class node_generator {
                // lock the copy constructor
                node_generator(node_generator &){}
                // store all possible types of node
                static map<string,node_generator*> prototypes;
            protected:
                // allow derived class to use it
                node_generator() {}
                // after you create a new node type, please register the factory of this node type by this function
                void register_node_type(node_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your node
                virtual node* generate(unsigned int _id) = 0;
            public:
                // you have to implement your own type() to return your node type
        	    virtual string type() = 0;
        	    // this function is used to generate any type of node derived
        	    static node * generate (string type, unsigned int _id) {
        	        if(id_node_table.find(_id)!=id_node_table.end()){
        	            std::cerr << "duplicate node id" << std::endl; // node id is duplicated
        	            return nullptr;
        	        }
        	        else if ( BROCAST_ID == _id ) {
        	            cerr << "BROCAST_ID cannot be used" << endl;
        	            return nullptr;
        	        }
            		else if(prototypes.find(type) != prototypes.end()){ // if this type derived exists 
            		    node * created_node = prototypes[type]->generate(_id);
            			return created_node; // generate it!!
            		}
            		std::cerr << "no such node type" << std::endl; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered node types: " << endl;
            	    for (map<string,node::node_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++)
            	        cout << it->second->type() << endl;
            	}
            	virtual ~node_generator(){};
        };
};
map<string,node::node_generator*> node::node_generator::prototypes;
map<unsigned int,node*> node::id_node_table;

void node::add_phy_neighbor (unsigned int _id){
    if (id == _id) return; // if the two nodes are the same...
    if (id_node_table.find(_id)==id_node_table.end()) return; // if this node does not exist
    if (phy_neighbors.find(_id)!=phy_neighbors.end()) return; // if this neighbor has been added
    phy_neighbors[_id] = true;
}
void node::del_phy_neighbor (unsigned int _id){
    phy_neighbors.erase(_id);
}

/***********************************************************************************/

class mycomp {
    bool reverse;
    
    public:
        mycomp(const bool& revparam = false) { reverse=revparam ; }
        bool operator() (const event* lhs, const event* rhs) const;
};

/***********************************************************************************/

class event {
        event(event*&){} // this constructor cannot be directly called by users
        static priority_queue < event*, vector < event* >, mycomp > events;
        static unsigned int cur_time; // timer
        static unsigned int end_time;
        
        unsigned int trigger_time;
        
        // get the next event
        static event * get_next_event() ;
        static void add_event (event *e) { events.push(e); }
        static hash<string> event_seq;
        
    protected:
        event(){} // it should not be used
        event(unsigned int _trigger_time): trigger_time(_trigger_time) {}
    public:
        virtual void trigger()=0;
        virtual ~event(){}

        virtual unsigned int event_priority() const = 0;
        unsigned int get_hash_value(string string_for_hash) const {
            unsigned int priority = event_seq (string_for_hash);
            return priority;
        }
        
        static void flush_events (); // only for debug
        
        GET(getTriggerTime,unsigned int,trigger_time);
        
        static void start_simulate( unsigned int _end_time ); // the function is used to start the simulation
        
        static unsigned int getCurTime() { return cur_time ; }
        static void getCurTime(unsigned int _cur_time) { cur_time = _cur_time; } 
        // static unsigned int getEndTime() { return end_time ; }
        // static void getEndTime(unsigned int _end_time) { end_time = _end_time; }
        
        virtual void print () const = 0; // the function is used to print the event information

        class event_generator{
                // lock the copy constructor
                event_generator(event_generator &){}
                // store all possible types of event
                static map<string,event_generator*> prototypes;
            protected:
                // allow derived class to use it
                event_generator() {}
                // after you create a new event type, please register the factory of this event type by this function
                void register_event_type(event_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your event
                virtual event* generate(unsigned int _trigger_time, void * data) = 0;
            public:
                // you have to implement your own type() to return your event type
        	    virtual string type() = 0;
        	    // this function is used to generate any type of event derived
        	    static event * generate (string type, unsigned int _trigger_time, void * data) {
            		if(prototypes.find(type) != prototypes.end()){ // if this type derived exists
            		    event * e = prototypes[type]->generate(_trigger_time, data);
            		    add_event(e);
            		    return e; // generate it!!
            		}
            		std::cerr << "no such event type" << std::endl; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered event types: " << endl;
            	    for (map<string,event::event_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++)
            	        cout << it->second->type() << endl;
            	}
            	virtual ~event_generator(){}
        };
};
map<string,event::event_generator*> event::event_generator::prototypes;
priority_queue < event*, vector< event* >, mycomp > event::events;
hash<string> event::event_seq;

unsigned int event::cur_time = 0;
unsigned int event::end_time = 0;

void event::flush_events()
{ 
    cout << "**flush begin" << endl;
    while ( ! events.empty() ) {
        cout << setw(11) << events.top()->trigger_time << ": " << setw(11) << events.top()->event_priority() << endl;
        delete events.top();
        events.pop();
    }
    cout << "**flush end" << endl;
}
event * event::get_next_event() {
    if(events.empty()) 
        return nullptr; 
    event * e = events.top();
    events.pop(); 
    // cout << events.size() << " events remains" << endl;
    return e; 
}
void event::start_simulate(unsigned int _end_time) {
    if (_end_time<0) {
        cerr << "you should give a positive value of _end_time" << endl;
        return;
    }
    end_time = _end_time;
    event *e; 
    e = event::get_next_event ();
    while ( e != nullptr && e->trigger_time <= end_time ) {
        if ( cur_time <= e->trigger_time )
            cur_time = e->trigger_time;
        else {
            cerr << "cur_time = " << cur_time << ", event trigger_time = " << e->trigger_time << endl;
            break;
        }

        // cout << "event trigger_time = " << e->trigger_time << endl;
        e->print(); // for log
        // cout << " event begin" << endl;
        e->trigger();
        // cout << " event end" << endl;
        delete e;
        e = event::get_next_event ();
    }
    // cout << "no more event" << endl;
}

bool mycomp::operator() (const event* lhs, const event* rhs) const {
    // cout << lhs->getTriggerTime() << ", " << rhs->getTriggerTime() << endl;
    // cout << lhs->type() << ", " << rhs->type() << endl;
    unsigned int lhs_pri = lhs->event_priority();
    unsigned int rhs_pri = rhs->event_priority();
    // cout << "lhs hash = " << lhs_pri << endl;
    // cout << "rhs hash = " << rhs_pri << endl;
    
    if (reverse) 
        return ((lhs->getTriggerTime()) == (rhs->getTriggerTime())) ? (lhs_pri < rhs_pri): ((lhs->getTriggerTime()) < (rhs->getTriggerTime()));
    else 
        return ((lhs->getTriggerTime()) == (rhs->getTriggerTime())) ? (lhs_pri > rhs_pri): ((lhs->getTriggerTime()) > (rhs->getTriggerTime()));
}

/***********************************************************************************/

class recv_event: public event {
    public:
        class recv_data; // forward declaration
            
    private:
        recv_event(recv_event&) {} // this constructor cannot be used
        recv_event() {} // we don't allow users to new a recv_event by themselv
        unsigned int senderID; // the sender
        unsigned int receiverID; // the receiver 
        packet *pkt; // the packet
        
    protected:
        // this constructor cannot be directly called by users; only by generator
        recv_event(unsigned int _trigger_time, void *data): event(_trigger_time), senderID(BROCAST_ID), receiverID(BROCAST_ID), pkt(nullptr){
            recv_data * data_ptr= (recv_data*) data;
            senderID = data_ptr->s_id;
            receiverID = data_ptr->r_id;
            pkt = data_ptr->_pkt;
        } 
        
    public:
        virtual ~recv_event(){}
        // recv_event will trigger the recv function
        virtual void trigger();
        
        unsigned int event_priority() const;
        
        class recv_event_generator;
        friend class recv_event_generator;
        // recv_event is derived from event_generator to generate a event
        class recv_event_generator : public event_generator{
                static recv_event_generator sample;
                // this constructor is only for sample to register this event type
                recv_event_generator() { /*cout << "recv_event registered" << endl;*/ register_event_type(&sample); }
            protected:
                virtual event * generate(unsigned int _trigger_time, void *data){ 
                    // cout << "recv_event generated" << endl; 
                    return new recv_event(_trigger_time, data); 
                }
                
            public:
                virtual string type() { return "recv_event";}
                ~recv_event_generator(){}
        };
        // this class is used to initialize the recv_event
        class recv_data{
            public:
                unsigned int s_id; //send
                unsigned int r_id; //receive
                packet *_pkt;
        };
        
        void print () const;
};
recv_event::recv_event_generator recv_event::recv_event_generator::sample;

void recv_event::trigger() {
    if (pkt == nullptr) {
        cerr << "recv_event error: no pkt!" << endl; 
        return ; 
    }
    else if (node::id_to_node(receiverID) == nullptr){
        cerr << "recv_event error: no node " << receiverID << "!" << endl;
        delete pkt; return ;
    }
    node::id_to_node(receiverID)->recv(pkt); 
}
unsigned int recv_event::event_priority() const {
    string string_for_hash;
    string_for_hash = to_string(getTriggerTime()) + to_string(senderID) + to_string (receiverID) + to_string (pkt->getPacketID());
    return get_hash_value(string_for_hash);
}
// the recv_event::print() function is used for log file
void recv_event::print () const {
    cout << "time "          << setw(11) << event::getCurTime() 
         << "   recID "      << setw(11) << receiverID 
         << "   pktID"       << setw(11) << pkt->getPacketID()
         << "   srcID "      << setw(11) << pkt->getHeader()->getSrcID() 
         << "   dstID"       << setw(11) << pkt->getHeader()->getDstID() 
         << "   preID"       << setw(11) << pkt->getHeader()->getPreID()
         << "   nexID"       << setw(11) << pkt->getHeader()->getNexID()
         << endl;
}

/***********************************************************************************/

class send_event: public event {
    public:
        class send_data; // forward declaration
            
    private:
        send_event (send_event &){}
        send_event (){} // we don't allow users to new a recv_event by themselves
        // this constructor cannot be directly called by users; only by generator
        unsigned int senderID; // the sender
        unsigned int receiverID; // the receiver 
        packet *pkt; // the packet
    
    protected:
        send_event (unsigned int _trigger_time, void *data): event(_trigger_time), senderID(BROCAST_ID), receiverID(BROCAST_ID), pkt(nullptr){
            send_data * data_ptr = (send_data*) data;
            senderID = data_ptr->s_id;
            receiverID = data_ptr->r_id;
            pkt = data_ptr->_pkt;
        } 
        
    public:
        virtual ~send_event(){}
        // send_event will trigger the send function
        virtual void trigger();
        
        unsigned int event_priority() const;
        
        class send_event_generator;
        friend class send_event_generator;
        // send_event is derived from event_generator to generate a event
        class send_event_generator : public event_generator{
                static send_event_generator sample;
                // this constructor is only for sample to register this event type
                send_event_generator() { /*cout << "send_event registered" << endl;*/ register_event_type(&sample); }
            protected:
                virtual event * generate(unsigned int _trigger_time, void *data){ 
                    // cout << "send_event generated" << endl; 
                    return new send_event(_trigger_time, data); 
                }
            
            public:
                virtual string type() { return "send_event";}
                ~send_event_generator(){}
        };
        // this class is used to initialize the send_event
        class send_data{
            public:
                unsigned int s_id; //send
                unsigned int r_id; //receive
                packet *_pkt;
        };
        
        void print () const;
};
send_event::send_event_generator send_event::send_event_generator::sample;

void send_event::trigger() {
    if (pkt == nullptr) {
        cerr << "send_event error: no pkt!" << endl; 
        return ; 
    }
    else if (node::id_to_node(senderID) == nullptr){
        cerr << "send_event error: no node " << senderID << "!" << endl;
        delete pkt; return ;
    }
    node::id_to_node(senderID)->send(pkt);
}
unsigned int send_event::event_priority() const {
    string string_for_hash;
    string_for_hash = to_string(getTriggerTime()) + to_string(senderID) + to_string (receiverID) + to_string (pkt->getPacketID());
    return get_hash_value(string_for_hash);
}
// the send_event::print() function is used for log file
void send_event::print () const {
    cout << "time "          << setw(11) << event::getCurTime() 
         << "   senID "      << setw(11) << senderID 
         << "   pktID"       << setw(11) << pkt->getPacketID()
         << "   srcID "      << setw(11) << pkt->getHeader()->getSrcID() 
         << "   dstID"       << setw(11) << pkt->getHeader()->getDstID() 
         << "   preID"       << setw(11) << pkt->getHeader()->getPreID()
         << "   nexID"       << setw(11) << pkt->getHeader()->getNexID()
         << endl;
}

/***********************************************************************************/

class LS3D_node: public node {
        map<unsigned int,unsigned int> storage; // it is used to store the other nodes' proxy information
        map<unsigned int,bool> two_hop_neighbors; // you can use this variable to record the node's 2-hop neighbors 
        unsigned int density = 0;

    protected:
        LS3D_node() {} // it should not be used
        LS3D_node(LS3D_node&) {} // it should not be used
        LS3D_node(unsigned int _id): node(_id){} // this constructor cannot be directly called by users
    
    public:
        ~LS3D_node(){}
        
        // please define recv_handler function to deal with the incoming packet
        virtual void recv_handler (packet *p);
        
        void add_node_proxy (unsigned node_id, unsigned proxy_id) { if(storage.find(node_id) == storage.end()) storage[node_id] = proxy_id; }
        unsigned int get_node_proxy (unsigned node_id) { return (storage.find(node_id) != storage.end()) ? storage[node_id]: BROCAST_ID; }
        
        void add_two_hop_neighbor (unsigned int n_id) { two_hop_neighbors[n_id] = true; }
        unsigned int get_two_hop_neighbor_num() { return two_hop_neighbors.size(); }
        
        // SET(setdensity, unsigned int, density, _density);
        // GET(getdensity, unsigned int, density);
        unsigned int getDensity();
        bool Hilltop_or_Valley(string condition);
        int find_next_node(string condition, LS3D_header* hdr);

        class LS3D_node_generator;
        friend class LS3D_node_generator;
        // LS3D_node is derived from node_generator to generate a node
        class LS3D_node_generator : public node_generator{
                static LS3D_node_generator sample;
                // this constructor is only for sample to register this node type
                LS3D_node_generator() { /*cout << "LS3D_node registered" << endl;*/ register_node_type(&sample); }
            protected:
                virtual node * generate(unsigned int _id){ /*cout << "LS3D_node generated" << endl;*/ return new LS3D_node(_id); }
            public:
                virtual string type() { return "LS3D_node";}
                ~LS3D_node_generator(){}
        };
};

LS3D_node::LS3D_node_generator LS3D_node::LS3D_node_generator::sample;

// the function is used to add an initial event
void add_initial_event (bool isPub, unsigned int src, unsigned int dst, unsigned int pro = 0, unsigned t = 0){
    if ( node::id_to_node(src) == nullptr || (dst != BROCAST_ID && node::id_to_node(dst) == nullptr) ) {
        cerr << "src or dst is incorrect" << endl; return ;
    }
    LS3D_packet *pkt = dynamic_cast<LS3D_packet*> ( packet::packet_generator::generate("LS3D_packet") );
    if (pkt == nullptr) { 
        cerr << "packet type is incorrect" << endl; return; 
    }
    LS3D_header *hdr = dynamic_cast<LS3D_header*> ( pkt->getHeader() );
    LS3D_payload *pld = dynamic_cast<LS3D_payload*> ( pkt->getPayload() );
    
    if (hdr == nullptr) {
        cerr << "header type is incorrect" << endl; return ;
    }
    if (pld == nullptr) {
        cerr << "payload type is incorrect" << endl; return ;
    }

    hdr->setSrcID(src);
    hdr->setDstID(dst);
    hdr->setPreID(src);
    hdr->setNexID(src);
    hdr->setIsPub(isPub);
    pld->setHostID(dst);
    pld->setProxyID(pro);
    
    hdr->setIsHilltopMode(true);

    recv_event::recv_data e_data;
    e_data.s_id = src;
    e_data.r_id = src;
    e_data._pkt = pkt;
    
    // for (unsigned int i = 0; i < 10; i ++){
    //     hdr->push_visited_node(i);
    // }
    // for (unsigned int i = 0; i < 5; i ++){
    //     hdr->mark_visited_node(i);
    // }
    
    recv_event *e = dynamic_cast<recv_event*> ( event::event_generator::generate("recv_event",t, (void *)&e_data) );
    if (e == nullptr) cerr << "event type is incorrect" << endl;
}

// send_handler function is used to transmit packet p based on the information in the header
// Note that the packet p will not be discard after send_handler ()

void node::send_handler(packet *p){
    packet *_p = packet::packet_generator::replicate(p);
    send_event::send_data e_data;
    e_data.s_id = _p->getHeader()->getPreID(); //send id becomes previous id
    e_data.r_id = _p->getHeader()->getNexID(); //receive id becomes next id
    e_data._pkt = _p;
    send_event *e = dynamic_cast<send_event*> (event::event_generator::generate("send_event",event::getCurTime(), (void *)&e_data) );
    if (e == nullptr) cerr << "event type is incorrect" << endl;
}

void node::send (packet *p){ // this function is called by event; not for the user
    if (p == nullptr) return;
    
    unsigned int _nexID = p->getHeader()->getNexID();
    for ( map<unsigned int,bool>::iterator it = phy_neighbors.begin(); it != phy_neighbors.end(); it ++) {
        unsigned int nb_id = it->first; // neighbor id
        
        if (nb_id != _nexID && BROCAST_ID != _nexID) continue; // this neighbor will not receive the packet
        
        unsigned int trigger_time = event::getCurTime() + ONE_HOP_DELAY ; // we simply assume that the delay is fixed
        // cout << "node " << id << " send to node " <<  nb_id << endl;
        recv_event::recv_data e_data;
        e_data.s_id = id; //self
        e_data.r_id = nb_id; //neigbor
        
        packet *p2 = packet::packet_generator::replicate(p);
        e_data._pkt = p2;
        
        recv_event *e = dynamic_cast<recv_event*> (event::event_generator::generate("recv_event", trigger_time, (void*) &e_data)); // send the packet to the neighbor
        if (e == nullptr) cerr << "event type is incorrect" << endl;
    }
    packet::discard(p);
}

/***********************************************************************************/

//get density of a node
unsigned int LS3D_node::getDensity(){

    map<unsigned int,bool>getnegibors = getPhyNeighbors();
    map<unsigned int,bool>getnegibors1;

    if (get_two_hop_neighbor_num()!=0)
        return density;
    
    else{
        for(map<unsigned int,bool>::iterator it=getnegibors.begin(); it!=getnegibors.end(); it++){
            add_two_hop_neighbor(it->first);
            getnegibors1 = id_to_node(it->first)->getPhyNeighbors();
            for(map<unsigned int,bool>::iterator it1=getnegibors1.begin(); it1!=getnegibors1.end(); it1++)
                add_two_hop_neighbor(it1->first);
        }
        density = get_two_hop_neighbor_num();
        return density;
    }
}

//check the node is hilltop or valley
bool LS3D_node::Hilltop_or_Valley(string condition){

    int my_density = getDensity();
    int target_node = getNodeID();
    map<unsigned int,bool>getnegibors = getPhyNeighbors();
    LS3D_node *n = nullptr;

    if (condition == "hilltop"){

        for (map<unsigned int,bool>::iterator it=getnegibors.begin(); it!=getnegibors.end(); it++){
            n = dynamic_cast<LS3D_node*> (id_to_node(it->first));
            if(n->getDensity() > my_density || (n->getDensity() == my_density && it->first > target_node))
                return false;
        }

        return true;
    }

    else if (condition == "valley"){

        for (map<unsigned int,bool>::iterator it=getnegibors.begin(); it!=getnegibors.end(); it++){
            n = dynamic_cast<LS3D_node*> (id_to_node(it->first));
            if(n->getDensity() < my_density || (n->getDensity() == my_density && it->first < target_node))
                return false;
        }

        return true;
    }

}

//find a minimun or maximun density node which has not been visited
int LS3D_node::find_next_node(string condition, LS3D_header* hdr){

    map<unsigned int,bool>getnegibors = getPhyNeighbors();
    int target_node = getNodeID();
    LS3D_node *n = nullptr;

    if(condition == "higher"){ //find max density
        int max_density = 0;

        for(map<unsigned int,bool>::iterator it=getnegibors.begin(); it!=getnegibors.end(); it++){
            n = dynamic_cast<LS3D_node*> (id_to_node(it->first));

            //if the density is the same, find the one having bigger ID
            if(!(hdr->check_visited_node(it->first)) && (n->getDensity() > max_density || (n->getDensity() == max_density && it->first > target_node))){
                max_density = n->getDensity();
                target_node = it->first;
            }
        }
        return target_node;
    }

    else if(condition == "lower"){ //find min density
        int min_density = BROCAST_ID;

        for(map<unsigned int,bool>::iterator it=getnegibors.begin(); it!=getnegibors.end(); it++){
            n = dynamic_cast<LS3D_node*> (id_to_node(it->first));

            //if the density is the same, find the one having smaller ID
            if(!(hdr->check_visited_node(it->first)) && (n->getDensity() < min_density || (n->getDensity() == min_density && it->first < target_node))){
                min_density = n->getDensity();
                target_node = it->first;
            }
        }	
        return target_node;
    }

}

// you have to write the code in recv_handler
void LS3D_node::recv_handler (packet *p){

    // this is a simple example
    // every node broadcasts its message to every node and every node relays the packet "only once"

    // this static variable is used to cache whether the packet has been received before
    // however, you should not use static vairable like the following "hi"
    // please use mutator "add_node_proxy(node_id, proxy_id)" and accessor "get_node_proxy(id)"
    // to store the proxy information in the "storage" in LS3D_node
    // static bool hi[10][10] = {false};
   
    int next_node;
    LS3D_packet *p2 = nullptr;
    LS3D_header *hdr2 = nullptr;
    LS3D_payload *pld2 = nullptr;

    if(p->type() == "LS3D_packet" && p->getHeader()->getNexID() == getNodeID()){ //p->type() for checking

        p2 = dynamic_cast<LS3D_packet*> (p);
        hdr2 = dynamic_cast<LS3D_header*> (p->getHeader());
        pld2 = dynamic_cast<LS3D_payload*> (p->getPayload());

        hdr2->push_visited_node(getNodeID());
        hdr2->mark_visited_node(getNodeID());

        /////////////////// hilltop mode ///////////////////////
        if(hdr2->getIsHilltopMode() == true){

            if(Hilltop_or_Valley("hilltop")){   //current node is hilltop

                //put the data
                if(hdr2->getIsPub() == true)
                    add_node_proxy(hdr2->getSrcID(), pld2->getProxyID());
                
                //get the data
                else if(hdr2->getIsPub() == false && get_node_proxy(hdr2->getDstID()) != BROCAST_ID){
                    cout << "The proxy of node " << hdr2->getDstID() << " is " << get_node_proxy(hdr2->getDstID()) << endl;
                    return;
                }

                //cannot find the data
                else if(hdr2->getIsPub() == false && hdr2->getIsHilltopOnce() == true){
                    cout << "The proxy of node " << hdr2->getDstID() << " not found!" << endl;
                    return;
                }

                if(hdr2->getIsHilltopOnce() == false){  
                    hdr2->setIsHilltopOnce(true);  //one hilltop has been found
                    hdr2->setIsHilltopMode(false);
                    next_node = find_next_node("lower", hdr2);
                    if (next_node == getNodeID())  //no road to go down
                        next_node = hdr2->pop_visited_node(); 
                }
                
                else{ return;}

            }

            else if(Hilltop_or_Valley("valley")){  //current node is valley
                next_node = find_next_node("higher", hdr2);

                if(next_node == getNodeID()){  //there is no road to go up
                    next_node = hdr2->pop_visited_node();
                    hdr2->setIsHilltopMode(false); //find another valley
                }
            }

            else{  //continue to go up
                next_node = find_next_node("higher", hdr2);
                if (next_node == getNodeID())  //there is no road to go up
                    next_node = hdr2->pop_visited_node();  
            }

        }

        /////////////////// valley mode ///////////////////////
        else if (hdr2->getIsHilltopMode() == false){

            if (Hilltop_or_Valley("valley")){  //current node is valley
                hdr2->setIsHilltopMode(true);
                next_node = find_next_node("higher", hdr2);

                if (next_node == getNodeID()){  //there is no road to go up
                    next_node = hdr2->pop_visited_node();
                    hdr2->setIsHilltopMode(false);
                }
            }
            else{   //continue to go down
                next_node = find_next_node("lower", hdr2);
                if (next_node == getNodeID()) //there is no road to go down
                    next_node = hdr2->pop_visited_node();  
            }

        }

        //back to the source if there is no road to go
        if (next_node == BROCAST_ID){
            if (hdr2->getIsPub() == false)
                cout<< "The proxy of node " << hdr2->getDstID() << " not found!" << endl;
            return;
        }
		
		//send the packet to next node
        p2->getHeader()->setPreID(getNodeID()); //set the previous id
        p2->getHeader()->setNexID(next_node);  //set the next id //nextID
        send_handler(p2);  //set the destination id      

        //self density
        /*if(getdensity() == 0)
            calculate_density(p2);
        cout << getdensity() << endl;*/
        
        //neighbors' density
        /*map<unsigned int,bool>getnegibors = getPhyNeighbors();
        map<unsigned int,bool>getnegibors1;
        map<unsigned int,bool>getnegibors2;
        int cur_node;

        for(map<unsigned int,bool>::iterator it=getnegibors.begin(); it!=getnegibors.end(); it++)
            if(it->second == true){           
                getnegibors1 = id_to_node(it->first)->getPhyNeighbors();
                for(map<unsigned int,bool>::iterator it1=getnegibors1.begin(); it1!=getnegibors1.end(); it1++)
                     if(it1->second == true){
                        add_two_hop_neighbor(it1->first);
                        cur_node = getNodeID();
                        for(map<unsigned int,bool>::iterator it2=getnegibors2.begin(); it2!=getnegibors2.end(); it2++)
                            if(it2->second == true && cur_node != it2->first)
                                 add_two_hop_neighbor(it2->first);
                    } 
            }
        setdensity(get_two_hop_neighbor_num());*/        
        
        // unsigned int a = dynamic_cast<LS3D_header*>(p2->getHeader())->pop_visited_node();
        // while ( a != BROCAST_ID){
        //     cout << a ;
        //     bool check = dynamic_cast<LS3D_header*>(p2->getHeader())->check_visited_node(a);
        //     cout << ", " << check << endl;
        //     a = dynamic_cast<LS3D_header*>(p2->getHeader())->pop_visited_node();
        // }
    }
    
    // you should implement the LS3D distributed algorithm in recv_hander
    // getNodeID() returns the id of the current node
    
    // The current node's neighbors are already stored in the following variable 
    // map<unsigned int,bool> node::phy_neighbors
    // you can use the function to get the other node
    // node * node::id_to_node(id)
    // Then you can use the function to get the node's neigbhors
    // map<unsigned int,bool> node::getPhyNeighbors ()

    // However, if you don't use the above function, you have to implement broadcast to get the neighbors' neighbors
    // To this end, you may define your own packet type and register it
    // If you do it by yourself, you will earn extra credit
    // The following two functions you may want to use
    // void LS3D_node::add_two_hop_neighbor (unsigned int n_id)
    // unsigned int LS3D_node::get_two_hop_neighbor_num ()
    
    // moreover, you can use map<unsigned int,bool>::iterator to enumerate all the neighbors in a for loop
    // for (map<unsigned int,bool>::iterator it = phy_neighbors.begin(); it != phy_neighbors.end(); it ++)
    // use *it to get each neighbor

    // you can use p->getHeader()->setSrcID() or getSrcID()
    //             p->getHeader()->setDstID() or getDstID()
    //             p->getHeader()->setPreID() or getPreID()
    //             p->getHeader()->setNexID() or getNexID() to change or read the packet header
    
    // in addition, you can use the following to get the packet, header, and payload with correct type (LS3D)
    // in fact, this is downcasting
    // LS3D_packet * pkt = dynamic_cast<LS3D_packet*> (p);
    // LS3D_header * hdr = dynamic_cast<LS3D_header*> (p->getHeader());
    // LS3D_payload * pld = dynamic_cast<LS3D_payload*> (p->getPayload());
    
    // you can also change the LS3D_header setting
    // hdr->setIsPub(true); // to publish the nodes' proxy nodes
    // or hdr->setIsPub(false); // to subscribe the other nodes' proxy nodes
    // void push_visited_node (unsigned int n_id): it is used to record the DFS path
    // unsigned int pop_visited_node (): it is used to return the last visited node. if the stack is empty, then return BROCAST_ID
    // void mark_visited_node (unsigned int n_id): it is used to record the visited nodes
    // bool check_visited_node (unsigned int n_id): it is used to examine whether the node is visited
    // void setIsHilltopOnce(bool): it is used to mark whether the packet visits a hilltop
    // bool getIsHilltopOnce(): it is used to check whether the packet visits a hilltop
    
    // you can also change the LS3D_payload setting
    // pld->setHostID(int);  // for publisher, the publisher node's id; for subscriber, the requested publisher node's id
    // or pld->setProxyID(int); // for publisher, the publisher node's proxy's id; for subscriber, nothing
    
    // besides, you can use packet::packet_generator::generate() to generate a new packet; note that you should fill the header and payload in the packet
    // moreover, you can use "packet *p2 = packet::packet_generator::replicate(p)" to make a clone p2 of packet p
    // note that if the packet is generated or replicated manually, you must delete it by packet::discard() manually before recv_hander finishes
    
    // to store or get the other nodes' proxy nodes, you can use the following two functions
    // void add_node_proxy(unsigned int node_id, unsigned int proxy_id): add one record of proxy node data for the other node
    // unsigned int get_node_proxy(unsigned int node_id): get the proxy node id for a specific node if we store it on the node; otherwise, it returns BROCAST_ID

    // "IMPORTANCE":
    // you have to "carefully" fill the correct information (e.g., srcID, dstID, ...) in the packet before you send it
    // note that if you want to transmit to only one next node (i.e., unicast), then you fill the ID of a specific node to "nexID" in the header
    // otherwise, i.e., you want to broadcasts, then you fill "BROCAST_ID" to "nexID" in the header
    // after that, you can use send() to transmit the packet 
    // usage: send_handler (p);
    
    // note that packet p will be discarded (deleted) after recv_hander(); you don't need to manually delete it
}

int main()
{
    // header::header_generator::print(); // print all registered headers
    // payload::payload_generator::print(); // print all registered payloads
    // packet::packet_generator::print(); // print all registered packets
    // node::node_generator::print(); // print all registered nodes
    // event::event_generator::print(); // print all registered events

    /*fstream inputstream;
    inputstream.open("input.txt");*/

    int node, link, duration;
    cin >> node >> link >> duration;

    // read the input, generate your nodes, and set their neighbors
    for (unsigned int id = 0; id < node; id ++){
        node::node_generator::generate("LS3D_node",id); //the type of node, node ID
    }
    
    int i, a, b, c;
    for(i=0; i<link; i++){
        cin >> a >> b >> c;
        node::id_to_node(b)->add_phy_neighbor(c);
        node::id_to_node(c)->add_phy_neighbor(b);
    }

    // generate all initial events you want to simulate in the networks

    /*unsigned int t = 0, src = 0, dst = BROCAST_ID, pro = 0;
    bool isPub = false;*/

    // read the input and use add_recv_event to add an initial event
    // you can use for loop to read the input

    /*for ( src = 0, t = 0; src < 1 ; src++, t += 50) //add events
        add_initial_event(isPub, src, dst, pro, t);*/

        //if it is an event for publishing the data (bool)
        //source: beginning node
        //no destination for publisher; destination is the target for subscriber
        //proxy node you need to store
        //time

    int publish, publisher, proxy;
    int subscribe, subscriber, data;
    int time;

    cin >> publish;
    for(i=0; i<publish; i++){
        cin >> time >> publisher >> proxy;
        add_initial_event(true, publisher, BROCAST_ID, proxy, time);
        //isPub, src, dst, pro, t
    }

    cin >> subscribe;
    for(i=0; i<subscribe; i++){
        cin >> time >> subscriber >> data;
        add_initial_event(false, subscriber, data, BROCAST_ID, time);
        //isPub, src, dst, pro, t
    }

    // start simulation!!
    event::start_simulate(duration); //ms
    // event::flush_events() ;
    // cout << packet::getLivePacketNum() << endl;
    return 0;
}




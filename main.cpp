#include "provided.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

#include "ExpandableHashMap.h" //FOR TESTING

bool loadDeliveryRequests(string deliveriesFile, GeoCoord& depot, vector<DeliveryRequest>& v);
bool parseDelivery(string line, string& lat, string& lon, string& item);

int main(int argc, char *argv[])
{
    //hash map test
    /*
    GeoCoord gc("34.0547000", "-118.4794734");
    GeoCoord gc2("34.0550808", "-118.4803801");
    ExpandableHashMap<GeoCoord, GeoCoord> hashMap;
    hashMap.associate(gc, gc2);
    //hashMap.associate(gc2, 9);
    hashMap.print();
    */
    //street map
    //street load test
    /*StreetMap map;
    map.load("/Users/roxanemartin/Desktop/CS32/proj4/proj4/mapdatas.txt");
    GeoCoord gc("34.0393431", "-118.4071546");
    GeoCoord gc1("34.0396737", "-118.4082925");
    GeoCoord gc2("34.0399340", " -118.4092305");*/
    
    //getSegmentsThatStartWithTest
    /*
     10th Helena Drive
     1
     34.0547000 -118.4794734 34.0544590 -118.4801137
     */
    //std::vector<StreetSegment> s;
    //map.getSegmentsThatStartWith(gc, s);
   // cout << "size: "<<s.size()<<endl;
        //cout<<s[0].print();
   /*StreetMap sm;
    sm.load("/Users/roxanemartin/Desktop/CS32/proj4/proj4/maptest.txt");
    PointToPointRouter pr(&sm);
    GeoCoord gc1("1.0", "3.0");
    GeoCoord gc2("4.0", "1.0");
    list<StreetSegment> route;
    double dist;
    pr.generatePointToPointRoute(gc1, gc2, route, dist);
    cout << "Distance: " << dist << endl;
    auto it = route.begin();
    while (it != route.end())
    {
        cout << (*it).print() <<"-->";
        it++;
    }
    cout << endl;*/
     
    
    //delivery planner test 
    /*StreetMap sm;
    sm.load("/Users/roxanemartin/Desktop/CS32/proj4/proj4/mapdata.txt");
    DeliveryPlanner dp(&sm);
    GeoCoord gc1("34.0625329", "-118.4470263");
    GeoCoord gc2("34.0636671", "-118.4461842");
    GeoCoord gc3("34.0614940", "-118.4411993");
    DeliveryRequest dr1("stuff", gc2);
    DeliveryRequest dr2("more stuff", gc3);
    vector<DeliveryRequest> del;
    del.push_back(dr1);
    del.push_back(dr2);
    vector<DeliveryCommand> com = {};
    double dist = 0;
    dp.generateDeliveryPlan(gc1, del, com, dist);
    for (int i=0; i<com.size(); i++)
    {
        cout<< com[i].description()<<endl;
    }
    cout << "total distance: "<< dist<< endl;
    */
    /*StreetMap map;
    map.load("/Users/roxanemartin/Desktop/CS32/proj4/proj4/mapdata.txt");;
    StreetMap* mapP = &map;
    PointToPointRouter p2p(mapP);
    GeoCoord A("34.0393431", "-118.4071546");
    GeoCoord B("34.0396737", "-118.4082925");
    GeoCoord C("34.0399340", " -118.4092305");
    GeoCoord D("34.0426164", "-118.5001481");
    GeoCoord E("34.0446433", "-118.4975646");
    vector<StreetSegment> r;
    list<StreetSegment> l;
    double dist=0;
    //map.getSegmentsThatStartWith(A, r);
    p2p.generatePointToPointRoute(D, E, l, dist);
    auto it = l.begin();
    while (it != l.end())
    {
        cout << it->print()<<"-->";
        it++;
    }
    
    */
    
    if (argc != 3)
    {
        cout << "Usage: " << argv[0] << " mapdata.txt deliveries.txt" << endl;
        return 1;
    }

    StreetMap sm;
        
    if (!sm.load(argv[1]))
    {
        cout << "Unable to load map data file " << argv[1] << endl;
        return 1;
    }

    GeoCoord depot;
    vector<DeliveryRequest> deliveries;
    if (!loadDeliveryRequests(argv[2], depot, deliveries))
    {
        cout << "Unable to load delivery request file " << argv[2] << endl;
        return 1;
    }

    cout << "Generating route...\n\n";

    DeliveryPlanner dp(&sm);
    vector<DeliveryCommand> dcs;
    double totalMiles;
    DeliveryResult result = dp.generateDeliveryPlan(depot, deliveries, dcs, totalMiles);
    if (result == BAD_COORD)
    {
        cout << "One or more depot or delivery coordinates are invalid." << endl;
        return 1;
    }
    if (result == NO_ROUTE)
    {
        cout << "No route can be found to deliver all items." << endl;
        return 1;
    }
    cout << "Starting at the depot...\n";
    for (const auto& dc : dcs)
        cout << dc.description() << endl;
    cout << "You are back at the depot and your deliveries are done!\n";
    cout.setf(ios::fixed);
    cout.precision(2);
    cout << totalMiles << " miles travelled for all deliveries." << endl;
}

bool loadDeliveryRequests(string deliveriesFile, GeoCoord& depot, vector<DeliveryRequest>& v)
{
    ifstream inf(deliveriesFile);
    if (!inf)
        return false;
    string lat;
    string lon;
    inf >> lat >> lon;
    inf.ignore(10000, '\n');
    depot = GeoCoord(lat, lon);
    string line;
    while (getline(inf, line))
    {
        string item;
        if (parseDelivery(line, lat, lon, item))
            v.push_back(DeliveryRequest(item, GeoCoord(lat, lon)));
    }
    return true;
}

bool parseDelivery(string line, string& lat, string& lon, string& item)
{
    const size_t colon = line.find(':');
    if (colon == string::npos)
    {
        cout << "Missing colon in deliveries file line: " << line << endl;
        return false;
    }
    istringstream iss(line.substr(0, colon));
    if (!(iss >> lat >> lon))
    {
        cout << "Bad format in deliveries file line: " << line << endl;
        return false;
    }
    item = line.substr(colon + 1);
    if (item.empty())
    {
        cout << "Missing item in deliveries file line: " << line << endl;
        return false;
    }
    return true;
}

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
    ExpandableHashMap<GeoCoord, int> hashMap;
    for (int i=0; i<=4; i++)
        hashMap.associate(gc, i);
    hashMap.associate(gc2, 9);
    hashMap.print();*/
    
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
    
    
    StreetMap map;
    map.load("/Users/roxanemartin/Desktop/CS32/proj4/proj4/mapdatas.txt");;
    StreetMap* mapP = &map;
    PointToPointRouter p2p(mapP);
    GeoCoord A("34.0393431", "-118.4071546");
    GeoCoord B("34.0396737", "-118.4082925");
    GeoCoord C("34.0399340", " -118.4092305");
    vector<StreetSegment> r;
    list<StreetSegment> l;
    double dist;
    map.getSegmentsThatStartWith(A, r);
    p2p.generatePointToPointRoute(A, C, l, dist);
    
    
    
    
    
    /*
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
    cout << totalMiles << " miles travelled for all deliveries." << endl;*/
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

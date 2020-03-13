#include "provided.h"
#include "ExpandableHashMap.h"
#include <list>
#include <set>
#include <queue>
#include <vector>
using namespace std;

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
private:
    struct Node
       {
           Node(GeoCoord l) : loc(l),f(0),g(0),h(0) {}
           GeoCoord loc;
           double f,g,h;
           
           bool operator<(const Node &rhs) const //define <operator for struct so set is ordered by value of f
           {
               return f < rhs.f;
           }
           
       };
               
    bool isDestination(const GeoCoord current, const GeoCoord end) const;
    double calculateHValue(const GeoCoord current, const GeoCoord end) const;
    void AStarSearch(const GeoCoord& start, const GeoCoord& end, list<StreetSegment>& route) const;
    const StreetMap* sMap;
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    sMap = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    //USED A STAR
    ExpandableHashMap<GeoCoord, GeoCoord> previousPointMap;
                   
        list<Node> closedList;
        set<Node> openList;
        
        route.clear();
               
        Node startNode(start);
        openList.insert(startNode);
        
        vector<StreetSegment> segs;
        
        while (!openList.empty())
        {
            Node currentNode = *openList.begin();
            openList.erase(openList.begin());
            
            closedList.push_back(currentNode);
            
            if (currentNode.loc.latitude == end.latitude && currentNode.loc.longitude == end.longitude) //if end has been reached
            {
                while (currentNode.loc != start)
                {
                   //cout << "current geo: "<<currentNode.loc.print()<<endl;
                  // cout << "bucket num: "<< previousPointMap.getBucketNumber(currentNode.loc);
                   GeoCoord* prev = previousPointMap.find(currentNode.loc);
                    if (prev == nullptr) //if currentNode.loc cannot be found
                        return BAD_COORD;
                   sMap->getSegmentsThatStartWith(*prev, segs);
                   for (int i=0; i<segs.size(); i++)
                   {
                       if (segs[i].end == currentNode.loc)
                       {
                          string name = segs[i].name;
                          StreetSegment s(*prev, currentNode.loc, name);
                          totalDistanceTravelled+= distanceEarthMiles(*prev, currentNode.loc);
               //cout << s.print()<<endl;
                          route.push_front(s);
                       }
                   }
                   currentNode.loc = *prev;
                   
                   }
                   return DELIVERY_SUCCESS;
            }
            
            sMap->getSegmentsThatStartWith(currentNode.loc, segs);
               if (segs.empty()) //if currentNode.loc cannot be found in sMap
                    return BAD_COORD;
            
            for (int i=0; i<segs.size(); i++)
            {
                bool skip = false;
                
                auto itC = closedList.begin();
                while (itC != closedList.end())
                {
                    if (segs[i].end == itC->loc)
                    {
                        skip = true;
                        break;
                    }//if child already in closed list, don't bother expanding
                    itC++;
                }
                
                if (!skip)
                {
                   Node child(segs[i].end); //create new child with location of end of segments
                   previousPointMap.associate(child.loc, currentNode.loc);
                   child.g = currentNode.g + distanceEarthMiles(child.loc, currentNode.loc); //set child's g value to current node's g plus distance from current node to child; same as total distance from start to child
                    child.h = calculateHValue(child.loc, end); //set child's h value to euclidean distance from child to end
                    child.f = child.g + child.h; //set child's f value as sum of g and h
                    
                    auto itO = openList.begin();
                    while (itO != openList.end())
                    {
                        if (child.loc == itO->loc && child.g>(itO->g)) //if child location already in open list and child's g is bigger than node's already in list, skip rest of loop
                        {
                            skip = true;
                            break;
                        }
                        itO++;
                    }
                    
                    if (!skip)
                        openList.insert(child); //insert new child to open list
                }
            }
        }
        cout <<"no route found"<<endl;
        return NO_ROUTE;   //didn't find path
    }


               
               
               
               

               
               
               
               

               
               
               
bool PointToPointRouterImpl::isDestination(const GeoCoord current, const GeoCoord end) const
{
    if (current==end)
        return true;
    return false;
}

double PointToPointRouterImpl::calculateHValue(const GeoCoord current, const GeoCoord end) const
{
    return distanceEarthMiles(current, end);
}
            

void PointToPointRouterImpl::AStarSearch(const GeoCoord& start, const GeoCoord& end, list<StreetSegment>& route) const
{
    ExpandableHashMap<GeoCoord, GeoCoord> previousPointMap;
               
    list<Node> closedList;
    set<Node> openList;
    
    Node startNode(start);
    openList.insert(startNode);
    
    vector<StreetSegment> segs;
    
    while (!openList.empty())
    {
        Node currentNode = *openList.begin();
        openList.erase(openList.begin());
        
        closedList.push_back(currentNode);
        
        if (currentNode.loc.latitude == end.latitude && currentNode.loc.longitude == end.longitude) //if end has been reached
        {
            //cout << "found path!" <<endl<<endl<<endl;; //implement trace path
            //previousPointMap.print();

            while (currentNode.loc != start)
            {
               //cout << "current geo: "<<currentNode.loc.print()<<endl;
              // cout << "bucket num: "<< previousPointMap.getBucketNumber(currentNode.loc);
               GeoCoord* prev = previousPointMap.find(currentNode.loc);
                     
               sMap->getSegmentsThatStartWith(*prev, segs);
               for (int i=0; i<segs.size(); i++)
               {
                   if (segs[i].end == currentNode.loc)
                   {
                      string name = segs[i].name;
                      StreetSegment s(*prev, currentNode.loc, name);
                      //cout << s.print()<<endl;
                      route.push_front(s);
                   }
               }
               currentNode.loc = *prev;
               
               }
               return;
        }
        
        sMap->getSegmentsThatStartWith(currentNode.loc, segs);
        
        for (int i=0; i<segs.size(); i++)
        {
            bool skip = false;
            
            auto itC = closedList.begin();
            while (itC != closedList.end())
            {
                if (segs[i].end == itC->loc)
                {
                    skip = true;
                    break; //does this skip correctly
                }//if child already in closed list, don't bother expanding
                itC++;
            }
            
            if (!skip)
            {
               Node child(segs[i].end); //create new child with location of end of segments
               //cout << "Child loc: "<<child.loc.print() <<endl;
               //cout << "Current loc: "<<currentNode.loc.print() <<endl;
               //cout << "child geo: " <<child.loc.print()<<endl;
               //cout << "bucket num: "<<previousPointMap.getBucketNumber(child.loc)<<endl;
               previousPointMap.associate(child.loc, currentNode.loc);
               child.g = currentNode.g + distanceEarthMiles(child.loc, currentNode.loc); //set child's g value to current node's g plus distance from current node to child; same as total distance from start to child
                child.h = calculateHValue(child.loc, end); //set child's h value to euclidean distance from child to end
                child.f = child.g + child.h; //set child's f value as sum of g and h
                
                auto itO = openList.begin();
                while (itO != openList.end())
                {
                    if (child.loc == itO->loc && child.g>(itO->g)) //if child location already in open list and child's g is bigger than node's already in list, skip rest of loop
                    {
                        skip = true;
                        break; //CHECK: does this continue correctly?
                    }
                    itO++;
                }
                
                if (!skip)
                    openList.insert(child); //insert new child to open list
            }
        }
    }
            //didn't find path
}


//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}

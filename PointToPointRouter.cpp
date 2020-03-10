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
    
    bool isDestination(const GeoCoord current, const GeoCoord end) const;
    double calculateHValue(const GeoCoord current, const GeoCoord end) const;
    void AStarSearch(GeoCoord start, GeoCoord end) const;
    
    struct Node
    {
        Node(GeoCoord l) : loc(l),f(0),g(0),h(0) {}
        GeoCoord prev;
        GeoCoord loc;
        double f,g,h;
        
        bool operator<(const Node &rhs) const //define less than operator for struct so set is ordered by value of f
        {
            return f < rhs.f;
        }
        
    };
    

   // ExpandableHashMap<GeoCoord, GeoCoord> locationOfPreviousWayPoint; //declare map mapping to previous way
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
        double& totalDistanceTravelled) const //implement distance
{
    AStarSearch(start, end);
            cout << "here?"<<endl;
    return NO_ROUTE;  // Delete this line and implement this function correctly
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

void PointToPointRouterImpl::AStarSearch(GeoCoord start, GeoCoord end) const
{
    cout << "get here"<<endl;
    list<Node> closedList;
    set<Node> openList;
    
    Node startNode(start);
    openList.insert(startNode);
    
    vector<StreetSegment> segs;
    
    while (!openList.empty())
    {
        bool skip = false;
        Node currentNode = *openList.begin();
        openList.erase(openList.begin());
        
        closedList.push_back(currentNode);
        
        if (currentNode.loc == end) //if end has been reached
        {
            cout << "found path!" <<endl; //implement trace path
            return;
        }
        
        sMap->getSegmentsThatStartWith(currentNode.loc, segs);
        
        for (int i=0; i<segs.size(); i++)
        {
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
            cout << "didn't find path";
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

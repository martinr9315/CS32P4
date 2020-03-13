#include "provided.h"
#include <string>
#include <vector>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* sMap;
    string getDirection(double angle) const;
    string getTurn(double angle) const;
    void generatePathSection(const list<StreetSegment> route, vector<DeliveryCommand>& commands, double& totalDistanceTravelled) const;
    
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
{
    sMap = sm;
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    /*double oldCrowDistance, newCrowDistance;
    vector<DeliveryRequest>& opDeliveries = deliveries;
    DeliveryOptimizer op(sMap);
    op.optimizeDeliveryOrder(depot, opDeliveries, oldCrowDistance, newCrowDistance); //optimize delivery order--for now does nothing at all*/

    PointToPointRouter router(sMap);
    list<StreetSegment> r;
    DeliveryResult returnResult = router.generatePointToPointRoute(depot, deliveries[0].location, r, totalDistanceTravelled);//find route from depot to first delivery
    if (returnResult == NO_ROUTE)
        return returnResult;
    generatePathSection(r, commands, totalDistanceTravelled);
    DeliveryCommand deliverC;
    deliverC.initAsDeliverCommand(deliveries[0].item);
    commands.push_back(deliverC);
    
    for (int i=1; i<deliveries.size(); i++)
    {
        returnResult = router.generatePointToPointRoute(deliveries[i-1].location, deliveries[i].location, r, totalDistanceTravelled);
        if (returnResult == NO_ROUTE)
            return returnResult;
        generatePathSection(r, commands, totalDistanceTravelled);
        deliverC.initAsDeliverCommand(deliveries[i].item);
        commands.push_back(deliverC);
    }
    
    returnResult = router.generatePointToPointRoute(deliveries[deliveries.size()-1].location, depot, r, totalDistanceTravelled);//find route from last deliver back to depot
    if (returnResult == NO_ROUTE)
        return returnResult;
    generatePathSection(r, commands, totalDistanceTravelled);
    
    return DELIVERY_SUCCESS;  // Delete this line and implement this function correctly
}

void DeliveryPlannerImpl::generatePathSection(const list<StreetSegment> route, vector<DeliveryCommand>& commands, double& totalDistanceTravelled) const
{
    auto it = route.begin();
    while (it!= route.end())
    {
        string direction = getDirection(angleOfLine(*it)); //get direction of segment
        double dist = distanceEarthMiles(it->start, it->end); //get length of segment
        string name = it->name;
        
        StreetSegment prev = *it;
        it++;
        string turn = getTurn(angleBetween2Lines(prev, (*it))); //get whether there is a turn between this segment and next
        while (prev.name == it->name) //if no turn onto next segment
        {
            dist +=distanceEarthMiles(it->start, it->end); //increment distance by next segment
            prev = *it; //update prev and iterator
            it++;
            turn = getTurn(angleBetween2Lines(prev, (*it))); //update turn between two segments to be able to check it
        }
        
        DeliveryCommand dc;
        dc.initAsProceedCommand(direction, name, dist); //create proceed command for combined segments
        totalDistanceTravelled+= dist; //update travel distance
        commands.push_back(dc); //push command in to vector
    
        if (it!= route.end())
        {
            dc.initAsTurnCommand(turn, it->name);
            commands.push_back(dc);
        }
    }
}

string DeliveryPlannerImpl::getDirection(double angle) const
{
    /*
     0 <= angle < 22.5: east
     o 22.5 <= angle < 67.5: northeast
     o 67.5 <= angle < 112.5: north
     o 112.5 <= angle < 157.5: northwest
     o 157.5 <= angle < 202.5: west
     o 202.5 <= angle < 247.5: southwest
     o 247.5 <= angle < 292.5: south
     o 292.5 <= angle < 337.5: southeast
     o angle >= 337.5: east
     */
    if (angle>=0 && angle<22.5)
        return "east";
    else if (angle < 67.5)
        return "northeast";
    else if (angle <112.5)
        return "north";
    else if (angle<157.5)
        return "northwest";
    else if (angle<202.5)
        return "west";
    else if (angle<247.5)
        return "southwest";
    else if (angle<292.5)
        return "south";
    else if (angle<337.5)
        return "southeast";
    else
        return "east";
}

string DeliveryPlannerImpl::getTurn(double angle) const
{
    /* <1 degrees or > 359 degrees: Do not generate a turn command. Instead, just
    generate a proceed command for the second street since there is no real turn
    occurring from the first street.
    o >= 1 degrees and < 180 degrees: Generate a left turn command.
    o >= 180 degrees and <= 359 degrees: Generate a right turn command*/
    if (angle>=1 && angle<180)
        return "left";
    else if (angle >= 180 && angle <=359)
        return "right";
    else // if angle <1 or angle > 359;
        return "straight";
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}
